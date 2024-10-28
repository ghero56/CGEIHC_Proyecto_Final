#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 norm;
layout (location = 3) in ivec4 boneIDs; // IDs de los huesos
layout (location = 4) in vec4 weights;  // Pesos de los huesos

out vec4 vCol;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec4 vColor;
out vec4 FragPosLightSpace; // Para las sombras

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 lightSpaceMatrix; // Matriz para la proyección desde la luz
uniform vec3 color;
uniform vec2 toffset;

const int MAX_BONES = 100;
uniform mat4 boneTransforms[MAX_BONES];

void main() {
    mat4 boneTransform = boneTransforms[boneIDs[0]] * weights[0] +
                         boneTransforms[boneIDs[1]] * weights[1] +
                         boneTransforms[boneIDs[2]] * weights[2] +
                         boneTransforms[boneIDs[3]] * weights[3];

    vec4 transformedPos = boneTransform * vec4(pos, 1.0);
    gl_Position = projection * view * model * transformedPos;

    FragPosLightSpace = lightSpaceMatrix * model * transformedPos; // Para las sombras

    vCol = vec4(0.0, 1.0, 0.0, 1.0f);
    vColor = vec4(color, 1.0f);
    TexCoord = tex + toffset;
    Normal = mat3(transpose(inverse(model))) * norm;
    FragPos = (model * transformedPos).xyz;
}
