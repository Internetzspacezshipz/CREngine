#include "CrSerialization.h"

#include "CrAssetList.h"

//Boost
#include <fstream>

Json CrSerialization::LoadFileToJson(Path InPath) const
{
	InPath += ".json";
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

bool CrSerialization::SaveJsonToFile(Path InPath, const Json& InJson) const
{
	InPath = DataPath / InPath;

	std::ofstream File(InPath += ".json", std::ios_base::out);
	if (!File.is_open() || File.fail())
	{
		return false;
	}
	File << InJson;
	File.close();
	return !File.fail();
}

Path CrSerialization::RefToPath(const CrAssetReference& In) const
{
	if (!In.IsValidID())
	{
		return Path();
	}
	Path InPath = In.AssetID.GetString();
	InPath += GetExtensionForClass(In.ClassID);
	InPath = BasePath() / InPath;
	return InPath;
}

CrAssetReference CrSerialization::PathToRef(const Path& In) const
{
	CrAssetReference Out;
	Out.AssetID = In.lexically_relative(BasePath()).replace_extension().generic_string();
	Out.ClassID = GetClassForExtension(In.extension().generic_string());
	return Out;
}

CrSerialization::CrSerialization() :
	ManifestFolderPath(std::filesystem::current_path() / DataPath)
{
	std::filesystem::create_directories(ManifestFolderPath);
}


void CrSerialization::Delete(SP<CrManagedObject> ToDelete)
{
	Path Target = RefToPath(CrAssetReference{ ToDelete->GetID(), ToDelete->GetClass() });
	std::filesystem::remove(Target);
}

bool CrSerialization::Exists(const CrAssetReference& Item)
{
	return std::filesystem::exists(RefToPath(Item));
}

bool CrSerialization::Move(const CrAssetReference& From, const CrAssetReference& To)
{
	//Do not allow to move to when it already exists.
	if (Exists(To))
	{
		return false;
	}

	if (Exists(From))
	{
		Path ToP = RefToPath(To);
		Path FromP = RefToPath(From);
		std::filesystem::rename(FromP, ToP);
	}
	return false;
}

SP<CrManagedObject> CrSerialization::LoadJson(const CrAssetReference& ToLoad)
{
	SP<CrManagedObject> Object;
	ReloadJson(Object, ToLoad);
	return Object;
}

void CrSerialization::ReloadJson(SP<CrManagedObject>& Target, const CrAssetReference& ToLoad)
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

	Json LoadedJson = LoadFileToJson(ToLoad.AssetID.GetString());

	if (LoadedJson.size() == 0)
	{
		return;
	}

	if (!Target)
	{
		CrID ClassID = LoadedJson[CLASS_JSON_VALUE];
		if (ClassID.IsValidID())
		{
			CrObjectFactory& Factory = CrObjectFactory::Get();
			//Find our actual class object ot check its class flags to see if we should add it to the non-instanced map.
			if (CrClass* Class = Factory.GetClass(ClassID))
			{
				auto Object = Factory.Create(ClassID, ToLoad.AssetID);

				if (Class->HasFlag(CrClassFlags_Unique))
				{
					NonInstancedObjects.emplace(ToLoad, Object);
				}

				Target.swap(Object);
			}
		}
	}

	if (Target)
	{
		//Target->Serialize(false, LoadedFile);
	}
}

void CrSerialization::SaveJson(SP<CrManagedObject> ToSave)
{
	if (ToSave.get() == nullptr)
	{
		return;
	}

	Json OutputJson;
	//ToSave->Serialize(true, OutputJson);

	SaveJsonToFile(ToSave->GetID().GetString(), OutputJson);
}

SP<CrManagedObject> CrSerialization::Load(const CrAssetReference& ToLoad)
{
	SP<CrManagedObject> Object;
	Reload(Object, ToLoad);
	return Object;
}

SP<CrManagedObject> CrSerialization::Load(const Path& ToLoad)
{
	return Load(PathToRef(ToLoad));
}

