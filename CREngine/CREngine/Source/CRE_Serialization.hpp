#pragma once

//Nlohmann json lib for simple JSON stuff.
#include "json.hpp"

//Boost
#include <filesystem>

//Include types for array.
#include "CRE_Types.hpp"
#include "CRE_ID.hpp"

//Global paths.
#include "CRE_Paths.hpp"

//Json value that will contain the class for objects
#define CLASS_JSON_VALUE "_CLASS_ID_TYPE_"

//The interface to use when wanting to make something serializable.
struct CRE_SerializerInterface
{
	virtual void Serialize(bool bSerializing, Json& TargetJson) = 0;
};

//Use type traits to define serializable types.
template<>
struct Has_Serializer_Function<CRE_SerializerInterface>
{
	constexpr static bool Value = true;
};

static void VarSerialize(bool bSerializing, Json& TargetJson, CRE_SerializerInterface& Value)
{
	//Simply calls the virtual func, while moving into the variable in the TargetJson.
	Value.Serialize(bSerializing, TargetJson);
}

template<>
struct Has_Serializer_Function<Path>
{
	constexpr static bool Value = true;
};

static void VarSerialize(bool bSerializing, Json& TargetJson, Path& Value)
{
	if (bSerializing)
	{
		TargetJson = Value.string();
	}
	else
	{
		//Must create a new path for this one.
		std::string ConvertString = TargetJson;
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
	VarSerialize(bSerializing, TargetJson[VarName], Variable);
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


//Templated Array functions to properly select whether or not to use the VarSerialize function that may be defined by the user.
template<typename Type>
static typename std::enable_if<Has_Serializer_Function<Type>::Value, void>::type JSON_SERIALIZE_VARIABLE_STRNAME(nlohmann::json& TargetJson, bool bSerializing, Array<Type>& InArray, const std::string& VarName)
{
	if (bSerializing)
	{
		nlohmann::json NewJsonArray;
		for (size_t i = 0; i < InArray.size(); i++)
		{
			VarSerialize(bSerializing, NewJsonArray[i], InArray[i]);
		}
		TargetJson[VarName] = NewJsonArray;
	}
	else if (TargetJson.contains(VarName))
	{
		for (auto Item : TargetJson[VarName])
		{
			Type DeserializedItem;
			VarSerialize(bSerializing, Item, DeserializedItem);
			InArray.push_back(DeserializedItem);
		}
	}
}

template<typename Type>
static typename std::enable_if<!Has_Serializer_Function<Type>::Value, void>::type JSON_SERIALIZE_VARIABLE_STRNAME(nlohmann::json& TargetJson, bool bSerializing, Array<Type>& InArray, const std::string& VarName)
{
	if (bSerializing)
	{
		nlohmann::json NewJsonArray;
		for (size_t i = 0; i < InArray.size(); i++)
		{
			NewJsonArray[i] = InArray[i];
		}
		TargetJson[VarName] = NewJsonArray;
	}
	else if (TargetJson.contains(VarName))
	{
		for (auto Item : TargetJson[VarName])
		{
			InArray.push_back(Item);
		}
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

const String FileExtension = ".json";

class CRE_ManagedObject;
class CRE_AssetList;

//Class that handles serialization of assets.
class CRE_Serialization
{
	//Total path to given folders for simplicity of use.
	Path ManifestFolderPath;

	CRE_Serialization();
	Json LoadFileToJson(Path Path) const;
	bool SaveJsonToFile(Path Path, const Json& Json) const;

	Map<CRE_ID, WP<CRE_ManagedObject>> NonInstancedObjects;

	Path IDToPath(const CRE_ID& In);

public:

	static CRE_Serialization& Get()
	{
		static CRE_Serialization DL = CRE_Serialization();
		return DL;
	}

	SP<CRE_ManagedObject> Load(const CRE_ID& ToLoad);
	//Deletes a given object serialized file.
	void Delete(const CRE_ID& ToDelete);
	bool Exists(const CRE_ID& Item);
	bool Move(const CRE_ID& From, const CRE_ID& To);

	void Reload(SP<CRE_ManagedObject>& Target, const CRE_ID& ToLoad);
	void Save(SP<CRE_ManagedObject> ToSave);

	Path GetBaseAssetPath();
};
