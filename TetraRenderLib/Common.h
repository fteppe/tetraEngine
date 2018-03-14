#pragma once

//This file contains common values used accross the application.

//Thses tags are used to determine what must and what mussn't be rendered during a render pass.
//This can be usefull in many cases for exemple to only render dynamic shadows for dynamic objects etc...
enum RenderTag {
	RT_TRANSPARENT,
	RT_REFLECTIVE,
	STATIC,
	DYNAMIC,
	WORLD_OBJECT ,
	POST_PROCESS
};

//General enums that can be used accross the engine.
enum EngineEnum {
	SCREEN_FRAMEBUFFER //This enum is used to build a frameBuffer that will render to the screen as a primary output.
};

//This will be a constant for our program. Shared accross the entire program.
#define WIDTH 1024
#define HEIGHT 720