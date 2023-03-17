#pragma once

#include "CRE_ManagedObject.hpp"

class CRE_RenderableObject : public CRE_ManagedObject
{
	DEF_CLASS(CRE_RenderableObject, CRE_ManagedObject);

	CRE_RenderableObject(const ObjGUID& InObjGUID) : Super(InObjGUID)
	{}

	virtual ~CRE_RenderableObject()
	{}

	std::shared_ptr<CRE_Mesh> MeshObject;
	CRE_Transform Transform{};

	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;
};

REGISTER_CLASS(CRE_RenderableObject, CRE_ManagedObject);

