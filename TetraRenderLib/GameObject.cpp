#include "stdafx.h"
#include "GameObject.h"
#include "Scene.h"

using namespace tetraRender;

GameObject::GameObject()
{
	rotation = 0;
	pos = glm::vec3(0);
	scale = glm::vec3(1);
	rotationAngle = glm::vec3(0, 0, 1);
	updateModelMatrix();
}


GameObject::~GameObject()
{
}

void GameObject::setPos(glm::vec3 pos)
{
	this->pos = pos;
	updateModelMatrix();
}

void GameObject::setScale(glm::vec3 scale)
{
	this->scale = scale;
	updateModelMatrix();
}

void GameObject::setRotation(float rotation, glm::vec3 rotationAngle)
{
	this->rotation = rotation;
	this->rotationAngle = rotationAngle;
	updateModelMatrix();
}

void GameObject::addTag(RenderTag tag)
{
	renderTags.insert(tag);
}

std::set<RenderTag> GameObject::getRenderTags()
{
	return renderTags;
}

glm::vec3 GameObject::getPos()
{
	return pos;
}

glm::vec3 GameObject::getScale()
{
	return scale;
}

glm::vec4 GameObject::getRotation()
{
	//TODO: see if this is right, might not be.
	return glm::vec4(rotationAngle, rotation);
}

glm::mat4 GameObject::getmodelMatrix() const
{
	return modelMatrix;
}

std::shared_ptr<Material> GameObject::getMaterial()
{
	return material_ptr;
}

void GameObject::draw(tetraRender::Scene & scene)
{
}

void GameObject::updateModelMatrix()
{
	modelMatrix = glm::rotate(rotation, rotationAngle) * glm::scale(scale) *  glm::translate(pos);
}