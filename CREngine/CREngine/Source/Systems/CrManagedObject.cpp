#include "CrManagedObject.h"

#include <iostream>

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