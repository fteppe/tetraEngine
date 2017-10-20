#version 330 core

uniform sampler2D specularMap;

vec3 fragLight(float light[7], vec3 normal, vec3 vertexPos);

vec3 specCalcFull(float light[7], vec3 normal, vec3 vertexPos, vec3 camPos)
{
	return vec3(0.0);
}

vec3 specCalc(float light[7], vec3 normal, vec3 vertexPos, vec3 camPos, float specVal)
{
	vec3 eyeVec = camPos - vertexPos;
	//we get the value of the light energy hitting the fragment
	vec3 lightIntensity = fragLight(light, normal, vertexPos);
	vec3 lightPos = vec3(light[0], light[1], light[2]);
	vec3 lightDir = normalize(vertexPos - lightPos);
	normal = normalize(normal);
	vec3 lightReflection = lightDir + 2 * normal;

	float specIntensity = pow( dot(eyeVec, lightReflection) , specVal);
	vec3 returnVal = lightIntensity * vec3(specIntensity);
	return returnVal;
}