#pragma once

#include "CrManagedObject.h"
#include "CrComponent.h"

//Destroying this object will destroy live owned assets referenced by it.
//All assets contained in this object will be saved into the saved level file
class CrVerse : public CrManagedObject
{
	DEF_CLASS(CrVerse, CrManagedObject);

	virtual ~CrVerse();

	virtual void BinSerialize(CrArchive& Arch) override;

	//Objects that are loaded with the universe
	Array<SP<CrManagedObject>> Objects;

	//Objects that are loaded while playing.
	Array<SP<CrManagedObject>> DynamicObjects;
};

