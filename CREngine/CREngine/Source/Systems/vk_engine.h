// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>
#include <vector>
#include <functional>
#include <deque>
#include <vk_mesh.h>
#include <unordered_map>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "ThirdParty/ThirdPartyLibs.h"

//for keycodes.
#include <SDL_keycode.h>

#include "CrKeySystem.h"
#include "CrUtilities.h"
#include "CrTypes.h"


class PipelineBuilder 
{
public:

	std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;
	VkPipelineVertexInputStateCreateInfo _vertexInputInfo;
	VkPipelineInputAssemblyStateCreateInfo _inputAssembly;
	VkViewport _viewport;
	VkRect2D _scissor;
	VkPipelineRasterizationStateCreateInfo _rasterizer;
	VkPipelineColorBlendAttachmentState _colorBlendAttachment;
	VkPipelineMultisampleStateCreateInfo _multisampling;
	VkPipelineLayout _pipelineLayout;
	VkPipelineDepthStencilStateCreateInfo _depthStencil;
	VkPipeline build_pipeline(VkDevice device, VkRenderPass pass);
};



struct DeletionQueue
{
    std::deque<std::function<void(class VulkanEngine* Engine)>> deletors;

    void push_deletion_function(std::function<void(VulkanEngine* Engine)>&& function) 
	{
        deletors.push_back(function);
    }

    void flush(VulkanEngine* Engine) 
	{
        // reverse iterate the deletion queue to execute all the functions
        for (auto it = deletors.rbegin(); it != deletors.rend(); it++) 
		{
            (*it)(Engine); //call functors
        }

        deletors.clear();
    }
};

//If in 2D mode, the engine will not bother with camera projection and such.
#define ENGINE_MODE_3D 0

//Must align to a certain boundry.
struct alignas(uint64_t) MeshPushConstants 
{
	glm::vec4 data;
#if ENGINE_MODE_3D
	glm::mat4 render_matrix;
#else
	Vec2 Translation;
	float Rotation;
	Vec2 Scale;
#endif

};

//A struct that is sent to the GPU in a large buffer for each object.
struct alignas(uint64_t) GPUObjectData
{
#if ENGINE_MODE_3D
	glm::mat4 modelMatrix;
#else
	Vec2 Translation;
	float Rotation;
	Vec2 Scale;
#endif
};

struct MaterialData 
{
	VkDescriptorSet textureSet{VK_NULL_HANDLE};
	VkPipeline pipeline = nullptr;
	VkPipelineLayout pipelineLayout = nullptr;
};

struct TextureData 
{
	AllocatedImage image;
	VkImageView imageView;

	VkDescriptorSet DescriptorSet;
	VkSampler Sampler;
};

struct RenderObject 
{
	virtual MeshData* GetMesh() = 0;
	virtual MaterialData* GetMaterial() = 0;

	//Data we push to the GPU every frame. Includes transform information.
	GPUObjectData GPUData;
};


struct FrameData 
{
	VkSemaphore _presentSemaphore, _renderSemaphore;
	VkFence _renderFence;

	DeletionQueue _frameDeletionQueue;

	VkCommandPool _commandPool;
	VkCommandBuffer _mainCommandBuffer;

	AllocatedBuffer cameraBuffer;
	VkDescriptorSet globalDescriptor;

	AllocatedBuffer objectBuffer;
	VkDescriptorSet objectDescriptor;
};

struct UploadContext 
{
	VkFence _uploadFence;
	VkCommandPool _commandPool;	
	VkCommandBuffer _commandBuffer;
};

struct GPUCameraData
{
	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 viewproj;
};

struct GPUSceneData 
{
	glm::vec4 fogColor; // w is for exponent
	glm::vec4 fogDistances; //x for min, y for max, zw unused.
	glm::vec4 ambientColor;
	glm::vec4 sunlightDirection; //w for sun power
	glm::vec4 sunlightColor;
};


constexpr unsigned int FRAME_OVERLAP = 2;


class VulkanEngine 
{
	friend DeletionQueue;
public:
	~VulkanEngine();

	bool _isInitialized{ false };

	bool bWantsQuit = false;

	bool bFramebufferResized = false;

	bool bShouldBeFullscreen = false;

	bool bDrawUI = false;



	//Call to this every frame for drawing ImGui UI elements.
	std::function<void()> UIDrawFunction;


	//Shared pointers to keybinds.
	SP<KeySubscriber> FullscreenKeybind_RAlt;
	SP<KeySubscriber> FullscreenKeybind_LAlt;



	uint64_t _frameNumber {0};
	int _selectedShader{ 0 };


