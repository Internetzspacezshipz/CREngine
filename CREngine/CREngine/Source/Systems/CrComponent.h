#pragma once

#include "CrManagedObject.h"

//CAUTION - NOT TESTED.

//Component base type - don't create this, use the template instead.
class CrComponentBase
{
protected:
	//Make friend with the serialize operator
	friend void operator <=>(CrArchive& Arch, CrComponentBase& ToSerialize);
	UP<CrManagedObject>Component;
};

//A component whose liftime is completely controlled by its owner.
template<typename ComponentType, StringLiteral Name>
class CrComponent : public CrComponentBase
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
		Component = MkUP(CrObjectFactory::Get().Create(ComponentType::StaticClass(), CrID::Constant<Name>()));
	}
};

__forceinline static void operator <=>(CrArchive& Arch, CrComponentBase& ToSerialize)
{
	//In between
	ToSerialize.Component->BinSerialize(Arch);
}
