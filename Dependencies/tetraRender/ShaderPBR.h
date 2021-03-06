#pragma once
#include "Shader.h"
//#include <vector>

namespace tetraRender
{
	class ShaderPBR :
		public Shader
	{
	public:
		ShaderPBR();
		ShaderPBR(std::vector<std::string> vertexShaders, std::vector<std::string> fragmentShaders);
		~ShaderPBR();

		void setProgramInformation(Scene& scene, const Solid& solid);
	};

}
