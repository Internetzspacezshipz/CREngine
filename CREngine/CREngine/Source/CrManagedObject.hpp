#pragma once

//#include "CrSerialization.hpp"
#include "CrSerializationBin.h"

#include "CrUtilities.hpp"

#include "CrID.hpp"


//Each class gets an ID as well, which will be used by serialization to make instances of the class.
typedef CrID ClassGUID;

class CrClass;

//Def Class should be done within the header of each class, inside the class definition.
#define DEF_CLASS(NEW_CLASS_NAME, BASE_CLASS_NAME) public:																			\
static ClassGUID StaticClass();																										\
virtual ClassGUID GetClass() const override;																						\
NEW_CLASS_NAME(const ObjGUID& InObjGUID) : Super(InObjGUID)																			\
{																																	\
	Construct();																													\
}																																	\
typedef BASE_CLASS_NAME Super;


//Version of RegisterClass that also takes in flags.
#define REGISTER_CLASS_FLAGS(NEW_CLASS_NAME, ...)																												\
static CrRegistrar<NEW_CLASS_NAME::Super, NEW_CLASS_NAME, #NEW_CLASS_NAME>__s_##NEW_CLASS_NAME##Creator{FlagCombiner<CrClassFlags>::Combine<__VA_ARGS__>::V};	\
ClassGUID NEW_CLASS_NAME::StaticClass()																															\
{ 																																								\
	return CrID::Constant<#NEW_CLASS_NAME>(); 																													\
}																																								\
ClassGUID NEW_CLASS_NAME::GetClass() const																														\
{ 																																								\
	return NEW_CLASS_NAME::StaticClass(); 																														\
}


//Register class should be done inside the CPP file of each class.
#define REGISTER_CLASS(NEW_CLASS_NAME) REGISTER_CLASS_FLAGS(NEW_CLASS_NAME, CrClassFlags_None)


//Virtual base class for all classes to inherit from.
//Must be set up by:
//1. inheriting from an object that inherits from CrManagedObject or is CrManagedObject itself.
//2. use DEF_CLASS macro in the body of the new class (preferrably at the top).
//3. use the REGISTER_CLASS macro in the cpp implementation file.
class CrManagedObject : public CrBinSerializable
{
	friend class CrSerialization;
	CrID ID;

public:
	//typedef super as void to make sure it follows the normal pattern for managed objects.
	typedef void Super;

	//delete copy.
	CrManagedObject(const CrManagedObject&) = delete;
	CrManagedObject& operator=(const CrManagedObject&) = delete;

	//Default move.
	CrManagedObject(CrManagedObject&&) = delete;
	CrManagedObject& operator=(CrManagedObject&&) = delete;

	//Implementation of StaticClass(), GetClass() and GetClassObj() for the class system to work properly.
	static ClassGUID StaticClass();
	virtual ClassGUID GetClass() const;
	CrClass* GetClassObj() const;

	CrManagedObject(const ObjGUID& InObjGUID) : ID(InObjGUID) {}
	virtual ~CrManagedObject() {}

	ObjGUID GetID() const { return ID; }

	void Rename(const ObjGUID& In);
	virtual void OnRename() {};

	CrAssetReference MakeAssetReference() const
	{
		CrAssetReference Out;
		Out.AssetID = ID;
		Out.ClassID = GetClass();//Get actual class type.
		return Out;
	}

	//The "Constructor", which we call in the actual constructor.
	virtual void Construct() {};

	// Inherited via CrSerializerInterface
	virtual void BinSerialize(CrArchive& Data) override;
};

//Class flags that adjust how this class can be used.
enum CrClassFlags : uint32_t
{
	CrClassFlags_None = 0,
	CrClassFlags_HAS_BEEN_SET = 1 << 0, //The flags have already been set for this class - do not allow them to be set again or crash.
	CrClassFlags_Unique = 1 << 1, //When loading this object, we do not want to make more than one instance ever.
	CrClassFlags_Transient = 1 << 2, //Can never save this asset.
	CrClassFlags_UNUSED = 1 << 3, //NOT USED YET.
};

//Contains info about what classes are derived, and what the parent class is.
class CrClass
{
protected:
	CrClass* Parent = nullptr;
	Set<CrClass*> Children;
	ClassGUID ThisGUID;
	CrClassFlags ClassFlags = CrClassFlags_None;
public:
	bool IsChildOf(ClassGUID ClassID) const
	{
		if (ThisGUID == ClassID)
		{
			return true;
		}
		if (Parent)
		{
			//Recurse.
			return Parent->IsChildOf(ClassID);
		}
		return false;
	}

