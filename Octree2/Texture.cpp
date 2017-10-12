#include "stdafx.h"
#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glew/glew.h>
#include <iostream>

Texture::Texture()
{
	glGenTextures(1, &textureID);
}


Texture::~Texture()
{
}

void Texture::loadTexture(std::string textureName)
{
	texturePath = textureName;
	data =  stbi_load(textureName.c_str(), &width, &height, &nrChannels, 0);
	textureData = std::vector<unsigned char>(data, data+width* height* nrChannels);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::applyTexture(GLuint program, std::string varName)
{
	
	// set the texture wrapping/filtering options (on the currently bound texture object)

	//data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		
		glUniform1i(glGetUniformLocation(program, varName.c_str()), 0); // set it manually
	}
	else
	{
		std::cout << "no texture to load" << std::endl;
	}

}
