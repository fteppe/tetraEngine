#include "RenderPipeline.h"
#include "ShaderPostProcess.h"

using namespace tetraRender;

RenderPipeline::RenderPipeline()
{
	std::unique_ptr<RenderPass> rp(new RenderPass);
	//We have to move the pointer since it is unique, so we have to transfer it and not do a simple copy.
	renderPasses.push_back(std::move(rp));
}


RenderPipeline::~RenderPipeline()
{
}

void tetraRender::RenderPipeline::setupRenderPasses()
{
	std::shared_ptr<Texture> normalsBuffer(new Texture);
	std::shared_ptr<Texture> depthBuffer(new Texture);
	std::shared_ptr<Texture> spec(new Texture);
	std::shared_ptr<Texture> fragPos(new Texture);
	gBuffer["color"] = std::shared_ptr<Texture>(new Texture);
	gBuffer["normals"] = normalsBuffer;
	gBuffer["depth"] = depthBuffer;
	gBuffer["specularity"] = spec;
	gBuffer["fragPos"] = fragPos;
	gBuffer["shadowDistance"] = std::shared_ptr<Texture>(new Texture);


	FrameBuffer * frame = new FrameBuffer;
	frame->setHDR(true);
	//frame->renderToScreen();

	////ShadowMap frameBuffer
	//gBuffer["shadowMap"] = std::shared_ptr<Texture>(new Texture);
	//frame->setDepthTexture(gBuffer["shadowMap"]);
	////shadow Passes
	
	
	//renderPasses.push_back(new RenderPass());
	//RenderPass * pass = renderPasses.back();
	//pass->setRenderOutput(frame);
	//pass->setRenderTagsIncluded({ WORLD_OBJECT });
	//pass->setCamera(&this->shadowProjection);
	////We create a specific material that is very simple to render everything through it.
	//shaders["transform"] = std::shared_ptr<Shader>(new Shader("transform.ver", "transform.frag"));
	//materials["shadowMapMat"] = std::shared_ptr<Material>(new Material(shaders["transform"]));
	//pass->setMat(materials["shadowMapMat"]);


	//Colors renderPass.
	frame = new FrameBuffer;
	frame->addColorOutputTexture(gBuffer["color"]);
	frame->setDepthTexture(depthBuffer);
	frame->addColorOutputTexture(normalsBuffer);
	frame->addColorOutputTexture(spec);
	frame->addColorOutputTexture(fragPos);
	frame->addColorOutputTexture(gBuffer["shadowDistance"]);


	std::unique_ptr<RenderPass> pass(new RenderPass);
	//We set the frame as the renderPass we want for the colors.

	pass->setRenderOutput(frame);
	pass->setRenderTagsIncluded({ WORLD_OBJECT });
	//pass->setCamera(&this->cam);
	// since pass is a unique_ptr we move it inside the vector.
	renderPasses.push_back(std::move(pass));

	//the second render pass we don't set a frameBuffer so it will render to the screen.
	pass.reset(new RenderPass);
	pass->setRenderTagsIncluded({ POST_PROCESS });
	//pass->setCamera(&this->cam);
}

void tetraRender::RenderPipeline::setupPostProcessing()
{
	//All this part is very exeprimental and hard coded weirdly which I would like to change in the future.

	//for the post processing we need a flat triangle
	std::vector<std::vector<int>> faces = { { 0,1,2 } };
	std::vector<glm::vec3> points = { glm::vec3(-1,-1,0), glm::vec3(3,-1,0),glm::vec3(-1,3,0) };
	VertexBufferObject * screen = new VertexBufferObject(points, faces);
	//the UVs of our screen, the way it is set up we should have the sides of the screen aligned with the side of the square in the triangle.
	std::vector<glm::vec3> uvs = { glm::vec3(0,0,0), glm::vec3(2,0,0), glm::vec3(0,2,0) };
	screen->setUVs(uvs);
	//we give it a name so we can more easely follow it.
	screen->setFilePath(std::pair<std::string, std::string>("hard", "screen"));
	std::shared_ptr<VertexBufferObject> vbo_ptr(screen);
	Solid* screenObj = new Solid(vbo_ptr);
	//models["hard"]["screen"] = vbo_ptr;
	//this triangle has a different shader than usual.

	Shader* shader = new ShaderPostProcess({ "postProcess.ver" }, { "postProcess.frag" });
	std::shared_ptr<Shader> shader_ptr(shader);
	Material* mat = new Material(shader_ptr);
	std::shared_ptr<Material> postProcessMat(mat);
	postProcessMat->setChannel(gBuffer["color"], "color");
	postProcessMat->setChannel(gBuffer["normals"], "normals");
	postProcessMat->setChannel(gBuffer["depth"], "depth");
	postProcessMat->setChannel(gBuffer["specularity"], "specularity");
	postProcessMat->setChannel(gBuffer["fragPos"], "fragPos");
	postProcessMat->setChannel(gBuffer["shadowDistance"], "shadowDistance");
	postProcessMat->setChannel(gBuffer["shadowMap"], "shadowMap");

	screenObj->setMaterial(postProcessMat);
	screenObj->addTag(POST_PROCESS);
	//We add all these newly created elements to the scene;
	
	//Push back is a bit complicated with a unique ptr;
	renderSurfaces.push_back(std::move(std::unique_ptr<Solid>( screenObj)));
}