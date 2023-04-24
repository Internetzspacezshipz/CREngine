#pragma once
#include "CrTypes.h"

enum GridType : uint8_t
{
	SquareGrid,
	HexGrid,//hex grid with equal row lengths
	HexGrid_B,//hex grid with row lengths between 
};

struct CrGridCell
{
	uint32_t LocX;
	uint32_t LocY;
};

template<GridType Type>
class CrGrid
{
	void GetAdjacentTo(SP<CrGridCell> Center, Array<SP<CrGridCell>>& OutAdjacent)
	{
		if constexpr (Type == SquareGrid)
		{
			OutAdjacent.reserve(4);
			OutAdjacent.emplace_back(GridActual[Center->LocX+1][Center->LocY]);
			OutAdjacent.emplace_back(GridActual[Center->LocX-1][Center->LocY]);
			OutAdjacent.emplace_back(GridActual[Center->LocX][Center->LocY+1]);
			OutAdjacent.emplace_back(GridActual[Center->LocX][Center->LocY-1]);
		}
		else
		{
			OutAdjacent.reserve(6);
			//not correct yet.
			OutAdjacent.emplace_back(GridActual[Center->LocX][Center->LocY]);
			OutAdjacent.emplace_back(GridActual[Center->LocX][Center->LocY]);
			OutAdjacent.emplace_back(GridActual[Center->LocX][Center->LocY+1]);
			OutAdjacent.emplace_back(GridActual[Center->LocX][Center->LocY+1]);
			OutAdjacent.emplace_back(GridActual[Center->LocX][Center->LocY-1]);
			OutAdjacent.emplace_back(GridActual[Center->LocX][Center->LocY-1]);
		}
	}

	void GetFracLoc(SP<CrGridCell> Center);

	//horizontal size
	uint32_t SizeX;
	//vertical size
	uint32_t SizeY;

	Array<Array<SP<CrGridCell>>> GridActual;

	forceinline bool IsVaidIndex(uint32_t X, uint32_t Y) const
	{
		if constexpr (Type == SquareGrid)
		{
			return SizeX > X && SizeY > Y;
		}
		else if constexpr (Type == HexGrid)
		{
			return SizeX > X && SizeY > Y;
		}
		else if constexpr (Type == HexGrid_B)
		{
			return (SizeX - (Y % 2)) > X && SizeY > Y;
		}
		return 0;
	}

	SP<CrGridCell> Access(uint32_t X, uint32_t Y)
	{
		if (IsVaidIndex(X, Y))
		{
			return GridActual[X][Y];
		}
		return SP<CrGridCell>(nullptr);
	}

	template<typename Lambda>
	static SP<CrGrid> GenerateGrid(uint32_t InSizeX, uint32_t InSizeY, const Lambda& SetupCellFunction)
	{
		SP<CrGrid> NewGrid = MkSP<CrGrid>();

		NewGrid->SizeX = InSizeX;
		NewGrid->SizeY = InSizeY;

		for (uint32_t X = 0; X < SizeX; X++)
		{
			auto& NewArr = NewGrid->GridActual.emplace_back(Array<SP<CrGridCell>>());
			NewArr.reserve(SizeY);

			for (uint32_t Y = 0; Y < SizeY; Y++)
			{
				NewArr.emplace_back(SetupCellFunction(X, Y));
			}
		}

		return NewGrid;
	}
};
