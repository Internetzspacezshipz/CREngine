#pragma once
#include "CRE_Device.hpp"
#include "CRE_Math.hpp"

//std incl
#include <vector>

class CRE_Mesh
{
public:


	CRE_Mesh(CRE_Device* InDevice, const std::vector<CRE_Vertex>& Verticies);
	~CRE_Mesh();

	CRE_Mesh(const CRE_Mesh&) = delete;
	CRE_Mesh& operator= (const CRE_Mesh&) = delete;

	void Bind(VkCommandBuffer CommandBuffer);
	void Draw(VkCommandBuffer CommandBuffer);

private:
	void CreateVertexBuffers(const std::vector<CRE_Vertex>& Verticies);

	CRE_Device* Device;
	VkBuffer VertexBuffer;
	VkDeviceMemory VertexBufferMemory;
	uint32_t VertexCount;
};

