#include "Pipeline.h"
// #include "imgui.h"

// void Pipeline::DestroyDescriptorSet(VkDevice device, DescriptorSet&descriptorSet){
//     switch (descriptorSet.binding.descriptorType){
//     case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
//     case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
//         destroyBuffer(device, descriptorSet.uniform);
//         break;
//     case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
//     case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
//         destroyBuffer(device, descriptorSet.storage);
//         break;
//     case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
//         destroyImage(device, descriptorSet.sampledImage);
//         break;
//     default:
//         break;
//     }
// }
void Pipeline::GetDescriptorSetBinding(const spirv_cross::CompilerGLSL&glsl, const spirv_cross::SmallVector<spirv_cross::Resource>&resource, VkDescriptorType type, std::vector<DescriptorSet>&out){
    if(resource.empty())return;
    DescriptorSet ds{};
    ds.binding.descriptorCount = 1;
    ds.binding.descriptorType = type;
    for (size_t i = 0; i < resource.size(); ++i){
        const spirv_cross::Resource descriptorSet = resource[i];
        ds.uiSet = glsl.get_decoration(descriptorSet.id, spv::DecorationDescriptorSet);
        if(type != VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER){
            ds.uiSize = glsl.get_declared_struct_size(glsl.get_type(descriptorSet.type_id));
        }
        // if(uiSet > descriptorSet.size())descriptorSet.resize(uiSet + 1);
        ds.binding.binding = glsl.get_decoration(descriptorSet.id, spv::DecorationBinding);
        ds.mName = descriptorSet.name;
        ds.binding.stageFlags = GetShaderStageFlags(glsl.get_entry_points_and_stages()[0].execution_model);
        out.push_back(ds);
    }
}
std::vector<ShaderInfo>::iterator Pipeline::GetShaders(VkShaderStageFlags stage){
    auto it = mShaders.begin();
    for (; it != mShaders.end(); ++it){
        if(it->mStage == stage)break;
    }
    return it;
}
VkShaderStageFlags Pipeline::GetShaderStageFlags(const spv::ExecutionModel&executionModel){
    VkShaderStageFlags stage;
    switch (executionModel){
    case spv::ExecutionModelVertex:
        stage = VK_SHADER_STAGE_VERTEX_BIT;
        break;
    case spv::ExecutionModelTessellationControl:
        stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        break;
    case spv::ExecutionModelTessellationEvaluation:
        stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        break;
    case spv::ExecutionModelGeometry:
        stage = VK_SHADER_STAGE_GEOMETRY_BIT;
        break;
    case spv::ExecutionModelFragment:
        stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        break;
    case spv::ExecutionModelGLCompute:
        stage = VK_SHADER_STAGE_COMPUTE_BIT;
        break;
    default:
        break;
    }
    return stage;
}
std::string Pipeline::GetShaderStageName(VkShaderStageFlags stage){
#define IS_STAGE(strStage, result) case strStage: result = #strStage;break;
    std::string result;
    switch (stage){
    IS_STAGE(VK_SHADER_STAGE_VERTEX_BIT, result);
    IS_STAGE(VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, result);
    IS_STAGE(VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, result);
    IS_STAGE(VK_SHADER_STAGE_GEOMETRY_BIT, result);
    IS_STAGE(VK_SHADER_STAGE_FRAGMENT_BIT, result);
    IS_STAGE(VK_SHADER_STAGE_COMPUTE_BIT, result);
    default:
        result = "未知类型";
        break;
    }
    result = result.c_str() + strlen("VK_SHADER_STAGE_");
    std::size_t pos = result.find("_BIT");
    if(pos != std::string::npos){
        result[pos] = 0;
    }
    pos = result.find('_');
    while (pos != std::string::npos){
        result[pos] = ' ';
        pos = result.find('_');
    }
    return result;
#undef IS_STAGE
}
Pipeline::Pipeline(){

}
Pipeline::~Pipeline(){

}
auto Pipeline::AddShader(VkDevice device, const std::vector<uint32_t>&code){
    spirv_cross::CompilerGLSL glsl(code);
    spirv_cross::SmallVector<spirv_cross::EntryPoint> entry = glsl.get_entry_points_and_stages();
    VkShaderStageFlags stage = GetShaderStageFlags(entry[0].execution_model);
    auto it = GetShaders(stage);
    if(it != mShaders.end()){
        DeleteShader(device, it->mStage);
    }
    mShaders.push_back(ShaderInfo(GetShaderStageName(stage), (VkShaderStageFlagBits)stage));
    it = GetShaders(stage);
    VkShaderModuleCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.pCode = code.data();
    info.codeSize = code.size() * sizeof(uint32_t);
    vkCreateShaderModule(device, &info, nullptr, &it->mModule);

    spirv_cross::ShaderResources resource = glsl.get_shader_resources();
    spirv_cross::SmallVector<spirv_cross::Resource>pcResource = resource.push_constant_buffers;
    if(!pcResource.empty()){
        it->mPushConstant.resize(pcResource.size());
        PushConstantInfo&pc = it->mPushConstant[0];
        pc.mName = pcResource[0].name;
        pc.mPushConstantRange.offset = 0;
        pc.mPushConstantRange.stageFlags = it->mStage;
        pc.mPushConstantRange.size = glsl.get_declared_struct_size(glsl.get_type(pcResource[0].type_id));
        // updatePushConstant(pc, bt_m);
    }

    //描述符集相关
    GetDescriptorSetBinding(glsl, resource.uniform_buffers, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, it->mDescriptorSet);
    GetDescriptorSetBinding(glsl, resource.storage_buffers, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, it->mDescriptorSet);
    GetDescriptorSetBinding(glsl, resource.sampled_images, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, it->mDescriptorSet);
    for (uint32_t uiDescriptorSet = 0; uiDescriptorSet < it->mDescriptorSet.size(); ++uiDescriptorSet){
        DescriptorSet&descriptorSetInfo = it->mDescriptorSet[uiDescriptorSet];
        descriptorSetInfo.mRange = (BufferRange)0;
    }
    return it;
}
// void Pipeline::BindDescriptorSet(VkCommandBuffer cmd, uint32_t uiSet, uint32_t binding){
    
