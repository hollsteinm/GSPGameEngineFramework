#include "GameObject.h"

GameObject::GameObject(void)
{
	mName = "UnNamed";
}

GameObject::GameObject(std::string name)
{
	mName = name;
}

GameObject::~GameObject(void)
{
}

std::string GameObject::getName()
{
	return mName;
}

const std::string GameObject::getName() const
{
	return mName;
}

//Unique Instance Identification getters
unsigned int GameObject::getUIID()
{
	return mUIID;
}

const unsigned int GameObject::getUIID() const
{
	return mUIID;
}

std::string GameObject::getUIIDString()
{
	std::string asString;
	char buffer[128];
	asString = itoa(mUIID, buffer, 10);
	asString = buffer;
	return asString;
}

const std::string GameObject::getUIDString() const
{
	std::string asString;
	char buffer[128];
	asString = itoa(mUIID, buffer, 10);
	asString = buffer;
	return asString;
}

//Object Identification getters
unsigned int GameObject::getOID()
{
	return mOID;
}

const unsigned int GameObject::getOID() const
{
	return mOID;
}

std::string GameObject::getOIDString()
{
	std::string asString;
	char buffer[128];
	asString = itoa(mOID, buffer, 10);
	asString = buffer;
	return asString;
}

const std::string GameObject::getOIDString() const
{
	std::string asString;
	char buffer[128];
	asString = itoa(mOID, buffer, 10);
	asString = buffer;
	return asString;
}