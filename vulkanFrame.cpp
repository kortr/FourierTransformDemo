#include <set>
#include <iostream>
#include <fstream>
#include "vulkanFrame.h"
static ImageInfo g_DepthImage;
static std::vector<VkImage> g_SwapchainImages;
QueueFamilyIndices g_QueueFamilyIndices;
static std::vector<VkImageView>g_SwapchainImageViews;

static VkSurfaceKHR				g_Surface;
static VkQueue						g_Present;
VkQueue						g_Graphics;
static VkDebugUtilsMessengerEXT 	g_Messenger;
static VkCommandPool				g_CommandPool;
VkDescriptorPool			g_DescriptorPool;
static VkPhysicalDeviceMemoryProperties g_MemoryProperties;
const char* cvmx_chip_type_to_string(VkResult type) {
	switch (type) {
	case VK_SUCCESS:return "VK_SUCCESS";
	case VK_NOT_READY:return "VK_NOT_READY";
	case VK_TIMEOUT:return "VK_TIMEOUT";
	case VK_EVENT_SET:return "VK_EVENT_SET";
	case VK_EVENT_RESET:return "VK_EVENT_RESET";
	case VK_INCOMPLETE:return "VK_INCOMPLETE";
	case VK_ERROR_OUT_OF_HOST_MEMORY:return "VK_ERROR_OUT_OF_HOST_MEMORY";
	case VK_ERROR_OUT_OF_DEVICE_MEMORY:return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
	case VK_ERROR_INITIALIZATION_FAILED:return "VK_ERROR_INITIALIZATION_FAILED";
	case VK_ERROR_DEVICE_LOST:return "VK_ERROR_DEVICE_LOST";
	case VK_ERROR_MEMORY_MAP_FAILED:return "VK_ERROR_MEMORY_MAP_FAILED";
	case VK_ERROR_LAYER_NOT_PRESENT:return "VK_ERROR_LAYER_NOT_PRESENT";
	case VK_ERROR_EXTENSION_NOT_PRESENT:return "VK_ERROR_EXTENSION_NOT_PRESENT";
	case VK_ERROR_FEATURE_NOT_PRESENT:return "VK_ERROR_FEATURE_NOT_PRESENT";
	case VK_ERROR_INCOMPATIBLE_DRIVER:return "VK_ERROR_INCOMPATIBLE_DRIVER";
	case VK_ERROR_TOO_MANY_OBJECTS:return "VK_ERROR_TOO_MANY_OBJECTS";
	case VK_ERROR_FORMAT_NOT_SUPPORTED:return "VK_ERROR_FORMAT_NOT_SUPPORTED";
	case VK_ERROR_FRAGMENTED_POOL:return "VK_ERROR_FRAGMENTED_POOL";
	case VK_ERROR_OUT_OF_POOL_MEMORY:return "VK_ERROR_OUT_OF_POOL_MEMORY";
	case VK_ERROR_INVALID_EXTERNAL_HANDLE:return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
	case VK_ERROR_FRAGMENTATION:return "VK_ERROR_FRAGMENTATION";
	case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
	case VK_ERROR_SURFACE_LOST_KHR:return "VK_ERROR_SURFACE_LOST_KHR";
	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
	case VK_SUBOPTIMAL_KHR:return "VK_SUBOPTIMAL_KHR";
	case VK_ERROR_OUT_OF_DATE_KHR:return "VK_ERROR_OUT_OF_DATE_KHR";
	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
	case VK_ERROR_VALIDATION_FAILED_EXT:return "VK_ERROR_VALIDATION_FAILED_EXT";
	case VK_ERROR_INVALID_SHADER_NV:return "VK_ERROR_INVALID_SHADER_NV";
	case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
	case VK_ERROR_NOT_PERMITTED_EXT:return "VK_ERROR_NOT_PERMITTED_EXT";
	case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
	default:
		break;
	}
	return "VK_ERROR_UNKNOWN";
}
VkBool32 VKAPI_PTR debugUtilsMessenger(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData){
    const char* strMessageSeverity = nullptr;//, *strMessageTypes = nullptr;
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
            strMessageSeverity = "VERBOSE";
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
            strMessageSeverity = "INFO";
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            strMessageSeverity = "WARNING";
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
            strMessageSeverity = "ERROR";
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT) {
            strMessageSeverity = "FLAG";
    }
    std::cout << "[VULKAN VALIDATION LAYER]\nSEVERITY:" << strMessageSeverity << "\nMESSAGE:" << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}
