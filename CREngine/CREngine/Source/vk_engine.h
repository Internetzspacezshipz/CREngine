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

//Vulkan bootstrap for helping with managment of swapchain and other vulkan parts.
#include "thirdparty/vkbootstrap/VkBootstrap.h"

//for keycodes.
#include <SDL_keycode.h>

#include "CRE_KeySystem.hpp"
#include "CRE_Utilities.hpp"

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

struct MeshPushConstants 
{
	glm::vec4 data;
	glm::mat4 render_matrix;
};

struct Material 
{
	VkDescriptorSet textureSet{VK_NULL_HANDLE};
	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;
};

struct Texture 
{
	AllocatedImage image;
	VkImageView imageView;

	VkDescriptorSet DescriptorSet;
	VkSampler Sampler;
};

struct RenderObject 
{
	Mesh* mesh;

	Material* material;

	glm::mat4 transformMatrix;

	bool bRenderEnable = false;
};

typedef std::weak_ptr<RenderObject> RO_wp;
typedef std::shared_ptr<RenderObject> RO_sp;

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

struct GPUObjectData 
{
	glm::mat4 modelMatrix;
};

constexpr unsigned int FRAME_OVERLAP = 2;


class VulkanEngine 
{
	friend DeletionQueue;
public:

	bool _isInitialized{ false };

	bool bWantsQuit = false;

	bool bFramebufferResized = false;

	bool bShouldBeFullscreen = false;

	bool bDrawUI = false;

	//Call to this every frame for drawing ImGui UI elements.
	std::function<void()> UIDrawFunction;

	CRE_KeySystem _KeySystem;

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

	//depth resources
	VkImageView _depthImageView;
	AllocatedImage _depthImage;

	//the format for the depth image
	VkFormat _depthFormat;

	VkDescriptorPool _descriptorPool;

	VkDescriptorSetLayout _globalSetLayout;
	VkDescriptorSetLayout _objectSetLayout;
	VkDescriptorSetLayout _singleTextureSetLayout;

	GPUSceneData _sceneParameters;
	AllocatedBuffer _sceneParameterBuffer;

	UploadContext _uploadContext;
	//initializes everything in the engine
	void init();

	//shuts down the engine
	void cleanup();

	//draw loop
	void Draw();

	//This function lives inside Draw, and will allow new objects to be drawn and altered.
	void DrawInterior(VkCommandBuffer cmd);

	//run main loop
	void run();
	
	FrameData& get_current_frame();
	FrameData& get_last_frame();

	//Pipelines

	VkPipelineLayout meshPipeLayout;
	VkPipelineLayout texturedPipeLayout;

	VkPipeline meshPipeline;
	VkPipeline texPipeline;


	//weak pointer array of render objects. If actual render object is destroyed by dereferencing, it will be destroyed here if not in use.
	std::vector<RO_wp> _renderables;

	//The minimum size of the _renderables vector
	void RemoveInvalidRenderables();

	//Map of paths to asset handles in order to check if we already have a thing loaded.
	//Asset handles are unique, so this map goes for all the materials, meshes, and loaded textures.
	std::unordered_map<std::string, AssetHandle> PathToHandle;

	std::unordered_map<AssetHandle, Material>		_materials;
	std::unordered_map<AssetHandle, Mesh>			_meshes;
	std::unordered_map<AssetHandle, Texture>		_loadedTextures;

	//annoying thing.
	DeletionQueue _mainDeletionQueue;

	//functions

	//create material and add it to the map
	Material* create_material(VkPipeline pipeline, VkPipelineLayout layout, const std::string& name);

	//returns nullptr if it cant be found
	Material* get_material(const std::string& name);
	Material* get_material(const AssetHandle& handle);

	//returns nullptr if it cant be found
	Mesh* get_mesh(const std::string& name);
	Mesh* get_mesh(const AssetHandle& handle);

	//returns nullptr if it cant be found
	Texture* get_texture(const std::string& name);
	Texture* get_texture(const AssetHandle& handle);

	//our draw function
	void draw_objects(VkCommandBuffer cmd, const std::vector<RO_wp>& ObjectVec);

	AllocatedBuffer create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);

	size_t pad_uniform_buffer_size(size_t originalSize);

	void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);


	//New CRE funcs

	AssetHandle LoadTexture(std::string Path);

	void UnloadTexture(AssetHandle Handle);

	AssetHandle LoadMesh(std::string Path);
	//Create a mesh with direct mesh access and no original path.
	AssetHandle LoadMesh(Mesh NewMesh);


private:

	AssetHandle CurHandle = 0;

	AssetHandle MakeHandle()
	{
		do
		{
			CurHandle++;
			//Disallow this ever being 0. Zero is an error.
		} while (CurHandle == 0);

		return CurHandle;
	}

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

	//loads a shader module from a spir-v file. Returns false if it errors
	bool load_shader_module(const char* filePath, VkShaderModule* outShaderModule);

	void load_meshes();

	void upload_mesh(Mesh& mesh);
};
