#include "FPSPlayer.h"
#include "Prop.h"
#include "XMLImporter.h"
#include "Light.h"
#include <vector>

#ifndef LEVELLOADER_H
#define LEVELLOADER_H

//class declared
class Level;


class LevelLoader
{
public:
	LevelLoader(void);
	~LevelLoader(void);

	void createNewProp(XMLImporter * xml, std::vector<Prop*>& props);
	void createNewPlayer(XMLImporter * xml, const FPSPlayer* player);
	void createNewLight(XMLImporter * xml, std::vector<Light*>& lights);
	void createNewCamera(XMLImporter * xml, std::vector<Camera*>& cameras);
};

#endif

