#include "stdafx.h"
#include "Shader.h"
#include "Scene.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <glew/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include "Solid.h"
#include "Light.h"
#include "PreprocessorShader.h"
using namespace tetraRender;

Shader::Shader()
{
	highestTextureUnitUsed = 0;
}

Shader::Shader(std::string vertex, std::string fragment) : Shader()
{

	vertex = shaderDir + vertex;
	fragment = shaderDir + fragment;
	std::string lightCalc = "shaders/lightCalc.frag";

	GLuint vertexId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentId = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint lumDiffuseCalc = glCreateShader(GL_FRAGMENT_SHADER);
	//compiling both shaders
	compileShader(vertexId, vertex);
	compileShader(fragmentId, fragment);
	compileShader(lumDiffuseCalc, lightCalc);

	program = glCreateProgram();
	glAttachShader(program, vertexId);
	glAttachShader(program, fragmentId);
	glAttachShader(program, lumDiffuseCalc);
	//once they are linked to a program they are deleted
	linkProgram();
	glDeleteShader(vertexId);
	glDeleteShader(fragmentId);
	glDeleteShader(lumDiffuseCalc);

	getUniformLocations();
}

Shader::Shader(std::vector<std::string> vertexShaders, std::vector<std::string> fragmentShaders) : Shader() 
{
	std::vector<GLuint> vertexs;
	std::vector<GLuint> fragments;
	//We create the shaders
	for (int i = 0; i < vertexShaders.size(); i++)
	{
		vertexs.push_back(glCreateShader(GL_VERTEX_SHADER));
	}
	for (int i = 0; i < fragmentShaders.size(); i++)
	{
		fragments.push_back(glCreateShader(GL_FRAGMENT_SHADER));
	}
	std::cout << "Compiling shaders" << std::endl;
	//We compile them
	for (int i = 0; i < vertexShaders.size(); i++)
	{
		compileShader(vertexs[i], shaderDir + vertexShaders[i]);
	}
	for (int i = 0; i < fragmentShaders.size(); i++)
	{
		compileShader(fragments[i], shaderDir + fragmentShaders[i]);
	}
	//we create a program
	program = glCreateProgram();
	//we attach each shader to the program
	for (int i = 0; i < vertexShaders.size(); i++)
	{
		glAttachShader(program, vertexs[i]);
	}
	for (int i = 0; i < fragmentShaders.size(); i++)
	{
		glAttachShader(program, fragments[i]);
	}
	//linking
	std::cout << "Linking" << std::endl;
	linkProgram();
	//one this is done we delete the shaders
	for (int i = 0; i < vertexShaders.size(); i++)
	{
		glDeleteShader(vertexs[i]);

	}
	for (int i = 0; i < fragmentShaders.size(); i++)
	{
		glDeleteShader(fragments[i]);
	}
	getUniformLocations();
}





Shader::~Shader()
{
	glDeleteProgram(program);
}

unsigned int Shader::getProgram() const
{
	return program;
}





void Shader::setProgramInformation(tetraRender::Scene& scene, Solid const& object)
{
	glUseProgram(program);
	Camera cam = scene.getCam();
	//Light light = scene.getLight();
	//We get the light data;
	//std::vector<float> lightData(light.getDataArray());
	//we get the camera space and calulculate the projection that will be done to all the vertices
	glm::mat4 cameraSpace = cam.getProjection();
	glm::mat4 objectSpace = object.getmodelMatrix();
	glm::mat4 worldSpace = cameraSpace * objectSpace;
	//the projection matrices sent to the shader
	sendMatrix4("objectSpace", objectSpace);
	sendMatrix4("mvp", worldSpace);
	sendFloat("near", cam.getNearFarPlanes().x);
	sendFloat("far", cam.getNearFarPlanes().y);
	//glUniformMatrix4fv(uniforms["mvp"], 1, false, glm::value_ptr(worldSpace));
	//the objectspace that can be used to calculate lights or the posiiton of a vertex to a point. We send it to the shader.
	
	//we send the light data to the shader, for now we can handle only one light
	//glUniform1fv(uniforms["light"], lightData.size(), &lightData[0]);

}