	VkExtent2D _windowExtent{ 1700 , 900 };

	struct SDL_Window* _window{ nullptr };




	VkInstance _instance;
	VkDebugUtilsMessengerEXT _debug_messenger;
	VkPhysicalDevice _chosenGPU;
	VkDevice _device;

	VkPhysicalDeviceProperties _gpuProperties;

	FrameData _frames[FRAME_OVERLAP];
	
	VkQueue _graphicsQueue;
	uint32_t _graphicsQueueFamily;
	
	VkRenderPass _renderPass;

	VkSurfaceKHR _surface;

	//Swapchain actual;
	vkb::Swapchain _vkbSwapchain{};
	uint32_t swapchainImageIndex;

	std::vector<VkImageView> _swapchainImageViews;
	std::vector<VkFramebuffer> _framebuffers;
	
	VmaAllocator _allocator; //vma lib allocator

	UP<vke::DescriptorAllocatorPool> DescriptorAllocator;
	forceinline vke::DescriptorAllocatorHandle GetDescAlloc() const { return DescriptorAllocator->GetAllocator(); }

	//depth resources
	VkImageView _depthImageView;
	AllocatedImage _depthImage;

	//the format for the depth image
	VkFormat _depthFormat;

	VkDescriptorSetLayout _globalSetLayout;
	VkDescriptorSetLayout _objectSetLayout;
	VkDescriptorSetLayout _singleTextureSetLayout;

	GPUSceneData _sceneParameters;
	AllocatedBuffer _sceneParameterBuffer;

	UploadContext _uploadContext;

	//initializes everything in the engine
	void init();

private:
	//shuts down the engine
	void cleanup();

public:
	//draw loop
	void Draw();

	//This function lives inside Draw, and will allow new objects to be drawn and altered.
	void DrawInterior(VkCommandBuffer cmd);

	//run main loop
	void run();
	
	FrameData& get_current_frame();
	FrameData& get_last_frame();

	//Pipelines

	//VkPipelineLayout meshPipeLayout;
	//VkPipelineLayout texturedPipeLayout;
	//
	//VkPipeline meshPipeline;
	//VkPipeline texPipeline;


	//weak pointer array of render objects. If actual render object is destroyed by dereferencing, it will be destroyed here if not in use.
	std::vector<WP<RenderObject>> _renderables;

	//The minimum size of the _renderables vector
	void RemoveInvalidRenderables();


	//annoying thing.
	DeletionQueue _mainDeletionQueue;

	//functions

	//our draw function
	void draw_objects(VkCommandBuffer cmd);

	AllocatedBuffer create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);

	size_t pad_uniform_buffer_size(size_t originalSize);

	void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);


	//New CRE funcs

	void UploadTexture(TextureData* NewTexture, VkFormat Format);
	void UnloadTexture(TextureData* DeleteTex);

	//Upload mesh to GPU
	void UploadMesh(MeshData* NewMesh);
	void UnloadMesh(MeshData* DeleteMesh);

	//loads a shader module from a spir-v file. Returns false if it errors
	bool LoadShaderModule(uint32_t* ShaderCode, uint64_t ShaderSize, VkShaderModule& OutShaderModule);
	void UnloadShaderModule(VkShaderModule& DeleteShader);

	void MakeDefaultPipeline(VkShaderModule VertShader, VkShaderModule FragShader, MaterialData& MaterialDataOut);
	void DestroyMaterial(MaterialData& MaterialDataOut);

	void SubmitRenderable(WP<RenderObject> RenderItem);

	Array<WP<RenderObject>> RenderItems;

	//Functions to delete objects that were to be unloaded during the frame, but might have already been asked to render.
	Array<Func<void(VulkanEngine*)>> NextFrameDeletors;

private:

	void init_vulkan();
	void cleanup_vulkan();

	VkDescriptorPool imguiPool;
	void init_imgui();
	void cleanup_imgui();

	//Swapchain funcs
	void init_swapchain();
	//Used to recreate the swapchain when window resize or fullscreen happens.
	void recreate_swapchain();
	void cleanup_swapchain();


	void init_default_renderpass();
	void cleanup_default_renderpass();

	void init_framebuffers();
	void cleanup_framebuffers();

	void init_commands();
	void cleanup_commands();

	void init_sync_structures();
	void cleanup_sync_structures();

	void init_pipelines();
	void cleanup_pipelines();

	void init_scene();
	void cleanup_scene();

	void init_descriptors();
	void cleanup_descriptors();


	void load_meshes();


	void RunPostFrameDeletors();
};
