#include "CRE_Serialization.hpp"

#include "CRE_AssetList.hpp"

//Boost
#include <fstream>

Json CRE_Serialization::LoadFileToJson(Path InPath) const
{
	InPath += FileExtension;
	InPath = ManifestSubFolder / InPath;
	std::ifstream File;
	File.open(InPath, std::ios_base::in);
	if (!File.is_open() || File.fail())
	{
		return nlohmann::json();
	}
	nlohmann::json OutJson;
	File >> OutJson;

	if (OutJson.size() == 0 || File.fail())
	{
		return nlohmann::json();
	}
	return OutJson;
}

bool CRE_Serialization::SaveJsonToFile(Path InPath, const Json& InJson) const
{
	InPath = ManifestSubFolder / InPath;

	std::ofstream File(InPath += FileExtension, std::ios_base::out);
	if (!File.is_open() || File.fail())
	{
		return false;
	}
	File << InJson;
	File.close();
	return !File.fail();
}

CRE_Serialization::CRE_Serialization() :
	ManifestFolderPath(std::filesystem::current_path() / ManifestSubFolder)
{
	std::filesystem::create_directories(ManifestFolderPath);
}

SP<CRE_ManagedObject> CRE_Serialization::Load(const CRE_ID& ToLoad)
{
	SP<CRE_ManagedObject> Object;
	Reload(Object, ToLoad);
	return Object;
}

void CRE_Serialization::Delete(const CRE_ID& ToDelete)
{
	Path InPath = ToDelete.GetString();
	InPath += FileExtension;
	InPath = ManifestSubFolder / InPath;
	std::filesystem::remove(InPath);
}

bool CRE_Serialization::Exists(const CRE_ID& Item)
{
	Path InPath = Item.GetString();
	InPath += FileExtension;
	InPath = ManifestSubFolder / InPath;
	return std::filesystem::exists(InPath);
}

void CRE_Serialization::Reload(SP<CRE_ManagedObject>& Target, const CRE_ID& ToLoad)
{
	Json LoadedJson = LoadFileToJson(ToLoad.GetString());
	if (LoadedJson.size() == 0)
	{
		return;
	}
	if (!Target)
	{
		CRE_ID ClassID = LoadedJson[CLASS_JSON_VALUE];
		if (ClassID.IsValidID())
		{
			auto Object = CRE_ObjectFactory::Get().Create(ClassID);
			Target.swap(Object);
		}
	}

	if (Target)
	{
		Target->Serialize(false, LoadedJson);
	}
}

void CRE_Serialization::Save(SP<CRE_ManagedObject> ToSave)
{
	if (ToSave.get() == nullptr)
	{
		return;
	}

	Json OutputJson;
	ToSave->Serialize(true, OutputJson);

	SaveJsonToFile(ToSave->GetID().GetString(), OutputJson);
}

SP<CRE_AssetList> CRE_Serialization::LoadManifest()
{
	SP<CRE_AssetList> Loaded = DCast<CRE_AssetList>(Load(CRE_ID(ManifestFileName.string())));

	if (Loaded)
	{
		return Loaded;
	}

	
	//return empty asset list if no manifest.
	return CRE_ObjectFactory::Get().Create<CRE_AssetList>();
}
