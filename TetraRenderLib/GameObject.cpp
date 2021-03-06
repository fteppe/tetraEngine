#include "stdafx.h"
#include "GameObject.h"
#include "Scene.h"
#include <iostream>
#include <glm/gtx/matrix_decompose.hpp>
using namespace tetraRender;
using TagPair = std::pair<RenderTag, std::string>;

const std::vector<TagPair> GameObject::tagTranslation = GameObject::initTranslation();
const std::string GameObject::pos = "pos";
const std::string GameObject::scale = "scale";
const std::string GameObject::rotationAngle = "rotAngle";
const std::string GameObject::childrenField = "children";
const std::string GameObject::material = "material";
const std::string GameObject::typeField = "type";

GameObject::GameObject()
{
	rotation = 0;
	setPos(glm::vec3(0));
	setScale(glm::vec3(1));
	glm::quat rotation;
	updateModelMatrix();
	setName("nullObject");
	setRotation(glm::quat(0,0,0,0));
	parametersContainer.set(typeField, std::string("GameObject"));

}


GameObject::~GameObject()
{
}

void GameObject::setPos(glm::vec3 position)
{
	parametersContainer.set(this->pos, position);
	updateModelMatrix();
}

void GameObject::setScale(glm::vec3 scaleVec)
{
	parametersContainer.set(scale, scaleVec);
	updateModelMatrix();
}

void GameObject::setRotation(glm::quat rotationQuaternion)
{
	this->rotation = rotation;

	glm::vec3 euler = glm::eulerAngles(rotationQuaternion);
	parametersContainer.set(rotationAngle, euler);
	updateModelMatrix();
}

void GameObject::addTag(RenderTag tag)
{
	renderTags.insert(tag);
}

bool tetraRender::GameObject::removeTag(RenderTag tag)
{
	bool deleted = false;
	auto found  = renderTags.find(tag);
	if (found != renderTags.end())
	{
		deleted = true;
		renderTags.erase(tag);
	}
	return deleted;
}

void tetraRender::GameObject::addChild(GameObject * child)
{

	child->setParent(this);

	//We now need to to cancel out the transformation matrix of the parent so the child doesn't move when it is placed under the parent.




	children.push_back(child);
}

void tetraRender::GameObject::addChildNoMove(GameObject * child)
{

	glm::vec3 pos;
	glm::quat quat;
	glm::vec3 scale;
	glm::mat4 transfo = getmodelMatrix();
	glm::vec3 skewness;
	glm::vec4 perpective;

	//glm::decompose((transfo), scale, quat, pos, skewness, perpective);

	glm::mat4 newTransfo = glm::inverse(transfo)  * child->getmodelMatrix();

	glm::decompose(newTransfo, scale, quat, pos, skewness, perpective);

	child->setPos(pos);
	child->setRotation(quat);
	child->setScale(scale);
	addChild(child);
}

void tetraRender::GameObject::setParent(GameObject * parent)
{
	this->parentNode = parent;
}

std::set<RenderTag> GameObject::getRenderTags()
{
	return renderTags;
}

glm::vec3 GameObject::getPos() const
{
	return parametersContainer.getVec3(pos);
}

glm::vec3 GameObject::getScale() const
{
	return parametersContainer.getVec3(scale);
}

glm::quat GameObject::getRotation() const
{
	//TODO: see if this is right, might not be.
	glm::vec3 rotationEuler = parametersContainer.getVec3(rotationAngle);
	glm::quat qPitch = glm::angleAxis(rotationEuler.x, glm::vec3(1, 0, 0));
	glm::quat qYaw = glm::angleAxis(rotationEuler.y, glm::vec3(0, 1, 0));
	glm::quat qRoll = glm::angleAxis(rotationEuler.z, glm::vec3(0, 0, 1));

	///x,y,z are in radians
	glm::quat rotQuat =  qPitch * qYaw * qRoll;
	return rotQuat;
}

glm::mat4 GameObject::getmodelMatrix() const
{
	GameObject* parent = this->parentNode;
	glm::mat4 mat = modelMatrix;
	while (parent != NULL)
	{

		//The basic way
		mat =  parent->modelMatrix * mat;
		parent = parent->parentNode;
	}

	return mat;
}


void GameObject::draw(tetraRender::Scene & scene)
{
	for (auto go : children)
	{
		go->draw(scene);
	}
}