// }
VkResult Pipeline::CreateCache(VkDevice device, const std::vector<uint32_t>&cacheData){
    VkPipelineCacheCreateInfo cacheInfo = {};
    cacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    cacheInfo.initialDataSize = cacheData.size() * sizeof(uint32_t);
    cacheInfo.pInitialData = cacheData.data();
    return vkCreatePipelineCache(device, &cacheInfo, nullptr, &mCache);
}
VkResult Pipeline::CreateDescriptorSetLayout(VkDevice device, std::vector<VkDescriptorSetLayout>&setLayouts){
    std::vector<std::vector<VkDescriptorSetLayoutBinding>>bindings(1);
    for (auto it = mShaders.begin(); it != mShaders.end(); ++it){
        for (size_t uiDescriptorSet = 0; uiDescriptorSet < it->mDescriptorSet.size(); ++uiDescriptorSet){
            const DescriptorSet&descriptorSet = it->mDescriptorSet[uiDescriptorSet];
            const uint32_t uiSetCount = descriptorSet.uiSet + 1;
            if(uiSetCount > bindings.size())bindings.resize(uiSetCount);
            bindings[descriptorSet.uiSet].push_back(descriptorSet.binding);
        }
    }
    VkResult result;
    setLayouts.resize(bindings.size());
    for (size_t i = 0; i < setLayouts.size(); ++i){
        VkDescriptorSetLayoutCreateInfo descriptorSetLayout{};
        descriptorSetLayout.pBindings = bindings[i].data();
        descriptorSetLayout.bindingCount = bindings[i].size();
        descriptorSetLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        result = vkCreateDescriptorSetLayout(device, &descriptorSetLayout, nullptr, &setLayouts[i]);
        if(result != VK_SUCCESS)return result;
    }
    return VK_SUCCESS;
}
VkResult Pipeline::CreateLayout(VkDevice device, const std::vector<VkDescriptorSetLayout>&setLayouts){
    std::vector<VkPushConstantRange>pc;
    for (auto it = mShaders.begin(); it != mShaders.end(); ++it){
        for (uint32_t uiPc = 0; uiPc < it->mPushConstant.size(); ++uiPc){
            pc.push_back(it->mPushConstant[uiPc].mPushConstantRange);
        }
    }
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pPushConstantRanges = pc.data();
    pipelineLayoutInfo.pSetLayouts = setLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = pc.size();
    pipelineLayoutInfo.setLayoutCount = setLayouts.size();
    return vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &mLayout);
}
void Pipeline::DeleteShader(VkDevice device, VkShaderStageFlags stage){
    auto it = GetShaders(stage);
    if(it == mShaders.end())return;
    if(it->mModule != VK_NULL_HANDLE)vkDestroyShaderModule(device, it->mModule, nullptr);
    // for (size_t uiDescriptorSet = 0; uiDescriptorSet < it->mDescriptorSet.size(); ++uiDescriptorSet){
    //     DestroyDescriptorSet(device, it->mDescriptorSet[uiDescriptorSet]);
    // }
}
void Pipeline::DestroyCache(VkDevice device, std::vector<uint32_t>&cacheData){
    GetCacheData(device, cacheData);
    vkDestroyPipelineCache(device, mCache, nullptr);
}
void Pipeline::DestroyLayout(VkDevice device){
    vkDestroyPipelineLayout(device, mLayout, nullptr);
}
void Pipeline::DestroyPipeline(VkDevice device){
    vkDestroyPipeline(device, mPipeline, nullptr);
}
void Pipeline::DestrotyShader(VkDevice device){
    for (auto it = mShaders.begin(); it != mShaders.end(); ++it){
        DestrotyShader(device, it);
    }
}
void Pipeline::DestrotyShader(VkDevice device, const std::vector<ShaderInfo>::iterator&it){
    vkDestroyShaderModule(device, it->mModule, nullptr);
}
void Pipeline::GetCacheData(VkDevice device, std::vector<uint32_t>&cacheData){
    size_t cacheDataSize;
    vkGetPipelineCacheData(device, mCache, &cacheDataSize, nullptr);
    cacheData.resize(cacheDataSize / sizeof(char));
    vkGetPipelineCacheData(device, mCache, &cacheDataSize, cacheData.data());
}

