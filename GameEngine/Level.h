#include "DefaultShader.h"
#include "FPSPlayer.h"
#include "Prop.h"
#include "XMLImporter.h"
#include "Light.h"
#include "Terrain.h"

#ifndef LEVEL_H
#define LEVEL_H

class Level
{
public:
	Level(void);
	
	~Level(void);

	void Load(std::string levelDataFile);
	void Update(float dt);
	void Render();
	void Release();

	void setPlayer(FPSPlayer* newPlayer);

	const FPSPlayer* getPlayer() const;
	const FPSPlayer* getPlayer();

	//getters for all of the data
	const std::vector<Prop*>* getLevelProps() const;
	const std::vector<Terrain*>* getLevelTerrains() const;
	const std::vector<Light*>* getLevelLights() const;

private:
	//private methods
	void LoadDataFiles(std::string levelXMLfile);

	//helpers for data file parsing
	void createNewProp(XMLImporter * xml);
	void createNewPlayer(XMLImporter * xml);
	void createNewLight(XMLImporter * xml);
	void createNewTerrain(XMLImporter * xml);
	void createNewCamera(XMLImporter * xml);

	//all of the levels contents for the local system
	//there is only ever one player, so choose one
	FPSPlayer* mPlayer;

	std::vector<Prop*> mLevelProps;
	std::vector<Camera*> mLevelCameras;
	std::vector<Light*> mLevelLights;
	std::vector<Terrain*> mLevelTerrains;
	//TODO: add gameobjects as they are created in the program
	//TODO: this would include zones, physicsWorld, collisionWorld, decorations, NPCs

	//level specific values
	unsigned int mLevelID;
	static unsigned int mNumLevels;
};

#endif

