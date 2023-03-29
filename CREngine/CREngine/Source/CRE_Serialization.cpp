#include "CRE_Serialization.hpp"

#include "CRE_AssetList.hpp"

//Boost
#include <fstream>

Json CRE_Serialization::LoadFileToJson(Path InPath) const
{
	InPath += FileExtension;
	InPath = DataPath / InPath;
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
	InPath = DataPath / InPath;

	std::ofstream File(InPath += FileExtension, std::ios_base::out);
	if (!File.is_open() || File.fail())
	{
		return false;
	}
	File << InJson;
	File.close();
	return !File.fail();
}

Path CRE_Serialization::IDToPath(const CRE_ID& In)
{
	if (!In.IsValidID())
	{
		return Path();
	}
	Path InPath = In.GetString();
	InPath += FileExtension;
	InPath = DataPath / InPath;
	return InPath;
}

CRE_Serialization::CRE_Serialization() :
	ManifestFolderPath(std::filesystem::current_path() / DataPath)
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
	Path Target = IDToPath(ToDelete);
	std::filesystem::remove(Target);
}

bool CRE_Serialization::Exists(const CRE_ID& Item)
{
	Path Target = IDToPath(Item);
	return std::filesystem::exists(Target);
}

bool CRE_Serialization::Move(const CRE_ID& From, const CRE_ID& To)
{
	if (Exists(To))
	{
		return false;
	}
	if (Exists(From))
	{
		Path FromP = IDToPath(From);
		Path ToP = IDToPath(To);
		std::filesystem::rename(FromP, ToP);
	}
	return false;
}

void CRE_Serialization::Reload(SP<CRE_ManagedObject>& Target, const CRE_ID& ToLoad)
{
	//If we're looking for something that can never be instanced, find it here instead of loading/creating it
	auto Found = NonInstancedObjects.find(ToLoad);
	if (Found != NonInstancedObjects.end())
	{
		if (!Found->second.expired())
		{
			Target = Found->second.lock();
			return;
		}
		else
		{
			//If there is an expired WP to the non-instanced object, remove it and reload it in the normal manner.
			NonInstancedObjects.erase(ToLoad);
		}
	}

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
			CRE_ObjectFactory& Factory = CRE_ObjectFactory::Get();
			//Find our actual class object ot check its class flags to see if we should add it to the non-instanced map.
			if (CRE_ClassBase* Class = Factory.GetClass(ClassID))
			{
				auto Object = Factory.Create(ClassID, ToLoad);

				if (Class->HasFlag(CRE_ClassFlags_Unique))
				{
					NonInstancedObjects.emplace(ToLoad, Object);
				}

				Target.swap(Object);
			}
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

Path CRE_Serialization::GetBaseAssetPath()
{
	return std::filesystem::current_path() / DataPath;
}
