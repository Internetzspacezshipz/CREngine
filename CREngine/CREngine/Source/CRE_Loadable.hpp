#pragma once
#include "CRE_ManagedObject.hpp"
#include "CRE_ID.hpp"
#include "CRE_Serialization.hpp"

//Shared pointer with an ID that can be loaded via json and create a new object for the shared pointer.
class CRE_Loadable : public CRE_SerializerInterface
{
	mutable Object_sp LoadedObject;
	CRE_ID Reference;

	void RefSync();

public:
	void Load() const;
	void Save();
	//Does not guarntee that the object was unloaded, only that the reference count goes down by one.
	void Unload();

	//Gets the loaded object
	Object_sp Get(bool bShouldLoad = false) const;

	//templated get, since it avoids branching if set to false
	template<bool bShouldLoad>
	Object_sp Get() const
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
	void Set(Object_sp InObject);
	void Set(CRE_ID InRef);

	// Inherited via CRE_SerializerInterface
	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;
	CRE_Loadable() = default;
	CRE_Loadable(Object_sp In) { Set(In); };
	CRE_Loadable(CRE_ID In) { Set(In); };

	//extra constructor that creates the shared pointer here if it is not already done.
	CRE_Loadable(CRE_ManagedObject* In) { Set(Object_sp(In)); };
};

//Declare this type has a serializer.
template<>
struct Has_Serializer_Function<CRE_Loadable>
{
	constexpr static bool Value = true;
};

//Helper function to load a whole array of CRE_Loadables
static forceinline void LoadArray(Array<CRE_Loadable>& Target)
{
	for (auto& Elem : Target)
	{
		Elem.Get<true>();
	}
	//Remove invalid objects here after loading.
	RemoveByPredicate(Target, [](const CRE_Loadable& Item) { return Item.Get() == nullptr; });
}

//Helper function to unload a whole array of CRE_Loadables
static forceinline void UnloadArray(Array<CRE_Loadable>& Target)
{
	for (auto& Elem : Target)
	{
		Elem.Unload();
	}
}

//Helper function to save/serialize a whole array of CRE_Loadables
static forceinline void SaveArray(Array<CRE_Loadable>& Target)
{
	for (auto& Elem : Target)
	{
		Elem.Save();
	}
}