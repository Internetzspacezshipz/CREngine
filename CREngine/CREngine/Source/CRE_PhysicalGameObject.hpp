#pragma once

#include "CRE_Mesh.hpp"

//glm math
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>


#include <memory>

struct CRE_TransformComponent
{
	glm::vec3 Translation{};
	glm::vec3 Scale{1.f, 1.f, 1.f};
	glm::quat Rotation{};

	void SetRotationFromEuler(float X, float Y, float Z)
	{
		Rotation = glm::quat(glm::vec3{ X,Y,Z });
	}

	void SetRotationFromEuler(glm::vec3 Euler)
	{
		Rotation = glm::quat(Euler);
	}

	glm::vec3 GetRotationAsEuler() const
	{
		return glm::eulerAngles(Rotation);
	}

	operator glm::mat4() const
	{
		auto Transform = glm::translate(glm::mat4{ 1.f }, Translation);
		Transform = glm::toMat4(Rotation) * Transform;
		Transform = glm::scale(Transform, Scale);
		return Transform;
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
	CRE_TransformComponent Transform{};

private:
	CRE_PhysicalGameObject(id_t InObjID)
		: ID(InObjID)
	{}

	id_t ID;
};

