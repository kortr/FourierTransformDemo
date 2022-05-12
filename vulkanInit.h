#ifndef VULKAN_INIT_HPP
#define VULKAN_INIT_HPP
#include <vector>
#include <iostream>
#include <vulkan/vulkan.h>
namespace vki{
	/*{{{*/
	VkApplicationInfo applicationInfo(const char *pAppName, uint32_t appVersion = 1, const char *pEngineName = nullptr, uint32_t engineVersion = 1, uint32_t apiVersion = 1);
	/*}}}*/
	/*{{{*/
	VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo(PFN_vkDebugUtilsMessengerCallbackEXT debugCallback, VkDebugUtilsMessageSeverityFlagsEXT messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, VkDebugUtilsMessageTypeFlagsEXT messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT);
	/*}}}*/
	/*{{{*/
	VkInstanceCreateInfo instanceCreateInfo(uint32_t layerCount = 0, uint32_t extensionCount = 0, const char *const*layerNames = nullptr, const char * const *extensionName = nullptr, const VkApplicationInfo *pAppInfo = nullptr);
	/*}}}*/
	/*{{{*/
	VkDeviceQueueCreateInfo deviceQueueCreateInfo(uint32_t queueFamilyIndex, uint32_t queueCount, const float *pQueuePriorities);
	/*}}}*/
	/*{{{*/
	//no enable layer
	VkDeviceCreateInfo deviceCreateInfo(const std::vector<VkDeviceQueueCreateInfo>&queueCreateInfo, uint32_t extenionCount = 0, const char * const * extensionNames = nullptr, const VkPhysicalDeviceFeatures *pDeviceFeatures = nullptr);
	/*}}}*/
	/*{{{*/
	// VkDeviceCreateInfo deviceCreateInfo(const std::vector<uint32_t>&queueFamilyIndex, uint32_t extenionCount = 0, const char * const * extensionNames = nullptr, float *pQueuePriorities = nullptr, const VkPhysicalDeviceFeatures *pDeviceFeatures = nullptr){
	// 	std::vector<VkDeviceQueueCreateInfo> queueCreateInfo(queueFamilyIndex.size());
	// 	for(uint32_t i = 0; i < queueCreateInfo.size(); ++i){
	// 		queueCreateInfo[i] = deviceQueueCreateInfo(queueFamilyIndex[i], 1, pQueuePriorities);
	// 	}
	// 	return deviceCreateInfo(queueCreateInfo, extenionCount, extensionNames, pDeviceFeatures);
	// }
	/*}}}*/
	/*{{{*/
	VkSwapchainCreateInfoKHR swapchainCreateInfoKHR(VkSurfaceKHR surface, VkFormat imageFormat, VkExtent2D imageExtent, VkImageUsageFlags imageUsage, uint32_t imageArrayLayers, uint32_t minImageCount, VkColorSpaceKHR colorSpace, VkBool32 clipped, VkCompositeAlphaFlagBitsKHR compositeAlpha, VkSurfaceTransformFlagBitsKHR currentTransform, VkPresentModeKHR presentMode, VkSharingMode imageSharingMode, const std::vector<uint32_t>&indices, VkSwapchainKHR oldSwapchain);	/*}}}*/
	/*{{{*/
	VkSwapchainCreateInfoKHR swapchainCreateInfoKHR(VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR surfaceCapabilities, const std::vector<VkPresentModeKHR>&presentModes, const std::vector<VkSurfaceFormatKHR>&surfaceFormats, VkSharingMode imageSharingMode = VK_SHARING_MODE_EXCLUSIVE, const std::vector<uint32_t>&indices = {});	/*}}}*/
	/*{{{*/
	VkRenderPassCreateInfo renderPassCreateInfo(uint32_t subpassCount, const VkSubpassDescription*pSubpass, uint32_t dependencyCount, const VkSubpassDependency *pDependency, uint32_t attachmentCount, const VkAttachmentDescription *pAttchments);
	/*}}}*/
	/*{{{*/
	VkAttachmentDescription attachmentDescription(VkFormat imageFormat, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp, VkImageLayout initialLayout, VkImageLayout finalLayout, VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE, VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);	/*}}}*/
	/*{{{*/
	VkAttachmentReference attachmentReference(uint32_t attachment, VkImageLayout layout);	/*}}}*/
	/*{{{*/
	VkSubpassDescription subPassDescription(uint32_t attachmentReferenceCount, const VkAttachmentReference *pColorAttachmentRefs, const VkAttachmentReference *pDepthStencilAttachment = nullptr, VkPipelineBindPoint pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS);	/*}}}*/
	/*{{{*/
	VkSubpassDependency subpassDependency(uint32_t srcSubpass = VK_SUBPASS_EXTERNAL, uint32_t dstSubpass = 0, VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VkAccessFlags srcAccessMask = 0, VkAccessFlags dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);	/*}}}*/
	/*{{{*/
	VkFramebufferCreateInfo framebufferCreateInfo(VkRenderPass renderPass, const VkExtent2D&frameExtent, uint32_t frameBufferAttachmentCount, const VkImageView *pFramebufferAttachments, uint32_t layers = 1);	/*}}}*/
	VkImageViewCreateInfo imageViewCreateInfo(VkFormat format, VkImageAspectFlags aspectMask, VkImageViewType type = VK_IMAGE_VIEW_TYPE_2D, uint32_t arrayLayer = 1);
	VkImageCreateInfo imageCreateInfo(VkExtent2D extent, VkImageUsageFlags usage, VkFormat format, VkImageTiling tiling, uint32_t arrayLayer = 1, uint32_t depth = 1);
	VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo(uint32_t vertexAttributeCount, const VkVertexInputAttributeDescription *pVertexAttributeDescriptions, const VkVertexInputBindingDescription *pVertexBindingDescriptions, uint32_t vertexBindingDescriptionCount = 1);
	VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VkBool32 primitiveRestartEnable = VK_FALSE);
	VkViewport viewportInfo(const VkExtent2D&extent, float x = 0.0f, float y = 0.0f);
	VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo(const VkViewport *pViewport, const VkRect2D *pScissor, uint32_t viewportCount = 1, uint32_t scissorCount = 1);
	VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo(VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT, VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE, VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL);
	VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo(VkBool32 sampleShadingEnable = VK_FALSE, VkSampleCountFlagBits rasterizationSamples = VK_SAMPLE_COUNT_1_BIT);
	VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState(VkBool32 bColorBlend);
	VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo(uint32_t colorBlendAttachmentCount, const VkPipelineColorBlendAttachmentState *pColorBlendAttachment);
	VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo(VkBool32 depthTestEnable = VK_FALSE, VkCompareOp depthCompareOp = VK_COMPARE_OP_LESS, VkBool32 stencilTestEnable = VK_FALSE);
	VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo(uint32_t dynamicCount, const VkDynamicState *pDynamic);
	VkBufferCreateInfo bufferCreateInfo(VkDeviceSize size, VkBufferUsageFlags usage, VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE);
	VkMemoryAllocateInfo memoryAllocateInfo(VkDeviceSize size, uint32_t memoryTypeIndex = 0);
	VkBufferImageCopy bufferImageCopy(VkExtent2D extent2d, uint32_t baseArrayLayer = 0, VkDeviceSize bufferOffset = 0, uint32_t layerCount = 1, VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, uint32_t depth = 1);
	VkPushConstantRange pushConstantRange(VkDeviceSize size, VkShaderStageFlags stageFlags = VK_SHADER_STAGE_VERTEX_BIT,uint32_t offset = 0);
	VkWriteDescriptorSet writeDescriptorSet(VkDescriptorSet dstSet, VkDescriptorType descriptorType, uint32_t dstBinding = 0, uint32_t descriptorCount = 1);
	VkSemaphoreCreateInfo semaphoreCreateInfo();
	VkFenceCreateInfo fenceCreateInfo(VkFenceCreateFlags flags = VK_FENCE_CREATE_SIGNALED_BIT);
	VkCommandBufferBeginInfo commandBufferBeginInfo();
	VkRenderPassBeginInfo renderPassBeginInfo(const VkClearValue *pClearValues, const VkExtent2D&windowSize, VkFramebuffer frameBuffer, VkRenderPass renderPass, uint32_t clearValueCount = 1, VkOffset2D offset = { 0 });
}
#endif