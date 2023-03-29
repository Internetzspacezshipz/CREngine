#include "CRE_Shader.h"
#include "CRE_Globals.h"

REGISTER_CLASS(CRE_Shader);

CRE_Shader::~CRE_Shader()
{
	UnloadShader();
}

void CRE_Shader::Serialize(bool bSerializing, nlohmann::json& TargetJson)
{
	Super::Serialize(bSerializing, TargetJson);

	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, File);

	if (!bSerializing)
	{
		LoadShader();
	}
}

bool CRE_Shader::LoadShader()
{
	auto FileStr = File.generic_string();
	if (FileStr.size())
	{
		VulkanEngine* Engine = CRE_Globals::GetEnginePointer();
		return Engine->LoadShaderModule(FileStr.c_str(), ShaderData);
	}
	return false;
}

void CRE_Shader::UnloadShader()
{
	if (ShaderData)
	{
		VulkanEngine* Engine = CRE_Globals::GetEnginePointer();
		Engine->UnloadShaderModule(ShaderData);
	}
}

VkShaderModule CRE_Shader::GetShader()
{
	if (!ShaderData)
	{
		LoadShader();
	}
	return ShaderData;
}
