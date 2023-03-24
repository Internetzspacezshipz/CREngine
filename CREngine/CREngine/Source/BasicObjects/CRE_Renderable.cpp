#include "CRE_Renderable.hpp"
#include "CRE_Globals.hpp"
#include "vk_Engine.h"

REGISTER_CLASS(CRE_Renderable, CRE_ManagedObject);

void CRE_Renderable::Serialize(bool bSerializing, nlohmann::json& TargetJson)
{
	//advise to do this for each child class of this one.
	Super::Serialize(bSerializing, TargetJson);


}

void CRE_Renderable::SetRenderingEnabled(bool bRenderingEnabled)
{
	if (bRenderingEnabled != bHasAddedRenderable)
	{
		VulkanEngine* Engine = CRE_Globals::GetEnginePointer();

		if (bRenderingEnabled)
		{
			Engine->_renderables.push_back(RenderableObject);
		}
		else
		{

		}
	}
}
