#pragma once
#include "Texture.h"

#include <string>
#include <vector>
#include <map>
#include <glew/glew.h>
#include <glm/matrix.hpp>

namespace tetraRender
{
	const std::string shaderDir = "shaders/";
	class Scene;
	class Solid;
	class Light;

	class Shader : public Resource
	{
	public:
		Shader();
		Shader(std::string vertex, std::string fragment);
		//This constructor allows several files to be linked in a single program.
		Shader(std::vector<std::string> vertexShaders, std::vector<std::string> fragmentShaders);
		//Shader(const Shader& shader);
		~Shader();
		void compileAll();
		unsigned int getProgram() const;
		virtual void setProgramInformation(Scene & scene, Solid const& object);
		virtual void sendTexChannels(std::map<std::string, std::shared_ptr<Texture>> textures);
		void resetTextureUnitCount();
		void use();
		void setShaderFiles(std::vector<std::pair<std::string, GLenum>> shaderFiles);
		std::vector<std::pair<std::string, GLenum>> getShaderFiles();

		void sendMatrix4(std::string name, glm::mat4);
		void sendFloat(std::string name, float floatIn);
		void sendInt(std::string name, int intIn);
		void sendVec3(std::string name, glm::vec3 vec);
		void sendLight(std::string name, Light light);
		void sendTexture(std::string channelName, std::shared_ptr<Texture>);

	protected:
		//All the shader files necessary to compile the things.
		std::vector<std::pair<std::string, GLenum>> shaderFiles;
		void compileShader(GLuint shader, std::string shaderPath);
		void linkProgram();
		virtual void getUniformLocations();
		GLint getUniformLocation(std::string uniform);

		unsigned int program;
		std::map<std::string, GLint> uniforms;

		//This is in the case we use the same program for different materials that don't use the same number of samplers.
		//In this case to make sure those samplers are unbound when they need to, we keep track of that.
		GLuint highestTextureUnitUsed;
	};

}
