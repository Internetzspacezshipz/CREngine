#pragma once

#include "CRE_Serialization.hpp"
#include "CRE_SimpleHashes.h"

#include <functional>
#include <inttypes.h>
#include <iostream>
#include <string>
#include <map>
#include <set>

//Individual objects get an id.
typedef uint32_t ObjGUID;

//Each class gets an ID as well, which will be used by serialization to make instances of the class.
typedef uint32_t ClassGUID;

//Virtual base class for all gameplay related classes to inherit from.
class CRE_ManagedObject : public CRE_SerializerInterface
{
protected:
	ObjGUID ID;

	//Default move.
	CRE_ManagedObject(CRE_ManagedObject&&) = default;
	CRE_ManagedObject& operator=(CRE_ManagedObject&&) = default;

	//Delete copy.
	CRE_ManagedObject(const CRE_ManagedObject&) = delete;
	CRE_ManagedObject& operator=(const CRE_ManagedObject&) = delete;

public:
	//Implementation of StaticClass() and GetClass() for the class system to work properly.
	static ClassGUID StaticClass();

	virtual ClassGUID GetClass() const { return 0; }
																																 
	CRE_ManagedObject(const ObjGUID& InObjGUID) : ID(InObjGUID) {}
	virtual ~CRE_ManagedObject() {}

	ObjGUID GetId() const { return ID; }

	//The "Constructor", which we call in the actual constructor.
	virtual void Construct() {};

	// Inherited via CRE_SerializerInterface
	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override {};
};

class CRE_ClassBase;

class CRE_ObjectFactory
{
	//constructors for each class type.
	std::map<ClassGUID, std::function<CRE_ManagedObject* (const ObjGUID&)>> ClassCreators;
	//Info objects about the layout of the class inheritance.
	std::map<ClassGUID, CRE_ClassBase*> ClassInfos;
	//Creation index. Not really important right now honestly.
	ObjGUID CurrentIndex = 0;

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
		return (it->second)(++CurrentIndex);
	}

	//Templated create.
	template<typename Class>
	Class* Create()
	{
		return reinterpret_cast<Class*>(Create(Class::StaticClass()));
	}

	CRE_ClassBase* GetClass(ClassGUID ClassID)  { return ClassInfos[ClassID]; }
};

//Contains info about what classes are derived, and what the parent class is.
class CRE_ClassBase
{
protected:
	CRE_ClassBase* Parent = nullptr;
	ClassGUID ThisGUID = 0;
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
	void RegisterParent()
	{
		//Parent must be 0 when this is called.
		assert(Parent == 0);
		Parent = &CRE_Class<T>::Get();
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
	explicit CRE_Registrar(ClassGUID ClassID)
	{
		CRE_ObjectFactory::Get().RegisterClass<New>(ClassID);
		//Must register the new class's parent (Base).
		CRE_Class<New>::Get().RegisterParent<Base>();
	}
};

//Def Class should be done within the header of each class, inside the class definition.
#define DEF_CLASS(NEW_CLASS_NAME, BASE_CLASS_NAME) public:																			\
static ClassGUID StaticClass()																										\
{ 																																	\
	static ClassGUID ConcreteClassGUID = crc32_CONST(""#NEW_CLASS_NAME"", sizeof(""#NEW_CLASS_NAME""));								\
	return ConcreteClassGUID; 																										\
}																																	\
virtual ClassGUID GetClass() const override																							\
{ 																																	\
	static ClassGUID ConcreteClassGUID = NEW_CLASS_NAME::StaticClass();																\
	return ConcreteClassGUID; 																										\
}																																	\
NEW_CLASS_NAME(const ObjGUID& InObjGUID) : Super(InObjGUID)																			\
{																																	\
	Construct();																													\
}																																	\
typedef BASE_CLASS_NAME Super;

//Register class should be done inside the CPP file of each class.
#define REGISTER_CLASS(NEW_CLASS_NAME, BASE_CLASS_NAME) static CRE_Registrar<BASE_CLASS_NAME, NEW_CLASS_NAME> s_##NEW_CLASS_NAME##Creator{crc32_CONST(""#NEW_CLASS_NAME"", sizeof(""#NEW_CLASS_NAME""))};

//Dynamic Cast.
template<typename To>
To* DCast(CRE_ManagedObject* Object)
{
	if (Object != nullptr)
	{
		CRE_ClassBase* Class = CRE_ObjectFactory::GetClass(Object->GetClass());
		if (Class->IsChildOf(To::StaticClass()))
		{
			return reinterpret_cast<To*>(Object);
		}
	}
	return nullptr;
}