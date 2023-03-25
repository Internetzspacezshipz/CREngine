#pragma once

#include "CRE_ManagedObject.hpp"

//std incl
#include <vector>
#include <memory>

class VulkanEngine;
class CRE_UI_Base;

class CRE_App
{
public:

	CRE_App();
	~CRE_App();

	CRE_App(const CRE_App&) = delete;
	CRE_App& operator= (const CRE_App&) = delete;

	void SetupGlobalVariables(VulkanEngine* InEnginePointer);

	SP<CRE_AssetList> GetRootAssetList() const { return RootObject; };

	void DrawUIObjects();

	void LoadInitialGameFiles();
	void SaveGame();

private:
	void LoadGameObjects();

	//The root game object that loads all other game objects we need.
	SP<CRE_AssetList> RootObject;

	//All UI objects we want to draw.
	std::vector<CRE_UI_Base*> UIObjects;
};