void Shader::sendTexChannels(std::map<std::string, std::shared_ptr<Texture>> textures)
{
	glUseProgram(program);
	//iterating through the map of channels.
	for (auto it = textures.begin(); it != textures.end(); it++)
	{
		//we send to the program the channel, with it's name and the texture unit.
		sendTexture(it->first, it->second);
	}

}

void tetraRender::Shader::resetTextureUnitCount()
{
	for (int i = 0; i < highestTextureUnitUsed; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	this->highestTextureUnitUsed = 0;
}

void Shader::compileShader(GLuint shader, std::string shaderPath)
{
	std::ifstream shaderSource(shaderPath);
	std::vector<std::string> includes;
	std::string source = PreprocessorShader::processFile(shaderPath, includes);
	if (source.size() == 0)
	{
		std::cout << __FILE__ << " " << __LINE__ << "empty shader File" <<shaderPath<< std::endl;
	}

	const char* shaderChar = source.c_str();
	glShaderSource(shader, 1, &shaderChar, NULL);
	glCompileShader(shader);

	int  success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::"<<shaderPath<<"::COMPILATION_FAILED\n" << infoLog;
		std::cout << "inclusion order : \n";
		for (unsigned i = 0; i < includes.size(); i++)
		{
			std::cout << i<<" :: "<<includes[i] << std::endl;
		}
	}
}

void Shader::linkProgram()
{
	glLinkProgram(program);

	int  success = 0;
	char infoLog[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINK FAILED\n" << infoLog << std::endl;
	}
}

void Shader::getUniformLocations()
{
	glUseProgram(program);
	uniforms["mvp"] = glGetUniformLocation(program, "mvp");
	uniforms["objectSpace"] = glGetUniformLocation(program, "objectSpace");
	uniforms["light"] = glGetUniformLocation(program, "light");
}

void Shader::sendMatrix4(std::string name, glm::mat4 matrix)
{
	GLint loc = getUniformLocation(name);
	glUniformMatrix4fv(loc, 1, false, glm::value_ptr(matrix));
}

void Shader::sendFloat(std::string name, float floatIn)
{
	//floatIn = 0.5;

	GLint loc = getUniformLocation(name);
	glUniform1f(loc, floatIn);

}

void tetraRender::Shader::sendInt(std::string name, int intIn)
{
	GLint pos = getUniformLocation(name);
	glUniform1i(pos, intIn);
}

void tetraRender::Shader::sendVec3(std::string name, glm::vec3 vec)
{
	GLint pos = getUniformLocation(name);
	glUniform3f(pos, vec.x, vec.y, vec.z);
}

void tetraRender::Shader::sendLight(std::string name, tetraRender::Light light)
{
	//Since the light is a struct we need to send each component.
	sendVec3(name + ".pos", light.getPos());
	//sendVec3(name + ".pos", glm::vec3(0,0,1));
	sendVec3(name + ".color", light.col);
	sendFloat(name + ".intensity", light.intensity);
	//By default we consider that no light has a shadow, and we change that when we send the shadows datas.
	sendInt(name + ".shadowIndex", -1);
	//GLint posIntensity = getUniformLocation(name + ".intensity");
}

void tetraRender::Shader::sendTexture(std::string channelName, std::shared_ptr<Texture> tex)
{
	
	if (uniforms.find(channelName) == uniforms.end())
	{
		uniforms[channelName] = glGetUniformLocation(program, channelName.c_str());
	}
	
	tex->applyTexture(this->program, uniforms[channelName], highestTextureUnitUsed);
	highestTextureUnitUsed++;
}

GLint tetraRender::Shader::getUniformLocation(std::string uniform)
{
	GLint loc;
	if (uniforms.find(uniform) == uniforms.end())
	{
		loc = glGetUniformLocation(program, uniform.c_str());
		uniforms[uniform] = loc;
	}
	else
	{
		loc = uniforms[uniform];
	}
	return loc;
}
