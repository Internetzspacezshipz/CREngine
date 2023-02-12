#pragma once

#include "CRE_Mesh.hpp"

#include <memory>

struct CRE_Transform2DComponent
{
	glm::vec2 Translation{};
	glm::vec2 Scale{1.f, 1.f};
	float Rotation;

	operator glm::mat2()
	{
		const float s = glm::sin(Rotation);
		const float c = glm::cos(Rotation);

		glm::mat2 RotMatrix{ 
			{c,  s}, 
			{-s, c} 
		};

		glm::mat2 ScaleMat{
			{Scale.x, 0.f}, 
			{0.f, Scale.y}
		};
		return RotMatrix * ScaleMat;
	}
};

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
	CRE_Transform2DComponent Transform{};
	glm::vec3 Color;

private:
	CRE_PhysicalGameObject(id_t InObjID)
		: ID(InObjID)
	{}

	id_t ID;
};

