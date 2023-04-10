#pragma once
#include "CrLoadable.h"

#include "ThirdParty/ThirdPartyLibs.h"

//For paths
#include "CrPaths.h"
#include <filesystem>

//For inline tex ref editor.
#include "BasicObjects/CrTexture.h"
#include "BasicObjects/CrShader.h"
#include "BasicObjects/CrMesh.h"
#include "BasicObjects/CrMaterial.h"

template<
	StringLiteral TextBoxName,
	StringLiteral RequiredExtension = "",
	uint32_t UpdateFrequency = 600,
	typename DirIterType = std::filesystem::recursive_directory_iterator>
static inline bool ComboBox_FilterableDirectoryIterator_Path(Path& IORef, const Path& StartPath)
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

	if (ImGui::BeginCombo(TextBoxName.Value, IORef.generic_string()))
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

			//check for required extension
			if constexpr (RequiredExtension.Size != 0)
			{
				constexpr StringV ReqExt = StringV(RequiredExtension.Value);
				if (Directory.has_extension() && Directory.extension().generic_string() != ReqExt)
				{
					continue;
				}
			}

			if (Filter.PassFilter(StringV(Str)))
			{
				if (ImGui::Selectable(StringV(Str), &bSelected))
				{
					IORef = Directory;
					bClicked = true;
				}
			}
		}
		ImGui::EndCombo();

		return bClicked;
	}
	return false;
};

template<
	StringLiteral TextBoxName,
	StringLiteral RequiredExtension = "",
	uint32_t UpdateFrequency = 600,
	typename DirIterType = std::filesystem::recursive_directory_iterator>
static inline bool ComboBox_FilterableDirectoryIterator_Asset(CrAssetReference& IORef, const Path& StartPath)
{
	Path IOPath;
	IOPath = IORef.GetString();
	if (ComboBox_FilterableDirectoryIterator_Path<TextBoxName, RequiredExtension, UpdateFrequency, DirIterType>(IOPath, StartPath))
	{
		String Str = IOPath.generic_string();
		StringV PathStrV = StringV(Str);
		size_t FromFront = PathStrV.find_last_of('.');
		FromFront = PathStrV.size() - FromFront;
		PathStrV.remove_suffix(FromFront);

		IORef.AssetID = PathStrV;
		IORef.ClassID = CrSerialization::Get().GetClassForExtension(IOPath.extension().generic_string());
		return true;
	}
	return false;
}

enum FolderLocation : uint8_t
{
	FolderLocation_Undefined, //allow implementation to decide.
	FolderLocation_Assets, //for unimported assets.
	FolderLocation_Shaders, //for unimported shaders.
	FolderLocation_Data, //for all imported/created data.
};

template<FolderLocation Loc = FolderLocation_Assets>
Path GetFolderLocation()
{
	if constexpr (Loc == FolderLocation_Assets)
	{
		return GetAssetsPath();
	}
	else if constexpr (Loc == FolderLocation_Shaders)
	{
		return GetShadersPath();
	}
	else if constexpr (Loc == FolderLocation_Data)
	{
		return GetDataPath();
	}
	else
	{
		return BasePath();
	}
}

//A specific type editor that will be used inside of other UI editors to edit fields that are not supported by ImGui by default 
template<
	StringLiteral TextBoxName,
	typename _EditedType,
	FolderLocation FLoc = FolderLocation_Undefined,
	StringLiteral Extension = "">
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

//Version for specifically setting a path to an importable item.
template<StringLiteral TextBoxName, FolderLocation Location = FolderLocation_Data, StringLiteral Extension = "">
static bool EditField(Path& Field)
{
	//Push the address of the input, it will be good enough for ImGui to not overlap.
	ImGui::PushID(&Field);
	bool ReturnVal = CrFieldEditor<TextBoxName, Path, Location, Extension>::Call(Field);
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

		//Unknown type - just give it a 
		if (ComboBox_FilterableDirectoryIterator_Asset<TextBoxName, ".crap">(IORef, GetAssetsPath()))
		{
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
		CrAssetReference IORef = Item.GetRef();

		if (ComboBox_FilterableDirectoryIterator_Asset<TextBoxName, ".crim">(IORef, GetDataPath()))
		{
			Item.Set(IORef);	
			return true;
		}
		return false;
	}
};

//Shader editor, specifically looks up shaders in the shader folder.
template<StringLiteral TextBoxName>
struct CrFieldEditor<TextBoxName, CrLoadable<CrShader>>
{
	static bool Call(CrLoadable<CrShader>& Item)
	{
		CrAssetReference IORef = Item.GetRef();

		if (ComboBox_FilterableDirectoryIterator_Asset<TextBoxName, ".crsh">(IORef, GetDataPath()))
		{
			Item.Set(IORef);
			return true;
		}
		return false;
	}
};

//Path selector. Customizable extension.
template<StringLiteral TextBoxName, FolderLocation Location, StringLiteral Extension>
struct CrFieldEditor<TextBoxName, Path, Location, Extension>
{
	static bool Call(Path& Item)
	{
		return ComboBox_FilterableDirectoryIterator_Path<TextBoxName, Extension>(Item, GetFolderLocation<Location>());
	}
};

//Mesh editor, specifically looks up meshes in the asset folder.
template<StringLiteral TextBoxName>
struct CrFieldEditor<TextBoxName, CrLoadable<CrMesh>>
{
	static bool Call(CrLoadable<CrMesh>& Item)
	{
		CrAssetReference IORef = Item.GetRef();

		if (ComboBox_FilterableDirectoryIterator_Asset<TextBoxName, ".crob">(IORef, GetDataPath()))
		{
			Item.Set(IORef);
			return true;
		}
		return false;
	}
};

//Material editor, specifically looks up materials in the data folder.
template<StringLiteral TextBoxName>
struct CrFieldEditor<TextBoxName, CrLoadable<CrMaterial>>
{
	static bool Call(CrLoadable<CrMaterial>& Item)
	{
		CrAssetReference IORef = Item.GetRef();

		if (ComboBox_FilterableDirectoryIterator_Asset<TextBoxName, ".crmat">(IORef, GetDataPath()))
		{
			Item.Set(IORef);
			return true;
		}
		return false;
	}
};

//Texture format editor.
template<StringLiteral TextBoxName>
struct CrFieldEditor<TextBoxName, CrTextureFormatTypes>
{
	static bool Call(CrTextureFormatTypes& Item)
	{
		if (ImGui::BeginCombo(TextBoxName.Value, TextureFormatStrings::Get(Item).String))
		{
			TextureFormatStrings::Visit(
				[&](const CrTextureFormatTypes K, const StringLiteral2 V)
				{
					if (ImGui::Selectable(StringV(V.String)))
					{
						Item = K;
					}
				});
			ImGui::EndCombo();
		}
		return false;
	}
};
