#pragma once

#include <cmath>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
//glm math
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

//See corecrt_math_defines.h - used for pi and other constants.
#define _USE_MATH_DEFINES
#include <math.h>

inline float ClampAxis(float Angle)
{
	// returns Angle in the range (-360,360)
	Angle = fmod(Angle, 360.f);

	if (Angle < 0.f)
	{
		// shift to [0,360) range
		Angle += 360.f;
	}

	return Angle;
}

inline float NormalizeAxis(float Angle)
{
	// returns Angle in the range [0,360)
	Angle = ClampAxis(Angle);

	if (Angle > 180.f)
	{
		// shift to (-180,180]
		Angle -= 360.f;
	}

	return Angle;
}

template<class T>
inline static auto RadiansToDegrees(T const& RadVal) -> decltype(RadVal* (180.f / (T)M_PI))
{
	return RadVal * (180.f / (T)M_PI);
}

template<class T>
inline static auto DegreesToRadians(T const& DegVal) -> decltype(DegVal* ((T)M_PI / 180.f))
{
	return DegVal * ((T)M_PI / 180.f);
}

inline float ClampAngle(float AngleDegrees, float MinAngleDegrees, float MaxAngleDegrees)
{
	float const MaxDelta = ClampAxis(MaxAngleDegrees - MinAngleDegrees) * 0.5f;			// 0..180
	float const RangeCenter = ClampAxis(MinAngleDegrees + MaxDelta);						// 0..360
	float const DeltaFromCenter = NormalizeAxis(AngleDegrees - RangeCenter);				// -180..180

	// maybe clamp to nearest edge
	if (DeltaFromCenter > MaxDelta)
	{
		return NormalizeAxis(RangeCenter + MaxDelta);
	}
	else if (DeltaFromCenter < -MaxDelta)
	{
		return NormalizeAxis(RangeCenter - MaxDelta);
	}

	// already in range, just return it
	return NormalizeAxis(AngleDegrees);
}

struct CRE_Transform
{
	glm::vec3 Translation{};
	glm::vec3 Scale{ 1.f, 1.f, 1.f };
	glm::quat Rotation{};


	void SetRotationFromEulerRad(glm::vec3 Euler)
	{
		Rotation = glm::quat(Euler);
	}
	void SetRotationFromEulerRad(float X, float Y, float Z)
	{
		SetRotationFromEulerRad(glm::vec3{ X,Y,Z });
	}

	void SetRotationFromEulerDeg(glm::vec3 Euler)
	{
		Euler.x = DegreesToRadians(NormalizeAxis(Euler.x));
		Euler.y = DegreesToRadians(NormalizeAxis(Euler.y));
		Euler.z = DegreesToRadians(NormalizeAxis(Euler.z));

		Rotation = glm::quat(Euler);
	}

	void SetRotationFromEulerDeg(float X, float Y, float Z)
	{
		SetRotationFromEulerDeg(glm::vec3{ X,Y,Z });
	}


	glm::vec3 GetRotationAsEulerRad() const
	{
		return glm::eulerAngles(Rotation);
	}

	glm::vec3 GetRotationAsEulerDeg() const
	{
		auto EulerOut = glm::eulerAngles(Rotation);
		EulerOut.x = NormalizeAxis(RadiansToDegrees(EulerOut.x));
		EulerOut.y = NormalizeAxis(RadiansToDegrees(EulerOut.y));
		EulerOut.z = NormalizeAxis(RadiansToDegrees(EulerOut.z));
		return EulerOut;
	}

	operator glm::mat4() const
	{
		auto Transform = glm::translate(glm::mat4{ 1.f }, Translation);
		Transform = glm::toMat4(Rotation) * Transform;
		Transform = glm::scale(Transform, Scale);
		return Transform;
	}
};

struct CRE_Vertex
{
	glm::vec3 Position;
	glm::vec3 Color;

	static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> AttributesDescriptions(2);
		AttributesDescriptions[0].binding = 0;
		AttributesDescriptions[0].location = 0;
		AttributesDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		AttributesDescriptions[0].offset = offsetof(CRE_Vertex, Position);

		AttributesDescriptions[1].binding = 0;
		AttributesDescriptions[1].location = 1;
		AttributesDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		AttributesDescriptions[1].offset = offsetof(CRE_Vertex, Color);

		return AttributesDescriptions;
	}

	static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> BindingDescriptions(1);
		BindingDescriptions[0].binding = 0;
		BindingDescriptions[0].stride = sizeof(CRE_Vertex);
		BindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return BindingDescriptions;
	}

};

//Used mainly for 2d features and such.
struct CRE_Transform2D
{
	glm::vec2 Translation;
	float Rotation;
	glm::vec2 Scale;
	//Distance from near clipping plane. If it is below 0 or above 1, it will not be visible.
	float CameraDistance;
};

//Useful for user interface and images.
struct CRE_TransformBox2D
{
	glm::vec2 TopLeft;
	glm::vec2 BotRight;
	float CameraDistance;

	std::vector<CRE_Vertex> GetRenderTris() const
	{
		CRE_Vertex TopRight = { {BotRight.x, TopLeft.y, CameraDistance}, {1.f, 0.f, 0.f} };
		CRE_Vertex BottomLeft = { {TopLeft.x, BotRight.y, CameraDistance}, {0.f, 1.f, 0.f} };

		std::vector<CRE_Vertex> Vertices
		{
			//Top left
			{ {TopLeft.x, TopLeft.y, CameraDistance}, { 0.f, 0.f, 0.f } },
			{ TopRight },
			{ BottomLeft },

			//Bottom right
			{ TopRight },
			{ {BotRight.x, BotRight.y, CameraDistance}, {1.f, 1.f, 0.f} },
			{ BottomLeft },
		};
		
		return Vertices;
	}

	void Scale(float X, float Y)
	{
		TopLeft.x *= X;
		TopLeft.y *= Y;

		BotRight.x *= X;
		BotRight.y *= Y;
	}
};