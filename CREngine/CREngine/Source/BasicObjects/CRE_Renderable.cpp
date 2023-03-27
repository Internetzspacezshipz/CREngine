#include "CRE_Renderable.hpp"
#include "CRE_Globals.h"
#include "vk_Engine.h"

REGISTER_CLASS(CRE_Renderable);

void CRE_Renderable::Serialize(bool bSerializing, nlohmann::json& TargetJson)
{
	//advise to do this for each child class of this one.
	Super::Serialize(bSerializing, TargetJson);

	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, Materials);
	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, Textures);
}

void CRE_Renderable::SetRenderingEnabled(bool bRenderingEnabled)
{
	if (bRenderingEnabled)
	{
		RenderableObject->bRenderEnable = true;
	}
	else
	{
		RenderableObject->bRenderEnable = false;
	}
}

void CRE_Renderable::AddRenderable()
{
	if (bHasAddedRenderable == false)
	{
		VulkanEngine* Engine = CRE_Globals::GetEnginePointer();
		Engine->_renderables.push_back(RenderableObject);
		bHasAddedRenderable = true;
	}
}
