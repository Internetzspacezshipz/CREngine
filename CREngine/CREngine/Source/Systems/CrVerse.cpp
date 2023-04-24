#include "CrVerse.h"
#include "CrSerialization.h"

REGISTER_CLASS(CrVerse);

REGISTER_EXTENSION(CrVerse, ".crlvl");

CrVerse::~CrVerse()
{
	Objects.clear();
	DynamicObjects.clear();
}

void CrVerse::BinSerialize(CrArchive& Arch)
{
	if (Arch.bSerializing)
	{
		//Remove all nullptrs.
		RemoveByPredicate(Objects, [](SP<CrManagedObject> Ob) { return Ob == nullptr; });

		uint32_t Size = Objects.size();
		Arch <=> Size;

		for (auto& Elem : Objects)
		{
			CrID Class = Elem->GetClass();
			CrID InstanceID = Elem->GetID();

			Arch <=> Class;
			Arch <=> InstanceID;

			Elem->BinSerialize(Arch);
		}
	}
	else
	{
		uint32_t Size = 0;
		Arch <=> Size;

		Objects.reserve(Size);
		CrObjectFactory& ObFact = CrObjectFactory::Get();
		for (uint32_t Index = 0; Index < Size; Index++)
		{
			CrID Class;
			CrID InstanceID;

			Arch <=> Class;
			Arch <=> InstanceID;

			SP<CrManagedObject> NewOb = Objects.emplace_back(ObFact.Create(Class, InstanceID));

			NewOb->BinSerialize(Arch);
		}
	}
}
