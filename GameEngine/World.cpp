#include "World.h"

//TODO: make a method that traverses the file directory and collects all 
//of the level data to be parsed.
#define DEFAULT_LEVEL_FILE_LOCATION "../LevelData/level0.xml"

World::World()
{
	testLevel = Level();
}

World::~World(void)
{
}

void World::LoadDataFiles()
{
	//TODO: logic for loading files containing all game information from editor
	testLevel.Load(DEFAULT_LEVEL_FILE_LOCATION);

	mWorldLevels.push_back(&testLevel);

	mCurrentLevel = &testLevel;
}

void World::Load()
{
	LoadDataFiles();

	//not test
	if(!mDiffuseShader.Init(CDXManager::instance()->getHWND()))
		Log::Error("Could not Load game", "mDiffuseShader", &mDiffuseShader);

	if(!mTerrainShader.Init(CDXManager::instance()->getHWND()))
		Log::Error("Could not Load game", "mTerrainShader", &mTerrainShader);
}

void World::Update(float dt)
{
	mCurrentLevel->Update(dt);
}

void World::Render()
{
	CDXManager::instance()->BeginScene();

	//////////////////////////////////////
	//Terrain Shader
	//////////////////////////////////////

	//TODO: figure out how to input more lights than levels... not using for loops it seems
	mTerrainShader.setCameraPosition(mCurrentLevel->getPlayer()->getPosition());
	mTerrainShader.setShaderLight(XMFLOAT3(0.0f, -2.0f, 1.0f), mCurrentLevel->getLevelLights()->at(0)->getDiffuseColor());

	for(unsigned int i = 0; i < 1/*mCurrentLevel->getLevelTerrains()->size()*/; ++i)
	{	
		mCurrentLevel->getLevelTerrains()->at(i)->Render();

		mTerrainShader.setBlendTexture0(mCurrentLevel->getLevelTerrains()->at(i)->getTexture()->getTextureView(0));
		mTerrainShader.setBlendTexture1(mCurrentLevel->getLevelTerrains()->at(i)->getTexture()->getTextureView(1));
		mTerrainShader.setBlendTexture2(mCurrentLevel->getLevelTerrains()->at(i)->getTexture()->getTextureView(2));
		mTerrainShader.setBlendTextureMaster(mCurrentLevel->getLevelTerrains()->at(i)->getTexture()->getTextureView(3));

		mTerrainShader.setShaderMatrices(mCurrentLevel->getLevelTerrains()->at(i)->getMovement()->getWorld(), 
			mCurrentLevel->getPlayer()->getView(), 
			mCurrentLevel->getPlayer()->getProjection());

		mTerrainShader.RenderShader(mCurrentLevel->getLevelTerrains()->at(i)->getMesh()->getNumIndices());
	}
	

	//////////////////////////////////////
	//Diffuse Shader
	//////////////////////////////////////
	mDiffuseShader.setCameraPosition(mCurrentLevel->getPlayer()->getPosition());
	mDiffuseShader.setShaderLight(XMFLOAT3(0.0f, -2.0f, 1.0f), mCurrentLevel->getLevelLights()->at(0)->getDiffuseColor());
	
	for(unsigned int i = 0; i < mCurrentLevel->getLevelProps()->size(); ++i)
	{
		mCurrentLevel->getLevelProps()->at(i)->Render();

		mDiffuseShader.setDiffuseTexture(mCurrentLevel->getLevelProps()->at(i)->getTextureView(0));
		mDiffuseShader.setNormalTexture(mCurrentLevel->getLevelProps()->at(i)->getTextureView(1));
		mDiffuseShader.setSpecularTexture(mCurrentLevel->getLevelProps()->at(i)->getTextureView(2));

		mDiffuseShader.setShaderMatrices(mCurrentLevel->getLevelProps()->at(i)->getWorld(), 
			mCurrentLevel->getPlayer()->getView(), 
			mCurrentLevel->getPlayer()->getProjection());

		mDiffuseShader.setSpecularPower(mCurrentLevel->getLevelProps()->at(i)->getSpecularPower());
		mDiffuseShader.setSpecularColor(mCurrentLevel->getLevelProps()->at(i)->getSpecularColor());

		mDiffuseShader.RenderShader(mCurrentLevel->getLevelProps()->at(i)->getNumIndices());
	}
	//////////////////////////////////////
	CDXManager::instance()->EndScene();
}

void World::Run(float dt)
{
	Update(dt);
	Render();
}

void World::OnExit()
{
	for(unsigned int i = 0; i < mWorldLevels.size(); ++i)
		mWorldLevels[i]->Release();
}
