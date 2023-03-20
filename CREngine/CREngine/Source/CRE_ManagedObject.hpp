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

	CRE_ManagedObject(const ObjGUID& InObjGUID) : ID(InObjGUID) {}
	virtual ~CRE_ManagedObject() {}

	ObjGUID GetId() const { return ID; }
	virtual ClassGUID GetClass() const { return 0; }

	//The "Constructor", which we call in the actual constructor.
	virtual void Construct() {};

	// Inherited via CRE_SerializerInterface
	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override {};
};

template<class NativeClass>
class CRE_Class;


template<>
class CRE_Class<CRE_ManagedObject>
{
	std::map<ClassGUID, std::function<CRE_ManagedObject* (const ObjGUID&)>> ClassCreators;
	//Creation index. Not really important right now honestly.
	ObjGUID CurrentIndex = 0;

public:
	static CRE_Class<CRE_ManagedObject>& Get()
	{
		static CRE_Class<CRE_ManagedObject> s_instance;
		return s_instance;
	}

	template<class T>
	void RegisterClass(ClassGUID ClassID)
	{
		ClassCreators.insert({ ClassID, [](const ObjGUID& Index)->CRE_ManagedObject* { return new T(Index); } });
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

	bool ContainsClass(ClassGUID ClassID)
	{
		return ClassCreators.contains(ClassID);
	}
};

template<class NativeClass>
class CRE_Class
{
	std::set<ClassGUID> DerivedClasses;
public:

	static CRE_Class<NativeClass>& Get()
	{
		static CRE_Class<NativeClass> s_instance;
		return s_instance;
	}

	template<class T>
	void RegisterClass(ClassGUID ClassID)
	{
		DerivedClasses.insert(ClassID);
	}

	NativeClass* Create(ClassGUID ClassID)
	{
		return reinterpret_cast<NativeClass*>(CRE_Class<CRE_ManagedObject>::Get().Create(ClassID));
	}

	NativeClass* Create()
	{
		return reinterpret_cast<NativeClass*>(CRE_Class<CRE_ManagedObject>::Get().Create(NativeClass::StaticClass()));
	}

	bool ContainsClass(ClassGUID ClassID)
	{
		if (NativeClass::StaticClass() == ClassID)
		{
			return true;
		}
		return DerivedClasses.contains(ClassID);
	}
};

//Base type for all object classes.
typedef CRE_Class<CRE_ManagedObject> CRE_ObjectFactory;

//Creates the class itself.
template<class Base, class New>
class CRE_Registrar
{
public:
	explicit CRE_Registrar(ClassGUID ClassID)
	{
		CRE_Class<Base>::Get().RegisterClass<New>(ClassID);
		//Must register with the child class manager as well in order to store type trees.
		CRE_Class<New>::Get().RegisterClass<New>(ClassID);
	}
};

#define DEF_CLASS(NEW_CLASS_NAME, BASE_CLASS_NAME) public:																			\
static ClassGUID StaticClass()																										\
{ 																																	\
	static ClassGUID ConcreteClassGUID = crc32_CONST(""#NEW_CLASS_NAME"", sizeof(NEW_CLASS_NAME));									\
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

#define REGISTER_CLASS(NEW_CLASS_NAME, BASE_CLASS_NAME) static CRE_Registrar<BASE_CLASS_NAME, NEW_CLASS_NAME> s_##NEW_CLASS_NAME##Creator{crc32_CONST(""#NEW_CLASS_NAME"", sizeof(NEW_CLASS_NAME))};

//Dynamic Cast.
template<typename To>
To* DCast(CRE_ManagedObject* Object)
{
	if (Object != nullptr)
	{
		if (CRE_Class<To>::Get().ContainsClass(Object->GetClass()))
		{
			return reinterpret_cast<To*>(Object);
		}
	}
	return nullptr;
}