#include "HeightMap.h"
#include "Mesh.h"
#include "Movement.h"
#include "Texture.h"
#include "GameObject.h"

#ifndef TERRAIN_H
#define TERRAIN_H

enum TERRAINTYPE
{
	MESHED = 0,
	MAPPED,
};

class Terrain : public GameObject
{
public:
	Terrain(void);
	Terrain(HeightMap heightmap, Texture texture, Movement movement);
	Terrain(Texture texture, Mesh mesh, Movement movement);

	~Terrain(void);

	void Update(float dt);
	void Render();
	void Load();
	void Release();

	const Mesh* getMesh() const;
	const Texture* getTexture() const;
	const Movement* getMovement() const;

	Mesh* getMesh();
	Texture* getTexture();
	Movement* getMovement();

private:
	//Name: createGrid()
	//Purpose: if Terrain is of Type mapped, creates the terrian based on heightmap
	//Return: none
	void createGrid();

	HeightMap* mHeightMap;
	Mesh* mMesh;
	Movement* mMovement;
	Texture* mTexture;

	TERRAINTYPE mType;
};

#endif

