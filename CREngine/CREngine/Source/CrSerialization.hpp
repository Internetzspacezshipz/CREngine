#pragma once

//Nlohmann json lib for simple JSON stuff.
#include "json.hpp"

//Boost
#include <filesystem>

//Include types for array.
#include "CrTypes.hpp"
#include "CrID.hpp"

//Global paths.
#include "CrPaths.hpp"

//Json value that will contain the class for objects
#define CLASS_JSON_VALUE "_CLASS_ID_TYPE_"

//The interface to use when wanting to make something serializable.
struct CrSerializerInterface
{
	virtual void Serialize(bool bSerializing, Json& TargetJson) = 0;
};

//Use type traits to define serializable types.
template<>
struct Has_Serializer_Function<CrSerializerInterface>
{
	constexpr static bool Value = true;
};

static void VarSerialize(bool bSerializing, Json& TargetJson, CrSerializerInterface& Value)
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

//CONST FOLDER LOCATIONS
//Manifest file locations so we can just load all the data we need immediately.

//CRAP - stands for CRunch Archive Protocol (lol)
const String GenericItemExt = ".crap";

class CrManagedObject;
class CrAssetList;

//Class that handles serialization of assets.
class CrSerialization
{
	//Total path to given folders for simplicity of use.
	Path ManifestFolderPath;

	CrSerialization();
	Json LoadFileToJson(Path Path) const;
	bool SaveJsonToFile(Path Path, const Json& Json) const;

	Map<CrAssetReference, WP<CrManagedObject>> NonInstancedObjects;

	//Extension to special class IDs to load as
	Map<String, CrID> ExtensionToClass;
	Map<CrID, String> ClassToExtension;


	Path RefToPath(const CrAssetReference& In) const;
	CrAssetReference PathToRef(const Path& In) const;
public:

	static CrSerialization& Get()
	{
		static CrSerialization DL = CrSerialization();
		return DL;
	}

	//Deletes a given object serialized file.
	void Delete(SP<CrManagedObject> ToDelete);
	bool Exists(const CrAssetReference& Item);
	bool Move(const CrAssetReference& From, const CrAssetReference& To);

	SP<CrManagedObject> LoadJson(const CrAssetReference& ToLoad);
	void ReloadJson(SP<CrManagedObject>& Target, const CrAssetReference& ToLoad);
	void SaveJson(SP<CrManagedObject> ToSave);

	SP<CrManagedObject> Load(const CrAssetReference& ToLoad);
	SP<CrManagedObject> Load(const Path& ToLoad);
	//Reload that only takes in a target. Can fail.
	void Reload(SP<CrManagedObject>& Target, const CrID& ToLoad);
	//Reload that must have a class if the target is not valid.
	void Reload(SP<CrManagedObject>& Target, const CrAssetReference& ToLoad);
	void Save(SP<CrManagedObject> ToSave);

	Path GetBaseAssetPath();

	void RegisterExtensionClass(String&& Extension, const CrID& ClassID);

	bool IsSupportedFileType(const Path& InPath);
	String GetExtensionForClass(const CrID& InID) const;
	CrID GetClassForExtension(const String& InExt) const;
};

template<typename SpecificClass, StringLiteral Extension>
struct ExtensionRegistrar
{
	ExtensionRegistrar()
	{
		CrSerialization::Get().RegisterExtensionClass(std::move(String(Extension.Value)), SpecificClass::StaticClass());
	}
};

#define REGISTER_EXTENSION(Class, Extension) static ExtensionRegistrar<Class, Extension>_ExtReg_##Class{};