#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <set>
#include <memory>
#include "Material.h"
#include "Common.h"

class Scene;

//Any object in the world. Goes with all the functions necessary to set coordinates, 
//rotation...
namespace tetraRender
{
	class GameObject
	{
	public:
		GameObject();
		~GameObject();
		void setPos(glm::vec3 pos);
		void setScale(glm::vec3 scale);
		void setRotation(float rotation, glm::vec3 rotationAngle);
		void addTag(RenderTag tag);
		std::set<RenderTag> getRenderTags();
		glm::vec3 getPos();
		glm::vec3 getScale();
		glm::vec4 getRotation();
		glm::mat4 getmodelMatrix() const;
		std::shared_ptr<Material> getMaterial();
		virtual void draw(Scene& scene);
	protected:
		void updateModelMatrix();
		glm::vec3 pos;
		glm::vec3 scale;
		glm::vec3 rotationAngle;
		float rotation;
		glm::mat4 modelMatrix;

		//These tags are an indication for the renderer. This mus always be sorted.
		std::set<RenderTag> renderTags;

		std::shared_ptr<Material> material_ptr;
	};

}