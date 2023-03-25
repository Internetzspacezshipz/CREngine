#include "CRE_Loadable.hpp"

void CRE_Loadable::RefSync()
{
	if (LoadedObject)
	{
		Reference = LoadedObject->GetId();
	}
}

void CRE_Loadable::Load() const
{
	//Nothing should be loaded at this point.
	assert(!LoadedObject && Reference);
	LoadedObject = Object_sp(CRE_Serialization::Get().Load(Reference));
}

void CRE_Loadable::Save()
{
	RefSync();
	assert(LoadedObject && Reference);
	CRE_Serialization::Get().Save(LoadedObject);
}

void CRE_Loadable::Unload()
{
	//Do ref sync here just in case we unload while out of sync and we want to load it again later.
	RefSync();
	LoadedObject = nullptr;
}

Object_sp CRE_Loadable::Get(bool bShouldLoad) const
{
	if (bShouldLoad)
	{
		if (!LoadedObject)
		{
			Load();
		}
	}
	return LoadedObject;
}

void CRE_Loadable::Set(Object_sp InObject)
{
	LoadedObject = InObject;
	Reference = InObject->GetId();
}

void CRE_Loadable::Set(CRE_ID InRef)
{
	//reset loaded object since it will no longer sync up.
	LoadedObject = nullptr;
	Reference = InRef;
}

void CRE_Loadable::Serialize(bool bSerializing, nlohmann::json& TargetJson)
{
	RefSync();
	//Direct serialization - no name assumed.;
	VarSerialize(bSerializing, TargetJson, Reference);
}
