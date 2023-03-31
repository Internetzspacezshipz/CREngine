#pragma once
#include <fstream>
#include <filesystem>
#include <assert.h>
#include "ThirdParty/archive/archive.h"

class CrArchive : public tser::BinaryArchive
{
	std::fstream Stream;
	FILE* pFile;
public:
	bool bSerializing;

	CrArchive(const std::string& Path, bool bIsSerializing)
		: bSerializing(bIsSerializing)
	{
		//fopen_s(&pFile, Path.c_str(), "wb+");
		//
		//assert(pFile);


		Stream = std::fstream{ Path, std::ios::binary | (bIsSerializing ? (std::ios::out) : (std::ios::in)) };
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
		//fclose(pFile);

		Stream.close();
	}

	void Save()
	{
		//auto Buf = get_buffer();
		//fwrite(Buf.data(), sizeof(char), Buf.size(), pFile);
		Stream << *this;
	};

	void Load() 
	{
		//auto Buf = get_buffer();
		//fseek(pFile, 0, SEEK_END);
		//auto FileSize = ftell(pFile);
		//fseek(pFile, 0, SEEK_SET);
		//fread(const_cast<char*>(Buf.data()), sizeof(char), FileSize, pFile);
		//assert(!ferror(pFile));
		
		// copies all data into buffer
		//    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

		std::string buffer(std::istreambuf_iterator<char>(Stream), {});
		initialize(buffer);
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
