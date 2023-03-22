#include "CRE_AssetList.hpp"

REGISTER_CLASS(CRE_AssetList, CRE_ManagedObject);

CRE_AssetList::~CRE_AssetList()
{
	LoadedObjects.clear();
}

void CRE_AssetList::Serialize(bool bSerializing, nlohmann::json& TargetJson)
{
	Super::Serialize(bSerializing, TargetJson);

	CRE_Serialization& Serializer = CRE_Serialization::Get();

	if (!bSerializing)
	{
		CRE_ObjectFactory& ObjectFactory = CRE_ObjectFactory::Get();

		nlohmann::json LoadJson = Serializer.LoadFileToJson(AssetListPath);

		for (auto& Elem : LoadJson)
		{
			auto NewObject = std::shared_ptr<CRE_ManagedObject>(ObjectFactory.Create(Elem[0]));
			NewObject->Serialize(false, Elem[1]);
			LoadedObjects.emplace_back(NewObject);
		}
	}
	else
	{
		nlohmann::json SaveJson{};

		for (auto Obj : LoadedObjects)
		{
			nlohmann::json Inner;
			Obj->Serialize(true, Inner);
			nlohmann::json BaseJson{ Obj->GetClass(), Inner };
			SaveJson.push_back(BaseJson);
		}

		Serializer.SaveJsonToFile(AssetListPath, SaveJson);
	}
}
