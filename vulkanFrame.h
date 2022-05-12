#ifndef VULKANFRAME_H
#define VULKANFRAME_H
#include <iostream>
// #include <vector>	
#include <assert.h>
// #include <array>
#include <optional>
#include <cstring>
#include <string>
#include <limits>
#include "vulkanInit.h"
// #define ENUM_CHIP_TYPE_CASE(x)   case x: return(#x)
/// @brief Helper macro to test the result of Vulkan calls which can return an error.
#define VK_CHECK(x)                                                 \
        do{                                                               \
                VkResult err = x;                                           \
                if (err != VK_SUCCESS){                                       \
                        printf("vulkan error:in function %s line %d information %s\n", __FUNCTION__, __LINE__, cvmx_chip_type_to_string(err)); \
                       	assert(0);           \
                }                                                    \
        } while (0)
#if !defined(NDEBUG) || defined(DEBUG) || defined(_DEBUG)
#define VKB_DEBUG
#endif
struct vulkanBasicInfo{
        VkDevice                device;
        VkInstance              instance;
        VkPhysicalDevice        physicalDevice;
};
struct vulkanWindowInfo{
        VkFence                         fences;
        VkSwapchainKHR                  swapchain;
        VkRenderPass                    renderPass;
        std::vector<VkFramebuffer>      framebuffers;
        std::vector<VkSemaphore>        imageAcquired;
        std::vector<VkSemaphore>        renderComplete;
};
struct pipelineSwitchInfo {
    VkBool32 bDepthTest;
    VkBool32 bColorBlend;
    VkBool32 bStencilTest;
    VkBool32 bPrimitiveRestart;
    VkCullModeFlagBits cullMode;
};
// struct descriptorSetInfo{
//         VkDescriptorSet                 set;
//         VkDescriptorSetLayout           layout;

