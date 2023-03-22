#include "CRE_2DRenderable.hpp"

REGISTER_CLASS(CRE_2DRenderable, CRE_ManagedObject);

void CRE_2DRenderable::Serialize(bool bSerializing, nlohmann::json& TargetJson)
{
	//advise to do this for each child class of this one.
	Super::Serialize(bSerializing, TargetJson);

}
