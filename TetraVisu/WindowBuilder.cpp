#include "stdafx.h"
#include "WindowBuilder.h"
#include <glew/glew.h>

#include <iostream>

#include <tetraRender/SceneSaver.h>
#include <tetraRender\Scene.h>
#include "imgui.h"
#include "imgui_impl_sdl_gl3.h"
#include <stdio.h>


std::shared_ptr<tetraRender::Scene> WindowBuilder::scene = nullptr;

//This will build a window using open GL and stuff, this is a way to unclutter the main.
WindowBuilder::WindowBuilder()
{
	std::cout << "starting application" << std::endl;
	/*
	WINDOW OPENGL INIT
	*/
	const std::string title = "openGL";


	unsigned width = tetraRender::WIDTH;
	unsigned height = tetraRender::HEIGHT;
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
	}

	// Setup window
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_DisplayMode current;
	SDL_GetCurrentDisplayMode(0, &current);
	window = SDL_CreateWindow("Tetra Render editor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE| SDL_RENDERER_PRESENTVSYNC);
	gl_context = SDL_GL_CreateContext(window);
	SDL_GL_SetSwapInterval(1); // Enable vsync

	glewExperimental = GL_TRUE;
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	ImGui_ImplSdlGL3_Init(window);

	// Setup style

	
	//apparently an old implementation bug tends to raise an error on startup. We call geterror to remove it.
	glGetError();
	if (SDL_GL_GetCurrentContext() == NULL)
	{
		std::cout << "no active openGL context!!\n";
	}
	std::cout << glGetString(GL_VERSION) << std::endl;
	


}

WindowBuilder::WindowBuilder(std::string sceneFile) : WindowBuilder()
{
	tetraRender::Camera cam(600.0f, 800.0f, 0.75f);
	cam.setUp(glm::vec3(0, 0, 0));
	scene = std::shared_ptr<tetraRender::Scene>(new tetraRender::Scene(cam));
	scene->load(sceneFile);
	tetraRender::SceneSaver saver;
	handler = EventHandler(scene);
	library = ResourcesLibrary(&scene->getResources());

}


WindowBuilder::~WindowBuilder()
{
}

void WindowBuilder::draw()
{


	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Main loop
	bool done = false;
	while (!done)
	{
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSdlGL3_ProcessEvent(&event);
			handler.handle(event);
			if (event.type == SDL_QUIT)
				done = true;
		}
		scene->renderScene();
		ImGui_ImplSdlGL3_NewFrame(window);
		library.display();
		ImGui::Begin("GameObjects");
		int i = 0;
		gameObjectTreeUI(scene->getGameObjects(),i);

		ImGui::End();

		ImGui::Begin("Game object editor");
		if (selectedObject)
		{
			gameObjectEditUI(selectedObject);
		}
		ImGui::End();

		ImGui::Begin("menu");
		ImGui::Button("save");
		if (ImGui::IsItemClicked())
		{
			tetraRender::SceneSaver().saveToFile(*scene, "scenes/saved.json");
			std::cout << "scene saved to " << "scenes/saved.json \n";
		}
		ImGui::End();
		ImGui::ShowDemoWindow();
		// Rendering
		glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
		ImGui::Render();
		ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);
	}

	// Cleanup
	ImGui_ImplSdlGL3_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	
}

glm::vec3 WindowBuilder::Vec3Input(glm::vec3 vector, std::string label)
{
	float vec[3] = { vector.x, vector.y, vector.z };
	//Imgui::InputFloat3(param.first.c_str(), vec, 5);
	ImGui::DragFloat3(label.c_str(), vec, 0.1f);
	vector.x = vec[0]; vector.y = vec[1]; vector.z = vec[2];
	return vector;
}

void WindowBuilder::MaterialUI(tetraRender::Material* mat)
{
	ImGui::Separator();
	auto& val = mat->getParameters();
	ImGui::Text(mat->getName().c_str());
	ImGui::Button(mat->getShaderProgram()->getName().c_str());
	std::shared_ptr<tetraRender::Shader> selectedShader = selectShader();
	
	if (selectedShader != nullptr)
	{
		mat->setShader( selectedShader  );
		mat->update();
	}
	selectedShader = nullptr;
	parameterInput(val, *mat);
	for (auto channel : mat->getChannels())
	{
		ImGui::Text(channel.first.c_str());
		if (channel.second != nullptr)
		{
			ImGui::SameLine();

			std::shared_ptr<tetraRender::Texture > tex = channel.second;
			ImGui::Button((tex->getName()).c_str());
		}
		else
		{
			ImGui::Button("set Texture");
		}
		std::shared_ptr<tetraRender::Texture> selectedTexture = selectTexture(channel.first);
		if (selectedTexture != nullptr)
		{
			mat->setChannel(selectedTexture, channel.first);
		}
	}
	ImGui::Separator();

}

void WindowBuilder::textureUI(tetraRender::Texture * tex)
{
	parameterInput(tex->getParameters(), *tex);
}

