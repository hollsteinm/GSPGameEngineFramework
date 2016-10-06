#include "Prop.h"

Prop::Prop(void)
{
}

Prop::Prop(
		Mesh mesh,
		Texture texture,
		Movement movement,
		AABBCollider collider,
		Material material)
		: Mesh(mesh),
		Texture(texture),
		Movement(movement),
		AABBCollider(collider),
		Material(material)
{
}

Prop::~Prop(void)
{
}

void Prop::Load()
{
	//test
	setSpecularPower(0.75f);
	setSpecularColor(WHITE);

	loadTextureFromFile();
	loadMeshFromOBJ();
	createMesh();
	computeExtentsFromPoints(getVertices(), getNumVertices());
}

void Prop::Update(float dt)
{
	setCenter(getPosition());
	transformToWorld();
}

void Prop::Render()
{
	RenderMeshes();
}

void Prop::Release()
{
	ReleaseMesh();
}
