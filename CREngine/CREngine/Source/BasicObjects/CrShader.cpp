#include "CrShader.h"
#include "CrGlobals.h"
#include "CrSerialization.hpp"

REGISTER_CLASS_FLAGS(CrShader, CrClassFlags_Unique, CrClassFlags_Transient, CrClassFlags_DataOnly);

REGISTER_EXTENSION(CrShader, ".spv");

CrShader::~CrShader()
{
	UnloadShader();
}

//void CrShader::Serialize(bool bSerializing, nlohmann::json& TargetJson)
//{
//	Super::Serialize(bSerializing, TargetJson);
//
//	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, File);
//
//	if (!bSerializing)
//	{
//		LoadShader();
//	}
//}

void CrShader::BinSerialize(CrArchive& Arch)
{
	if (!Arch.bSerializing)
	{
		assert(LoadShader());
	}
}

bool CrShader::LoadShader()
{
	auto FileStr = MakeAssetReference().GetString();
	if (FileStr.size())
	{
		VulkanEngine* Engine = CrGlobals::GetEnginePointer();
		return Engine->LoadShaderModule(FileStr.c_str(), ShaderData);
	}
	return false;
}

void CrShader::UnloadShader()
{
	if (ShaderData)
	{
		VulkanEngine* Engine = CrGlobals::GetEnginePointer();
		Engine->UnloadShaderModule(ShaderData);
	}
}

VkShaderModule CrShader::GetShader()
{
	if (!ShaderData)
	{
		LoadShader();
	}
	return ShaderData;
}

void CrShader::Construct()
{
	//ensure this is zerofilled.
	ShaderData = nullptr;
}
