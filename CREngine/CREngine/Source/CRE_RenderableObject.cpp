#include "CRE_RenderableObject.hpp"

void CRE_RenderableObject::Serialize(bool bSerializing, nlohmann::json& TargetJson)
{
	//advise to do this for each child class of this one.
	Super::Serialize(bSerializing, TargetJson);
	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, SomeRandomNumber);
	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, SomeOtherNumber);
	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, Something);
}
