#include "Mesh.h"
#include "Texture.h"
#include "Movement.h"
#include "GameObject.h"
#include "DX10Util.h"
#include "Collision.h"
#include "Material.h"

#ifndef PROP_H
#define PROP_H

class Prop : public Mesh, public Texture, public Movement, public GameObject, public AABBCollider, public Material
{
public:
	Prop(void);
	
	Prop(
		Mesh mesh,
		Texture texture,
		Movement movement,
		AABBCollider collider,
		Material material);

	~Prop(void);

	void Load();
	void Update(float dt);
	void Render();
	void Release();

private:
	ID3D10Device* pD3DDevice;
};

#endif

