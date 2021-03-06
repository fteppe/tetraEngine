#include "stdafx.h"
#include "Material.h"
#include <iostream>

using namespace tetraRender;

const std::string Material::shaderField = "shader";

Material::Material()
{
	setName("mat");
}
Material::Material(std::shared_ptr<Shader> shader) : Material()
{
	shader_ptr = shader;
	fillParameterContainer();
}

Material::~Material()
{
}

void tetraRender::Material::setShader(std::shared_ptr<Shader> newShader)
{
	std::string name = getName();
	this->shader_ptr = newShader;
	fillParameterContainer();
	if (name.size())
	{
		setName(name);

	}
}

void Material::setChannel( std::shared_ptr<Texture> text, std::string channel )
{
	textures[channel] = text;
}

void Material::setProgramInformation(Scene & scene, Solid const & object)
{
	shader_ptr->setProgramInformation(scene, object);
	shader_ptr->sendTexChannels(textures);
}

void Material::apply(Mesh* const& VBO, Scene & scene, Solid const& solid)
{
	shader_ptr->use();

	shader_ptr->setProgramInformation(scene, solid);
	shader_ptr->sendTexChannels(textures);
	//We take all the parameters of that material and send it to the shader.
	//This is great to have custom public variables and to be able to change them on the fly.
	for (auto param : parametersContainer.getParameters())
	{
		if (param.second == ParameterType::VEC3)
		{
			shader_ptr->sendVec3(param.first, parametersContainer.getVec3(param.first));
		}
		else if (param.second == ParameterType::FLOAT)
		{
			shader_ptr->sendFloat(param.first, parametersContainer.getFloat(param.first));
		}
	}
	VBO->drawObject(*shader_ptr);
	shader_ptr->resetTextureUnitCount();

}

void tetraRender::Material::update()
{
	//fillParameterContainer();
}

std::shared_ptr<Shader> Material::getShaderProgram()
{
	return shader_ptr;
}

const std::map<std::string, std::shared_ptr<Texture>> tetraRender::Material::getChannels()
{
	return this->textures;
}

void tetraRender::Material::fillParameterContainer()
{
	GLint count;
	glGetProgramiv(shader_ptr->getProgram(), GL_ACTIVE_UNIFORMS, &count);

	textures.clear();
	parametersContainer = ParameterContainer();

	for (int i = 0; i < count; i++)
	{
		std::string uniformName;
		char* name = new char[GL_ACTIVE_UNIFORM_MAX_LENGTH];
		GLsizei length;
		int size;
		GLenum type;
		glGetActiveUniform(shader_ptr->getProgram(), i, GL_ACTIVE_UNIFORM_MAX_LENGTH, &length, &size, &type, name);
		uniformName = std::string(name);
		uniformName.shrink_to_fit();
		if (uniformName.substr(0, 3) == "pu_")
		{
			if (type == GL_FLOAT_VEC3)
			{
				parametersContainer.set(uniformName, glm::vec3(0));
			}
			else if (type == GL_FLOAT)
			{
				parametersContainer.set(uniformName, 0.0f);
			}
		}
		if (type == GL_SAMPLER_2D ||type == GL_SAMPLER_3D)
		{
			setChannel(nullptr, uniformName);
		}
		
	}
	
}

