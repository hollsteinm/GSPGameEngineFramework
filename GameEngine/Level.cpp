#include "Level.h"
#include "XMLImporter.h"

unsigned int Level::mNumLevels = 0;

Level::Level(void)
{
	mPlayer = 0;
	
	mLevelID = mNumLevels;
	mNumLevels++;
}

Level::~Level(void)
{
	mNumLevels--;
}

void Level::Load(std::string levelDataFiles)
{
	LoadDataFiles(levelDataFiles);

	//if no player was defined in the level creation files, create a defualt player
	if(mPlayer == 0)
		mPlayer = new FPSPlayer();

	mPlayer->Load();
	for(unsigned int i = 0; i < mLevelProps.size(); ++i)
	{
		mLevelProps[i]->Load();
	}
	for(unsigned int i = 0; i < mLevelTerrains.size(); ++i)
	{
		mLevelTerrains[i]->Load();
		//terrain does not move but must be updated
		mLevelTerrains[i]->Update(1.0f);
	}
}

void Level::Release()
{
	if(mPlayer)
		delete mPlayer;

	for(unsigned int i = 0; i < mLevelProps.size(); ++i)
	{
		mLevelProps[i]->Release();
		if(mLevelProps[i])
			delete mLevelProps[i];
	}
	mLevelProps.empty();

	for(unsigned int i = 0; i < mLevelLights.size(); ++i)
	{
		if(mLevelLights[i])
			delete mLevelLights[i];
	}
	mLevelLights.empty();

	for(unsigned int i = 0; i < mLevelTerrains.size(); ++i)
	{
		if(mLevelTerrains[i])
		{
			mLevelTerrains[i]->Release();
			delete mLevelTerrains[i];
		}
	}
	mLevelTerrains.empty();

	for(unsigned int i = 0; i < mLevelCameras.size(); ++i)
	{
		if(mLevelCameras[i])
			delete mLevelCameras[i];
	}
	mLevelCameras.empty();
}

void Level::Update(float dt)
{
	mPlayer->Update(dt);
	for(unsigned int i = 0; i < mLevelProps.size(); ++i)
	{
		mLevelProps[i]->Update(dt);
	}
}

void Level::Render()
{
	//TODO: consider using shader flags and render only in world
	//TODO: render everything in the level with shaders
}

void Level::setPlayer(FPSPlayer* newPlayer)
{
	assert(newPlayer != 0);
	mPlayer = newPlayer;
}

const FPSPlayer* Level::getPlayer() const
{
	assert(mPlayer != 0);
	return mPlayer;
}

const FPSPlayer* Level::getPlayer()
{
	assert(mPlayer != 0);
	return mPlayer;
}

const std::vector<Terrain*>* Level::getLevelTerrains() const
{
	return &mLevelTerrains;
}

const std::vector<Prop*>* Level::getLevelProps() const
{
	return &mLevelProps;
}

const std::vector<Light*>* Level::getLevelLights() const
{
	return &mLevelLights;
}

void Level::LoadDataFiles(std::string levelXMLfile)
{
	XMLImporter xml;
	xml.openXMLFile(levelXMLfile.c_str());
	xml.setRootNode(LEVEL);

	if(xml.getRoot()->first_node(PROP) != 0)
		createNewProp(&xml);

	if(xml.getRoot()->first_node(FPSPLAYER) != 0)
		createNewPlayer(&xml);

	if(xml.getRoot()->first_node(TERRAIN) != 0)
		createNewTerrain(&xml);

	if(xml.getRoot()->first_node(LIGHT) != 0)
		createNewLight(&xml);

	//close after all parsing is complete
	xml.Shutdown();
}

//helpers to parse low level (base classes, no inheritance, is superclass) data
void parseMovement(xmlnode* node){}
void parseCamera(xmlnode* node){}
void parseAABBCollider(xmlnode* node){}
void parseTexture(xmlnode* node){}
void parseMesh(xmlnode* node){}
void parseMaterial(xmlnode* node){}

