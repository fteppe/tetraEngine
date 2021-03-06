#version 430 core

#include "sampleEquirectangular.glsl"

in vec3 vertexPos;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 light;
layout(location = 4) out float fragDepthShadowOut;
uniform sampler2D skybox;

//We ahve a equirectangular projection of a sky and we want to project it on a cube.
void main()
{
	fragDepthShadowOut = 1;
    vec3 pos = normalize(vec3(vertexPos.x,vertexPos.y,vertexPos.z));
    float theta = acos (pos.y);
	float phi = atan (pos.x, -pos.z);
	light = vec3(0);
	normal  = vec3(0);
    FragColor = sampleEquirectangular(skybox, pos);
	
}
