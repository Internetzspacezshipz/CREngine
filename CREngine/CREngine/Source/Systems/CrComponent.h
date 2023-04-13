#pragma once

#include "CrManagedObject.h"

//CAUTION - NOT TESTED.

template<typename ComponentType, StringLiteral Name>
struct CrComponent;

//Component base type - don't create this, use the template instead.
struct CrComponentBase
{
protected:
	//Make friend with the serialize operator
	template<typename CompType, StringLiteral Name>
	friend void operator <=>(CrArchive& Arch, CrComponent<CompType, Name>& ToSerialize);
	SP<CrManagedObject>Component;

public:
	SP<CrManagedObject> Get() const 
	{
		return Component;
	}
};

//A component whose liftime is completely controlled by its owner.
template<typename ComponentType, StringLiteral Name>
struct CrComponent : public CrComponentBase
{
	ComponentType* operator->()
	{
		return static_cast<ComponentType*>(Component.get());
	}

	const ComponentType* operator->() const
	{
		return static_cast<ComponentType*>(Component.get());
	}

	CrComponent()
	{
		Component = CrObjectFactory::Get().Create(ComponentType::StaticClass(), CrID::Constant<Name>());
	}
};

template<typename CompType, StringLiteral Name>
__forceinline static void operator <=>(CrArchive& Arch, CrComponent<CompType, Name>& ToSerialize)
{
	//Simply forward the serialization to the object.
	ToSerialize.Component->BinSerialize(Arch);
}
