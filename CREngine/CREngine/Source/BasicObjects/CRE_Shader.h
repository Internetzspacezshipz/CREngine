#pragma once

#include "CRE_ManagedObject.hpp"
#include <vk_engine.h>

//A material which can be loaded into the GPU and used in the scene.
class CRE_Shader : public CRE_ManagedObject
{
	DEF_CLASS(CRE_Shader, CRE_ManagedObject);

	virtual ~CRE_Shader();

	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;

	bool LoadShader();
	void UnloadShader();

	VkShaderModule GetShader();

	Path File;
private:
	VkShaderModule ShaderData;
};