void CrSerialization::Reload(SP<CrManagedObject>& Target, const CrID& ToLoad)
{
	if (Target)
	{
		Reload(Target, CrAssetReference{ ToLoad, Target->GetClass() });
	}
}

void CrSerialization::Reload(SP<CrManagedObject>& Target, const CrAssetReference& ToLoad)
{
	//Load the base class of what we're trying to load.
	CrClass* BaseClassInfo = CrObjectFactory::Get().GetClass(ToLoad.ClassID);

	//If it's a data only class type, then don't load anything from it.
	bool bIsDataOnlyClass = BaseClassInfo->HasFlag(CrClassFlags_DataOnly);

	//If we're looking for something that can never be instanced, find it here instead of loading/creating it
	if (BaseClassInfo->HasFlag(CrClassFlags_Unique))
	{
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
	}

	Path TargetPath = RefToPath(ToLoad);
	
	if (std::filesystem::file_size(TargetPath) == 0)
	{
		CrLOG("%s has a file size of 0. We cannot load that!", TargetPath.generic_string().c_str());
		return; 
	}

	CrArchiveIn LoadedFile { TargetPath.generic_string() };

	if (!Target)
	{
		CrID ClassID;
		CrID NameID = ToLoad.AssetID;
		if (bIsDataOnlyClass)
		{
			ClassID = GetClassForExtension(TargetPath.extension().generic_string());
		}
		else
		{
			LoadedFile <=> ClassID;
			LoadedFile <=> NameID;
		}
	
		if (ClassID.IsValidID())
		{
			CrObjectFactory& Factory = CrObjectFactory::Get();
			//Find our actual class object ot check its class flags to see if we should add it to the non-instanced map.
			if (CrClass* Class = Factory.GetClass(ClassID))
			{
				auto Object = Factory.Create(ClassID, ToLoad.AssetID);

				if (Class->HasFlag(CrClassFlags_Unique))
				{
					NonInstancedObjects.emplace(ToLoad, Object);
				}

				Target.swap(Object);
			}
		}
		Target->ID = NameID;
	}

	if (Target)
	{
		Target->BinSerialize(LoadedFile);
	}
}

void CrSerialization::Save(SP<CrManagedObject> ToSave)
{
	if (ToSave.get() == nullptr)
	{
		return;
	}

	if (ToSave->GetClassObj()->HasFlag(CrClassFlags_Transient))
	{
		return;
	}

	const String StrPath = (BasePath() / ToSave->GetID().GetString()).generic_string() + GetExtensionForClass(ToSave->GetClass());
	CrArchiveOut OutArch { StrPath };
	CrID ClassID = ToSave->GetClass();
	OutArch <=> ClassID;
	OutArch <=> ToSave->ID;
	ToSave->BinSerialize(OutArch);
}

Path CrSerialization::GetBaseAssetPath()
{
	return BasePath();
}

void CrSerialization::RegisterExtensionClass(String&& Extension, const CrID& ClassID)
{
	transform(Extension.begin(), Extension.end(), Extension.begin(), ::tolower);

	ExtensionToClass.emplace(Extension, ClassID);
	ClassToExtension.emplace(ClassID, Extension);
}

bool CrSerialization::IsSupportedFileType(const Path& InPath)
{
	auto Ext = InPath.extension().generic_string();
	transform(Ext.begin(), Ext.end(), Ext.begin(), ::tolower);

	if (Ext == GenericItemExt)
	{
		return true;
	}

	return ExtensionToClass.contains(Ext);
}

String CrSerialization::GetExtensionForClass(const CrID& InID) const
{
	auto Found = ClassToExtension.find(InID);
	if (Found != ClassToExtension.end())
	{
		return Found->second;
	}
	return GenericItemExt;
}

CrID CrSerialization::GetClassForExtension(const String& InExt) const
{
	if (InExt.starts_with('.') == false)
	{
		assert(false);
		return CrID();
	}

	if (InExt == GenericItemExt)
	{
		return CrManagedObject::StaticClass();
	}

	auto Found = ExtensionToClass.find(InExt);
	if (Found != ExtensionToClass.end())
	{
		return Found->second;
	}
	assert(false);
	return CrID();
}
