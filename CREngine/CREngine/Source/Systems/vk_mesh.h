#pragma once

#include <vk_types.h>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "CrTypes.h"

struct VertexInputDescription 
{
	std::vector<VkVertexInputBindingDescription> bindings;
	std::vector<VkVertexInputAttributeDescription> attributes;

	VkPipelineVertexInputStateCreateFlags flags = 0;
};

struct Vertex 
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 uv;
	static VertexInputDescription get_vertex_description();
}; 

enum Shape : uint16_t
{
	ShapeNone = 0,
	ShapeTriangle = 1 << 0,
	ShapeQuad = 1 << 1,
};

struct MeshData
{
	std::vector<Vertex> Verts;

	AllocatedBuffer _vertexBuffer;

	bool LoadFromObj(const Path& filename);

	//Simple hardcoded shapes that will conform to having a maximum extent of a unit vector.
	void MakeFromShape(Shape InShape);
};