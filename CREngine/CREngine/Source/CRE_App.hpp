#pragma once

#include "CRE_Window.hpp"
#include "CRE_Renderer.hpp"
#include "CRE_Device.hpp"
#include "CRE_ManagedObject.hpp"

//std incl
#include <vector>
#include <memory>

class CRE_App
{
public:
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;

	CRE_App();
	~CRE_App();

	CRE_App(const CRE_App&) = delete;
	CRE_App& operator= (const CRE_App&) = delete;

	void Run();

private:
	void LoadGameObjects();

	CRE_Window* Window = nullptr;
	CRE_Device* Device = nullptr;
	CRE_Renderer* Renderer = nullptr;

	//List of managed objects which we control. We should move this somewhere else soon. This is a shitty spot for it.
	std::vector<CRE_ManagedObject*> GameObjects;
};
