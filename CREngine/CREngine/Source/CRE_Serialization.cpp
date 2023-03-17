#include "CRE_Serialization.hpp"

//Boost
#include "boost/filesystem/fstream.hpp"

nlohmann::json CRE_Serialization::LoadFileToJson(boost::filesystem::path Path) const
{
	Path += FileExtension;
	boost::filesystem::ifstream File;
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

bool CRE_Serialization::SaveJsonToFile(boost::filesystem::path Path, const nlohmann::json& InJson) const
{
	boost::filesystem::ofstream File(Path += FileExtension, std::ios_base::out);
	if (!File.is_open() || File.fail())
	{
		return false;
	}
	File << InJson;
	File.close();
	return !File.fail();
}

CRE_Serialization::CRE_Serialization() :
	ManifestFolderPath(boost::filesystem::current_path() / ManifestSubFolder)
{
	boost::filesystem::create_directories(ManifestFolderPath);
}

nlohmann::json CRE_Serialization::LoadManifest() const
{
	return LoadFileToJson(ManifestFolderPath / ManifestFileName);
}

bool CRE_Serialization::SaveManifest(nlohmann::json& InJson) const
{
	return SaveJsonToFile(ManifestFolderPath / ManifestFileName, InJson);
}
