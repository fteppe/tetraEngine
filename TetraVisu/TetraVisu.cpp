// TetraVisu.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include "WindowBuilder.h"
#include <tetraRender\Scene.h>
#include <tetraRender\Camera.h>
#include "Cam2.h"
class WindowBuilder;

int main(int argc, char **argv)
{

	_CrtSetDbgFlag(
		_CRTDBG_ALLOC_MEM_DF |
		_CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ERROR,
		_CRTDBG_MODE_DEBUG);


	_ASSERT(_CrtCheckMemory());
	Cam2* cam = new Cam2;
	//tetraRender::Camera* cam = new tetraRender::Camera;
	_ASSERT(_CrtCheckMemory());
	WindowBuilder window;

	//SceneLoader sceneloader;
}
