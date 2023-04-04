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

	//Imports the shader code from ImportPath
	bool Import();

	bool LoadShader();
	void UnloadShader();

	VkShaderModule GetShader();

	//Path we have imported from.
	Path ImportPath;

	//Push constant size in bytes.
	int32_t PushConstantSize = 0;

	virtual void Construct() override;
private:

	VkShaderModule ShaderData;
	BinArray ShaderCode;
};


