#pragma once

#include "CrManagedObject.hpp"
#include <vk_engine.h>

//A material which can be loaded into the GPU and used in the scene.
class CrShader : public CrManagedObject
{
	DEF_CLASS(CrShader, CrManagedObject);

	virtual ~CrShader();

	//virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;
	virtual void BinSerialize(CrArchive& Arch) override;

	bool LoadShader();
	void UnloadShader();

	VkShaderModule GetShader();

	virtual void Construct() override;
private:
	VkShaderModule ShaderData;
};


