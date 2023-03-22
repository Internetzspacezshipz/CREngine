#include "CRE_ManagedObject.hpp"

ClassGUID CRE_ManagedObject::StaticClass()
{
	static ClassGUID ConcreteClassGUID = crc32_CONST("CRE_ManagedObject", sizeof("CRE_ManagedObject"));
	return ConcreteClassGUID;
}
