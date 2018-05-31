#include "ParameterContainer.h"
#include "Common.h"

using namespace tetraRender;

ParameterContainer::ParameterContainer()
{
}


ParameterContainer::~ParameterContainer()
{
}

void tetraRender::ParameterContainer::set(std::string valName, glm::vec3 val)
{
	bool paramExists = checkParameterExistance(valName, ParameterType::VEC3);
	//if the parameter doesn't exist we add it to the list. If it already exist no need to change the list but we edit it's value.
	if (paramExists == false)
	{
		parameters.push_back(std::pair<std::string, tetraRender::ParameterType>(valName, ParameterType::VEC3));
	}
	vectors[valName] = val;
}

void tetraRender::ParameterContainer::set(std::string valName, std::string val)
{
	if (!checkParameterExistance(valName, ParameterType::STRING))
	{
		parameters.push_back(parameter(valName, ParameterType::STRING));
	}
	strings[valName] = val;
}

glm::vec3 tetraRender::ParameterContainer::getVec3(std::string valname)
{
	glm::vec3 val = glm::vec3(0);
	if (vectors.find(valname) != vectors.end())
	{
		val = vectors[valname];
	}
	return val;
}

std::string tetraRender::ParameterContainer::getString(std::string valName)
{
	std::string val = "";
	if (strings.find(valName) != strings.end())
	{
		val = strings[valName];
	}
	return val;
}

std::vector<parameter> tetraRender::ParameterContainer::getParameters()
{
	return parameters;
}

bool tetraRender::ParameterContainer::checkParameterExistance(std::string paramName, ParameterType type)
{
	bool paramExists = false;
	for (auto param : parameters)
	{
		//It means that this parameter already exists, so no need to add it to the list.
		if (param == std::pair<std::string, tetraRender::ParameterType>(paramName, type))
		{
			paramExists = true;
		}
	}

	return paramExists;
}