//uint32_t g_QueueFamily[2] = { -1, -1 };, PFN_vkDebugUtilsMessengerCallbackEXT debugCallback
void setupVulkan(const std::vector<const char*>& instanceExtensions, const std::vector<const char*>& deviceExtensions, vulkanBasicInfo& vulkanBasic, void(*createSurfaceFun)(VkInstance instance, VkSurfaceKHR&surface, void* userData), void* userData){
	std::vector<const char *>extensions = instanceExtensions;
#ifdef VKB_DEBUG
	std::vector<const char *>layers;
	layers.push_back("VK_LAYER_KHRONOS_validation");
	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	VkInstanceCreateInfo instanceInfo = vki::instanceCreateInfo(layers.size(), extensions.size(), layers.data(), extensions.data());
	VK_CHECK(vkCreateInstance(&instanceInfo, nullptr, &vulkanBasic.instance));
	VkDebugUtilsMessengerCreateInfoEXT debugInfo = vki::debugUtilsMessengerCreateInfo(debugUtilsMessenger);
	auto fun = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkanBasic.instance, "vkCreateDebugUtilsMessengerEXT");
	if(fun)VK_CHECK(fun(vulkanBasic.instance, &debugInfo, nullptr, &g_Messenger));
#else
	VkInstanceCreateInfo instanceInfo = vki::instanceCreateInfo(0, extensions.size(), nullptr, extensions.data());
	VK_CHECK(vkCreateInstance(&instanceInfo, nullptr, &vulkanBasic.instance));
