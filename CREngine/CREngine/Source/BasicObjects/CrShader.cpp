#include "CrShader.h"
#include "CrGlobals.h"
#include "CrSerialization.h"

REGISTER_CLASS_FLAGS(CrShader, CrClassFlags_Unique);

REGISTER_EXTENSION(CrShader, ".crsh");

CrShader::~CrShader()
{
	UnloadShader();
}

void CrShader::BinSerialize(CrArchive& Arch)
{
	Arch <=> ImportPath;
	Arch <=> PushConstantSize;

	if (Arch.bSerializing)
	{
		//Ensure we do the import if we're serializing.
		Import();
	}

	//Import/export shader code.
	Arch <=> ShaderCode;

	if (!Arch.bSerializing)
	{
		assert(LoadShader());
	}
}

bool CrShader::Import()
{
	ShaderCode.clear();

	auto FP = BasePath() / ImportPath;

	//find what the size of the file is by looking up the location of the cursor
	//because the cursor is at the end, it gives the size directly in bytes
	std::streamsize fileSize = std::filesystem::file_size(FP);

	std::ifstream file(FP, std::ios::binary);

	if (!file.is_open())
	{
		return false;
	}


	//put file cursor at beginning
	file.seekg(0, file.beg);

	//spirv expects the buffer to be on uint32, so make sure to reserve a buffer big enough for the entire file
	ShaderCode.resize(fileSize + (fileSize % 4));

	//load the entire file into the buffer
	file.read(ShaderCode.data(), fileSize);

	//now that the file is loaded into the buffer, we can close it
	file.close();
	return true;
}

bool CrShader::LoadShader()
{
	if (ShaderCode.size())
	{
		VulkanEngine* Engine = CrGlobals::GetEnginePointer();
		return Engine->LoadShaderModule((uint32_t*)ShaderCode.data(), ShaderCode.size(), ShaderData);
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
