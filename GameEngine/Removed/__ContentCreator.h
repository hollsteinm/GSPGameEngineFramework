#include <string.h>
#include <ostream>
#include <fstream>
#include <istream>
#include <iostream>
#include <sstream>
#include "Level.h"

#ifndef CONTENTCREATOR_H
#define CONTENTCREATOR_H

/*
Content files will have the following syntax:
# - indicates a level identifier, starting at zero, unsigned int

identifiers:
prop
fpsplayer

identifier data:
movement,
state,
controller,
aabbcollider,
spherecollider,
camera,
light,
material,
mesh,
texture,
gameobject

level identifiers:
level,
shader

all identifiers will be lower case only and case sensitive.
identifiers are objects derived from gameobject

identifiers will contain data for thier constructors and be stored in memory
*/

enum CONTENTKEYS
{
	PROP = 0,
	FPSPLAYER,
	LEVEL,
	MOVEMENT,
	STATE,
	CONTROLLER,
	AABBCOLLIDER,
	SPHERECOLLIDER,
	CAMERA,
	LIGHT,
	MATERIAL,
	MESH,
	SHADER,
	TEXTURE,
	GAMEOBJECT,
};

//data structure to tell parser what the object is, the start location in stream
//and the level it belongs in.
struct ParsedData
{
	CONTENTKEYS key;
	unsigned int streamLocation;
	unsigned int owningLevelID;
};

class ContentCreator
{
public:
	ContentCreator(void);
	~ContentCreator(void);

	//each game requires one level
	void loadContentFileToStream(std::string filename);
	void loadContentCacheToData();
	void loadLevelDataFromContent(Level* level);

	void Shutdown();

private:
	bool OpenContentFile(std::string filename);
	bool CloseContentFile();

	bool CreateCacheStream();
	bool CloseCacheStream();

	void parseFPSPlayer();
	void parseProp();

	//TODO: add all other gameobjects to be parsed

	//parsing data
	ParsedData* mData;
	unsigned int numContentKeys;
	unsigned int numContentLines;

	//internal data
	std::ifstream mFileStream;
	std::string* mCacheStream;
};

#endif