void Level::createNewPlayer(XMLImporter* xml)
{
	Log::Inform("Begin Parsing of FPSPlayer from XML", "Level", this);

	//needed data
	Camera camera;
	Movement movement;
	AABBCollider aabbcollider;

	//only one player data allowed locally, so no need to iterate
	xmlnode * n = xml->getRoot()->first_node(FPSPLAYER);

	xmlnode * node = n->first_node(CAMERA);
	if(node != 0)
	{
		camera = Camera(
			XMFLOAT3(
			(float) atof(node->first_node(VIEW)->first_node(X)->value()),
			(float) atof(node->first_node(VIEW)->first_node(Y)->value()),
			(float) atof(node->first_node(VIEW)->first_node(Z)->value())),

			XMFLOAT3(
			(float) atof(node->first_node(UP)->first_node(X)->value()),
			(float) atof(node->first_node(UP)->first_node(Y)->value()),
			(float) atof(node->first_node(UP)->first_node(Z)->value()))
		);
	}

	node = n->first_node(MOVEMENT);
	if(node != 0)
	{
		XMFLOAT3 position;
		XMFLOAT3 rotation;
		XMFLOAT3 scale;
		
		if(node->first_node(POSITION) != 0)
		{
			position = XMFLOAT3(
				(float) atof(node->first_node(POSITION)->first_node(X)->value()),
				(float) atof(node->first_node(POSITION)->first_node(Y)->value()),
				(float) atof(node->first_node(POSITION)->first_node(Z)->value()));
		}
		if(node->first_node(ROTATION) != 0)
		{
			rotation = XMFLOAT3(
				(float) atof(node->first_node(ROTATION)->first_node(X)->value()),
				(float) atof(node->first_node(ROTATION)->first_node(Y)->value()),
				(float) atof(node->first_node(ROTATION)->first_node(Z)->value()));
		}

		if(node->first_node(SCALE) != 0)
		{
			scale = XMFLOAT3(
				(float) atof(node->first_node(SCALE)->first_node(X)->value()),
				(float) atof(node->first_node(SCALE)->first_node(Y)->value()),
				(float) atof(node->first_node(SCALE)->first_node(Z)->value()));
		}
		movement = Movement(position, rotation, scale);
	}

	try
	{
		mPlayer = new FPSPlayer(aabbcollider, camera, movement);
	}
	catch(std::bad_alloc())
	{
		throw(&std::bad_alloc());
		Log::Error("Creation of Player failed", "Level", this);
		return;
	}
}

void Level::createNewProp(XMLImporter* xml)
{
	//iterate over elements and create prop objects accordingly
	for(xmlnode * n = xml->getRoot()->first_node(PROP); n; n = n->next_sibling())
	{
		Log::Inform("Begin Parsing of Prop from XML", "Level", this);

		//data needed
		Prop prop;
		Mesh mesh;
		Texture texture;
		Movement movement;
		AABBCollider aabbcollider;
		Material material;

		xmlnode * node = n->first_node(MESH);
		if(node != 0)
		{
			mesh = Mesh( node->first_attribute(MESHLOCATION)->value() );
		}

		node = n->first_node(TEXTURE);
		if(node != 0)
		{
			std::vector<std::string> textures;
			textures.empty();
			
			if(node->first_attribute(TEXTURELOCATION) != 0)
				textures.push_back(node->first_attribute(TEXTURELOCATION)->value());

			if(node->first_attribute(NORMALMAPLOCATION) != 0)
				textures.push_back(node->first_attribute(NORMALMAPLOCATION)->value());

			if(node->first_attribute(SPECULARLOCATION) != 0)
				textures.push_back(node->first_attribute(SPECULARLOCATION)->value());

			texture = Texture(textures);
		}

		node = n->first_node(MOVEMENT);
		if(node != 0)
		{
			XMFLOAT3 position;
			XMFLOAT3 rotation;
			XMFLOAT3 scale;

			if(node->first_node(POSITION) != 0)
			{
				position = XMFLOAT3(
					(float) atof(node->first_node(POSITION)->first_node(X)->value()),
					(float) atof(node->first_node(POSITION)->first_node(Y)->value()),
					(float) atof(node->first_node(POSITION)->first_node(Z)->value()));
			}

			if(node->first_node(ROTATION) != 0)
			{
				rotation = XMFLOAT3(
					(float) atof(node->first_node(ROTATION)->first_node(X)->value()),
					(float) atof(node->first_node(ROTATION)->first_node(Y)->value()),
					(float) atof(node->first_node(ROTATION)->first_node(Z)->value()));
			}

			if(node->first_node(SCALE) != 0)
			{
				scale = XMFLOAT3(
					(float) atof(node->first_node(SCALE)->first_node(X)->value()),
					(float) atof(node->first_node(SCALE)->first_node(Y)->value()),
					(float) atof(node->first_node(SCALE)->first_node(Z)->value()));
			}
			movement = Movement(position, rotation, scale);
		}
		//end prop import

		//create the new prop
		prop = Prop(mesh, texture, movement, aabbcollider, material);

		try
		{
			mLevelProps.push_back( new Prop(prop) );
		}
		catch(std::bad_alloc())
		{
			throw(&std::bad_alloc());
			Log::Error("Creation failed", "Level", this);
			return;
		}

		Log::Inform("Parsing and creation complete.", "Level", this);
	}
}

