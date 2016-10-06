#include "LevelLoader.h"


LevelLoader::LevelLoader(void)
{
}

LevelLoader::~LevelLoader(void)
{
}

void LevelLoader::createNewPlayer(XMLImporter* xml, const FPSPlayer* player)
{
	Log::Inform("Begin Parsing of FPSPlayer from XML", "LevelLoader", this);

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
	}

	try
	{
		player = new FPSPlayer(aabbcollider, camera, movement);
	}
	catch(std::bad_alloc())
	{
		throw(&std::bad_alloc());
		Log::Error("Creation of Player failed", "LevelLoader", this);
		return;
	}
}

void LevelLoader::createNewProp(XMLImporter* xml, std::vector<Prop*>& props)
{
	//iterate over elements and create prop objects accordingly
	for(xmlnode * n = xml->getRoot()->first_node(PROP); n; n = n->next_sibling())
	{
		Log::Inform("Begin Parsing of Prop from XML", "LevelLoader", this);

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
		}
		//end prop import

		//create the new prop
		prop = Prop(mesh, texture, movement, aabbcollider, material);

		try
		{
			props.push_back( new Prop(prop) );
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

void LevelLoader::createNewLight(XMLImporter* xml, std::vector<Light*>& lights)
{
}

void LevelLoader::createNewCamera(XMLImporter* xml, std::vector<Camera*>& cameras)
{
}