#endif
	uint32_t count;
	vkEnumeratePhysicalDevices(vulkanBasic.instance, &count, nullptr);
	if(count == 0){
		printf("No suitable graphics card!\n");
		return;
	}
	std::vector<VkPhysicalDevice>physicalDevices(count);
	vkEnumeratePhysicalDevices(vulkanBasic.instance, &count, physicalDevices.data());
	vulkanBasic.physicalDevice = physicalDevices[0];
	if(vulkanBasic.physicalDevice == VK_NULL_HANDLE){
		printf("No suitable graphics card!\n");
		return;
	}
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(vulkanBasic.physicalDevice, &physicalDeviceProperties);
	printf("gpu count:%d, gpu name:%s\n", count, physicalDeviceProperties.deviceName);

	vkGetPhysicalDeviceMemoryProperties(vulkanBasic.physicalDevice, &g_MemoryProperties);

	if(createSurfaceFun)createSurfaceFun(vulkanBasic.instance, g_Surface, userData);

	float queuePriorities = 1.0f;
	g_QueueFamilyIndices = findQueueFamilies(vulkanBasic.physicalDevice, g_Surface);
	std::set<size_t> sQueuefamily = { g_QueueFamilyIndices.graphicsFamily.value(), g_QueueFamilyIndices.presentFamily.value() };
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfo;
	for(int queueFamily : sQueuefamily){
		queueCreateInfo.push_back(vki::deviceQueueCreateInfo(queueFamily, 1, &queuePriorities));
	}
	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(vulkanBasic.physicalDevice, &features);
	features.samplerAnisotropy = VK_TRUE;//保证该值一定为true
	extensions = deviceExtensions;
	extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	VkDeviceCreateInfo deviceInfo = vki::deviceCreateInfo(queueCreateInfo, extensions.size(), extensions.data(), &features);
	VK_CHECK(vkCreateDevice(vulkanBasic.physicalDevice, &deviceInfo, nullptr, &vulkanBasic.device));
	vkGetDeviceQueue(vulkanBasic.device, g_QueueFamilyIndices.graphicsFamily.value(), 0, &g_Graphics);
	vkGetDeviceQueue(vulkanBasic.device, g_QueueFamilyIndices.presentFamily.value(), 0, &g_Present);
}
void setupVulkanWindow(VkPhysicalDevice physicalDevice, VkDevice device, vulkanWindowInfo&vulkanWindow, VkBool32 bEnableDephtImage){
	uint32_t count;
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, g_Surface, &surfaceCapabilities);

	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, g_Surface, &count, nullptr);
	if(count == 0){
		printf("in function:%s:No suitable physical device surface formats!\n", __FUNCTION__);
		return;
	}
	std::vector<VkSurfaceFormatKHR>surfaceFormats(count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, g_Surface, &count, surfaceFormats.data());

	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, g_Surface, &count, nullptr);
	if(count == 0){
		printf("in function %s:No suitable physical device surface present modes!\n", __FUNCTION__);
		return;
	}
	std::vector<VkPresentModeKHR>presentModes(count);
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, g_Surface, &count, presentModes.data());

	VkSwapchainCreateInfoKHR swapchainInfo = vki::swapchainCreateInfoKHR(g_Surface, surfaceCapabilities, presentModes, surfaceFormats);
	VK_CHECK(vkCreateSwapchainKHR(device, &swapchainInfo, nullptr, &vulkanWindow.swapchain));

	vkGetSwapchainImagesKHR(device, vulkanWindow.swapchain, &count, nullptr);
	if(count == 0){
		printf("in function %s:No swapchain image!\n", __FUNCTION__);
		return;
	}
	g_SwapchainImages.resize(count);
	vkGetSwapchainImagesKHR(device, vulkanWindow.swapchain, &count, g_SwapchainImages.data());
	g_SwapchainImageViews.resize(g_SwapchainImages.size());
	if(bEnableDephtImage){
		if(VK_NULL_HANDLE == g_CommandPool)createPool(device, 1);
		createImage(device, swapchainInfo.imageExtent, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_IMAGE_TILING_OPTIMAL, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT, g_DepthImage.image, g_DepthImage.memory);
		createImageView(device, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_IMAGE_ASPECT_DEPTH_BIT, g_DepthImage.image, g_DepthImage.view);
		
		transitionImageLayout(device, g_DepthImage.image, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}

	VkSubpassDescription subPass;
	VkSubpassDependency dependency = vki::subpassDependency();
	VkAttachmentReference colorAttachmentRef = vki::attachmentReference(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	VkAttachmentReference depthAttachmentRef = vki::attachmentReference(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	std::vector<VkAttachmentDescription> attachments = { vki::attachmentDescription(swapchainInfo.imageFormat, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) };
	if(bEnableDephtImage){
		attachments.push_back(vki::attachmentDescription(VK_FORMAT_D32_SFLOAT_S8_UINT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL));
		subPass = vki::subPassDescription(1, &colorAttachmentRef, &depthAttachmentRef);
	}
	else{
		subPass = vki::subPassDescription(1, &colorAttachmentRef);		
	}
	VkRenderPassCreateInfo renderPassInfo = vki::renderPassCreateInfo(1, &subPass, 1, &dependency, attachments.size(), attachments.data());
	// renderPassInfo.pSubpasses = &subPass;
	VK_CHECK(vkCreateRenderPass(device, &renderPassInfo, nullptr, &vulkanWindow.renderPass));
	
	vulkanWindow.framebuffers.resize(g_SwapchainImages.size());
	std::vector<VkImageView> frameBufferAttachments(1);
	if(bEnableDephtImage)frameBufferAttachments.push_back(g_DepthImage.view);
	for (size_t i = 0; i < vulkanWindow.framebuffers.size(); i++){
		VK_CHECK(createImageView(device, swapchainInfo.imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, g_SwapchainImages[i], g_SwapchainImageViews[i]));
		frameBufferAttachments[0] = g_SwapchainImageViews[i];
		VkFramebufferCreateInfo frameBufferInfo = vki::framebufferCreateInfo(vulkanWindow.renderPass, surfaceCapabilities.maxImageExtent, frameBufferAttachments.size(), frameBufferAttachments.data());
		VK_CHECK(vkCreateFramebuffer(device, &frameBufferInfo, nullptr, &vulkanWindow.framebuffers[i]));
	}
	VkFenceCreateInfo fenceInfo = vki::fenceCreateInfo();
	VkSemaphoreCreateInfo semaphoreInfo = vki::semaphoreCreateInfo();
	vulkanWindow.imageAcquired.resize(vulkanWindow.framebuffers.size());
	vulkanWindow.renderComplete.resize(vulkanWindow.framebuffers.size());
	//imagesInFlight.resize(g_FrameBuffer.size(), VK_NULL_HANDLE);
	VK_CHECK(vkCreateFence(device, &fenceInfo, nullptr, &vulkanWindow.fences));
	for (size_t i = 0; i < vulkanWindow.framebuffers.size(); i++) {
		VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &vulkanWindow.renderComplete[i]));
		VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &vulkanWindow.imageAcquired[i]));
	}
}
VkResult createImageView(VkDevice device, VkFormat format, VkImageAspectFlags aspectMask, VkImage image, VkImageView&imageView, VkImageViewType type, uint32_t arrayLayer) {
	VkImageViewCreateInfo imageViewInfo = vki::imageViewCreateInfo(format, aspectMask, type, arrayLayer);
	imageViewInfo.image = image;
	return vkCreateImageView(device, &imageViewInfo, nullptr, &imageView);
}
void createImage(VkDevice device,VkExtent2D extent, VkImageUsageFlags usage, VkFormat format, VkImageTiling tiling, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, uint32_t arrayLayer, uint32_t depth) {
	VkImageCreateInfo imageInfo = vki::imageCreateInfo({ extent.width, extent.height }, usage, format, tiling, arrayLayer, depth);
	VK_CHECK(vkCreateImage(device, &imageInfo, nullptr, &image));
	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(device, image, &memoryRequirements);
	// VkMemoryAllocateInfo allocateInfo = vki::memoryAllocateInfo(memoryRequirements.size);
	VkMemoryAllocateInfo allocateInfo = vki::memoryAllocateInfo(memoryRequirements.size, findMemoryTypeIndex(memoryRequirements.memoryTypeBits, properties));
	// allocateInfo.memoryTypeIndex = findMemoryTypeIndex(memoryRequirements.memoryTypeBits, properties);
	VK_CHECK(vkAllocateMemory(device, &allocateInfo, nullptr, &imageMemory));
	VK_CHECK(vkBindImageMemory(device, image, imageMemory, 0));
}
uint32_t findMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	for (size_t i = 0; i < g_MemoryProperties.memoryTypeCount; i++){
		if (typeFilter & (1 << i) && (g_MemoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	return -1;
}
// size_t isSupportedExtension(const std::vector<const char *>&extensions) {
// 	if (extensions.empty())return 0;
// 	uint32_t count;
// 	size_t noSuppertedExtensionIndex = extensions.size();
// 	vkEnumerateInstanceExtensionProperties(nullptr, &count,nullptr);
// 	std::vector<VkExtensionProperties>instanceExtensions(count);
// 	vkEnumerateInstanceExtensionProperties(nullptr, &count, instanceExtensions.data());
// 	for (size_t i = 0; i < extensions.size(); i++){
// 		size_t j = 0;
// 		for (; j < instanceExtensions.size() && strcmp(extensions[i], instanceExtensions[j].extensionName); j++);
// 		if (j == count) {
// 			noSuppertedExtensionIndex = i;
// 			break;
// 		}
// 	}
// 	return noSuppertedExtensionIndex;
// }
//command pool and descriptor pool
void createPool(VkDevice device, uint32_t descriptorCount, VkCommandPoolCreateFlags flags){
	if(VK_NULL_HANDLE != g_CommandPool || VK_NULL_HANDLE != g_DescriptorPool)cleanupPool(device);
	VkCommandPoolCreateInfo commandPoolInfo = {};
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.flags = flags | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	//commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolInfo.queueFamilyIndex = g_QueueFamilyIndices.graphicsFamily.value();
	VK_CHECK(vkCreateCommandPool(device, &commandPoolInfo, nullptr, &g_CommandPool));
	std::vector<VkDescriptorPoolSize> descriptorPoolSize = {
			{ VK_DESCRIPTOR_TYPE_SAMPLER, descriptorCount },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, descriptorCount },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, descriptorCount },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, descriptorCount },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, descriptorCount },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, descriptorCount },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, descriptorCount },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, descriptorCount },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, descriptorCount },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, descriptorCount },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, descriptorCount}
	};

	VkDescriptorPoolCreateInfo descriptorPoolInfo;
	descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolInfo.flags = 0;
	descriptorPoolInfo.pNext = nullptr;
	descriptorPoolInfo.maxSets = descriptorCount;
	descriptorPoolInfo.poolSizeCount = descriptorPoolSize.size();
	descriptorPoolInfo.pPoolSizes = descriptorPoolSize.data();
	VK_CHECK(vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &g_DescriptorPool));
}

