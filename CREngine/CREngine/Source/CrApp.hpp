#pragma once

#include "CrManagedObject.hpp"

//std incl
#include <vector>
#include <memory>

class VulkanEngine;
class CrUI_Base;

//Mainly handles user interface and initial game setup.
class CrApp
{
public:

	CrApp();
	~CrApp();

	CrApp(const CrApp&) = delete;
	CrApp& operator= (const CrApp&) = delete;

	void SetupGlobalVariables(VulkanEngine* InEnginePointer);

	void DrawUIObjects();

	void LoadInitialGameFiles();
	void SaveGame();

	//Makes a UI with a default name from a class ID and
	//passes it back to whoever called this function after adding it to the active UI list under the instance ID.
	SP<CrUI_Base> MakeUI(CrID Class);

	template<typename Type>
	SP<Type> MakeUI() { return DCast<Type>(MakeUI(Type::StaticClass())); }

	//Removes UI with an ID.
	void RemoveUI(CrID Name);
	//Removes UI by the pointer (can do by doing RemoveUI(this)). Really just uses the ID under the hood.
	void RemoveUI(CrUI_Base* ActualUI);

private:
	void LoadGameObjects();

	//Adds UI with a specified name - recommend using MakeUI instead if you can.
	void AddUI(CrID Name, SP<CrUI_Base> NewUI);

	//All UI objects we want to draw.
	Map<CrID, SP<CrUI_Base>> UIObjects;
};