void GameObject::draw(tetraRender::Scene & scene,std::shared_ptr<Material> mat)
{
	for (auto go : children)
	{
		go->draw(scene, mat);
	}
}

std::shared_ptr<Material> tetraRender::GameObject::getMaterial()
{
	return material_ptr;
}


void GameObject::setMaterial(std::shared_ptr<Material> const & mat)
{
	material_ptr = mat;
}

GameObjectType tetraRender::GameObject::getType() const
{
	return GameObjectType::GAME_OBJECT;
}

std::vector<GameObject*> tetraRender::GameObject::getChildren()
{
	return children;
}

GameObject * tetraRender::GameObject::getDeepCopy()
{
	GameObject * copy = new GameObject(*this);
	//the reason we do a deep copy of the children as well is that if we delete the copy we don't want to also delete the original's children.
	copy->copyChildren();

	return copy;

}

GameObject * tetraRender::GameObject::getParent()
{
	return parentNode;
}

GameObject * tetraRender::GameObject::removeChild(GameObject * childToRemove)
{
	//We want to remove the child in parameter from the list of child of this node. It should be noted that if this vector is 
	//not part of the children, nothing will hapen.
	GameObject* returnObj = nullptr;
	std::vector<GameObject*> newChildren;
	for (auto child : children)
	{
		if (child != childToRemove)
		{
			newChildren.push_back(child);
		}
		else
		{
			returnObj = child;
		}
	}
	children = newChildren;

	return returnObj;
}

GameObject * tetraRender::GameObject::removeFromParent()
{
	GameObject* parent = getParent();
	if (parent != nullptr)
	{
		if (parent->removeChild(this) == nullptr)
		{
			std::cout << "removed from parent failed \n";
		}
		else
		{
			//We apply the parent's transformation to the child.
			glm::vec3 pos;
			glm::quat quat;
			glm::vec3 scale;
			glm::mat4 transfo = getmodelMatrix();
			glm::vec3 skewness;
			glm::vec4 perpective;
			glm::decompose(transfo, scale, quat, pos, skewness, perpective);

			setPos(pos);
			setRotation(quat);
			setScale(scale);
		}
		
	}
	return parent;
}

void tetraRender::GameObject::update()
{
	updateModelMatrix();
}

std::string tetraRender::GameObject::getTagString(RenderTag tag)
{
	std::string tagString = "";
	for (std::pair<RenderTag, std::string> tagPair : tagTranslation)
	{
		if (tagPair.first == tag)
		{
			tagString = tagPair.second;
		}
	}
	return tagString;
}

RenderTag tetraRender::GameObject::getTagEnum(std::string tag)
{
	RenderTag tagEnum = RenderTag();
	for (std::pair<RenderTag, std::string> tagPair : tagTranslation)
	{
		if (tagPair.second == tag)
		{
			tagEnum = tagPair.first;
		}
	}
	return tagEnum;
}

std::vector<std::pair<RenderTag, std::string>> tetraRender::GameObject::initTranslation()
{
	std::vector<TagPair> pairs;
	pairs.push_back(TagPair(WORLD_OBJECT, "WORLD_OBJECT"));
	pairs.push_back(TagPair(RenderTag::FORWARD_RENDER, "FORWARD_RENDER"));
	pairs.push_back(TagPair(RenderTag::POST_PROCESS, "POST_PROCESS"));

	return pairs;
}

void GameObject::updateModelMatrix()
{
	glm::vec3 posVec = parametersContainer.getVec3(pos);
	glm::quat rotation = getRotation();

	modelMatrix = glm::translate(getPos()) * glm::toMat4(rotation) * glm::scale(getScale());
}

void tetraRender::GameObject::copyChildren()
{
	//In this function we make a deep copy of all the childrenand replace them by the copies. There might be a better way to do it.
	//Because there is only one owner of the children, so in theory this case shouldn't even happen and it can create memory leaks if you copy the children without havin
	//a game object that keeps a reference to the real children.
	std::vector<GameObject*> newChildren;

	for (GameObject* currentChild : this->getChildren())
	{
		newChildren.push_back(currentChild->getDeepCopy());
	}
	this->children.clear();

	for (GameObject* newChild : newChildren)
	{
		addChild(newChild);
	}
}
