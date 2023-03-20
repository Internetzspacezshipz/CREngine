#pragma once

#include "CRE_ManagedObject.hpp"

//std incl
#include <vector>
#include <memory>

class VulkanEngine;

class CRE_App
{
public:

	CRE_App();
	~CRE_App();

	CRE_App(const CRE_App&) = delete;
	CRE_App& operator= (const CRE_App&) = delete;

	void SetupEnginePointer(VulkanEngine* InEnginePointer);

	void LoadInitialGameFiles();
	void SaveGame();

private:
	void LoadGameObjects();

	//The root game object that loads all other game objects we need.
	class CRE_AssetList* RootObject;
};