	bool IsSameAs(ClassGUID ClassID) const
	{
		return ThisGUID == ClassID;
	}

	bool HasFlag(CrClassFlags Flag) const
	{
		return ClassFlags & Flag;
	}

	ClassGUID GetClassGUID() const { return ThisGUID; }
	Set<CrClass*> GetChildren() const { return Children; }
	StringV GetClassName() const { return ThisGUID.GetString(); }
	StringV GetClassPrettyName() const { return ThisGUID.GetStringPretty(); }
};

class CrObjectFactory
{
	//constructors for each class type.
	Map<ClassGUID, std::function<SP<CrManagedObject>(const ObjGUID&)>> ClassCreators;
	//Info objects about the layout of the class inheritance.
	Map<ClassGUID, CrClass*> ClassInfos;

public:

	static CrObjectFactory& Get();

	template<class T>
	void RegisterClass(const ClassGUID& ClassID)
	{
		CrLOG("Register Class : Index: <%ul>   Name: <%s>     x", ClassID.GetNumber(), String(ClassID.GetString()).c_str());
		ClassCreators.insert({ ClassID, [](const ObjGUID& Name)->SP<CrManagedObject> { return DCast<CrManagedObject>(std::make_shared<T>(Name)); } });
		ClassInfos.insert({ ClassID, &CrClassConcrete<T>::Get() });
	}

	SP<CrManagedObject> Create(const ClassGUID& ClassID, CrID Name = CrID())
	{
		const auto it = ClassCreators.find(ClassID);
		if (it == ClassCreators.end())
		{
			return nullptr; // not a derived class
		}
		if (!Name.IsValidID())
		{
			Name = CrObjectIDRegistry::CreateUniqueID(String(ClassInfos[it->first]->GetClassName()));
		}

		SP<CrManagedObject> NewOb = (it->second)(Name);
		//If we want to save a list of all objects in the future, this is the spot to do it.
		return NewOb;
	}

	//Templated create.
	template<typename Class>
	SP<Class> Create(CrID Name = CrID())
	{
		SP<Class> NewItem = DCast<Class>(Create(Class::StaticClass(), Name));
		assert(NewItem);
		return NewItem;
	}

	template<typename Class>
	SP<Class> Create(CrAssetReference Ref)
	{
		SP<Class> NewItem = DCast<Class>(Create(Class::StaticClass(), Ref.AssetID));
		assert(NewItem);

		if (Ref.ClassID.IsValidID())
		{
			assert(Ref.ClassID == Class::StaticClass());
		}

		return NewItem;
	}

	Map<ClassGUID, CrClass*>& GetClassInfos() { return ClassInfos; };

	CrClass* GetClass(ClassGUID ClassID)  { return ClassInfos[ClassID]; }
	template<typename Type>
	CrClass* GetClass()  { return ClassInfos[Type::StaticClass()]; }
};

template<class NativeClass>
class CrClassConcrete : public CrClass
{
public:

	static CrClassConcrete<NativeClass>& Get()
	{
		static CrClassConcrete<NativeClass> s_instance;
		return s_instance;
	}

	template<class T>
	CrClassConcrete<NativeClass>& RegisterParent()
	{
		//Parent must be 0 when this is called.
		assert(Parent == 0);
		Parent = &CrClassConcrete<T>::Get();
		return *this;
	}

	CrClassConcrete<NativeClass>& SetClassID(const CrID& InID)
	{
		//ClassName must be 0 when this is called.
		assert(ThisGUID.IsValidID() == false);
		ThisGUID = InID;
		return *this;
	}

	CrClassConcrete<NativeClass>& SetClassFlags(const CrClassFlags& InFlags)
	{
		//Must not allow setting of class flags more than once.
		assert(HasFlag(CrClassFlags_HAS_BEEN_SET) == false);
		ClassFlags = (CrClassFlags)(InFlags | CrClassFlags_HAS_BEEN_SET);
		return *this;
	}

	template<class T>
	CrClassConcrete<NativeClass>& RegisterChild()
	{
		Children.emplace(&CrClassConcrete<T>::Get());
		return *this;
	}

	SP<NativeClass> Create()
	{
		return CrObjectFactory::Get().Create<NativeClass>();
	}
};

