#pragma once

//Nlohmann json lib for simple JSON stuff.
#include "json.hpp"

//Boost
#include "boost/filesystem.hpp"

//Serialization helper macros.

//Serializes a single variable.
#define JSON_SERIALIZE_VARIABLE(JsonVariable, bIsSerializing, VariableName)	   \
if (bIsSerializing)															   \
{																			   \
	JsonVariable[""#VariableName""] = VariableName;							   \
}																			   \
else if (JsonVariable.contains(""#VariableName""))							   \
{																			   \
	VariableName = JsonVariable[""#VariableName""];							   \
}

//Serializes an objects which can have any number of variables. The object must use the serialization interface.
#define JSON_SERIALIZE_OBJECT(JsonVariable, bIsSerializing, VariableName)	   \
if (bIsSerializing)															   \
{																			   \
	nlohmann::json NewObject;												   \
	VariableName.Serialize(bIsSerializing, NewObject);						   \
	JsonVariable[""#VariableName""] = NewObject;							   \
}																			   \
else  if (JsonVariable.contains(""#VariableName""))							   \
{																			   \
	nlohmann::json InnerObject = JsonVariable[""#VariableName""];			   \
	VariableName.Serialize(bIsSerializing, InnerObject);					   \
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

//The interface to use when wanting to make something serializable.
struct CRE_SerializerInterface
{
	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) = 0;
};

//CONST FOLDER LOCATIONS
const boost::filesystem::path UseSettingsFileName = "Settings";

const boost::filesystem::path SettingsSubFolder = "Settings";

const std::string FileExtension = ".json";

//Class that handles serialization of assets.
class CRE_Serialization
{
	//Total path to given folders for simplicity of use.
	boost::filesystem::path SettingsFolderPath;

	nlohmann::json LoadFileToJson(boost::filesystem::path Path) const;
	bool SaveJsonToFile(boost::filesystem::path Path, const nlohmann::json& Json) const;

	CRE_Serialization();
public:

	static CRE_Serialization& Get()
	{
		static CRE_Serialization DL = CRE_Serialization();
		return DL;
	}

	nlohmann::json LoadUserSettings() const;
	bool SaveUserSettings(nlohmann::json& InJson) const;
};
