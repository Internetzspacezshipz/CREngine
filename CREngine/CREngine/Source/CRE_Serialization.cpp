#include "CRE_Serialization.hpp"

#include "CRE_AssetList.hpp"

//Boost
#include <fstream>

nlohmann::json CRE_Serialization::LoadFileToJson(std::filesystem::path Path) const
{
	Path += FileExtension;
	Path = ManifestSubFolder / Path;
	std::ifstream File;
	File.open(Path, std::ios_base::in);
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

bool CRE_Serialization::SaveJsonToFile(std::filesystem::path Path, const nlohmann::json& InJson) const
{
		Path = ManifestSubFolder / Path;

	std::ofstream File(Path += FileExtension, std::ios_base::out);
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

CRE_AssetList* CRE_Serialization::LoadManifest() const
{
	CRE_AssetList* OutObject = CRE_ObjectFactory::Get().Create<CRE_AssetList>();
	OutObject->AssetListPath = ManifestFolderPath / ManifestFileName;
	nlohmann::json Manifest = LoadManifest_Internal();
	OutObject->Serialize(false, Manifest);
	return OutObject;
}

bool CRE_Serialization::SaveManifest(CRE_AssetList* InManifest) const
{
	nlohmann::json OutManifest;
	InManifest->Serialize(true, OutManifest);
	return SaveManifest_Internal(OutManifest);
}

nlohmann::json CRE_Serialization::LoadManifest_Internal() const
{
	return LoadFileToJson(ManifestFolderPath / ManifestFileName);
}

bool CRE_Serialization::SaveManifest_Internal(nlohmann::json& InJson) const
{
	return SaveJsonToFile(ManifestFolderPath / ManifestFileName, InJson);
}
