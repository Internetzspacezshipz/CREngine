#pragma once

#include "CrManagedObject.h"
#include <vk_engine.h>

enum CrPushConstantType : uint8_t
{
	CrPushConstantType_int8_t,
	CrPushConstantType_int16_t,
	CrPushConstantType_int32_t,
	CrPushConstantType_int64_t,
	CrPushConstantType_uint8_t,
	CrPushConstantType_uint16_t,
	CrPushConstantType_uint32_t,
	CrPushConstantType_uint64_t,
	CrPushConstantType_float,
	CrPushConstantType_double,
};

#define CrPushConstInfo(Type) SPair<CrPushConstantType_##Type, sizeof(##Type)>

using CrPushConstantTypeData = Chain<
	CrPushConstInfo(int8_t),
	CrPushConstInfo(int16_t),
	CrPushConstInfo(int32_t),
	CrPushConstInfo(int64_t),
	CrPushConstInfo(uint8_t),
	CrPushConstInfo(uint16_t),
	CrPushConstInfo(uint32_t),
	CrPushConstInfo(uint64_t),
	CrPushConstInfo(float),
	CrPushConstInfo(double)
>;

struct CrPushConstantContainer
{
	~CrPushConstantContainer()
	{
		free(Data);
	}
	size_t Size;
	void* Data;
};

#define PADDING_WARNING 1

class CrPushConstantLayout
{
	//Friend serialize operator.
	friend void operator<=>(CrArchive& Arch, CrPushConstantLayout& ToSerialize);
	Array<CrPushConstantType> Layout;
	uint16_t CachedSize = 0;

	uint16_t MakeLayoutSize()
	{
		size_t CurSize = 0;
		for (auto& Elem : Layout)
		{
			CurSize += CrPushConstantTypeData::Get(Elem);
		}

		size_t Padding = CurSize % 4;

		CrCLOGD(Padding, PADDING_WARNING, "Warning : push constant is incorrectly sized. Must be a maxium of 128 bytes, and must be aligned by 4 bytes. It will be padded.");

		if (CurSize > 128)
		{
			CrLOGD(PADDING_WARNING, "Warning : push constant is incorrectly sized. It is too large, and will be clamped to 128 bytes.");
			return 128;
		}
		return CurSize + Padding;
	}

public:
	uint16_t GetLayoutSize() const
	{
		return CachedSize;
	}

	void SetLayout(const Array<CrPushConstantType>& InLayout)
	{
		Layout = InLayout;
		CachedSize = MakeLayoutSize();
	}

	void Make(CrPushConstantContainer& ToMake)
	{
		ToMake.Size = CachedSize;
		ToMake.Data = malloc(CachedSize);
		//Ensure it is null filled at creation.
		memset(ToMake.Data, 0, CachedSize);
	}

	template<typename T>
	void WriteData(const T& In, int Element, CrPushConstantContainer& ToWriteTo)
	{
		size_t Offset = 0;
		for (size_t i = 0; i < Element; i++)
		{
			Offset += CrPushConstantTypeData::Get(Layout[i]);
		}

		((uint8_t*)ToWriteTo.Data)[Offset] = In;
	}
};

__forceinline static void operator <=>(CrArchive& Arch, CrPushConstantLayout& ToSerialize)
{
	Arch <=> ToSerialize.Layout;
	Arch <=> ToSerialize.CachedSize;
}

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

	//The layout for the push constants used in the shader.
	CrPushConstantLayout PushConstantsLayout;

	virtual void Construct() override;
private:

	VkShaderModule ShaderData;
	BinArray ShaderCode;
};


