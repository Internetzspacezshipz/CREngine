#include "CRE_ManagedObject.hpp"

static CRE_Registrar<void, CRE_ManagedObject>s_CRE_ManagedObjectCreator{ crc32_CONST("CRE_ManagedObject", sizeof("CRE_ManagedObject")),"CRE_ManagedObject" };

ClassGUID CRE_ManagedObject::StaticClass()
{
	static ClassGUID ConcreteClassGUID = crc32_CONST("CRE_ManagedObject", sizeof("CRE_ManagedObject"));
	return ConcreteClassGUID;
}
