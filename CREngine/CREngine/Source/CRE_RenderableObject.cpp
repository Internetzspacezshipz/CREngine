#include "CRE_RenderableObject.hpp"

void CRE_RenderableObject::Serialize(bool bSerializing, nlohmann::json& TargetJson)
{
	//advise to do this for each child class of this one.
	Super::Serialize(bSerializing, TargetJson);

}
