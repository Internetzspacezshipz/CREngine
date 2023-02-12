#include "CRE_Mesh.hpp"

std::vector<VkVertexInputAttributeDescription> CRE_Mesh::Vertex::GetAttributeDescriptions()
{
	std::vector<VkVertexInputAttributeDescription> AttributesDescriptions(1);
	AttributesDescriptions[0].binding = 0;
	AttributesDescriptions[0].location = 0;
	AttributesDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	AttributesDescriptions[0].offset = 0;
	return AttributesDescriptions;
}

std::vector<VkVertexInputBindingDescription> CRE_Mesh::Vertex::GetBindingDescriptions()
{
	std::vector<VkVertexInputBindingDescription> BindingDescriptions(1);
	BindingDescriptions[0].binding = 0;
	BindingDescriptions[0].stride = sizeof(Vertex);
	BindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return BindingDescriptions;
}

CRE_Mesh::CRE_Mesh(CRE_Device* InDevice, const std::vector<Vertex>& Verticies)
	: Device(InDevice)
{
	CreateVertexBuffers(Verticies);
}

CRE_Mesh::~CRE_Mesh()
{
	vkDestroyBuffer(Device->device(), VertexBuffer, nullptr);
	vkFreeMemory(Device->device(), VertexBufferMemory, nullptr);

	Device = nullptr;
}

void CRE_Mesh::Bind(VkCommandBuffer CommandBuffer)
{
	VkBuffer Buffers[] = { VertexBuffer };
	VkDeviceSize Offsets[] = { 0 };
	vkCmdBindVertexBuffers(CommandBuffer, 0, 1, Buffers, Offsets);
}

void CRE_Mesh::Draw(VkCommandBuffer CommandBuffer)
{
	vkCmdDraw(CommandBuffer, VertexCount, 1, 0 ,0);
}

void CRE_Mesh::CreateVertexBuffers(const std::vector<Vertex>& Verticies)
{
	VertexCount = static_cast<uint32_t>(Verticies.size());
	assert(VertexCount >= 3 && "Vertex count must be at least three");

	VkDeviceSize BufferSize = sizeof(Verticies[0]) * VertexCount;

	Device->createBuffer(
		BufferSize,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		VertexBuffer,
		VertexBufferMemory
	);

	void* Data;
	vkMapMemory(Device->device(), VertexBufferMemory, 0, BufferSize, 0, &Data);
	memcpy(Data, Verticies.data(), static_cast<size_t>(BufferSize));
	vkUnmapMemory(Device->device(), VertexBufferMemory);
}
