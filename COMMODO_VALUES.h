#ifndef COMMONVALS
#define COMMONVALS

#include "include/stb/stb_image.h"

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace)

const float toRadians = 3.14159265f / 180.0f;
const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 4;

#endif




//int pointLightCount = 10;
//int spotLightCount = 10;