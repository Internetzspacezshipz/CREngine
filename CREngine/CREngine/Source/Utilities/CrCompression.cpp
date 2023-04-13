#include "CrCompression.h"
#include "ThirdParty/texcompression/ConvectionKernels.h"
#include <stb_image.h>

//Use if wanting concurrency::parallel_for()
#include <ppl.h>
#include <ppltasks.h>

namespace CrCompression
{
	bool GetFileInfo(const String& FilePath, int& Width, int& Height, int& Channels, bool& bIsFP)
	{
		if (stbi_info(FilePath.c_str(), &Width, &Height, &Channels))
		{
			bIsFP = stbi_is_hdr(FilePath.c_str());
			return true;
		}
		return false;
	}

	//Annoying gigantic fake function to sort out specific format types.
	template<uint8_t FormatType, typename BlockType>
	forceinline void EncodeFunction(Array<uint8_t>& outputBlock, const BlockType* pixelBlocks, const cvtt::Options& options, const cvtt::BC7EncodingPlan& EncPlan)
	{
		if constexpr (FormatType == BC1)
		{
			cvtt::Kernels::EncodeBC1(outputBlock.data(), pixelBlocks, options);
		}
		else if constexpr (FormatType == BC2)
		{
			cvtt::Kernels::EncodeBC2(outputBlock.data(), pixelBlocks, options);
		}
		else if constexpr (FormatType == BC3)
		{
			cvtt::Kernels::EncodeBC3(outputBlock.data(), pixelBlocks, options);
		}
		else if constexpr (FormatType == BC4U)
		{
			cvtt::Kernels::EncodeBC4U(outputBlock.data(), pixelBlocks, options);
		}
		else if constexpr (FormatType == BC4S)
		{
			cvtt::Kernels::EncodeBC4S(outputBlock.data(), pixelBlocks, options);
		}
		else if constexpr (FormatType == BC5U)
		{
			cvtt::Kernels::EncodeBC5U(outputBlock.data(), pixelBlocks, options);
		}
		else if constexpr (FormatType == BC5S)
		{
			cvtt::Kernels::EncodeBC5S(outputBlock.data(), pixelBlocks, options);
		}
		else if constexpr (FormatType == BC6HU)
		{
			cvtt::Kernels::EncodeBC6HU(outputBlock.data(), pixelBlocks, options);
		}
		else if constexpr (FormatType == BC6HS)
		{
			cvtt::Kernels::EncodeBC6HS(outputBlock.data(), pixelBlocks, options);
		}
		else if constexpr (FormatType == BC7)
		{
			cvtt::Kernels::EncodeBC7(outputBlock.data(), pixelBlocks, options, EncPlan);
		}
	}

	template<typename BlockType, int blockSizeBytes, int PixelSize, uint8_t FormatType>
	static void Encode(BinArray& OutData, stbi_uc* PixelsIn, int InChannel, int TextureWidth, int TextureHeight)
	{
		cvtt::Options options;
		cvtt::BC7EncodingPlan EncPlan;


		constexpr size_t inputBlockChunkSize = sizeof(BlockType) * cvtt::NumParallelBlocks;
		constexpr size_t outputBlockChunkSize = blockSizeBytes * cvtt::NumParallelBlocks;



		//The total number of bytes in the image from the source.
		const size_t SourceTotalBytes = (size_t)TextureWidth * (size_t)TextureHeight * (size_t)InChannel * (size_t)PixelSize;

		//The number of bytes along a total row strip from the source.
		const size_t SourceRowBytes = (size_t)TextureWidth * (size_t)InChannel * (size_t)PixelSize;

		size_t PreparedBlocks = 0;
		
		BlockType pixelBlocks[cvtt::NumParallelBlocks];

		//for concurrent writing of data on worker threads.
		int FracturedImageIndex = 0;
		OutData.resize((SourceTotalBytes / inputBlockChunkSize)*outputBlockChunkSize);
		Array<concurrency::task<void>> TaskHandles;

		//step + 4 for each axis of the pixel blocks.
		for (size_t CurHeight = 0; CurHeight < TextureHeight; CurHeight += 4)
		{
			for (size_t CurWidth = 0; CurWidth < TextureWidth; CurWidth += 4)
			{
				//Base point
				size_t BaseIndex = (CurHeight * SourceRowBytes) + (CurWidth * InChannel);

				//Pack the pixel block here.
				//Move down one row.
				for (size_t CurBlockWidth = 0; CurBlockWidth < 4; CurBlockWidth++)
				{
					for (size_t CurBlockHeight = 0; CurBlockHeight < 4; CurBlockHeight++)
					{
						size_t CellBaseIndex = ((CurBlockHeight * SourceRowBytes) + (CurBlockWidth * InChannel)) + BaseIndex;

						for (size_t CurBlockDepth = 0; CurBlockDepth < InChannel; CurBlockDepth++)
						{
							uint8_t CurPix = PixelsIn[CellBaseIndex + CurBlockDepth];
							pixelBlocks[PreparedBlocks].m_pixels[(CurBlockHeight * 4) + CurBlockWidth][CurBlockDepth] = CurPix;
						}
					}
				}

				PreparedBlocks++;
				if (PreparedBlocks == cvtt::NumParallelBlocks)
				{
					const size_t OutputIndex = FracturedImageIndex * outputBlockChunkSize;
					//Copy pixel blocks, which might be a fairly sizable copy, but it is probably better than another giant array.
					TaskHandles.push_back(concurrency::create_task(
						[&OutData, OutputIndex, pixelBlocks, &options, &EncPlan]()
						{
							Array<uint8_t> outputBlock;
							outputBlock.resize(outputBlockChunkSize);

							EncodeFunction<FormatType, BlockType>(outputBlock, pixelBlocks, options, EncPlan);

							memcpy(&OutData[OutputIndex], (char*)outputBlock.data(), outputBlockChunkSize);
						}));
					FracturedImageIndex++;
					PreparedBlocks = 0;
				}
			}
		}

		//Wait for all done before continuing.
		concurrency::when_all(begin(TaskHandles), end(TaskHandles)).wait();
	}

