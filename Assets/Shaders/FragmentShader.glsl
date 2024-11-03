#version 460 core

in vec4 vCol;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec4 vColor;

out vec4 color;

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 3;

struct Light {
    vec3 color;
    float ambientIntensity;
    float diffuseIntensity;
};

struct DirectionalLight {
    Light base;
    vec3 direction;
};

struct PointLight {
    Light base;
    vec3 position;
    float constant;
    float linear;
    float exponent;
};

struct SpotLight {
    PointLight base;
    vec3 direction;
    float edge;
};

struct Material {
    float specularIntensity;
    float shininess;
};

uniform int pointLightCount;
uniform int spotLightCount;
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform sampler2D theTexture;
uniform Material material;
uniform vec3 eyePosition;
uniform sampler2D shadowMap; // Shadow map

vec4 CalcLightByDirection(Light light, vec3 direction) {
    vec4 ambientColor = vec4(light.color, 1.0f) * light.ambientIntensity;
    float diffuseFactor = max(dot(normalize(Normal), normalize(direction)), 0.0f);
    vec4 diffuseColor = vec4(light.color * light.diffuseIntensity * diffuseFactor, 1.0f);

    vec4 specularColor = vec4(0, 0, 0, 0);
    if (diffuseFactor > 0.0f) {
        vec3 fragToEye = normalize(eyePosition - FragPos);
        vec3 reflectedVertex = normalize(reflect(-direction, normalize(Normal)));
        float specularFactor = dot(fragToEye, reflectedVertex);
        if (specularFactor > 0.0f) {
            specularFactor = pow(specularFactor, material.shininess);
            specularColor = vec4(light.color * material.specularIntensity * specularFactor, 1.0f);
        }
    }
    return (ambientColor + diffuseColor + specularColor);
}

vec4 CalcDirectionalLight() {
    return CalcLightByDirection(directionalLight.base, normalize(directionalLight.direction));
}

vec4 CalcPointLight(PointLight pLight) {
    vec3 direction = FragPos - pLight.position;
    float distance = length(direction);
    direction = normalize(direction);
    vec4 lightColor = CalcLightByDirection(pLight.base, direction);
    float attenuation = pLight.exponent * distance * distance + pLight.linear * distance + pLight.constant;

    if (attenuation > 0.0) {
        return (lightColor / attenuation);
    } else {
        return vec4(0.0);
    }
}

vec4 CalcSpotLight(SpotLight sLight) {
    vec3 rayDirection = normalize(FragPos - sLight.base.position);
    float slFactor = dot(rayDirection, normalize(sLight.direction));
    if (slFactor > cos(sLight.edge)) { // Compare with cos(edge)
        vec4 lightColor = CalcPointLight(sLight.base);
        return lightColor * (1.0f - (1.0f - slFactor) / (1.0f - cos(sLight.edge)));
    } else {
        return vec4(0, 0, 0, 0);
    }
}

vec4 CalcPointLights() {
    vec4 totalColor = vec4(0, 0, 0, 0);
    for (int i = 0; i < pointLightCount; i++) {
        totalColor += CalcPointLight(pointLights[i]);
    }
    return totalColor;
}

vec4 CalcSpotLights() {
    vec4 totalColor = vec4(0, 0, 0, 0);
    for (int i = 0; i < spotLightCount; i++) {
        totalColor += CalcSpotLight(spotLights[i]);
    }
    return totalColor;
}

void main() {
    vec4 finalColor = CalcDirectionalLight();
    finalColor += CalcPointLights();
    finalColor += CalcSpotLights();

    // Combine texture color with lighting
    color = texture(theTexture, TexCoord) * vColor * finalColor;
    // color = texture(theTexture, TexCoord) * finalColor;
    //color = texture(theTexture, TexCoord);


    // Clamping to prevent out-of-range values
    color = clamp(color, 0.0, 1.0);
}
