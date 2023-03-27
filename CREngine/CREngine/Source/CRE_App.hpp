#pragma once

#include "CRE_ManagedObject.hpp"

//std incl
#include <vector>
#include <memory>

class VulkanEngine;
class CRE_UI_Base;

//Mainly handles user interface and initial game setup.
class CRE_App
{
public:

	CRE_App();
	~CRE_App();

	CRE_App(const CRE_App&) = delete;
	CRE_App& operator= (const CRE_App&) = delete;

	void SetupGlobalVariables(VulkanEngine* InEnginePointer);

	void DrawUIObjects();

	void LoadInitialGameFiles();
	void SaveGame();

	//Makes a UI with a default name from a class ID and
	//passes it back to whoever called this function after adding it to the active UI list under the instance ID.
	SP<CRE_UI_Base> MakeUI(CRE_ID Class);

	template<typename Type>
	SP<Type> MakeUI() { return DCast<Type>(MakeUI(Type::StaticClass())); }

	//Removes UI with an ID.
	void RemoveUI(CRE_ID Name);
	//Removes UI by the pointer (can do by doing RemoveUI(this)). Really just uses the ID under the hood.
	void RemoveUI(CRE_UI_Base* ActualUI);

private:
	void LoadGameObjects();

	//Adds UI with a specified name - recommend using MakeUI instead if you can.
	void AddUI(CRE_ID Name, SP<CRE_UI_Base> NewUI);

	//All UI objects we want to draw.
	Map<CRE_ID, SP<CRE_UI_Base>> UIObjects;
};