//Creates the class itself.
template<class Base, class New, StringLiteral LiteralString>
class CrRegistrar
{
public:
	explicit CrRegistrar(CrClassFlags Flags = CrClassFlags_None)
	{
		CrID NewID = CrID::Constant<LiteralString>();
		CrObjectFactory::Get()
			.RegisterClass<New>(NewID);
		//Must register the new class's parent (Base).
		CrClassConcrete<Base>::Get()
			.RegisterChild<New>();
		//Set up our new class.
		CrClassConcrete<New>::Get()
			.RegisterParent<Base>()
			.SetClassID(NewID)
			.SetClassFlags(Flags);
	}
};

//For registering the absolute base class.
template<class New, StringLiteral LiteralString>
class CrRegistrar<void, New, LiteralString>
{
public:
	explicit CrRegistrar(CrClassFlags Flags = CrClassFlags_None)
	{
		CrID NewID = CrID::Constant<LiteralString>();
		CrObjectFactory::Get()
			.RegisterClass<New>(NewID);
		CrClassConcrete<New>::Get()
			.SetClassID(NewID)
			.SetClassFlags(Flags);
	}
};

//Dynamic Cast.
template<typename To>
To* DCast(CrManagedObject* Object)
{
	if (Object != nullptr)
	{
		CrClass* FromClass = CrObjectFactory::Get().GetClass(Object->GetClass());
		CrClass* ToClass = CrObjectFactory::Get().GetClass(To::StaticClass());

		if (FromClass->IsChildOf(ToClass->GetClassGUID()))
		{
			return reinterpret_cast<To*>(Object);
		}
	}
	return nullptr;
}

//Dynamic casts for smart pointers:

//Upcasting
//Use some new fancy C++20 shit to determine upcasting.
template<typename To, typename From> requires (std::is_base_of_v<To, From> && !std::is_same_v<To, From>)
static SP<To> DCast(SP<From>&& Object)
{
	return SP<To>(std::move(Object), Object.get());
}

template<typename To, typename From> requires (std::is_base_of_v<To, From> && !std::is_same_v<To, From>)
static WP<To> DCast(WP<From>&& Object)
{
	return WP<To>(std::move(Object), Object.lock());
}

//Same type - no need to do anything.
template<typename To, typename From> requires (std::is_same_v<To, From>)
static SP<To> DCast(SP<From>&& Object)
{
	return Object;
}

template<typename To, typename From> requires (std::is_same_v<To, From>)
static WP<To> DCast(WP<From>&& Object)
{
	return Object;
}

//Downcasting.
template<typename To, typename From> requires (std::is_base_of_v<From, To> && !std::is_same_v<To, From>)
static SP<To> DCast(SP<From>&& Object)
{
	To* Item = DCast<To>(Object.get());
	if (Item)
	{
		return SP<To>(std::move(Object), Item);
	}
	return SP<To>();
}

template<typename To, typename From> requires (std::is_base_of_v<From, To> && !std::is_same_v<To, From>)
static WP<To> DCast(WP<From>&& Object)
{
	To* Item = DCast<To>(Object.lock());
	if (Item)
	{
		return WP<To>(std::move(Object), Item);
	}
	return WP<To>();
}

//Upcasting
//Use some new fancy C++20 shit to determine upcasting.
template<typename To, typename From> requires (std::is_base_of_v<To, From> && !std::is_same_v<To, From>)
static SP<To> DCast(SP<From>& Object)
{
	return SP<To>(Object, Object.get());
}

template<typename To, typename From> requires (std::is_base_of_v<To, From> && !std::is_same_v<To, From>)
static WP<To> DCast(WP<From>& Object)
{
	return WP<To>(Object, Object.lock());
}

//Same type - no need to do anything.
template<typename To, typename From> requires (std::is_same_v<To, From>)
static SP<To> DCast(SP<From>& Object)
{
	return Object;
}

template<typename To, typename From> requires (std::is_same_v<To, From>)
static WP<To> DCast(WP<From>& Object)
{
	return Object;
}

//Downcasting.
template<typename To, typename From> requires (std::is_base_of_v<From, To> && !std::is_same_v<To, From>)
static SP<To> DCast(SP<From>& Object)
{
	To* Item = DCast<To>(Object.get());
	if (Item)
	{
		return SP<To>(Object, Item);
	}
	return SP<To>();
}

template<typename To, typename From> requires (std::is_base_of_v<From, To> && !std::is_same_v<To, From>)
static WP<To> DCast(WP<From>& Object)
{
	To* Item = DCast<To>(Object.lock());
	if (Item)
	{
		return WP<To>(Object, Item);
	}
	return WP<To>();
}
