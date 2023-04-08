#pragma once
#include "CrSerializationBin.h"
#include "CrUtilities.h"

//https://www.reedbeta.com/blog/understanding-bcn-texture-compression-formats/
enum CrTextureFormatTypes : uint8_t
{
	BC1,	//highly compressed colour maps with 1 bit alpha (0.5 byte/px)
	BC2,	//colour maps with 4 bit alpha (1 byte/px)
	BC3,	//colour maps with 8 bit alpha (1 byte/px)
	BC4U,	//greyscale UNSIGNED (0.5 byte/px)
	BC4S,	//greyscale SIGNED (0.5 byte/px)
	BC5U,	//RG (tangent-space normals) UNSIGNED (1 byte/px)
	BC5S,	//RG (tangent-space normals) SIGNED (1 byte/px)
	BC6HU,	//HDR unsigned float
	BC6HS,	//HDR signed float
	BC7,	//High quality colour maps full alpha (1 byte/px)
};

//literal text items for the above enum (this is used for UI)
//intellisense thinks this has errors, but it does not.
using TextureFormatStrings = Chain<
	EnumText(BC1),
	EnumText(BC2),
	EnumText(BC3),
	EnumText(BC4U),
	EnumText(BC4S),
	EnumText(BC5U),
	EnumText(BC5S),
	EnumText(BC6HU),
	EnumText(BC6HS),
	EnumText(BC7)
>;


//Attributes of any specific compression type.
struct CrCompressionAttributes
{
	int Channels;
	int BytesPerBlock;
	bool bFloat;
	bool bSigned;
};

using CompressionAttributes = Chain<
	SPairT<BC1,		4, 8,	false,	false>,
	SPairT<BC2,		4, 16,	false,	false>,
	SPairT<BC3,		4, 16,	false,	false>,
	SPairT<BC4U,	1, 8,	false,	false>,
	SPairT<BC4S,	1, 8,	false,	true>,
	SPairT<BC5U,	2, 16,	false,	false>,
	SPairT<BC5S,	2, 16,	false,	true>,
	SPairT<BC6HU,	3, 16,	true,	true>,
	SPairT<BC6HS,	3, 16,	true,	true>,
	SPairT<BC7,		4, 16,	false,	false>
>;

namespace CrCompression
{
	extern bool GetFileInfo(const String& FilePath, int& Width, int& Height, int& Channels, bool& bIsFP);
	extern bool ImportAndCompress(BinArray& OutData, const String& FilePath, CrTextureFormatTypes FormatType);
	extern CrCompressionAttributes GetFormatAttributes(CrTextureFormatTypes Format);
};

