#pragma once

#include "CRE_ManagedObject.hpp"
#include "CRE_Math.hpp"
#include "vk_engine.h"

//A 2d object that is renderable in the scene used for UI mainly.
class CRE_Renderable : public CRE_ManagedObject
{
	DEF_CLASS(CRE_Renderable, CRE_ManagedObject);

	virtual ~CRE_Renderable()
	{}

	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;

	void SetRenderingEnabled(bool bRenderingEnabled);

protected:
	bool bHasAddedRenderable = false;
	//Actual render data. Shared pointer here so that when this object is destroyed, the engine will remove its weak pointers.
	RO_sp RenderableObject;
};


