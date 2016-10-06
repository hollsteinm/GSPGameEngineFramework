#include "../rapidxml-1.13/rapidxml.hpp"
#include "../rapidxml-1.13/rapidxml_utils.hpp"

//The current xml parser used is rapidxml (http://rapidxml.sourceforge.net/)

#ifndef XMLIMPORTER_H
#define XMLIMPORTER_H

//define all attributes related to engine
/////////////////////////////////////////
//elements
#define PROP			"Prop"
#define FPSPLAYER		"Fpsplayer"
#define GAMEOBJECT		"Gameobject"
#define MOVEMENT		"Movement"
#define STATE			"State"
#define CONTROLLER		"Controller"
#define AABBCOLLIDER	"Aabbcollider"
#define SPHERECOLLIDER	"Spherecollider"
#define PHYSICS			"Physics"
#define CAMERA			"Camera"
#define LIGHT			"Light"
#define MATERIAL		"Material"
#define TEXTURE			"Texture"
#define MESH			"Mesh"
#define POSITION		"Position"
#define ROTATION		"Rotation"
#define SCALE			"Scale"
#define LEVEL			"Level"
#define VIEW			"View"
#define UP				"Up"
#define HEIGHTMAP		"Heightmap"
#define TERRAIN			"Terrain"
#define X				"X"
#define Y				"Z"
#define Z				"Y"
#define W				"X"

//attributes
#define NAME				"name"
#define TEXTURELOCATION		"texturelocation"
#define NORMALMAPLOCATION	"normallocation"
#define SPECULARLOCATION	"specularlocation"
#define MESHLOCATION		"meshlocation"
#define MESHTYPE			"meshtype"
#define MASS				"mass"
#define	ACCELERATION		"acceleration"
#define VELOCITY			"velocity"
#define DIFFUSE				"diffuse"
#define SPECULAR			"specular"
#define SPOTPOWER			"spotpower"
#define SPECULARPOWER		"specularpower"
#define RANGE				"range"
#define ATTENTUATION		"attentuation"
#define AMBIENT				"ambient"
#define HEIGHTMAPLOCATION	"heightmaplocation"
#define MAPWIDTH			"width"
#define MAPHEIGHT			"height"
#define HEIGHTSCALE			"heightscale"
#define HEIGHTOFFSET		"heightoffset"
#define TYPE				"type"
#define BITS				"bits"
#define BLEND0				"blend0"
#define BLEND1				"blend1"
#define BLEND2				"blend2"
#define BLENDMASTER			"blendmaster"
#define DIRECTIONALLIGHT	"directional"
#define SPOTLIGHT			"spot"
#define POINTLIGHT			"point"

//mesh types
#define OBJ		"obj"
#define DAE		"dae"
///////////////////////////////////////////

//typedefs for simple transition so we are not embedding rapidxml into our system
typedef rapidxml::xml_node<char> xmlnode;
typedef rapidxml::xml_document<char> xmldoc;

class XMLImporter
{
public:
	XMLImporter(void);
	~XMLImporter(void);

	void openXMLFile(const char* filename);
	void Shutdown();

	void setRootNode(std::string node);

	//returns a node within set root node
	xmlnode* getNode(std::string node);
	xmlnode* getRoot();
	const char* getValueOfNode(std::string value) const;

private:
	xmldoc mXMLDocument;
	xmlnode * mRootNode;
	xmlnode * mNode;

	//This will retain the current xmldocuments data so it does not fall out of scope
	//as the intrinsic nature of the current API is to store pointers to the document.
	//What this means we load it into system memory in a buffer from the file.
	std::ifstream mXMLFile;
	std::vector<char> buffer;
};

#endif

