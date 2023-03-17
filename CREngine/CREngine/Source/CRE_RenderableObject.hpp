#pragma once

#include "CRE_ManagedObject.hpp"

class CRE_RenderableObject : public CRE_ManagedObject
{
	DEF_CLASS(CRE_ManagedObject, CRE_RenderableObject);

	CRE_RenderableObject(const ObjGUID& InObjGUID) : Super(InObjGUID)
	{}

	virtual ~CRE_RenderableObject()
	{}

	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;



	int SomeRandomNumber = 4242;
	int SomeOtherNumber = 5151;
	bool Something = true;
};

REGISTER_CLASS(CRE_ManagedObject, CRE_RenderableObject);