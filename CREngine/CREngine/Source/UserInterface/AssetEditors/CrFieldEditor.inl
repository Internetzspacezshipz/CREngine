#pragma once
#include "CrLoadable.hpp"

#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_stdlib.h"

//For paths
#include "CrPaths.hpp"
#include <filesystem>

//For inline tex ref editor.
#include "BasicObjects/CrTexture.h"
#include "BasicObjects/CrShader.h"


template<StringLiteral TextBoxName, uint32_t UpdateFrequency = 600, typename DirIterType = std::filesystem::recursive_directory_iterator>
static inline bool ComboBox_FilterableDirectoryIterator(CrAssetReference& IORef, const Path& StartPath)
{
	bool bSelected = false;
	static uint32_t UpdateCounter = UpdateFrequency;
	static Array<Path> Items;

	if (UpdateCounter > UpdateFrequency)
	{
		Items.clear();
		UpdateCounter = 0;
		for (auto& Directory : DirIterType{ StartPath })
		{
			Items.push_back(Directory.path().lexically_relative(BasePath()));
		}
	}
	else
	{
		UpdateCounter++;
	}

	if (ImGui::BeginCombo(TextBoxName.Value, IORef.AssetID.GetString().data()))
	{
		// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
		ImGui::SetItemDefaultFocus();
		ImGuiTextFilter Filter;
		Filter.Draw();
		bool bClicked = false;
		//iterate depth first to find... This will probably be very expensive when the assets directory gets larger.
		for (auto& Directory : Items)
		{
			String Str = Directory.generic_string();
			if (Filter.PassFilter(Str.c_str()))
			{
				if (ImGui::Selectable(Str.c_str(), &bSelected))
				{
					bClicked = true;
					IORef.AssetID = Str;
					IORef.ClassID = Directory.extension().generic_string();
				}
			}
		}
		ImGui::EndCombo();

		return bClicked;
	}
	return false;
};


//A specific type editor that will be used inside of other UI editors to edit fields that are not supported by ImGui by default 
template<StringLiteral TextBoxName, typename _EditedType>
struct CrFieldEditor { };

//Call the EditField function externally to edit a field.
template<StringLiteral TextBoxName, typename EditedType>
static bool EditField(EditedType& Field)
{
	//Push the address of the input, it will be good enough for ImGui to not overlap.
	ImGui::PushID(&Field);
	bool ReturnVal = CrFieldEditor<TextBoxName, EditedType>::Call(Field);
	ImGui::PopID();
	return ReturnVal;
}

//Specialize templates and implement the Call function to allow editing of that field type.
template<StringLiteral TextBoxName, typename Type>
struct CrFieldEditor<TextBoxName, CrLoadable<Type>>
{
	static bool Call(CrLoadable<Type>& Item)
	{
		CrAssetReference IORef = Item.GetRef();

		String TempStr = String(IORef.AssetID.GetString());

		if (ImGui::InputText(TextBoxName.Value, &TempStr), ImGuiInputTextFlags_EnterReturnsTrue)
		{
			IORef.AssetID = TempStr;
			Item.Set(IORef);
			return true;
		}
		return false;
	}
};

//Tex editor, specifically looks up textures in the asset folder.
template<StringLiteral TextBoxName>
struct CrFieldEditor<TextBoxName, CrLoadable<CrTexture>>
{
	static bool Call(CrLoadable<CrTexture>& Item)
	{
		bool bSelected = false;
		CrAssetReference IORef = Item.GetRef();

		if (ComboBox_FilterableDirectoryIterator<TextBoxName>(IORef, GetAssetsPath()))
		{
			Item.Set(IORef);	
			return true;
		}
		return false;
	}
};

//Tex editor, specifically looks up textures in the asset folder.
template<StringLiteral TextBoxName>
struct CrFieldEditor<TextBoxName, CrLoadable<CrShader>>
{
	static bool Call(CrLoadable<CrShader>& Item)
	{
		bool bSelected = false;
		CrAssetReference IORef = Item.GetRef();

		if (ComboBox_FilterableDirectoryIterator<TextBoxName>(IORef, GetShadersPath()))
		{
			Item.Set(IORef);
			return true;
		}
		return false;
	}
};
