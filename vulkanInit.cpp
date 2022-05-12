#include "vulkanInit.h"
VkApplicationInfo vki::applicationInfo(const char *pAppName, uint32_t appVersion, const char *pEngineName, uint32_t engineVersion, uint32_t apiVersion){
	VkApplicationInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	info.apiVersion = apiVersion;
	info.pEngineName = pEngineName;
	info.pApplicationName = pAppName;
	info.engineVersion = engineVersion;
	info.applicationVersion = appVersion;
	return info;
}
/*}}}*/
/*{{{*/
VkDebugUtilsMessengerCreateInfoEXT vki::debugUtilsMessengerCreateInfo(PFN_vkDebugUtilsMessengerCallbackEXT debugCallback, VkDebugUtilsMessageSeverityFlagsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType){
    VkDebugUtilsMessengerCreateInfoEXT info = {};
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    info.messageType = messageType;
    info.pfnUserCallback = debugCallback;
    info.messageSeverity = messageSeverity;
    return info;
}
/*}}}*/
/*{{{*/
VkInstanceCreateInfo vki::instanceCreateInfo(uint32_t layerCount, uint32_t extensionCount, const char *const*layerNames, const char * const *extensionName, const VkApplicationInfo *pAppInfo){
    VkInstanceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pApplicationInfo = pAppInfo;
    info.enabledLayerCount = layerCount;
    info.ppEnabledLayerNames = layerNames;
    info.enabledExtensionCount = extensionCount;
    info.ppEnabledExtensionNames = extensionName;
    return info;
}
/*}}}*/
/*{{{*/
// 	VkInstanceCreateInfo instanceCreateInfo(bool bEnableDebugReport, const VkApplicationInfo *pAppInfo = nullptr){
// 		std::vector<const char *>layer;
// 		if(bEnableDebugReport)layer.push_back("VK_LAYER_KHRONOS_validation");
// 		std::vector<const char *>extensions = { VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
// #ifdef __linux
// 		extensions.push_back("VK_KHR_xcb_surface");
// #endif
// 		return instanceCreateInfo(layer, extensions, pAppInfo);
// 	}
/*}}}*/
/*{{{*/
VkDeviceQueueCreateInfo vki::deviceQueueCreateInfo(uint32_t queueFamilyIndex, uint32_t queueCount, const float *pQueuePriorities){
    VkDeviceQueueCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    info.queueCount = queueCount;
    info.queueFamilyIndex = queueFamilyIndex;
    info.pQueuePriorities = pQueuePriorities;
    return info;
}
/*}}}*/
/*{{{*/
//no enable layer
VkDeviceCreateInfo vki::deviceCreateInfo(const std::vector<VkDeviceQueueCreateInfo>&queueCreateInfo, uint32_t extenionCount, const char * const * extensionNames, const VkPhysicalDeviceFeatures *pDeviceFeatures){
    VkDeviceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    //info.enabledLayerCount = 0;
    //info.ppEnabledLayerNames = nullptr;
    info.pEnabledFeatures = pDeviceFeatures;
    info.enabledExtensionCount = extenionCount;
    info.pQueueCreateInfos = queueCreateInfo.data();
    info.ppEnabledExtensionNames = extensionNames;
    info.queueCreateInfoCount = queueCreateInfo.size();
    return info;
}
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
VkSwapchainCreateInfoKHR vki::swapchainCreateInfoKHR(VkSurfaceKHR surface, VkFormat imageFormat, VkExtent2D imageExtent, VkImageUsageFlags imageUsage, uint32_t imageArrayLayers, uint32_t minImageCount, VkColorSpaceKHR colorSpace, VkBool32 clipped, VkCompositeAlphaFlagBitsKHR compositeAlpha, VkSurfaceTransformFlagBitsKHR currentTransform, VkPresentModeKHR presentMode, VkSharingMode imageSharingMode, const std::vector<uint32_t>&indices, VkSwapchainKHR oldSwapchain){
    VkSwapchainCreateInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.surface = surface;
    info.clipped = clipped;
    info.imageUsage = imageUsage;
    info.presentMode = presentMode;
    info.imageFormat = imageFormat;
    info.imageExtent = imageExtent;
    info.oldSwapchain = oldSwapchain;
    info.imageColorSpace = colorSpace;
    info.minImageCount = minImageCount;
    info.preTransform = currentTransform;
    info.compositeAlpha = compositeAlpha;
    info.imageArrayLayers = imageArrayLayers;
    info.imageSharingMode = imageSharingMode;
    info.queueFamilyIndexCount = indices.size();
    info.pQueueFamilyIndices = indices.data();
    return info;
}
/*}}}*/
/*{{{*/
VkSwapchainCreateInfoKHR vki::swapchainCreateInfoKHR(VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR surfaceCapabilities, const std::vector<VkPresentModeKHR>&presentModes, const std::vector<VkSurfaceFormatKHR>&surfaceFormats, VkSharingMode imageSharingMode, const std::vector<uint32_t>&indices){
    VkFormat imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
    VkColorSpaceKHR imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    uint32_t minImageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount > 0 && surfaceCapabilities.minImageCount > surfaceCapabilities.maxImageCount) {
        minImageCount = surfaceCapabilities.maxImageCount;
    }
    if(minImageCount < 2)minImageCount = 2;
	if(minImageCount > 3)minImageCount = 3;
    if (surfaceCapabilities.maxImageCount > 0 && surfaceCapabilities.minImageCount > surfaceCapabilities.maxImageCount) {
        minImageCount = surfaceCapabilities.maxImageCount;
    }
    if(minImageCount < 2)minImageCount = 2;
	if(minImageCount > 3)minImageCount = 3;
    for (size_t i = 0; i < presentModes.size(); i++){
        if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            presentMode = presentModes[i];
            break;
        }
    }
    if (presentMode == VK_PRESENT_MODE_FIFO_KHR) {
        for (size_t i = 0; i < presentModes.size(); i++) {
            if (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
                presentMode = presentModes[i];
                break;
            }
        }
    }
    // std::cout << "surface format:" << imageFormat << ";surface color space:" << imageColorSpace << std::endl;
    return swapchainCreateInfoKHR(surface, imageFormat, surfaceCapabilities.currentExtent, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 1, minImageCount + 1, imageColorSpace, VK_TRUE, VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, surfaceCapabilities.currentTransform, presentMode, imageSharingMode, indices, VK_NULL_HANDLE);
}
/*}}}*/
/*{{{*/
VkRenderPassCreateInfo vki::renderPassCreateInfo(uint32_t subpassCount, const VkSubpassDescription*pSubpass, uint32_t dependencyCount, const VkSubpassDependency *pDependency, uint32_t attachmentCount, const VkAttachmentDescription *pAttchments){
    VkRenderPassCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.pSubpasses = pSubpass;
    info.pAttachments = pAttchments;
    info.pDependencies = pDependency;
    info.subpassCount = subpassCount;
    info.attachmentCount = attachmentCount;
    info.dependencyCount = dependencyCount;
    return info;
}
/*}}}*/
/*{{{*/
VkAttachmentDescription vki::attachmentDescription(VkFormat imageFormat, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp, VkImageLayout initialLayout, VkImageLayout finalLayout, VkAttachmentLoadOp stencilLoadOp, VkAttachmentStoreOp stencilStoreOp, VkSampleCountFlagBits samples){
    VkAttachmentDescription info = {};
    //colorAttachment.flags = 0;
    info.loadOp = loadOp;//VK_ATTACHMENT_LOAD_OP_CLEAR;
    info.storeOp = storeOp;//VK_ATTACHMENT_STORE_OP_STORE;// VK_ATTACHMENT_STORE_OP_DONT_CARE;
    info.samples = samples;
    info.format = imageFormat;
    info.finalLayout = finalLayout;//VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    info.initialLayout = initialLayout;//VK_IMAGE_LAYOUT_UNDEFINED;
    info.stencilLoadOp = stencilLoadOp;
    info.stencilStoreOp = stencilStoreOp;
    return info;
}
/*}}}*/
/*{{{*/
VkAttachmentReference vki::attachmentReference(uint32_t attachment, VkImageLayout layout){
    VkAttachmentReference info;
    info.layout = layout;
    info.attachment = attachment;
    return info;
}
/*}}}*/
/*{{{*/
VkSubpassDescription vki::subPassDescription(uint32_t attachmentReferenceCount, const VkAttachmentReference *pColorAttachmentRefs, const VkAttachmentReference *pDepthStencilAttachment, VkPipelineBindPoint pipelineBindPoint){
    VkSubpassDescription subPass = {};
    subPass.pipelineBindPoint = pipelineBindPoint;
    subPass.pColorAttachments = pColorAttachmentRefs;
    subPass.colorAttachmentCount = attachmentReferenceCount;
    subPass.pDepthStencilAttachment = pDepthStencilAttachment;
    return subPass;
}
/*}}}*/
/*{{{*/
VkSubpassDependency vki::subpassDependency(uint32_t srcSubpass, uint32_t dstSubpass, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask){
    VkSubpassDependency dependency = {};
    dependency.srcSubpass = srcSubpass;
    dependency.dstSubpass = dstSubpass;
    dependency.srcStageMask = srcStageMask;
    dependency.dstStageMask = dstStageMask;
    dependency.srcAccessMask = srcAccessMask;
    dependency.dstAccessMask = dstAccessMask;
    return dependency;
}
/*}}}*/
/*{{{*/
VkFramebufferCreateInfo vki::framebufferCreateInfo(VkRenderPass renderPass, const VkExtent2D&frameExtent, uint32_t frameBufferAttachmentCount, const VkImageView *pFramebufferAttachments, uint32_t layers){
    VkFramebufferCreateInfo frameBufferInfo = {};
    frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferInfo.layers = layers;
    frameBufferInfo.renderPass = renderPass;
    frameBufferInfo.width = frameExtent.width;
    frameBufferInfo.height = frameExtent.height;
    frameBufferInfo.pAttachments = pFramebufferAttachments;
    frameBufferInfo.attachmentCount = frameBufferAttachmentCount;
    return frameBufferInfo;
}
/*}}}*/
VkImageViewCreateInfo vki::imageViewCreateInfo(VkFormat format, VkImageAspectFlags aspectMask, VkImageViewType type, uint32_t arrayLayer){
    VkImageViewCreateInfo imageViewInfo = {};
    imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewInfo.format = format;
    // imageViewInfo.image = image;
    imageViewInfo.viewType = type;
    //imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewInfo.subresourceRange.aspectMask = aspectMask;
    imageViewInfo.subresourceRange.layerCount = arrayLayer;
    imageViewInfo.subresourceRange.levelCount = 1;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.baseMipLevel = 0;
    return imageViewInfo;
}
VkImageCreateInfo vki::imageCreateInfo(VkExtent2D extent, VkImageUsageFlags usage, VkFormat format, VkImageTiling tiling, uint32_t arrayLayer, uint32_t depth){
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.extent = { extent.width, extent.height, depth };
    imageInfo.format = format;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.arrayLayers = arrayLayer;
    imageInfo.mipLevels = 1;
    imageInfo.tiling = tiling;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    return imageInfo;
}
VkPipelineVertexInputStateCreateInfo vki::pipelineVertexInputStateCreateInfo(uint32_t vertexAttributeCount, const VkVertexInputAttributeDescription *pVertexAttributeDescriptions, const VkVertexInputBindingDescription *pVertexBindingDescriptions, uint32_t vertexBindingDescriptionCount){
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    vertexInputInfo.vertexBindingDescriptionCount = vertexBindingDescriptionCount;
    vertexInputInfo.vertexAttributeDescriptionCount = vertexAttributeCount;
    vertexInputInfo.pVertexBindingDescriptions = pVertexBindingDescriptions;
    vertexInputInfo.pVertexAttributeDescriptions = pVertexAttributeDescriptions;
    return vertexInputInfo;
}
VkPipelineInputAssemblyStateCreateInfo vki::pipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable){
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = topology;
    inputAssembly.primitiveRestartEnable = primitiveRestartEnable;
    return inputAssembly;
}
VkViewport vki::viewportInfo(const VkExtent2D&extent, float x, float y){
    VkViewport viewport = {};
    viewport.x = x;
    viewport.y = y;
    viewport.width = extent.width;
    viewport.height = extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    return viewport;
}
VkPipelineViewportStateCreateInfo vki::pipelineViewportStateCreateInfo(const VkViewport *pViewport, const VkRect2D *pScissor, uint32_t viewportCount, uint32_t scissorCount){
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pScissors = pScissor;
    viewportState.pViewports = pViewport;
    viewportState.scissorCount = scissorCount;
    viewportState.viewportCount = viewportCount;
    return viewportState;
}
VkPipelineRasterizationStateCreateInfo vki::pipelineRasterizationStateCreateInfo(VkCullModeFlags cullMode, VkFrontFace frontFace, VkPolygonMode polygonMode){
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = cullMode;
    rasterizer.frontFace = frontFace;
    rasterizer.polygonMode = polygonMode;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthClampEnable = VK_FALSE;//depthClampEnable ��Ա��������Ϊ VK_TRUE ��ʾ�ڽ�ƽ���Զƽ �����Ƭ�λᱻ�ض�Ϊ�ڽ�ƽ���Զƽ���ϣ�������ֱ�Ӷ�����ЩƬ�Ρ� �������Ӱ��ͼ�����ɺ�����
    rasterizer.rasterizerDiscardEnable =  VK_FALSE;//�������Ϊtrue�����ֹһ��Ƭ�������֡���壬ԭ�������м���ͼԪ����ͨ����դ���׶�
    //rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    return rasterizer;
}
VkPipelineMultisampleStateCreateInfo vki::pipelineMultisampleStateCreateInfo(VkBool32 sampleShadingEnable, VkSampleCountFlagBits rasterizationSamples){
    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = sampleShadingEnable;
    multisampling.rasterizationSamples = rasterizationSamples;
    // multisampling.minSampleShading = ;
    // multisampling.pSampleMask = ;
    // multisampling.alphaToCoverageEnable = ;
    // multisampling.alphaToOneEnable = ;
    return multisampling;
}
VkPipelineColorBlendAttachmentState vki::pipelineColorBlendAttachmentState(VkBool32 bColorBlend){
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = bColorBlend;

    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    return colorBlendAttachment;
}
VkPipelineColorBlendStateCreateInfo vki::pipelineColorBlendStateCreateInfo(uint32_t colorBlendAttachmentCount, const VkPipelineColorBlendAttachmentState *pColorBlendAttachment){
    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.pAttachments = pColorBlendAttachment;
    colorBlending.attachmentCount = colorBlendAttachmentCount;
    return colorBlending;
}
VkPipelineDepthStencilStateCreateInfo vki::pipelineDepthStencilStateCreateInfo(VkBool32 depthTestEnable, VkCompareOp depthCompareOp, VkBool32 stencilTestEnable){
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo = {};
    depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilInfo.depthTestEnable = depthTestEnable;
    depthStencilInfo.depthWriteEnable = VK_TRUE;//ʹ����������Ա��������ʵ��͸��Ч��,͸�������Ƭ�ε����ֵ�� Ҫ��֮ǰ��͸������Ƭ�ε����ֵ���бȽϣ���͸�������Ƭ�ε����ֵ ����Ҫд����Ȼ��塣 
    depthStencilInfo.depthCompareOp = depthCompareOp;//depthCompareOp ��Ա��������ָ����Ȳ���ʹ�õıȽ����㡣���� ����ָ����Ȳ���ʹ��С�ڱȽ����㣬��һ�����£��µ�Ƭ��ֻ�������� ���ֵС����Ȼ����е����ֵʱ�Żᱻд����ɫ���š� 
    depthStencilInfo.depthBoundsTestEnable = VK_FALSE;//depthBoundsTestEnable��minDepthBounds��maxDepthBounds��Ա ��������ָ����ѡ����ȷ�Χ���ԡ���һ���Կ�����ֻ�����ֵλ��ָ�� ��Χ�ڵ�Ƭ�βŲ��ᱻ�������������ǲ�ʹ����һ���ܡ�
    depthStencilInfo.maxDepthBounds = 1.0f;
    //stencilTestEnable��front �� back ��Ա��������ģ����ԣ������ǵĽ� ����û���õ������������Ҫʹ��ģ����ԣ���Ҫע��ʹ�ð���ģ����ɫ ͨ����ͼ�����ݸ�ʽ
    depthStencilInfo.stencilTestEnable = stencilTestEnable;
    depthStencilInfo.back.reference = 1;
    depthStencilInfo.back.writeMask = 0xFF;
    depthStencilInfo.back.compareMask = 0xFF;
    depthStencilInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
    depthStencilInfo.back.failOp = VK_STENCIL_OP_REPLACE;
    depthStencilInfo.back.depthFailOp = VK_STENCIL_OP_REPLACE;
    depthStencilInfo.back.passOp = VK_STENCIL_OP_REPLACE;
    depthStencilInfo.front = depthStencilInfo.back;
    return depthStencilInfo;
}
VkPipelineDynamicStateCreateInfo vki::pipelineDynamicStateCreateInfo(uint32_t dynamicCount, const VkDynamicState *pDynamic){
    VkPipelineDynamicStateCreateInfo dynamicState;
    // VkDynamicState dynamic[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.flags= 0;
    dynamicState.pNext = nullptr;
    dynamicState.pDynamicStates = pDynamic;
    dynamicState.dynamicStateCount = dynamicCount;//sizeof(dynamic) / sizeof(VkDynamicState);
    return dynamicState;
}
VkBufferCreateInfo vki::bufferCreateInfo(VkDeviceSize size, VkBufferUsageFlags usage, VkSharingMode sharingMode){
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = sharingMode;
    return bufferInfo;
}
VkMemoryAllocateInfo vki::memoryAllocateInfo(VkDeviceSize size, uint32_t memoryTypeIndex){
    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.allocationSize = size;
    allocateInfo.memoryTypeIndex = memoryTypeIndex;
    return allocateInfo;
}
VkBufferImageCopy vki::bufferImageCopy(VkExtent2D extent2d, uint32_t baseArrayLayer, VkDeviceSize bufferOffset, uint32_t layerCount, VkImageAspectFlags aspectMask, uint32_t depth){
    VkExtent3D extent3d = { extent2d.width, extent2d.height, depth };
    VkBufferImageCopy bufferCopyRegions = {};
    bufferCopyRegions.imageSubresource.baseArrayLayer = baseArrayLayer;
    bufferCopyRegions.imageSubresource.layerCount = layerCount;
    bufferCopyRegions.imageSubresource.aspectMask = aspectMask;
    bufferCopyRegions.bufferOffset = bufferOffset;
    bufferCopyRegions.imageExtent = extent3d;
    return bufferCopyRegions;
}
VkPushConstantRange vki::pushConstantRange(VkDeviceSize size, VkShaderStageFlags stageFlags,uint32_t offset){
    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.size = size;
    pushConstantRange.offset = offset;
    pushConstantRange.stageFlags = stageFlags;
    return pushConstantRange;
}
VkWriteDescriptorSet vki::writeDescriptorSet(VkDescriptorSet dstSet, VkDescriptorType descriptorType, uint32_t dstBinding, uint32_t descriptorCount){
    VkWriteDescriptorSet info = {};
    info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    info.descriptorCount = descriptorCount;
    info.descriptorType = descriptorType;
    info.dstBinding = dstBinding;
    info.dstSet = dstSet;
    return info;
}
VkSemaphoreCreateInfo vki::semaphoreCreateInfo(){
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    return semaphoreInfo;
}
VkFenceCreateInfo vki::fenceCreateInfo(VkFenceCreateFlags flags){
	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = flags;
    return fenceInfo;
}
VkCommandBufferBeginInfo vki::commandBufferBeginInfo(){
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;;
    return info;
}
VkRenderPassBeginInfo vki::renderPassBeginInfo(const VkClearValue *pClearValues, const VkExtent2D&windowSize, VkFramebuffer frameBuffer, VkRenderPass renderPass, uint32_t clearValueCount, VkOffset2D offset){
    VkRenderPassBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.renderPass = renderPass;
    info.clearValueCount = clearValueCount;
    info.framebuffer = frameBuffer;
    info.pClearValues = pClearValues;
    info.renderArea = { offset, windowSize };
    return info;
}