void Level::createNewLight(XMLImporter * xml)
{
	for(xmlnode * n = xml->getRoot()->first_node(LIGHT); n; n = n->next_sibling())
	{
		Log::Inform("Begin parsing of Light", "Level", this);

		//data needed
		XMFLOAT4 acolor, dcolor, scolor;
		acolor = dcolor = scolor = WHITE;

		XMFLOAT3 att = XMFLOAT3(0.0f, 0.0f, 0.0f);

		float spotpower, range;
		spotpower = range = 1.0f;

		light::lighttype type = light::DIRECTIONAL;
		
		Light light;

		if(n->first_attribute(TYPE) != 0)
		{
			std::string typestring(n->first_attribute(TYPE)->value());

			if(typestring == DIRECTIONALLIGHT)
			{
				type = light::DIRECTIONAL;
			}

			else if(typestring == SPOTLIGHT)
			{
				type = light::SPOT;
			}

			else if(typestring == POINTLIGHT)
			{
				type = light::POINT;
			}

			else if(typestring == AMBIENT)
			{
				type = light::AMBIENTLIGHT;
			}

			else
			{
				type = light::DIRECTIONAL;
			}
	
		}

		xmlnode * node = n->first_node(AMBIENT);
		if(node != 0)
		{
			acolor = XMFLOAT4(
				atof(node->first_attribute(X)->value()),
				atof(node->first_attribute(Y)->value()),
				atof(node->first_attribute(Z)->value()),
				atof(node->first_attribute(W)->value()));
		}

		node = n->first_node(DIFFUSE);
		if(node != 0)
		{
			dcolor = XMFLOAT4(
				atof(node->first_attribute(X)->value()),
				atof(node->first_attribute(Y)->value()),
				atof(node->first_attribute(Z)->value()),
				atof(node->first_attribute(W)->value()));
		}

		node = n->first_node(SPECULAR);
		if(node != 0)
		{
			scolor = XMFLOAT4(
				atof(node->first_attribute(X)->value()),
				atof(node->first_attribute(Y)->value()),
				atof(node->first_attribute(Z)->value()),
				atof(node->first_attribute(W)->value()));
		}

		node = n->first_node(ATTENTUATION);
		if(node != 0)
		{
			att = XMFLOAT3(
				atof(node->first_attribute(X)->value()),
				atof(node->first_attribute(Y)->value()),
				atof(node->first_attribute(Z)->value()));
		}

		node = n->first_node(SPOTPOWER);
		if(node != 0)
		{
			spotpower = atof(node->value());
		}

		node = n->first_node(RANGE);
		if(node != 0)
		{
			range = atof(node->value());
		}

		light = Light(type, acolor, dcolor, scolor, att, spotpower, range);

		try
		{
			mLevelLights.push_back(new Light(light));
		}
		catch(std::bad_alloc())
		{
			Log::Error("Could not create level light", "Level", this);
			throw(&std::bad_alloc());
		}

		Log::Inform("Light successfully parsed from XML", "Level", this);
	}
}

