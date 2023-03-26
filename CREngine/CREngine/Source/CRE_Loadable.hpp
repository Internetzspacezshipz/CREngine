#pragma once
#include "CRE_ManagedObject.hpp"
#include "CRE_ID.hpp"
#include "CRE_Serialization.hpp"

//Shared pointer with an ID that can be loaded via json and create a new object for the shared pointer.
template<typename Type>
class CRE_Loadable : public CRE_SerializerInterface
{
	mutable SP<Type> LoadedObject;
	mutable CRE_ID Reference;

	void RefSync() const
	{
		if (LoadedObject)
		{
			Reference = LoadedObject->GetID();
		}
	}

public:
	void Load() const
	{
		//Nothing should be loaded at this point.
		assert(!LoadedObject && Reference.IsValidID());
		LoadedObject = DCast<Type>(CRE_Serialization::Get().Load(Reference));
	}

	void Reload() const
	{
		if (!LoadedObject)
		{
			Load();
		}
		else
		{
			LoadedObject = DCast<Type>(CRE_Serialization::Get().Reload(LoadedObject, Reference));
		}
	}

	void Save()
	{
		RefSync();
		assert(LoadedObject && Reference.IsValidID());
		CRE_Serialization::Get().Save(LoadedObject);
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
	forceinline bool IsLoadable() const				{ return HasValidID() ? CRE_Serialization::Get().Exists(Reference.GetString()) : false; }
	//The object exists - doesn't necessarily come from a file.
	forceinline bool IsLoaded() const				{ return LoadedObject.get(); }
	//The object exists or can be loaded
	forceinline bool IsLoadedOrLoadable() const		{ return IsLoaded() ? true : IsLoadable(); }
	//Gets the ID.
	forceinline CRE_ID GetID() const { RefSync(); return Reference; }

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
		Reference = InObject->GetID();
	}

	void Set(CRE_ID InRef)
	{
		//reset loaded object since it will no longer sync up.
		LoadedObject = nullptr;
		Reference = InRef;
	}


	// Inherited via CRE_SerializerInterface
	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override
	{
		RefSync();
		//Direct serialization - no name assumed.;
		VarSerialize(bSerializing, TargetJson, Reference);
	}

	CRE_Loadable() = default;
	CRE_Loadable(SP<Type> In) { Set(In); };
	CRE_Loadable(CRE_ID In) { Set(In); };

	//extra constructor that creates the shared pointer here if it is not already done.
	CRE_Loadable(CRE_ManagedObject* In) { Set(SP<Type>(In)); };

	Type* operator ->() const { return LoadedObject.get(); }
};

using CRE_LoadableBase = CRE_Loadable<CRE_ManagedObject>;

//Declare this type has a serializer.
template<typename Type>
struct Has_Serializer_Function<CRE_Loadable<Type>>
{
	constexpr static bool Value = true;
};

//Helper function to load a whole array of CRE_Loadables
template<typename Type>
static forceinline void LoadArray(Array<CRE_Loadable<Type>>& Target)
{
	//Clear the array before we load so we don't have doubling up of existing items.
	Target.clear();
	for (auto& Elem : Target)
	{
		Elem.Get<true>();
	}
	//Remove invalid objects here after loading.
	RemoveByPredicate(Target, [](const CRE_Loadable<Type>& Item) { return Item.Get() == nullptr; });
}

//Helper function to unload a whole array of CRE_Loadables
template<typename Type>
static forceinline void UnloadArray(Array<CRE_Loadable<Type>>& Target)
{
	for (auto& Elem : Target)
	{
		Elem.Unload();
	}
}

//Helper function to save/serialize a whole array of CRE_Loadables
template<typename Type>
static forceinline void SaveArray(Array<CRE_Loadable<Type>>& Target)
{
	for (auto& Elem : Target)
	{
		Elem.Save();
	}
}

//Helper function to save/serialize a whole array of CRE_Loadables
template<typename Type>
static forceinline void SaveArraySafe(Array<CRE_Loadable<Type>>& Target)
{
	for (auto& Elem : Target)
	{
		if (Elem.IsLoaded())
		{
			Elem.Save();
		}
	}
}