void WindowBuilder::gameObjectTreeUI(tetraRender::GameObject * gameObject, int pos)
{

	std::string name = gameObject->getName();
	auto children = gameObject->getChildren();
	if (children.size() > 0)
	{
		bool tree = ImGui::TreeNode(std::to_string(pos).c_str(), name.c_str());
		gameObjectContext(gameObject, pos);
		if (ImGui::IsItemClicked())
		{
			selectedObject = gameObject;
		}
		if (tree)
		{
			int childPos = pos;
			for (auto child : gameObject->getChildren())
			{
				childPos++;
				gameObjectTreeUI(child,childPos);

			}
			ImGui::TreePop();

		}
	}
	else
	{
		ImGui::Button(name.c_str()), std::to_string(pos).c_str();
		gameObjectContext(gameObject, pos);
		if (ImGui::IsItemClicked())
		{
			selectedObject = gameObject;
		}
	}


}

void WindowBuilder::gameObjectEditUI(tetraRender::GameObject * gameObject)
{
	ImGui::Text(gameObject->getName().c_str());
	auto& paramContainer = gameObject->getParameters();
	parameterInput(paramContainer, *gameObject);


	if (gameObject->getType() == tetraRender::GameObjectType::SOLID)
	{
		//We can do static cast because we know for sure that it's a Solid thanks to the check above.
		tetraRender::Material * mat = static_cast<tetraRender::Solid*>(gameObject)->getMaterial();
		ImGui::Text(("Material : " + mat->getName()).c_str());

		MaterialUI(mat);

	}
	gameObject->update();
}

void WindowBuilder::parameterInput(tetraRender::ParameterContainer & param, tetraRender::Resource & resource)
{
	auto& paramContainer = resource.getParameters();
	bool parametersChanged = false;
	for (auto param : paramContainer.getParameters())
	{
		std::string label = param.first + resource.getName();

		if (param.second == tetraRender::ParameterType::VEC3)
		{
			glm::vec3 vector = resource.getParameters().getVec3(param.first);
			glm::vec3 witness = vector;
			vector = Vec3Input(vector, param.first.c_str());
			parametersChanged = parametersChanged || (witness != vector);
			paramContainer.set(param.first, vector);

		}
		else if (param.second == tetraRender::ParameterType::FLOAT)
		{
			float val = resource.getParameters().getFloat(param.first);
			float witness = val;
			ImGui::DragFloat(param.first.c_str(), &val, 0.1f);
			parametersChanged = parametersChanged || (witness != val);
			paramContainer.set(param.first, val);
		}
		else if (param.second == tetraRender::ParameterType::BOOL)
		{
			bool val = resource.getParameters().getBool(param.first);
			bool witness = val;
			ImGui::Checkbox((param.first+"##"+label).c_str(), &val);
			//if (ImGui::IsItemClicked())
			//{
			//	val = !val;
			//}
			parametersChanged = parametersChanged || (witness != val);
			paramContainer.set(param.first, val);
		}
	}
	if (parametersChanged)
	{
		resource.update();
	}
}

std::shared_ptr<tetraRender::Shader> WindowBuilder::selectShader()
{
	std::shared_ptr<tetraRender::Shader> returnVal = nullptr;
	tetraRender::ResourceAtlas& atlas = scene->getResources();
	if (ImGui::IsItemClicked())
	{
		ImGui::OpenPopup("choose Shader");

	}
	if (ImGui::BeginPopupModal("choose Shader"))
	{
		for (auto shaderEntry : atlas.getShaders())
		{
			ImGui::Button(shaderEntry.second->getName().c_str());
			if (ImGui::IsItemClicked())
			{
				returnVal = shaderEntry.second;
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::Button("cancel");
		if (ImGui::IsItemClicked())
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	return returnVal;
}

std::shared_ptr<tetraRender::Texture> WindowBuilder::selectTexture(std::string channel)
{
	std::shared_ptr<tetraRender::Texture> selectedTexture = nullptr;
	tetraRender::ResourceAtlas& atlas = scene->getResources();
	if (ImGui::IsItemClicked())
	{
		ImGui::OpenPopup(("choose tex##"+channel).c_str());

	}
	if (ImGui::BeginPopupModal(("choose tex##" + channel).c_str()))
	{
		for (auto textureEntry : atlas.getTextures())
		{
			ImGui::Button(textureEntry.second->getName().c_str());
			if (ImGui::IsItemClicked())
			{
				selectedTexture = textureEntry.second;
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::Button("cancel");
		if (ImGui::IsItemClicked())
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
	return selectedTexture;
}

void WindowBuilder::gameObjectContext(tetraRender::GameObject * gameobject, int id)
{ 
	std::string idCharString = std::to_string(id);
	const char* idChar = idCharString.c_str();
	if (ImGui::BeginPopupContextItem(idChar))
	{
		ImGui::Button("delete");
		if (ImGui::IsItemClicked())
		{
			if (gameobject != nullptr)
			{
				gameobject->removeFromParent();
				delete gameobject;
				if (gameobject == selectedObject)
				{
					selectedObject = nullptr;
				}
			}
		}
		ImGui::Button("move here");
		if (ImGui::IsItemClicked())
		{
			if (gameobject != selectedObject && gameobject != nullptr)
			{
				selectedObject->removeFromParent();
				selectedObject->setParent(gameobject);
				gameobject->addChild(selectedObject);
			}
		}
		ImGui::EndPopup();
	}
}

