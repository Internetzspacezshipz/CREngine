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

	void AddUI(CRE_ID Name, SP<CRE_UI_Base> NewUI);
	void RemoveUI(CRE_ID Name);

private:
	void LoadGameObjects();

	//The root game object that loads all other game objects we need.
	SP<CRE_AssetList> RootObject;

	//All UI objects we want to draw.
	Map<CRE_ID, SP<CRE_UI_Base>> UIObjects;
};
