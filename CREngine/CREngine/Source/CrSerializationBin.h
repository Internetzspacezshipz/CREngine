#pragma once
#include <fstream>
#include <filesystem>
#include <assert.h>
#include "ThirdParty/archive/archive.h"

struct CrArchive : public tser::BinaryArchive
{
	std::fstream Stream;
	bool bSerializing;

	CrArchive(const std::string& Path, bool bIsSerializing)
		: bSerializing(bIsSerializing)
	{
		Stream = std::fstream{ Path, std::ios::binary | (bIsSerializing ? std::ios::out : std::ios::in) };
		if (!Stream.is_open())
		{
			assert(0);
		}
		if (!bSerializing)
		{
			Load();
		}
	}


	~CrArchive()
	{
		if (bSerializing)
		{
			Save();
		}
		Stream.close();
	}

	void Save() 
	{
		Stream << this;
	};

	void Load() 
	{
		*this << Stream;
	};
};

//use spaceship operator because I want to. Implement this to allow serialization of special classes.
template<typename T>
inline static void operator <=>(CrArchive& Arch, T& ToSerialize)
{
	if (Arch.bSerializing)
	{
		Arch.save(ToSerialize);
	}
	else
	{
		Arch.load(ToSerialize);
	}
}

//Specialization for paths.
template<>
inline static void operator <=><std::filesystem::path>(CrArchive& Arch, std::filesystem::path& ToSerialize)
{
	if (Arch.bSerializing)
	{
		Arch.save(ToSerialize.generic_string());
	}
	else
	{
		std::string LoadStr;
		Arch.load(LoadStr);
		ToSerialize = std::filesystem::path(std::move(LoadStr));
	}
}

struct CrBinSerializable
{
	virtual void BinSerialize(CrArchive& Data) = 0;
};

//template<>
//void operator <=><CrArchive>(CrArchive& Arch, CrArchive& ToSerialize)
//{
//	if (Arch.bSerializing)
//	{
//		Arch.save(ToSerialize);
//	}
//	else
//	{
//		Arch.load(ToSerialize);
//	}
//}
