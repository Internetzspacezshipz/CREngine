#pragma once
#include "CrTypes.h"

enum CrColliderType : uint8_t
{
	CrColliderType_None,
	CrColliderType_Circle,
	CrColliderType_Square,
	CrColliderType_Triangle,
};

class CrCollider
{
	CrColliderType Type;
};

class CrCollision
{

};

