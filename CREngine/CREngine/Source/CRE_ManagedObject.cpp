#include "CRE_ManagedObject.hpp"

#include <iostream>

REGISTER_CLASS(CRE_ManagedObject, void);

CRE_ClassBase* CRE_ManagedObject::GetClassObj() const
{
	return CRE_ObjectFactory::Get().GetClass(GetClass());
}

void CRE_ManagedObject::Rename(const ObjGUID& In)
{
	ID = In;
}

void CRE_ManagedObject::Serialize(bool bSerializing, nlohmann::json& TargetJson)
{
	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, ID);

	//Manually serialize the class only when we are actually serializing this.
	if (bSerializing)
	{
		//Must do this to avoid binding reference - we're only allowing outgoing anyway.
		std::string OutStr = GetClass().GetString();
		JSON_SERIALIZE_VARIABLE_STRNAME<std::string>(TargetJson, bSerializing, OutStr, CLASS_JSON_VALUE);
		std::cout << "object saved: " << ID.GetString() << "  -  with class: " << GetClass().GetString() << std::endl;
	}
	else if (!TargetJson.is_null())
	{
		//sanity check here if we're loading something.
		assert(TargetJson[CLASS_JSON_VALUE] == GetClass());
		std::cout << "object loaded: " << ID.GetString() << "  -  with class: " << GetClass().GetString() << std::endl;
	}
}