void allocateCommandBuffers(VkDevice device, uint32_t count, VkCommandBuffer* commandBuffer) {
	allocateCommandBuffers(device, g_CommandPool, count, commandBuffer);
}
void allocateCommandBuffers(VkDevice device, VkCommandPool commandPool, uint32_t count, VkCommandBuffer *commandBuffer) {
	VkCommandBufferAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.commandBufferCount = count;
	allocateInfo.commandPool = commandPool;
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	VK_CHECK(vkAllocateCommandBuffers(device, &allocateInfo, commandBuffer));
}
void allocateDescriptorSets(VkDevice device, uint32_t count, VkDescriptorSetLayout&layout, VkDescriptorSet&descriptorSet) {
	allocateDescriptorSets(device, g_DescriptorPool, count, layout, descriptorSet);
}
void allocateDescriptorSets(VkDevice device, VkDescriptorPool descriptorPool, uint32_t count, VkDescriptorSetLayout&layout, VkDescriptorSet&descriptorSet) {
	VkDescriptorSetAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocateInfo.descriptorPool = descriptorPool;
	allocateInfo.descriptorSetCount = count;
	allocateInfo.pSetLayouts = &layout;
	VK_CHECK(vkAllocateDescriptorSets(device, &allocateInfo, &descriptorSet));
}
VkResult beginSingleTimeCommands(VkDevice device, VkCommandBuffer&commandBuffer){
	return beginSingleTimeCommands(device, g_CommandPool, commandBuffer);
}
VkResult beginSingleTimeCommands(VkDevice device, VkCommandPool commandPool, VkCommandBuffer&commandBuffer){
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	return vkBeginCommandBuffer(commandBuffer, &beginInfo);
}
void endSingleTimeCommands(VkDevice device, VkCommandBuffer commandBuffer) {
	endSingleTimeCommands(device, g_CommandPool, commandBuffer);
}
void endSingleTimeCommands(VkDevice device, VkCommandPool commandPool, VkCommandBuffer commandBuffer){
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(g_Graphics, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(g_Graphics);

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}
		if (surface) {
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
			if (presentSupport) {
				indices.presentFamily = i;
			}
		}
		if (indices.isComplete()) {
			break;
		}
		i++;
	}
	return indices;
}
void cleanupVulkan(VkInstance& instance, VkDevice& device) {
	vkDestroyDevice(device, nullptr);
	auto fun = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if(fun)fun(instance, g_Messenger, nullptr);
	if(g_Surface)vkDestroySurfaceKHR(instance, g_Surface, nullptr);
	vkDestroyInstance(instance, nullptr);
}
void cleanupVulkanWindow(VkDevice device, vulkanWindowInfo&windowObj) {
	destroyImage(device, g_DepthImage);
	//cleanupSwapchain(device, swapchain, renderPass, frameBuffer);
	vkDestroyRenderPass(device, windowObj.renderPass, nullptr);
	for (size_t i = 0; i < windowObj.framebuffers.size(); i++) {
		vkDestroyFramebuffer(device, windowObj.framebuffers[i], nullptr);
	}
	for (size_t i = 0; i < g_SwapchainImageViews.size(); i++) {
		vkDestroyImageView(device, g_SwapchainImageViews[i], nullptr);
	}
	vkDestroyFence(device, windowObj.fences, nullptr);
	for (size_t i = 0; i < windowObj.imageAcquired.size(); i++){//����Ҫ����ͼƬ
		vkDestroySemaphore(device, windowObj.imageAcquired[i], nullptr);
		vkDestroySemaphore(device, windowObj.renderComplete[i], nullptr);
	}
	vkDestroySwapchainKHR(device, windowObj.swapchain, nullptr);
	windowObj.framebuffers.clear();
	g_SwapchainImageViews.clear();
}
//void cleanupSwapchain(VkDevice device, VkSwapchainKHR&swapchain, VkRenderPass& renderPass, std::vector<VkFramebuffer>& frameBuffer) {
//	vkDestroyRenderPass(device, renderPass, nullptr);
//	for (size_t i = 0; i < frameBuffer.size(); i++) {
//		vkDestroyFramebuffer(device, frameBuffer[i], nullptr);
//	}
//	//for (size_t i = 0; i < g_SwapchainImages.size(); i++){//����Ҫ����ͼƬ
//	//	vkDestroyImage(device, g_SwapchainImages[i], nullptr);
//	//}
//	for (size_t i = 0; i < g_SwapchainImageViews.size(); i++){
//		vkDestroyImageView(device, g_SwapchainImageViews[i], nullptr);
//	}
//	vkDestroySwapchainKHR(device, swapchain, nullptr);
//	frameBuffer.clear();
//	g_SwapchainImages.clear();
//	g_SwapchainImageViews.clear();
//}
void cleanupPool(VkDevice device) {
	vkDestroyCommandPool(device, g_CommandPool, nullptr);g_CommandPool = VK_NULL_HANDLE;
	vkDestroyDescriptorPool(device, g_DescriptorPool, nullptr);g_DescriptorPool = VK_NULL_HANDLE;
}
// void createDescriptorSetLayout(VkDevice device, const std::vector<VkDescriptorSetLayoutBinding>&binding, VkDescriptorSetLayout&layout, VkDescriptorSetLayoutCreateFlags flags) {
// 	VkDescriptorSetLayoutCreateInfo descriptorSetLayout;
// 	descriptorSetLayout.flags = flags;
//     descriptorSetLayout.pNext = nullptr;
// 	descriptorSetLayout.pBindings = binding.data();
// 	descriptorSetLayout.bindingCount = binding.size();
// 	descriptorSetLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
// 	VK_CHECK(vkCreateDescriptorSetLayout(device, &descriptorSetLayout, nullptr, &layout));
// }
void createBuffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, bufferInfo&buffer) {
	buffer.size = size;
	VkBufferCreateInfo bufferInfo = vki::bufferCreateInfo(size, usage);
	VK_CHECK(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer.buffer));
	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(device, buffer.buffer, &memoryRequirements);
	VkMemoryAllocateInfo allocateInfo = vki::memoryAllocateInfo(memoryRequirements.size, findMemoryTypeIndex(memoryRequirements.memoryTypeBits, properties));
	VK_CHECK(vkAllocateMemory(device, &allocateInfo, nullptr, &buffer.memory));
	VK_CHECK(vkBindBufferMemory(device, buffer.buffer, buffer.memory, 0));
}
void destroyImage(VkDevice device, ImageInfo& image) {
	vkFreeMemory(device, image.memory, nullptr);
	vkDestroyImage(device, image.image, nullptr);
	vkDestroyImageView(device, image.view, nullptr);
}
void destroyBuffer(VkDevice device, bufferInfo&buffer) {
	vkFreeMemory(device, buffer.memory, nullptr);
	vkDestroyBuffer(device, buffer.buffer, nullptr);
	memset(&buffer, 0, sizeof(bufferInfo));
}
void bufferData(VkDevice device, VkDeviceSize size, const void * pData, VkDeviceMemory memory, VkDeviceSize offset) {
	void* data;
	VK_CHECK(vkMapMemory(device, memory, offset, size, 0, &data));
	memcpy(data, pData, size);
	vkUnmapMemory(device, memory);
}
void drawFrame(VkDevice device, uint32_t currentFrame, const VkCommandBuffer& commandbuffers, const vulkanWindowInfo&vulkanWindow, void(*recreateSwapchain)(void* userData), void* userData){
	uint32_t imageIndex;
	VK_CHECK(vkWaitForFences(device, 1, &vulkanWindow.fences, VK_TRUE, std::numeric_limits<uint64_t>::max()));
	VkResult result = vkAcquireNextImageKHR(device, vulkanWindow.swapchain, UINT64_MAX, vulkanWindow.imageAcquired[currentFrame], VK_NULL_HANDLE, &imageIndex);
	if (recreateSwapchain && result == VK_ERROR_OUT_OF_DATE_KHR) {//VK_SUBOPTIMAL_KHR��ʾ��������Ȼ����;��Ȼ����治��ȫƥ�䣬����ɹ����ͼƬ
		recreateSwapchain(userData);// && result != VK_SUBOPTIMAL_KHR
		return;
	}
	else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){
		printf("failed to acquire swap chain image!\n");
		return;
	}
	if (vulkanWindow.fences != VK_NULL_HANDLE) {
		VK_CHECK(vkResetFences(device, 1, &vulkanWindow.fences));//�ᵼ�������wait�޷�����ִ��
	}
	// imagesInFlight[imageIndex] = inFlightFences[currentFrame];
	VkSubmitInfo submitInfo = {};
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandbuffers;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &vulkanWindow.imageAcquired[currentFrame];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &vulkanWindow.renderComplete[currentFrame];
	submitInfo.pWaitDstStageMask = waitStages;
	VK_CHECK(vkQueueSubmit(g_Graphics, 1, &submitInfo, vulkanWindow.fences));
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &vulkanWindow.swapchain;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &vulkanWindow.renderComplete[currentFrame];
	result = vkQueuePresentKHR(g_Present, &presentInfo);
	if (recreateSwapchain && (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)) {
		recreateSwapchain(userData);
	}
	else if(VK_SUCCESS != result){
		printf("failed to acquire swap chain image!\n");
	}
}

