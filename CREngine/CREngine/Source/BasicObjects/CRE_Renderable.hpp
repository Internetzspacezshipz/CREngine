#pragma once

#include "CRE_ManagedObject.hpp"
#include "CRE_Math.hpp"

//A 2d object that is renderable in the scene used for UI mainly.
class CRE_Renderable : public CRE_ManagedObject
{
	DEF_CLASS(CRE_Renderable, CRE_ManagedObject);

	virtual ~CRE_Renderable()
	{}

	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;


};


