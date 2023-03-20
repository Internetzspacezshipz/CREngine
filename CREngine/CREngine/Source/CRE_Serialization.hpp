#pragma once

//Nlohmann json lib for simple JSON stuff.
#include "json.hpp"

//Boost
#include <filesystem>

template<typename Type>
struct Has_Serializer_Function
{
	constexpr static bool Value = false;
};

//The interface to use when wanting to make something serializable.
struct CRE_SerializerInterface
{
	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) = 0;
};

//Use type traits to define non-CRE types as serializable.
template<>
struct Has_Serializer_Function<CRE_SerializerInterface>
{
	constexpr static bool Value = true;
};

static void VarSerialize(bool bSerializing, nlohmann::json& TargetJson, const std::string& VarName, CRE_SerializerInterface& Value)
{
	//Simply calls the virtual func, while moving into the variable in the TargetJson.
	Value.Serialize(bSerializing, TargetJson[VarName]);
}

template<>
struct Has_Serializer_Function<std::filesystem::path>
{
	constexpr static bool Value = true;
};

static void VarSerialize(bool bSerializing, nlohmann::json& TargetJson, const std::string& VarName, std::filesystem::path& Value)
{
	if (bSerializing)
	{
		TargetJson[VarName] = Value.string();
	}
	else if (TargetJson.contains(VarName))
	{
		//Must create a new path for this one.
		std::string ConvertString = TargetJson[VarName];
		Value.assign(std::filesystem::path(ConvertString));
	}
}


//Serialization helper macros.

//Serializes a single variable.
#define JSON_SERIALIZE_VARIABLE(JsonVariable, bIsSerializing, VariableName)	JSON_SERIALIZE_VARIABLE_STRNAME(JsonVariable, bIsSerializing, VariableName, ""#VariableName"")

//Templated functions to properly select whether or not to use the VarSerialize function that may be defined by the user.
template<typename Type>
static typename std::enable_if<Has_Serializer_Function<Type>::Value, void>::type JSON_SERIALIZE_VARIABLE_STRNAME(nlohmann::json& TargetJson, bool bSerializing, Type& Variable, const std::string& VarName)
{
	VarSerialize(bSerializing, TargetJson, VarName, Variable);
}

template<typename Type>
static typename std::enable_if<!Has_Serializer_Function<Type>::Value, void>::type JSON_SERIALIZE_VARIABLE_STRNAME(nlohmann::json& TargetJson, bool bSerializing, Type& Variable, const std::string& VarName)
{
	if (bSerializing)
	{
		TargetJson[VarName] = Variable;
	}
	else if (TargetJson.contains(VarName))
	{
		Variable = TargetJson[VarName];
	}
}

//Serializes an array of objects which can have any number of variables. The objects must use the serialization interface.
#define JSON_SERIALIZE_OBJECT_ARRAY(JsonVariable, bIsSerializing, ArrayName, ObjectType)	\
if (bIsSerializing)																			\
{																							\
	nlohmann::json NewArray = nlohmann::json::array({});									\
	for (auto& Elem : ArrayName)															\
	{																						\
		nlohmann::json NewObject;															\
		Elem.Serialize(bSerializing, NewObject);											\
		NewArray.push_back(NewObject);														\
	}																						\
	JsonVariable[""#ArrayName""] = NewArray;												\
}																							\
else if (JsonVariable.contains(""#ArrayName""))												\
{																							\
	ArrayName.clear();																		\
	auto& InnerArray = JsonVariable[""#ArrayName""];										\
	for (auto& Elem : InnerArray)															\
	{																						\
		ObjectType NewSubObject;															\
		NewSubObject.Serialize(bSerializing, Elem);											\
		ArrayName.push_back(NewSubObject);													\
	}																						\
}

//Serializes a dynamic size array of variables.
#define JSON_SERIALIZE_ARRAY(JsonVariable, bIsSerializing, ArrayName, ObjectType)	\
if (bIsSerializing)																	\
{																					\
	nlohmann::json NewArray = nlohmann::json::array({});							\
	for (auto& Elem : ArrayName)													\
	{																				\
		NewArray.push_back(Elem);													\
	}																				\
	JsonVariable[""#ArrayName""] = NewArray;										\
}																					\
else if (JsonVariable.contains(""#ArrayName""))										\
{																					\
	ArrayName.clear(); 																\
	auto& InnerArray = JsonVariable[""#ArrayName""];								\
	for (auto& Elem : InnerArray)													\
	{																				\
		ArrayName.push_back(Elem);													\
	}																				\
}

//Serializes a C-style array of variables.
#define JSON_SERIALIZE_CARRAY(JsonVariable, bIsSerializing, ArrayName, ObjectType)	\
if (bIsSerializing)																	\
{																					\
	nlohmann::json NewArray = nlohmann::json::array({});							\
	for (auto& Elem : ArrayName)													\
	{																				\
		NewArray.push_back(Elem);													\
	}																				\
	JsonVariable[""#ArrayName""] = NewArray;										\
}																					\
else if (JsonVariable.contains(""#ArrayName""))										\
{																					\
	auto& InnerArray = JsonVariable[""#ArrayName""];								\
	for (int i = 0; i < ArrayName.size(); i++)										\
	{																				\
		ArrayName[i] = InnerArray[i];												\
	}																				\
}

//CONST FOLDER LOCATIONS
//Manifest file locations so we can just load all the data we need immediately.
const std::filesystem::path ManifestFileName = "Manifest";
const std::filesystem::path ManifestSubFolder = "Manifest";

const std::string FileExtension = ".json";

//Class that handles serialization of assets.
class CRE_Serialization
{
	//Total path to given folders for simplicity of use.
	std::filesystem::path ManifestFolderPath;

	CRE_Serialization();
public:
	nlohmann::json LoadFileToJson(std::filesystem::path Path) const;
	bool SaveJsonToFile(std::filesystem::path Path, const nlohmann::json& Json) const;

	static CRE_Serialization& Get()
	{
		static CRE_Serialization DL = CRE_Serialization();
		return DL;
	}

	nlohmann::json LoadManifest() const;
	bool SaveManifest(nlohmann::json& InJson) const;
};