void transitionImageLayout(VkDevice device, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
	VkCommandBuffer commandBuffer;
	beginSingleTimeCommands(device, commandBuffer);
	pipelineBarrier(commandBuffer, format, oldLayout, newLayout, image);
	endSingleTimeCommands(device, commandBuffer);
}
void pipelineBarrier(VkCommandBuffer commandBuffer, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkImage image, uint32_t arrayLayer) {
	     VkImageMemoryBarrier barrier = {};
	     barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	     barrier.oldLayout = oldLayout;
	     barrier.newLayout = newLayout;
	     barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	     barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	     barrier.image = image;
	     if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
	         barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

	         if (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT) {
	             barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	         }
	     }
	     else {
	         barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	     }

	     barrier.subresourceRange.baseMipLevel = 0;
	     barrier.subresourceRange.levelCount = 1;
	     barrier.subresourceRange.baseArrayLayer = 0;
	     barrier.subresourceRange.layerCount = arrayLayer;

	     VkPipelineStageFlags sourceStage;
	     VkPipelineStageFlags destinationStage;

	     if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
	         barrier.srcAccessMask = 0;
	         barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

	         sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	         destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	     }
	     else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
	         barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	         barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	         sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	         destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	     }
	     else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
	         barrier.srcAccessMask = 0;
	         barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	         sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	         destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	     }
	     else {
	         throw std::invalid_argument("unsupported layout transition!");
	     }

		 vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}
