#pragma once
#include "CRE_Loadable.hpp"

#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_stdlib.h"

enum CRE_FieldEditorReturn : uint16_t
{
	CRE_FieldEditorReturn_Unchanged = 0,
	CRE_FieldEditorReturn_WasEdited = 1 << 0,
	CRE_FieldEditorReturn_Error = 1 << 1,
};

//A specific type editor that will be used inside of other UI editors to edit fields that are not supported by ImGui by default 
template<typename _EditedType>
struct CRE_FieldEditor { };

//Call the EditField function externally to edit a field.
template<typename EditedType>
static CRE_FieldEditorReturn EditField(EditedType& Field)
{
	//Push the address of the input, it will be good enough for ImGui to not overlap.
	ImGui::PushID(&Field);
	CRE_FieldEditorReturn ReturnVal = CRE_FieldEditor<EditedType>::Call(Field);
	ImGui::PopID();
	return ReturnVal;
}

//Specialize templates and implement the Call function to allow editing of that field type.
template<>
struct CRE_FieldEditor<CRE_Loadable<CRE_ManagedObject>>
{
	static CRE_FieldEditorReturn Call(CRE_Loadable<CRE_ManagedObject>& Item)
	{
		//Gonna fix this later.
		return CRE_FieldEditorReturn_Error;
		//String CurRefString = Item.GetID().GetString();
		//if (ImGui::InputText("Edit Loadable", &CurRefString), ImGuiInputTextFlags_EnterReturnsTrue)
		//{
		//	if (Item.Rename(CurRefString))
		//	{
		//		return CRE_FieldEditorReturn_WasEdited;
		//	}
		//	return CRE_FieldEditorReturn_Error;
		//}
		//return CRE_FieldEditorReturn_Unchanged;
	}
};
