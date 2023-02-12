#pragma once
#include "CRE_Device.hpp"

//glm graphics incl
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

//std incl
#include <vector>

class CRE_Mesh
{
public:

	struct Vertex
	{
		glm::vec2 Position;
		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
		static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
	};

	CRE_Mesh(CRE_Device* InDevice, const std::vector<Vertex>& Verticies);
	~CRE_Mesh();

	CRE_Mesh(const CRE_Mesh&) = delete;
	CRE_Mesh& operator= (const CRE_Mesh&) = delete;

	void Bind(VkCommandBuffer CommandBuffer);
	void Draw(VkCommandBuffer CommandBuffer);

private:
	void CreateVertexBuffers(const std::vector<Vertex>& Verticies);

	CRE_Device* Device;
	VkBuffer VertexBuffer;
	VkDeviceMemory VertexBufferMemory;
	uint32_t VertexCount;
};

