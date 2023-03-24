#include "CRE_Renderable.hpp"

REGISTER_CLASS(CRE_Renderable, CRE_ManagedObject);

void CRE_Renderable::Serialize(bool bSerializing, nlohmann::json& TargetJson)
{
	//advise to do this for each child class of this one.
	Super::Serialize(bSerializing, TargetJson);


}