void freeCommandBufferMemory(VkDevice device, uint32_t count, VkCommandBuffer* commandBuffer){
	vkFreeCommandBuffers(device, g_CommandPool, count, commandBuffer);
}
 
void createTextureImage(VkDevice device, const void *datas, uint32_t width, uint32_t height, ImageInfo&image) {
	bufferInfo tempStorageBuffer;
	VkDeviceSize imageSize = width * height * 4;
	createBuffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, tempStorageBuffer);
	bufferData(device, imageSize, datas, tempStorageBuffer.memory);
	createTextureImage(device, tempStorageBuffer, width, height, image);
	vkFreeMemory(device, tempStorageBuffer.memory, nullptr);
	vkDestroyBuffer(device, tempStorageBuffer.buffer, nullptr);
}
void createTextureImage(VkDevice device, const bufferInfo&dataBuffer, uint32_t width, uint32_t height, ImageInfo&image) {
	createImage(device, { width, height }, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image.image, image.memory);
	VkCommandBuffer cmd;
	beginSingleTimeCommands(device, cmd);
	pipelineBarrier(cmd, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, image.image);
	VkBufferImageCopy bufferCopyRegions = vki::bufferImageCopy({ width, height });
	vkCmdCopyBufferToImage(cmd, dataBuffer.buffer, image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferCopyRegions);
	pipelineBarrier(cmd, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, image.image);
	endSingleTimeCommands(device, cmd);
	VK_CHECK(createImageView(device, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, image.image, image.view, VK_IMAGE_VIEW_TYPE_2D));
}
void create2DImageArray(VkDevice device, const void * const * datas, uint32_t imageCount, uint32_t width, uint32_t height, ImageInfo&image) {
	// size_t imageCount = datas.size();
	bufferInfo tempStorageBuffer;
	VkDeviceSize imageSize = width * height * 4;
	createBuffer(device, imageSize * imageCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, tempStorageBuffer);
	for(uint32_t i = 0; i < imageCount; ++i){
		bufferData(device, imageSize, datas[i], tempStorageBuffer.memory, i * imageSize);
	}
	create2DImageArray(device, tempStorageBuffer, imageCount, width, height, image);
	vkFreeMemory(device, tempStorageBuffer.memory, nullptr);
	vkDestroyBuffer(device, tempStorageBuffer.buffer, nullptr);
}
void create2DImageArray(VkDevice device, const bufferInfo&dataBuffer, uint32_t imageCount, uint32_t width, uint32_t height, ImageInfo&image) {
	VkDeviceSize imageSize = width * height * 4;
	createImage(device, { width, height }, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image.image, image.memory, imageCount);
	VkCommandBuffer cmd;
	beginSingleTimeCommands(device, cmd);

	pipelineBarrier(cmd, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, image.image, imageCount);
	std::vector<VkBufferImageCopy> bufferCopyRegions;
	for (size_t i = 0; i < imageCount; ++i) {
		bufferCopyRegions.push_back(vki::bufferImageCopy({ width, height }, i, imageSize * i));
	}
	vkCmdCopyBufferToImage(cmd, dataBuffer.buffer, image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, bufferCopyRegions.size(), bufferCopyRegions.data());
	pipelineBarrier(cmd, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, image.image, imageCount);
	endSingleTimeCommands(device, cmd);
	VK_CHECK(createImageView(device, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, image.image, image.view, VK_IMAGE_VIEW_TYPE_2D_ARRAY, imageCount));
}
void createTextureSampler(VkDevice device, VkSampler& sampler){
    VkSamplerCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    createInfo.magFilter = VK_FILTER_LINEAR;
    createInfo.minFilter = VK_FILTER_LINEAR;
    createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.anisotropyEnable = VK_TRUE;
    createInfo.maxAnisotropy = 16;
    createInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    createInfo.unnormalizedCoordinates = VK_FALSE;
    createInfo.compareEnable = VK_FALSE;
    createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    VK_CHECK(vkCreateSampler(device, &createInfo, nullptr, &sampler));
}
// void updateDescriptorSets(VkDevice device, const std::vector<VkDescriptorType>&descriptorTypes, const std::vector<bufferInfo>&descriptorBuffer, const std::vector<ImageInfo>&descriptorImage, VkDescriptorSet&destSet, const VkSampler&textureSampler){
//     //  = { g_PianoDynamicUniformBuffer };
//     std::vector<uint32_t>index(2);//一个uniform一个图片采样器。如果需要其他则个数必须增加
//     std::vector<VkDescriptorImageInfo>descriptorImageInfo(descriptorImage.size());
//     std::vector<VkDescriptorBufferInfo> descriptorBufferInfo(descriptorBuffer.size());
//     std::vector<VkWriteDescriptorSet>writeDescriptorSet(descriptorBuffer.size() + descriptorImage.size());
//     for (size_t i = 0; i < writeDescriptorSet.size(); ++i){
//         writeDescriptorSet[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//         writeDescriptorSet[i].descriptorCount = 1;
//         writeDescriptorSet[i].dstBinding = i;
//         writeDescriptorSet[i].dstSet = destSet;
//         writeDescriptorSet[i].descriptorType = descriptorTypes[i];
//         switch (writeDescriptorSet[i].descriptorType){
//         case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
//             descriptorImageInfo[index[1]].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//             descriptorImageInfo[index[1]].imageView = descriptorImage[index[1]].view;
//             descriptorImageInfo[index[1]].sampler = textureSampler;
//             writeDescriptorSet[i].pImageInfo = &descriptorImageInfo[index[1]++];
//             break;
//         case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
//         case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
// //            descriptorBufferInfo[i].offset = 0;
//             descriptorBufferInfo[index[0]].range = descriptorBuffer[index[0]].size;
//             descriptorBufferInfo[index[0]].buffer = descriptorBuffer[index[0]].buffer;
//             writeDescriptorSet[i].pBufferInfo = &descriptorBufferInfo[index[0]++];
//             break;
//         default:
//             break;
//         }
//     }
//     vkUpdateDescriptorSets(device, writeDescriptorSet.size(), writeDescriptorSet.data(), 0, nullptr);
// }
uint32_t GetFileSize(FILE *fp){
    uint32_t size = 0;
    if(fp){
        fseek(fp, 0, SEEK_END);
        size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
    }
    return size;
}
uint32_t GetFileContent(const std::string&file, std::vector<uint32_t>&data){
    FILE *fp = fopen(file.c_str(), "rb");
    if(!fp){
        perror("open file error");
        return 0;
    }
    uint32_t size = GetFileSize(fp);
    data.resize(size / sizeof(uint32_t));
    fread(data.data(), size, 1, fp);
    fclose(fp);
    return size;
}
bool writeFileContent(const std::string&file, const void *data, uint32_t size){
    if(data == nullptr){
        std::cout << "write file content error:data is nullptr" << std::endl;
        return false;
    }
    FILE *fp = fopen(file.c_str(), "wb");
    if(!fp){
        perror("open file error");
        std::cout << "file name is " << file << std::endl;
        return false;
    }
    fwrite(data, size, 1, fp);
    fclose(fp);
    return true; 
}