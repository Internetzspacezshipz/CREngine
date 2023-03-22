#pragma once

#include "CRE_ManagedObject.hpp"
#include "CRE_Math.hpp"

//A 2d object that is renderable in the scene used for UI mainly.
class CRE_2DRenderable : public CRE_ManagedObject
{
	DEF_CLASS(CRE_2DRenderable, CRE_ManagedObject);

	virtual ~CRE_2DRenderable()
	{}

	//std::shared_ptr<CRE_Mesh> MeshObject;
	CRE_TransformBox2D Transform;

	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;
};


