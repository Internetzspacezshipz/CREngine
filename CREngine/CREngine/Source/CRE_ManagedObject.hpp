#pragma once

#include "CRE_Serialization.hpp"

#include "CRE_Utilities.hpp"

#include "CRE_ID.hpp"


//Each class gets an ID as well, which will be used by serialization to make instances of the class.
typedef CRE_ID ClassGUID;

class CRE_ClassBase;

//Def Class should be done within the header of each class, inside the class definition.
#define DEF_CLASS(NEW_CLASS_NAME, BASE_CLASS_NAME) public:																			\
static ClassGUID StaticClass();																										\
virtual ClassGUID GetClass() const override;																						\
NEW_CLASS_NAME(const ObjGUID& InObjGUID) : Super(InObjGUID)																			\
{																																	\
	Construct();																													\
}																																	\
typedef BASE_CLASS_NAME Super;

//Register class should be done inside the CPP file of each class.
#define REGISTER_CLASS(NEW_CLASS_NAME, BASE_CLASS_NAME)																				\
static CRE_Registrar<BASE_CLASS_NAME, NEW_CLASS_NAME>s_##NEW_CLASS_NAME##Creator{""#NEW_CLASS_NAME""};								\
ClassGUID NEW_CLASS_NAME::StaticClass()																								\
{ 																																	\
	static ClassGUID ConcreteClassGUID = ClassGUID(""#NEW_CLASS_NAME"");															\
	return ConcreteClassGUID; 																										\
}																																	\
ClassGUID NEW_CLASS_NAME::GetClass() const																							\
{ 																																	\
	return NEW_CLASS_NAME::StaticClass(); 																							\
}

//Virtual base class for all gameplay related classes to inherit from.
class CRE_ManagedObject : public CRE_SerializerInterface
{
protected:
	CRE_ID ID;

public:

	//delete copy.
	CRE_ManagedObject(const CRE_ManagedObject&) = delete;
	CRE_ManagedObject& operator=(const CRE_ManagedObject&) = delete;

	//Default move.
	CRE_ManagedObject(CRE_ManagedObject&&) = delete;
	CRE_ManagedObject& operator=(CRE_ManagedObject&&) = delete;

	//Implementation of StaticClass(), GetClass() and GetClassObj() for the class system to work properly.
	static ClassGUID StaticClass();
	virtual ClassGUID GetClass() const;
	CRE_ClassBase* GetClassObj() const;

	CRE_ManagedObject(const ObjGUID& InObjGUID) : ID(InObjGUID) {}
	virtual ~CRE_ManagedObject() {}

	ObjGUID GetID() const { return ID; }

	void Rename(const ObjGUID& In);

	//The "Constructor", which we call in the actual constructor.
	virtual void Construct() {};

	// Inherited via CRE_SerializerInterface
	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;
};

//Got sick of writing the whole thing every time

typedef SP<CRE_ManagedObject> Object_sp;
typedef WP<CRE_ManagedObject> Object_wp;

typedef CRE_ManagedObject CRE_Obj;

//Contains info about what classes are derived, and what the parent class is.
class CRE_ClassBase
{
protected:
	CRE_ClassBase* Parent = nullptr;
	Set<CRE_ClassBase*> Children;
	ClassGUID ThisGUID;
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

	ClassGUID GetClassGUID() const { return ThisGUID; }
	Set<CRE_ClassBase*> GetChildren() const { return Children; }
	std::string GetClassFriendlyName() const { return ThisGUID.GetString(); }
};

class CRE_ObjectFactory
{
	//constructors for each class type.
	Map<ClassGUID, std::function<SP<CRE_ManagedObject>(const ObjGUID&)>> ClassCreators;
	//Info objects about the layout of the class inheritance.
	Map<ClassGUID, CRE_ClassBase*> ClassInfos;
	//Creation index. Not really important right now honestly.

public:

	static CRE_ObjectFactory& Get();

	template<class T>
	void RegisterClass(ClassGUID ClassID)
	{
		ClassCreators.insert({ ClassID, [](const ObjGUID& Name)->SP<CRE_ManagedObject> { return DCast<CRE_ManagedObject>(std::make_shared<T>(Name)); } });
		ClassInfos.insert({ ClassID, &CRE_Class<T>::Get() });
	}

	SP<CRE_ManagedObject> Create(ClassGUID ClassID)
	{
		const auto it = ClassCreators.find(ClassID);
		if (it == ClassCreators.end())
		{
			return nullptr; // not a derived class
		}

		std::string UniqueString = CRE_ObjectIDRegistry::CreateUniqueString(ClassInfos[it->first]->GetClassFriendlyName());
		return (it->second)(CRE_ID(UniqueString));
	}

	//Templated create.
	template<typename Class>
	SP<Class> Create()
	{
		SP<Class> NewItem = DCast<Class>(Create(Class::StaticClass()));
		assert(NewItem);
		return NewItem;
	}

	Map<ClassGUID, CRE_ClassBase*>& GetClassInfos() { return ClassInfos; };

	CRE_ClassBase* GetClass(ClassGUID ClassID)  { return ClassInfos[ClassID]; }
	template<typename Type>
	CRE_ClassBase* GetClass()  { return ClassInfos[Type::StaticClass()]; }
};

template<class NativeClass>
class CRE_Class : public CRE_ClassBase
{
public:

	static CRE_Class<NativeClass>& Get()
	{
		static CRE_Class<NativeClass> s_instance;
		return s_instance;
	}

	template<class T>
	CRE_Class<NativeClass>& RegisterParent()
	{
		//Parent must be 0 when this is called.
		assert(Parent == 0);
		Parent = &CRE_Class<T>::Get();
		return *this;
	}

	template<class T>
	CRE_Class<NativeClass>& RegisterChild()
	{
		Children.emplace(&CRE_Class<T>::Get());
		return *this;
	}

	CRE_Class<NativeClass>& SetClassID(const CRE_ID& InID)
	{
		//ClassName must be 0 when this is called.
		assert(ThisGUID.IsValidID() == false);
		ThisGUID = InID;
		return *this;
	}

	SP<NativeClass> Create()
	{
		return CRE_ObjectFactory::Get().Create<NativeClass>();
	}
};

//Creates the class itself.
template<class Base, class New>
class CRE_Registrar
{
public:
	explicit CRE_Registrar(const std::string& FriendlyClassName)
	{
		CRE_ID NewID = CRE_ID(FriendlyClassName);
		CRE_ObjectFactory::Get()
			.RegisterClass<New>(NewID);
		//Must register the new class's parent (Base).
		CRE_Class<Base>::Get()
			.RegisterChild<New>();
		CRE_Class<New>::Get()
			.RegisterParent<Base>()
			.SetClassID(NewID);
	}
};

//For registering the absolute base class.
template<class New>
class CRE_Registrar<void, New>
{
public:
	explicit CRE_Registrar(const std::string& FriendlyClassName)
	{
		CRE_ID NewID = CRE_ID(FriendlyClassName);
		CRE_ObjectFactory::Get()
			.RegisterClass<New>(NewID);
		CRE_Class<New>::Get()
			.SetClassID(NewID);
	}
};

//Dynamic Cast.
template<typename To>
To* DCast(CRE_ManagedObject* Object)
{
	if (Object != nullptr)
	{
		CRE_ClassBase* FromClass = CRE_ObjectFactory::Get().GetClass(Object->GetClass());
		CRE_ClassBase* ToClass = CRE_ObjectFactory::Get().GetClass(To::StaticClass());
		if (ToClass->IsChildOf(FromClass->GetClassGUID()))
		{
			return reinterpret_cast<To*>(Object);
		}
	}
	return nullptr;
}

//PROBL:EM WITH DYNAMIC CASTS = PLS FIX



//Dynamic Casts for smart pointers:
//template<typename To, typename From>
//static SP<To> DCast(SP<From> Object)
//{
//	return SP<To>(DCast<To>(Object.get()));
//}
//
//template<typename To, typename From>
//static WP<To> DCast(WP<From> Object)
//{
//	return WP<To>(DCast<To>(Object.lock()));
//}


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



#if 1

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

//using move
#else

//Upcasting
//Use some new fancy C++20 shit to determine upcasting.
template<typename To, typename From> requires (std::is_base_of_v<To, From> && !std::is_same_v<To, From>)
static SP<To> DCast(SP<From>& Object)
{
	return SP<To>(std::move(Object), Object.get());
}

template<typename To, typename From> requires (std::is_base_of_v<To, From> && !std::is_same_v<To, From>)
static WP<To> DCast(WP<From>& Object)
{
	return WP<To>(std::move(Object), Object.lock());
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
		return SP<To>(std::move(Object), Item);
	}
	return SP<To>();
}

template<typename To, typename From> requires (std::is_base_of_v<From, To> && !std::is_same_v<To, From>)
static WP<To> DCast(WP<From>& Object)
{
	To* Item = DCast<To>(Object.lock());
	if (Item)
	{
		return WP<To>(std::move(Object), Item);
	}
	return WP<To>();
}
#endif