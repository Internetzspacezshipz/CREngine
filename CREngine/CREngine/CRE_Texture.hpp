#pragma once

#include "CRE_ManagedObject.hpp"
#include "vk_mesh.h" 

//In charge of loading textures from disk and managing them.
class CRE_Texture : public CRE_ManagedObject
{
	DEF_CLASS(CRE_Texture, CRE_ManagedObject);

	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;

	std::filesystem::path File;
};

REGISTER_CLASS(CRE_Texture, CRE_ManagedObject);