void Level::createNewTerrain(XMLImporter * xml)
{
	for(xmlnode * n = xml->getRoot()->first_node(TERRAIN); n; n = n->next_sibling())
	{
		bool fromFile = false;
		Terrain terrain;
		Mesh mesh;
		Movement movement;
		Texture texture;
		HeightMap heightmap;
		Log::Inform("Begin Parsing of Terrain from XML", "Level", this);

		xmlnode * node = n->first_node(MESH);
		if(node != 0)
		{
			mesh = Mesh( node->first_attribute(MESHLOCATION)->value() );
			fromFile = false;
		}

		node = n->first_node(HEIGHTMAP);
		if(node != 0)
		{
			unsigned int i = 0;
			unsigned int j = 0;
			std::string filename;
			float heightScale = 1.0f;
			float heightOffset = 0.0f;
			RAWBITSIZE size = EIGHTBIT;			

			if(node->first_attribute(HEIGHTMAPLOCATION) != 0)
			{
				filename = node->first_attribute(HEIGHTMAPLOCATION)->value();
				fromFile = true;
			}

			if(node->first_attribute(MAPWIDTH) != 0)
				i = atoi(node->first_attribute(MAPWIDTH)->value());

			if(node->first_attribute(MAPHEIGHT) != 0)
				j = atoi(node->first_attribute(MAPHEIGHT)->value());

			if(node->first_attribute(HEIGHTSCALE) != 0)
				heightScale = (float)atol(node->first_attribute(HEIGHTSCALE)->value());

			if(node->first_attribute(HEIGHTOFFSET) != 0)
				heightOffset = (float)atol(node->first_attribute(HEIGHTOFFSET)->value());

			if(node->first_attribute(BITS) != 0)
				size = (RAWBITSIZE)atoi(node->first_attribute(BITS)->value());

			if(fromFile)
				heightmap = HeightMap(i, j, filename, heightScale, heightOffset, size);
			else
				heightmap = HeightMap(i, j);
		}

		node = n->first_node(TEXTURE);
		if(node != 0)
		{
			std::vector<std::string> textures;
			textures.empty();
			
			if(node->first_attribute(TEXTURELOCATION) != 0)
				textures.push_back(node->first_attribute(TEXTURELOCATION)->value());

			//if(node->first_attribute(NORMALMAPLOCATION) != 0)
				//textures.push_back(node->first_attribute(NORMALMAPLOCATION)->value());

			//if(node->first_attribute(SPECULARLOCATION) != 0)
				//textures.push_back(node->first_attribute(SPECULARLOCATION)->value());
			
			if(node->first_attribute(BLEND0) != 0)
				textures.push_back(node->first_attribute(BLEND0)->value());

			if(node->first_attribute(BLEND1) != 0)
				textures.push_back(node->first_attribute(BLEND1)->value());

			if(node->first_attribute(BLEND2) != 0)
				textures.push_back(node->first_attribute(BLEND2)->value());

			if(node->first_attribute(BLENDMASTER) != 0)
				textures.push_back(node->first_attribute(BLENDMASTER)->value());

			texture = Texture(textures);
		}

		node = n->first_node(MOVEMENT);
		if(node != 0)
		{
			XMFLOAT3 position;
			XMFLOAT3 rotation;
			XMFLOAT3 scale;

			if(node->first_node(POSITION) != 0)
			{
				position = XMFLOAT3(
					(float) atof(node->first_node(POSITION)->first_node(X)->value()),
					(float) atof(node->first_node(POSITION)->first_node(Y)->value()),
					(float) atof(node->first_node(POSITION)->first_node(Z)->value()));
			}

			if(node->first_node(ROTATION) != 0)
			{
				rotation = XMFLOAT3(
					(float) atof(node->first_node(ROTATION)->first_node(X)->value()),
					(float) atof(node->first_node(ROTATION)->first_node(Y)->value()),
					(float) atof(node->first_node(ROTATION)->first_node(Z)->value()));
			}

			if(node->first_node(SCALE) != 0)
			{
				scale = XMFLOAT3(
					(float) atof(node->first_node(SCALE)->first_node(X)->value()),
					(float) atof(node->first_node(SCALE)->first_node(Y)->value()),
					(float) atof(node->first_node(SCALE)->first_node(Z)->value()));
			}
			movement = Movement(position, rotation, scale);
		}
		//end terrain import
		if(fromFile)
			terrain = Terrain(heightmap, texture, movement);
		else
			terrain = Terrain(texture, mesh, movement);

		//create the new terrain
		try
		{
			mLevelTerrains.push_back( new Terrain(terrain) );
		}
		catch(std::bad_alloc())
		{
			throw(&std::bad_alloc());
			Log::Error("Creation failed", "Level", this);
			return;
		}

		Log::Inform("Parsing and creation complete.", "Level", this);
	}
}

void createNewCamera(XMLImporter * xml)
{
}