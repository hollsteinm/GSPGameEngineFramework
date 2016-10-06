#include <string>

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

class GameObject
{
public:
	GameObject(void);
	GameObject(std::string name);
	virtual ~GameObject(void);

	virtual void Load() = 0;
	virtual void Update(float dt) = 0;
	virtual void Render() = 0;
	virtual void Release() = 0;

	std::string getName();
	const std::string getName() const;

	//Unique Instance Identification getters
	unsigned int getUIID();
	const unsigned int getUIID() const;

	std::string getUIIDString();
	const std::string getUIDString() const;

	//Object Identification getters
	unsigned int getOID();
	const unsigned int getOID() const;

	std::string getOIDString();
	const std::string getOIDString() const;

private:
	std::string mName;
	unsigned int mUIID;
	unsigned int mOID;
};

#endif
