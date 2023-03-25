#pragma once

#include "CRE_Types.hpp"

typedef uint32_t IDNum_t;

class CRE_ObjectIDRegistry
{
	friend class CRE_ID;
	friend class CRE_ObjectFactory;


	static Map<IDNum_t, std::string>& GetMap();
	static std::string CreateUniqueString(const std::string& In);
};

class CRE_ID
{
	IDNum_t Number = 0;
	bool bHasBeenSet = false;
public:
	std::string GetString() const;
	IDNum_t GetNumber() const { return Number; }

	bool IsValidID() const;

	CRE_ID& operator = (const CRE_ID& CopyFrom);
	CRE_ID& operator = (const std::string& MakeFrom)
	{
		return *this = CRE_ID(MakeFrom);
	}
	bool operator == (const CRE_ID& CompareWith) const { return Number == CompareWith.Number; }
	operator bool() const { return IsValidID(); };

	CRE_ID(std::string Name);
	CRE_ID();
};

//Implement std::hash for the object id so it can be used in maps.
namespace std
{
	template <>
	struct hash<CRE_ID>
	{
		std::size_t operator()(const CRE_ID& Item) const
		{
			auto out = Item.GetNumber();
			return out;
		}
	};
}

typedef CRE_ID ObjGUID;

template<>
struct Has_Serializer_Function<CRE_ID>
{
	constexpr static bool Value = true;
};

//manual serialize code for CRE_ID
static void VarSerialize(bool bSerializing, nlohmann::json& TargetJson, CRE_ID& Value)
{
	if (bSerializing)
	{
		TargetJson = Value.GetString();
	}
	else if (TargetJson.is_string())
	{
		Value = CRE_ID(TargetJson);
	}
}

