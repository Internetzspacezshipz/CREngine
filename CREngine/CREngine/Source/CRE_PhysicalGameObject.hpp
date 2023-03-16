#pragma once

#include "CRE_Mesh.hpp"

#include <memory>

class CRE_PhysicalGameObject
{
public:
	using id_t = unsigned int;

	static CRE_PhysicalGameObject CreateGameObject()
	{
		static id_t CurrentID = 0;
		return CRE_PhysicalGameObject(++CurrentID);
	}

	//Delete copy.
	CRE_PhysicalGameObject(const CRE_PhysicalGameObject&) = delete;
	CRE_PhysicalGameObject& operator=(const CRE_PhysicalGameObject&) = delete;

	//Default move.
	CRE_PhysicalGameObject(CRE_PhysicalGameObject&&) = default;
	CRE_PhysicalGameObject& operator=(CRE_PhysicalGameObject&&) = default;


	id_t GetId() const { return ID; }

	std::shared_ptr<CRE_Mesh> MeshObject;
	CRE_Transform Transform{};

private:
	CRE_PhysicalGameObject(id_t InObjID)
		: ID(InObjID)
	{}

	id_t ID;
};

