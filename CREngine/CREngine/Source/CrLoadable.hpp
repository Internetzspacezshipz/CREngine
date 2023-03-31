#pragma once
#include "CrManagedObject.hpp"
#include "CrID.hpp"
#include "CrSerialization.hpp"

//forward decl of self
template<typename Type>
class CrLoadable;

//Special struct to circumvent some friend stuff.
struct _CrLoadableAccessor
{
	template<typename Type>
	static forceinline CrAssetReference& Access(CrLoadable<Type>& Item)
	{
		return Item.Reference;
	}
};

//Shared pointer with an ID that can be loaded via json and create a new object for the shared pointer.
template<typename Type>
class CrLoadable : public CrSerializerInterface
{
	friend _CrLoadableAccessor;

	mutable CrAssetReference Reference;

	mutable SP<Type> LoadedObject;

	void RefSync() const
	{
		if (LoadedObject)
		{
			Reference.AssetID = LoadedObject->GetID();
			Reference.ClassID = LoadedObject->GetClass();
		}
	}

public:

	void Load() const
	{
		//Nothing should be loaded at this point.
		assert(!LoadedObject && Reference.IsValidID());
		LoadedObject = DCast<Type>(CrSerialization::Get().Load(Reference));
	}

	//Safer version of above for general use.
	void SafeLoad() const
	{
		//Nothing should be loaded at this point.
		if (!IsLoaded() && IsLoadable())
		{
			Load();
		}
	}

	//Loads the item if it is valid, or creates an empty one if no reference.
	void LoadOrCreate()
	{
		SafeLoad();
		if (!IsLoaded())
		{
			LoadedObject = CrObjectFactory::Get().Create<Type>(Reference);
			Save();
		}
	}

	void Reload() const
	{
		if (!LoadedObject)
		{
			Load();
		}
		else
		{
			LoadedObject = DCast<Type>(CrSerialization::Get().Reload(LoadedObject, Reference));
		}
	}

	void Save()
	{
		RefSync();
		assert(LoadedObject && Reference.IsValidID());
		CrSerialization::Get().Save(LoadedObject);
	}

	//Does not guarntee that the object was unloaded, only that the reference count goes down by one.
	void Unload()
	{
		//Do ref sync here just in case we unload while out of sync and we want to load it again later.
		RefSync();
		LoadedObject = nullptr;
	}

	//Query functions
	
	//Has an id that can be turned into a string and back, or used to save or load a file.
	forceinline bool HasValidID() const				{ return Reference.IsValidID(); }
	//The ID corresponds to a file.
	forceinline bool IsLoadable() const				{ return HasValidID() ? CrSerialization::Get().Exists(Reference) : false; }
	//The object exists - doesn't necessarily come from a file.
	forceinline bool IsLoaded() const				{ return LoadedObject.get(); }
	//The object exists or can be loaded
	forceinline bool IsLoadedOrLoadable() const		{ return IsLoaded() ? true : IsLoadable(); }
	//Gets the ID.
	forceinline CrID GetID() const { RefSync(); return Reference.AssetID; }
	//Gets the ID.
	forceinline CrAssetReference GetRef() const { RefSync(); return Reference; }

	//templated get, since it avoids branching if set to false
	template<bool bShouldLoad = false>
	SP<Type> Get() const
	{
		if constexpr (bShouldLoad)
		{
			if (!LoadedObject)
			{
				Load();
			}
		}
		return LoadedObject;
	}


	//Runtime set - when we make a new object this should be called to create a reference that exists outside of runtime pointers.
	void Set(SP<Type> InObject)
	{
		LoadedObject = InObject;
		RefSync();
	}

	//Set a new reference
	void Set(CrAssetReference InRef)
	{
		//reset loaded object since it will no longer sync up.
		LoadedObject = nullptr;
		Reference = InRef;
	}

	//Change the name of the asset. Remember to resave it!
	void Rename(CrID InNewName)
	{
		LoadedObject->Rename(InNewName);
		RefSync();
	}


	// Inherited via CrSerializerInterface
	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override
	{
		RefSync();
		//Direct serialization - no name assumed.;
		//VarSerialize(bSerializing, TargetJson, Reference);
	}

	CrLoadable() = default;
	CrLoadable(SP<Type> In) { Set(In); };
	CrLoadable(CrID In) { Set(In); };

	//extra constructor that creates the shared pointer here if it is not already done.
	CrLoadable(CrManagedObject* In) { Set(SP<Type>(In)); };

	Type* operator ->() const { return LoadedObject.get(); }
};

//Declare this type has a serializer.
template<typename Type>
struct Has_Serializer_Function<CrLoadable<Type>>
{
	constexpr static bool Value = true;
};

//Helper function to load a whole array of CrLoadables
template<typename Type>
static forceinline void LoadArray(Array<CrLoadable<Type>>& Target)
{
	//Clear the array before we load so we don't have doubling up of existing items.
	Target.clear();
	for (auto& Elem : Target)
	{
		Elem.Get<true>();
	}
	//Remove invalid objects here after loading.
	RemoveByPredicate(Target, [](const CrLoadable<Type>& Item) { return Item.Get() == nullptr; });
}

//Helper function to unload a whole array of CrLoadables
template<typename Type>
static forceinline void UnloadArray(Array<CrLoadable<Type>>& Target)
{
	for (auto& Elem : Target)
	{
		Elem.Unload();
	}
}

//Helper function to save/serialize a whole array of CrLoadables
template<typename Type>
static forceinline void SaveArray(Array<CrLoadable<Type>>& Target)
{
	for (auto& Elem : Target)
	{
		Elem.Save();
	}
}

//Helper function to save/serialize a whole array of CrLoadables
template<typename Type>
static forceinline void SaveArraySafe(Array<CrLoadable<Type>>& Target)
{
	for (auto& Elem : Target)
	{
		if (Elem.IsLoaded())
		{
			Elem.Save();
		}
	}
}

//Special serialize for loadables.
template<typename LoadableType>
inline static void operator <=>(CrArchive& Arch, CrLoadable<LoadableType>& ToSerialize)
{
	Arch <=> _CrLoadableAccessor::Access<LoadableType>(ToSerialize);
}
