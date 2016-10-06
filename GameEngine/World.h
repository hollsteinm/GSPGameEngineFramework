#include "DefaultShader.h"
#include "TerrainShader.h"
#include "Level.h"

#ifndef WORLD_H
#define WORLD_H

class World
{
public:
	World();
	~World(void);

	void Load();
	void Run(float dt);
	void OnExit();

private:
	void Update(float dt);
	void Render();
	void LoadDataFiles();

	DefaultShader mDiffuseShader;
	TerrainShader mTerrainShader;

	//List of levels in game
	std::vector<Level*> mWorldLevels;
	Level* mCurrentLevel;

	Level testLevel;
};

#endif

