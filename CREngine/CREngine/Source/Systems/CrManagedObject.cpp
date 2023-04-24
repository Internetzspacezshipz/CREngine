#include "CrManagedObject.h"

REGISTER_CLASS(CrManagedObject);

CrObjectFactory& CrObjectFactory::Get()
{
	static CrObjectFactory ObjFactory;
	return ObjFactory;
}

CrClass* CrManagedObject::GetClassObj() const
{
	return CrObjectFactory::Get().GetClass(GetClass());
}

void CrManagedObject::Rename(const ObjGUID& In)
{
	if (In.IsValidID() && In != ID)
	{
		ID = In;
		OnRename();
	}
}

void CrManagedObject::BinSerialize(CrArchive& Data)
{
}

CrManagedObject::CrManagedObject()
{
	//CrLOGD(TRACK_OBJECT_LIFETIMES, "Object was created...");
}

CrManagedObject::~CrManagedObject()
{
	CrLOGD(TRACK_OBJECT_LIFETIMES, "Object was destroyed: Name: {} - Class:{}", GetID().GetString(), GetClass().GetString());
}
