#pragma once

#include "CRE_Serialization.hpp"
#include "CRE_SimpleHashes.h"

#include <functional>
#include <inttypes.h>
#include <iostream>
#include <string>
#include <map>
#include <set>


//Individual objects get an id - it's really big too.

typedef uint32_t IDNum_t;

//Json value that will contain the class for objects
#define CLASS_JSON_VALUE "_CLASS_ID_TYPE_"

class CRE_ObjectIDRegistry
{
	friend class CRE_ObjID;
	friend class CRE_ObjectFactory;


	static std::unordered_map<IDNum_t, std::string>& GetMap();
	static std::string CreateUniqueString(const std::string& In);
};

//Todo: finish this class... It probably won't work well right now.
class CRE_ObjID
{
	IDNum_t Number = 0;
	bool bHasBeenSet = false;
public:
	std::string GetString() const;
	IDNum_t GetNumber() const { return Number; }

	bool IsValidID() const;

	CRE_ObjID& operator = (const CRE_ObjID& CopyFrom);
	CRE_ObjID& operator = (const std::string& MakeFrom)
	{
		return *this = CRE_ObjID(MakeFrom);
	}
	bool operator == (const CRE_ObjID& CompareWith) const { return Number == CompareWith.Number; }
	operator bool() const { return IsValidID(); };

	CRE_ObjID(std::string Name);
	CRE_ObjID();
};

//Implement std::hash for the object id so it can be used in maps.
namespace std
{
	template <>
	struct hash<CRE_ObjID>
	{
		std::size_t operator()(const CRE_ObjID& Item) const
		{
			auto out = Item.GetNumber();
			return out;
		}
	};
}

typedef CRE_ObjID ObjGUID;

template<>
struct Has_Serializer_Function<CRE_ObjID>
{
	constexpr static bool Value = true;
};

//manual serialize code for CRE_ObjID
static void VarSerialize(bool bSerializing, nlohmann::json& TargetJson, const std::string& VarName, CRE_ObjID& Value)
{
	if (bSerializing)
	{
		TargetJson[VarName] = Value.GetString();
	}
	else if (TargetJson.contains(VarName))
	{
		Value = CRE_ObjID(TargetJson[VarName]);
	}
}



//Each class gets an ID as well, which will be used by serialization to make instances of the class.
typedef CRE_ObjID ClassGUID;

class CRE_ClassBase;

//Def Class should be done within the header of each class, inside the class definition.
#define DEF_CLASS(NEW_CLASS_NAME, BASE_CLASS_NAME) public:																			\
static ClassGUID StaticClass();																										\
CRE_ClassBase* GetClassObj() const;																									\
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
}																																	\
CRE_ClassBase* NEW_CLASS_NAME::GetClassObj() const																					\
{																																	\
	return CRE_ObjectFactory::Get().GetClass<NEW_CLASS_NAME>();																		\
}

//Virtual base class for all gameplay related classes to inherit from.
class CRE_ManagedObject : public CRE_SerializerInterface
{
protected:
	CRE_ObjID ID;

	//Default move.
	CRE_ManagedObject(CRE_ManagedObject&&) = default;
	CRE_ManagedObject& operator=(CRE_ManagedObject&&) = default;

	//Delete copy.
	CRE_ManagedObject(const CRE_ManagedObject&) = delete;
	CRE_ManagedObject& operator=(const CRE_ManagedObject&) = delete;

public:
	//Implementation of StaticClass(), GetClass() and GetClassObj() for the class system to work properly.
	static ClassGUID StaticClass();
	virtual ClassGUID GetClass() const;
	CRE_ClassBase* GetClassObj() const;

	CRE_ManagedObject(const ObjGUID& InObjGUID) : ID(InObjGUID) {}
	virtual ~CRE_ManagedObject() {}

	ObjGUID GetId() const { return ID; }
	void Rename(const ObjGUID& In) { ID = In; }

	//The "Constructor", which we call in the actual constructor.
	virtual void Construct() {};

	// Inherited via CRE_SerializerInterface
	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;
};

typedef std::shared_ptr<CRE_ManagedObject> Object_sp;
typedef std::weak_ptr<CRE_ManagedObject> Object_wp;

//Contains info about what classes are derived, and what the parent class is.
class CRE_ClassBase
{
protected:
	CRE_ClassBase* Parent = nullptr;
	std::set<CRE_ClassBase*> Children;
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
	std::set<CRE_ClassBase*> GetChildren() const { return Children; }
	std::string GetClassFriendlyName() const { return ThisGUID.GetString(); }
};

class CRE_ObjectFactory
{
	//constructors for each class type.
	std::unordered_map<ClassGUID, std::function<CRE_ManagedObject* (const ObjGUID&)>> ClassCreators;
	//Info objects about the layout of the class inheritance.
	std::unordered_map<ClassGUID, CRE_ClassBase*> ClassInfos;
	//Creation index. Not really important right now honestly.

public:

	static CRE_ObjectFactory& Get()
	{
		static CRE_ObjectFactory ObjFactory;
		return ObjFactory;
	}

	template<class T>
	void RegisterClass(ClassGUID ClassID)
	{
		ClassCreators.insert({ ClassID, [](const ObjGUID& Index)->CRE_ManagedObject* { return new T(Index); } });
		ClassInfos.insert({ ClassID, &CRE_Class<T>::Get() });
	}

	CRE_ManagedObject* Create(ClassGUID ClassID)
	{
		const auto it = ClassCreators.find(ClassID);
		if (it == ClassCreators.end())
		{
			return nullptr; // not a derived class
		}

		std::string UniqueString = CRE_ObjectIDRegistry::CreateUniqueString(ClassInfos[it->first]->GetClassFriendlyName());
		return (it->second)(CRE_ObjID(UniqueString));
	}

	//Templated create.
	template<typename Class>
	Class* Create()
	{
		return reinterpret_cast<Class*>(Create(Class::StaticClass()));
	}

	std::unordered_map<ClassGUID, CRE_ClassBase*>& GetClassInfos() { return ClassInfos; };

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

	CRE_Class<NativeClass>& SetClassID(const CRE_ObjID& InID)
	{
		//ClassName must be 0 when this is called.
		assert(ThisGUID.IsValidID() == false);
		ThisGUID = InID;
		return *this;
	}

	NativeClass* Create()
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
		CRE_ObjID NewID = CRE_ObjID(FriendlyClassName);
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
		CRE_ObjID NewID = CRE_ObjID(FriendlyClassName);
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
		CRE_ClassBase* Class = CRE_ObjectFactory::Get().GetClass(Object->GetClass());
		if (Class->IsChildOf(To::StaticClass()))
		{
			return reinterpret_cast<To*>(Object);
		}
	}
	return nullptr;
}