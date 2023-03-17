#pragma once

#include "CRE_Mesh.hpp"
#include "CRE_Serialization.hpp"
#include "CRE_SimpleHashes.h"

#include <functional>
#include <inttypes.h>
#include <iostream>
#include <string>
#include <map>

//Individual objects get an id.
typedef uint32_t ObjGUID;

//Each class gets an ID as well, which will be used by serialization to make instances of the class.
typedef uint32_t ClassGUID;

class CRE_ManagedObject : public CRE_SerializerInterface
{
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

	std::shared_ptr<CRE_Mesh> MeshObject;
	CRE_Transform Transform{};

	// Inherited via CRE_SerializerInterface
	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;
};

template<class B>
class CRE_ManagedObjectFactory
{
	std::map<ClassGUID, std::function<B*(const ObjGUID&)>> ClassCreators;
	//Creation index. Not really important right now honestly.
	ObjGUID CurrentIndex = 0;

public:
	static CRE_ManagedObjectFactory<B>& Get()
	{
		static CRE_ManagedObjectFactory<B> s_instance;
		return s_instance;
	}

	template<class T>
	void RegisterClass(ClassGUID ClassID)
	{
		ClassCreators.insert( { ClassID, [](const ObjGUID& Index)->B*{ return new T(Index); } } );
	}

	B* Create(ClassGUID ClassID)
	{
		const auto it = ClassCreators.find(ClassID);
		if (it == ClassCreators.end())
		{
			return nullptr; // not a derived class
		}
		return (it->second)(++CurrentIndex);
	}
};

typedef CRE_ManagedObjectFactory<CRE_ManagedObject> CRE_ObjectFactory;

template<class B, class T>
class CRE_Creator
{
public:
	explicit CRE_Creator(ClassGUID ClassID)
	{
		CRE_ManagedObjectFactory<B>::Get().RegisterClass<T>(ClassID);
	}
};

#define DEF_CLASS(BASE_CLASS_NAME, NEW_CLASS_NAME) public:																			\
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
typedef BASE_CLASS_NAME Super;

#define REGISTER_CLASS(BASE_CLASS_NAME, NEW_CLASS_NAME) static CRE_Creator<BASE_CLASS_NAME, NEW_CLASS_NAME> s_##NEW_CLASS_NAME##Creator{crc32_CONST(""#NEW_CLASS_NAME"", sizeof(NEW_CLASS_NAME))};
