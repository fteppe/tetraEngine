#version 430 core

in vec3 vertexPos;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 light;
layout(location = 4) out float fragDepthShadowOut;
uniform samplerCube skybox;

void main()
{
	fragDepthShadowOut = 1;
	vec3 pos = vec3(vertexPos.x,vertexPos.y,vertexPos.z);
	light = vec3(1);
	normal  = vec3(0);
    FragColor = texture(skybox , normalize(pos)) * 5;
	
}