GraphicsPipeline::GraphicsPipeline(){
    // mState.mPolygonMode = VK_POLYGON_MODE_FILL;
}
GraphicsPipeline::GraphicsPipeline(const GraphicsPipelineStateInfo&stateInfo){
    mState = stateInfo;
}
GraphicsPipeline::~GraphicsPipeline(){

}
// VkResult GraphicsPipeline::CreatePipeline(){

// }
// //如果传进来的是其他子类或父类。将直接崩溃
// bool GraphicsPipeline::operator==(const Pipeline&pipeline){
//     const GraphicsPipeline *gp = (GraphicsPipeline *)&pipeline;

//     // if(*this == pipeline){

//     // }
// }
bool GraphicsPipeline::BindPipeline(VkCommandBuffer cmd){
    if(mPipeline != VK_NULL_HANDLE){
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);
        return true;
    }
    return false;
}
VkResult GraphicsPipeline::CreatePipeline(VkDevice device, VkRenderPass&renderPass){
    std::vector<VkPipelineShaderStageCreateInfo>ShaderStageCreateInfo;
    for (auto it = ShaderBegin(); it != ShaderEnd(); ++it){
        VkPipelineShaderStageCreateInfo shaderInfo{};
        shaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderInfo.pName = "main";
        shaderInfo.stage = (VkShaderStageFlagBits)it->mStage;
        shaderInfo.module = it->mModule;
        ShaderStageCreateInfo.push_back(shaderInfo);
    }
    VkPipelineVertexInputStateCreateInfo VertexInputStateCreateInfo{};// = vkt::Pipline::VertexInputStateCreateInfo(mVertexInput.mBindingDescription, mVertexInput.mAttributeDescriptions);
    VertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    VertexInputStateCreateInfo.pVertexBindingDescriptions = &mVertexInput.mBindingDescription;
    VertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
    std::vector<VkVertexInputAttributeDescription>vertexInputAttributeDescription;
    for (size_t i = 0; i < mVertexInput.mVertexInputAttributeDescription.size(); ++i){
        vertexInputAttributeDescription.push_back(mVertexInput.mVertexInputAttributeDescription[i].mAttributeDescriptions);
    }
    VertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributeDescription.data();
    VertexInputStateCreateInfo.vertexAttributeDescriptionCount = mVertexInput.mVertexInputAttributeDescription.size();
    // VkPipelineInputAssemblyStateCreateInfo InputAssemblyStateCreateInfo{};// = vkt::Pipline::InputAssemblyStateCreateInfo(pPipeline->mTopology, pPipeline->mPrimitiveRestart); 
    // InputAssemblyStateCreateInfo.topology = mState.mTopology;
    // InputAssemblyStateCreateInfo.primitiveRestartEnable = mState.mPrimitiveRestart;
    // VkPipelineMultisampleStateCreateInfo multisampling{};// = vkt::Pipline::MultisampleStateCreateInfo();
    // multisampling.sType = ;
    // multisampling.minSampleShading = 1.0f;
    // multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    VkPipelineColorBlendStateCreateInfo ColorBlendStateCreateInfo{};
    ColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    ColorBlendStateCreateInfo.attachmentCount = 1;
    ColorBlendStateCreateInfo.pAttachments = &mState.mColorBlend;
    //if(blendConstants)memcpy(info.blendConstants, blendConstants, sizeof(float) * 4);
    ColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
    // ColorBlendStateCreateInfo.logicOpEnable = logicOpEnable;false
    VkPipelineViewportStateCreateInfo ViewportStateCreateInfo;
    ViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    ViewportStateCreateInfo.flags = 0;
    ViewportStateCreateInfo.pNext = nullptr;
    ViewportStateCreateInfo.scissorCount = mScissor.size();
    ViewportStateCreateInfo.viewportCount = mViewport.size();
    ViewportStateCreateInfo.pScissors = mScissor.data();
    ViewportStateCreateInfo.pViewports = mViewport.data();
    VkPipelineDynamicStateCreateInfo *pDynamicState = nullptr;
    std::vector<VkDynamicState>dynamic = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    if (ViewportStateCreateInfo.viewportCount < 1 || ViewportStateCreateInfo.scissorCount < 1) {
            pDynamicState = new VkPipelineDynamicStateCreateInfo;
            VkPipelineDynamicStateCreateInfo dynamicState;
            dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamicState.flags= 0;
            dynamicState.pNext = nullptr;
            dynamicState.pDynamicStates = dynamic.data();
            dynamicState.dynamicStateCount = dynamic.size();
            memcpy(pDynamicState, &dynamicState, sizeof(VkPipelineDynamicStateCreateInfo));
    }
    // RasterizationStateCreateInfo.lineWidth = 1.0f;//线宽，大于 1.0需要启用相应的cpu特性
    // RasterizationStateCreateInfo.cullMode = mState.mCullMode;
    // RasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    // RasterizationStateCreateInfo.polygonMode = mState.mPolygonMode;
    // RasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    // RasterizationStateCreateInfo.depthClampEnable = mState.mDepthClampEnable;//如果设置为true,则会将超出近平面和远平面的片段截断为近平面和远平面上，而不是直接丢弃。阴影贴图可能需要用到
    // RasterizationStateCreateInfo.rasterizerDiscardEnable =  VK_FALSE;//如果设置为true，则不会输出任何图形
    VkGraphicsPipelineCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    info.pStages = ShaderStageCreateInfo.data();
    info.stageCount = ShaderStageCreateInfo.size();
    info.pVertexInputState = &VertexInputStateCreateInfo;
    info.pInputAssemblyState = &mState.mInputAssembly;
    info.pViewportState = &ViewportStateCreateInfo;
    info.pDynamicState = pDynamicState;
    info.layout = mLayout;
    info.renderPass = renderPass;
    info.pColorBlendState = &ColorBlendStateCreateInfo;
    info.pDepthStencilState = &mState.mDepthStencil;
    info.pMultisampleState = &mState.mMultisample;
    info.pRasterizationState = &mState.mRasterization;
    return vkCreateGraphicsPipelines(device, mCache, 1, &info, nullptr, &mPipeline);
    // if(pDynamicState)delete pDynamicState;
}
void GraphicsPipeline::IncreaseShader(VkDevice device, const std::vector<uint32_t>&code){
    auto it = AddShader(device, code);
    if(it->mStage == VK_SHADER_STAGE_VERTEX_BIT){//测试得知。stage input确实不是顶点着色器独有
        spirv_cross::CompilerGLSL glsl(code);
        spirv_cross::ShaderResources resource = glsl.get_shader_resources();
        spirv_cross::SmallVector<spirv_cross::Resource>stageInput = resource.stage_inputs;//确实有阶段输出
        uint32_t stride = 0;
        VkFormat vertexFormat[] = { VK_FORMAT_UNDEFINED, VK_FORMAT_R32_SFLOAT, VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32B32A32_SFLOAT };
        for (int i = stageInput.size() - 1; i >= 0; --i){
            uint32_t size = GetVecSize(glsl, stageInput[i]);
            uint32_t uiLocation = GetLocation(glsl, stageInput[i]);
            VkVertexInputAttributeDescription vertexInput;
            vertexInput.binding = 0;
            vertexInput.format = vertexFormat[size];
            vertexInput.location = uiLocation;
            vertexInput.offset = stride;
            VertexInputAttributeDescription vertexInputAttributeDescription;
            vertexInputAttributeDescription.mName = stageInput[i].name;
            vertexInputAttributeDescription.mRange = (VertexRange)0;
            vertexInputAttributeDescription.mAttributeDescriptions = vertexInput;
            mVertexInput.mVertexInputAttributeDescription.push_back(vertexInputAttributeDescription);
            stride += size * sizeof(float);
        }
        mVertexInput.mBindingDescription.binding = 0;
        mVertexInput.mBindingDescription.stride = stride;
        mVertexInput.mBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    }
}
// void GraphicsPipeline::BindDescriptorSets(VkCommandBuffer cmd, const VkDescriptorSet *pDescriptorSets, uint32_t descriptorSetCount){
//     BindDescriptorSets(cmd, 0, pDescriptorSets, descriptorSetCount);
// }
// void GraphicsPipeline::BindDescriptorSets(VkCommandBuffer cmd, uint32_t uiSet, const VkDescriptorSet *pDescriptorSets, uint32_t descriptorSetCount){
//     vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mLayout, uiSet, descriptorSetCount, pDescriptorSets, 0, nullptr);
// }
// void GraphicsPipeline::PushConstant(VkCommandBuffer cmd){
//     for (size_t uiShader = 0; uiShader < mShaders.size(); ++uiShader){
        
//     }
// }
// void GraphicsPipeline::PushConstant(VkCommandBuffer cmd, const std::vector<glm::mat4>&data){

// }