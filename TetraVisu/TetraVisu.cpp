// TetraVisu.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include "WindowBuilder.h"
#include <tetraRender\Scene.h>

class WindowBuilder;

int main(int argc, char **argv)
{

	_CrtSetDbgFlag(
		_CRTDBG_ALLOC_MEM_DF |
		_CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ERROR,
		_CRTDBG_MODE_DEBUG);

	tetraRender::Light* l = new tetraRender::Light();
	tetraRender:: Camera* cam = new tetraRender:: Camera();
	tetraRender::Camera* cam2 = new tetraRender::Camera();

	WindowBuilder window;

	//SceneLoader sceneloader;
}