// };
struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};
struct bufferInfo{
        VkBuffer buffer;
        VkDeviceSize size;
        VkDeviceMemory memory;
};
struct ImageInfo{
    VkImage image;
    std::string name;
//     uint32_t width;
//     uint32_t height;
    VkImageView view;
    VkDeviceMemory memory;
};
void cleanupPool(VkDevice device);
void cleanupVulkanWindow(VkDevice device, vulkanWindowInfo&windowObj);
void cleanupVulkan(VkInstance& instance, VkDevice& device);
void createPool(VkDevice device, uint32_t descriptorCount, VkCommandPoolCreateFlags flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
void setupVulkanWindow(VkPhysicalDevice physicalDevice, VkDevice device, vulkanWindowInfo&vulkanWindow, VkBool32 bEnableDephtImage = VK_FALSE);
void setupVulkan(const std::vector<const char*>&instanceExtensions, const std::vector<const char*>&deviceExtensions, vulkanBasicInfo&vulkanBasic, void(*createSurfaceFun)(VkInstance instance, VkSurfaceKHR&surface, void* userData) = nullptr, void *userData = nullptr);

const char *cvmx_chip_type_to_string(VkResult type);
void createTextureSampler(VkDevice device, VkSampler& sampler);
// size_t isSupportedExtension(const std::vector<const char*>& extensions);
// VkShaderModule createShaderModule(VkDevice device, const std::string& file);
// VkShaderModule createShaderModule(VkDevice device, const std::vector<char>& code);
uint32_t findMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags properties);
// VkShaderModule createShaderModule(VkDevice device, const uint32_t* code, size_t codeSize);
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
void createTextureImage(VkDevice device, const void * data, uint32_t width, uint32_t height, ImageInfo&image);
void bufferData(VkDevice device, VkDeviceSize size, const void* pData, VkDeviceMemory memory, VkDeviceSize offset = 0);
void createTextureImage(VkDevice device, const bufferInfo&dataBuffer, uint32_t width, uint32_t height, ImageInfo&image);
void createBuffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, bufferInfo&buffer);
//void cleanupSwapchain(VkDevice device, VkSwapchainKHR& swapchain, VkRenderPass& renderPass, std::vector<VkFramebuffer>& frameBuffer);
void create2DImageArray(VkDevice device, const void * const * datas, uint32_t imageCount, uint32_t width, uint32_t height, ImageInfo&image);
void create2DImageArray(VkDevice device, const bufferInfo&dataBuffer, uint32_t imageCount, uint32_t width, uint32_t height, ImageInfo&image);
// void createDescriptorSetLayout(VkDevice device, const std::vector<VkDescriptorSetLayoutBinding>& binding, VkDescriptorSetLayout& layout, VkDescriptorSetLayoutCreateFlags flags = 0);
void drawFrame(VkDevice device, uint32_t currentFrame, const VkCommandBuffer&commandbuffers, const vulkanWindowInfo&vulkanWindow, void(*recreateSwapchain)(void*userData) = nullptr, void *userData = nullptr);
VkResult createImageView(VkDevice device, VkFormat format, VkImageAspectFlags aspectMask, VkImage image, VkImageView& imageView, VkImageViewType type = VK_IMAGE_VIEW_TYPE_2D, uint32_t arrayLayer = 1);
// VkResult createPipelineLayout(VkDevice device, const std::vector<VkDescriptorSetLayout>& descriptorSetLayout, const std::vector<VkPushConstantRange>& pushConstantRange, VkPipelineLayout& pipelineLayout);
void createImage(VkDevice device, VkExtent2D extent, VkImageUsageFlags usage, VkFormat format, VkImageTiling tiling, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, uint32_t arrayLayer = 1, uint32_t depth = 1);
// VkResult createPipelineCache(VkDevice device, const std::string&file, VkPipelineCache&cache);
// void updateDescriptorSets(VkDevice device, const std::vector<VkDescriptorType>&descriptorTypes, const std::vector<bufferInfo>&descriptorBuffer, const std::vector<ImageInfo>&descriptorImage, VkDescriptorSet&destSet, const VkSampler&textureSampler = VK_NULL_HANDLE);
// VkResult createGraphicsPipeline(VkDevice device, VkRenderPass renderPass, const VkExtent2D&windowExtent, VkBool32 bColorBlend, VkBool32 bDepthTest, const VkVertexInputBindingDescription&vertexBinding, const std::vector<VkVertexInputAttributeDescription>&vertexAttribute, const std::vector<VkPipelineShaderStageCreateInfo>& shader, pipelineInfo&pipeline);
// VkResult createGraphicsPipeline(VkDevice device, VkRenderPass renderPass, uint32_t subpass, const std::vector<VkPipelineShaderStageCreateInfo> *shaderStage, const VkPipelineVertexInputStateCreateInfo *vertexInputState, pipelineInfo&Pipeline,
//     const VkPipelineViewportStateCreateInfo *viewportState = nullptr,
//     const VkPipelineInputAssemblyStateCreateInfo *inputAssemblyState = nullptr,
//     const VkPipelineRasterizationStateCreateInfo *rasterizationState = nullptr,
//     const VkPipelineMultisampleStateCreateInfo *multiSampleState = nullptr,
//     const VkPipelineDynamicStateCreateInfo *dynamicState = nullptr,
//     const VkPipelineColorBlendStateCreateInfo *colorBlendState = nullptr,
//     const VkPipelineDepthStencilStateCreateInfo *depthStencilState = nullptr,
//     const VkPipelineTessellationStateCreateInfo *tesellationState = nullptr
// );
void destroyImage(VkDevice device, ImageInfo& image);
void destroyBuffer(VkDevice device, bufferInfo& buffer);
// void destroyPipeline(VkDevice device, const std::string& file, pipelineInfo& pipeline);
// void destroyPipelineCache(VkDevice device, const std::string& file, VkPipelineCache& cache);
void freeCommandBufferMemory(VkDevice device, uint32_t count, VkCommandBuffer* commandBuffer);
//-----
void transitionImageLayout(VkDevice device, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
void pipelineBarrier(VkCommandBuffer commandBuffer, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkImage image, uint32_t arrayLayer = 1);
	//--------
void allocateCommandBuffers(VkDevice device, uint32_t count, VkCommandBuffer* commandBuffer);
void allocateCommandBuffers(VkDevice device, VkCommandPool commandPool, uint32_t count, VkCommandBuffer* commandBuffer);
void allocateDescriptorSets(VkDevice device, uint32_t count, VkDescriptorSetLayout&layout, VkDescriptorSet&descriptorSet);
void allocateDescriptorSets(VkDevice device, VkDescriptorPool descriptorPool, uint32_t count, VkDescriptorSetLayout&layout, VkDescriptorSet&descriptorSet);

VkResult beginSingleTimeCommands(VkDevice device, VkCommandBuffer&commandBuffer);
VkResult beginSingleTimeCommands(VkDevice device, VkCommandPool commandPool, VkCommandBuffer&commandBuffer);
void endSingleTimeCommands(VkDevice device, VkCommandBuffer commandBuffer);
void endSingleTimeCommands(VkDevice device, VkCommandPool commandPool, VkCommandBuffer commandBuffer);

uint32_t GetFileSize(FILE *fp);
uint32_t GetFileContent(const std::string&file, std::vector<uint32_t>&data);
bool writeFileContent(const std::string&file, const void *data, uint32_t size);
#endif // !VULKANFRAME_H
