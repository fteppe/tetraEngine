#pragma once
#include <glew/glew.h>
//This file contains common values used accross the application.

//Thses tags are used to determine what must and what mussn't be rendered during a render pass.
//This can be usefull in many cases for exemple to only render dynamic shadows for dynamic objects etc...
enum RenderTag {
	RT_TRANSPARENT,
	RT_REFLECTIVE,
	STATIC,
	DYNAMIC,
	WORLD_OBJECT ,
	POST_PROCESS,
	FORWARD_RENDER
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Values that represent game object types. </summary>
///
/// <remarks>	Fteppe, 25/04/2018. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////


//General enums that can be used accross the engine.
enum EngineEnum {
	SCREEN_FRAMEBUFFER //This enum is used to build a frameBuffer that will render to the screen as a primary output.
};

namespace tetraRender {
	enum class  ParameterType { VEC3, FLOAT, INT, STRING, BOOL };

	enum class GameObjectType {
		LIGHT,
		SOLID,
		GAME_OBJECT,
		CAMERA
	};
}

#define tetraBitField GLbitfield

#define VBO_CONTAINER std::map<std::string, std::map<std::string, std::shared_ptr<Mesh>>>
#define MAT_CONTAINER std::map<std::string, std::shared_ptr<Material>>
#define TEXTURE_CONTAINER std::map<std::string, std::shared_ptr<Texture>>
#define SHADER_CONTAINER std::map<std::string, std::shared_ptr<Shader>>

//This will be a constant for our program. Shared accross the entire program.

namespace tetraRender {
	const unsigned int WIDTH = 1280;
	const unsigned int HEIGHT = 720;

	const unsigned int MAX_NUM_SHADOWMAP = 3;
}