	bool ImportAndCompress(BinArray& OutData, const String& FilePath, CrTextureFormatTypes FormatType)
	{
		static_assert(STBI_grey == 1);
		static_assert(STBI_grey_alpha == 2);
		static_assert(STBI_rgb == 3);
		static_assert(STBI_rgb_alpha == 4);

		OutData.clear();

		int TextureWidth, TextureHeight, TextureChannelsActual, ImportChannels;

		CrCompressionAttributes Attrib = GetFormatAttributes(FormatType);
		ImportChannels = Attrib.Channels;
		const unsigned int blockSizeBytes = Attrib.BytesPerBlock;

		bool bIsFP = false;

		if (GetFileInfo(FilePath, TextureWidth, TextureHeight, TextureChannelsActual, bIsFP) == false)
		{
			CrLOG("Failed to load \"{}\"", FilePath);
			return false;
		}

		if (bIsFP != Attrib.bFloat)
		{
			CrLOG("Filename \"{}\" was in a different format than expected.", FilePath);
			return false;
		}

		stbi_uc* Pix = stbi_load(FilePath.c_str(), &TextureWidth, &TextureHeight, &TextureChannelsActual, ImportChannels);

		//Switch on the format type to get an optimized template compression/encoding function
		switch (FormatType)
		{
		default://default to BC1 since it's pretty basic.
		case BC1:
			Encode<cvtt::PixelBlockU8, 8, 1, BC1>(OutData, Pix, ImportChannels, TextureWidth, TextureHeight);
			break;
		case BC2:
			Encode<cvtt::PixelBlockU8, 16, 1, BC2>(OutData, Pix, ImportChannels, TextureWidth, TextureHeight);
			break;
		case BC3:
			Encode<cvtt::PixelBlockU8, 16, 1, BC3>(OutData, Pix, ImportChannels, TextureWidth, TextureHeight);
			break;
		case BC4U:
			Encode<cvtt::PixelBlockU8, 8, 1, BC4U>(OutData, Pix, ImportChannels, TextureWidth, TextureHeight);
			break;
		case BC4S:
			Encode<cvtt::PixelBlockS8, 8, 1, BC4S>(OutData, Pix, ImportChannels, TextureWidth, TextureHeight);
			break;
		case BC5U:
			Encode<cvtt::PixelBlockU8, 16, 1, BC5U>(OutData, Pix, ImportChannels, TextureWidth, TextureHeight);
			break;
		case BC5S:
			Encode<cvtt::PixelBlockS8, 16, 1, BC5S>(OutData, Pix, ImportChannels, TextureWidth, TextureHeight);
			break;
		case BC6HU:
			Encode<cvtt::PixelBlockF16, 16, 2, BC6HU>(OutData, Pix, ImportChannels, TextureWidth, TextureHeight);
			break;
		case BC6HS:
			Encode<cvtt::PixelBlockF16, 16, 2, BC6HS>(OutData, Pix, ImportChannels, TextureWidth, TextureHeight);
			break;
		case BC7:
			Encode<cvtt::PixelBlockU8, 16, 1, BC7>(OutData, Pix, ImportChannels, TextureWidth, TextureHeight);
			break;
		}

		stbi_image_free(Pix);

		return OutData.size();
	}

	CrCompressionAttributes GetFormatAttributes(CrTextureFormatTypes Format)
	{
		auto Tuple = CompressionAttributes::Get(Format);
		return CrCompressionAttributes{ std::get<0>(Tuple), std::get<1>(Tuple), std::get<2>(Tuple), std::get<3>(Tuple) };
	}

};