#include "CRE_AssetList.hpp"
#include <iostream>

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
		//Empty objects since we don't want to double up on items... LoadedObjects are all shared pointers - they will be automatically deleted here.
		LoadedObjects.clear();

		CRE_ObjectFactory& ObjectFactory = CRE_ObjectFactory::Get();

		nlohmann::json LoadJson = Serializer.LoadFileToJson(AssetListPath);
		for (auto& Elem : LoadJson)
		{
			if (CRE_ManagedObject* NewObject = ObjectFactory.Create(Elem[CLASS_JSON_VALUE]))
			{
				auto NewObject_SP = std::shared_ptr<CRE_ManagedObject>(NewObject);

				LoadedObjects.emplace_back(NewObject_SP);
				NewObject_SP->Serialize(false, Elem);
			}
			else
			{
				std::cout << "Failed to load object" << Elem[0] << std::endl;
			}
		}
	}
	else
	{
		nlohmann::json SaveJson{};

		for (auto Obj : LoadedObjects)
		{
			nlohmann::json Inner;
			Obj->Serialize(true, Inner);

			//nlohmann::json BaseJson{ Obj->GetClass(), Inner };
			//SaveJson.push_back(BaseJson);

			SaveJson.push_back(Inner);
		}

		if (false == Serializer.SaveJsonToFile(AssetListPath, SaveJson))
		{
			assert(0);
		}
	}
}
