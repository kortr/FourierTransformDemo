#ifndef PIPELINE_H 
#define PIPELINE_H
#include <vector>
#include <vulkan/vulkan.h>
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>

#include "spirv_glsl.hpp"
#define GetVecSize(glsl, stageInput) (glsl.get_type(stageInput.type_id).vecsize)
#define GetLocation(glsl, stageInput) (glsl.get_decoration(stageInput.id, spv::DecorationLocation))
// #include "vulkanFrame.h"
enum VertexRange{
    POSITION = 0,
    TEXCOORD,
    NORMAL,
    JOINTS,
    WEIGHTS
};
enum BufferRange{
    bt_m = 0,
    bt_v,
    bt_p,
    bt_vp,
    bt_mvp
};
struct VertexInputAttributeDescription{
    std::string mName;
    VertexRange mRange;
    VkVertexInputAttributeDescription mAttributeDescriptions;
};
struct VertexInput{
    VkVertexInputBindingDescription mBindingDescription;
    std::vector<VertexInputAttributeDescription>mVertexInputAttributeDescription;
};
struct DescriptorSet{
    uint32_t uiSet;
    uint32_t uiSize;
    std::string mName;
    BufferRange mRange;
    // bufferInfo uniform;
    // bufferInfo storage;
    // ImageInfo sampledImage;
    VkDescriptorSetLayoutBinding binding;
};
struct PushConstantInfo{
    BufferRange mRange;
    std::string mName;
    VkPushConstantRange mPushConstantRange;
    // std::vector<glm::mat4>data;//虽然可支持多个4x4以内的矩阵。但其他类型怎么办?
};
struct ShaderInfo{
    std::string mName;
    VkShaderModule mModule;
    VkShaderStageFlags mStage;
    std::vector<DescriptorSet>mDescriptorSet;
    std::vector<PushConstantInfo>mPushConstant;
    // VkPipelineShaderStageCreateInfo mShaderStageInfo;
    ShaderInfo() = default;
    ShaderInfo(const std::string&name, VkShaderStageFlagBits stage){
        mName = name;
        mStage = stage;
        // mShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        // mShaderStageInfo.pName = "main";
        // mShaderStageInfo.module = VK_NULL_HANDLE;
        // mShaderStageInfo.stage = stage;
    }
    VkResult CreateShaderModule(VkDevice device, const std::vector<uint32_t>&code){
        VkShaderModuleCreateInfo info{};
        info.codeSize = code.size() * sizeof(uint32_t);
        info.pCode = code.data();
        return vkCreateShaderModule(device, &info, nullptr, &mModule);
    }
    void DestroyShaderModule(VkDevice device){
        vkDestroyShaderModule(device, mModule, nullptr);
    }
};
class Pipeline{
protected:
    VkPipeline mPipeline;
    VkPipelineCache mCache;
    VkPipelineLayout mLayout;
    std::vector<ShaderInfo>mShaders;
    // void DestroyDescriptorSet(VkDevice device, DescriptorSet&descriptorSet);
    void GetDescriptorSetBinding(const spirv_cross::CompilerGLSL&glsl, const spirv_cross::SmallVector<spirv_cross::Resource>&resource, VkDescriptorType type, std::vector<DescriptorSet>&out);
    auto AddShader(VkDevice device, const std::vector<uint32_t>&code);
public:
    Pipeline();
    ~Pipeline();
    // virtual void BindDescriptorSet(VkCommandBuffer cmd, uint32_t uiSet, uint32_t binding = 0);
    virtual VkResult CreateCache(VkDevice device, const std::vector<uint32_t>&cacheData);
    virtual VkResult CreateDescriptorSetLayout(VkDevice device, std::vector<VkDescriptorSetLayout>&setLayout);
    virtual VkResult CreateLayout(VkDevice device, const std::vector<VkDescriptorSetLayout>&setLayouts);
    virtual void DeleteShader(VkDevice device, VkShaderStageFlags stage);
    virtual void DestroyCache(VkDevice device, std::vector<uint32_t>&cacheData);
    virtual void DestroyLayout(VkDevice device);
    virtual void DestroyPipeline(VkDevice device);
    virtual void DestrotyShader(VkDevice device);
    virtual void DestrotyShader(VkDevice device, const std::vector<ShaderInfo>::iterator&it);
    virtual void GetCacheData(VkDevice device, std::vector<uint32_t>&cacheData);

    inline auto ShaderBegin()const{
        return mShaders.begin();
    }
    inline auto ShaderEnd()const{
        return mShaders.end();
    }
    inline auto ShaderBegin(){
        return mShaders.begin();
    }
    inline auto ShaderEnd(){
        return mShaders.end();
    }
    inline VkPipelineLayout GetLayout(){
        return mLayout;
    }
    std::vector<ShaderInfo>::iterator GetShaders(VkShaderStageFlags stage);
    VkShaderStageFlags GetShaderStageFlags(const spv::ExecutionModel&executionModel);
    std::string GetShaderStageName(VkShaderStageFlags stage);
    
    virtual VkResult CreatePipeline(VkDevice device, VkRenderPass&renderPass) = 0;
    // virtual void IncreaseShader(VkDevice device, const std::vector<uint32_t>&code) = 0;
};
struct GraphicsPipelineStateInfo{
    std::string name;
    VkPipelineColorBlendAttachmentState mColorBlend{};
    VkPipelineMultisampleStateCreateInfo mMultisample{};
    VkPipelineDepthStencilStateCreateInfo mDepthStencil{};
    VkPipelineRasterizationStateCreateInfo mRasterization{};
    VkPipelineInputAssemblyStateCreateInfo mInputAssembly{};
    GraphicsPipelineStateInfo(){
        mRasterization.lineWidth = 1.0f;
        mMultisample.minSampleShading = 1.0f;
        mMultisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        mInputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        mMultisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        mDepthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        mRasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        mInputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        mColorBlend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    }
    ~GraphicsPipelineStateInfo(){

    }
};
class GraphicsPipeline:public Pipeline{
    VertexInput mVertexInput;
    std::vector<VkRect2D>mScissor;
    std::vector<VkViewport>mViewport;
    GraphicsPipelineStateInfo mState;
public:
    GraphicsPipeline();
    GraphicsPipeline(const GraphicsPipelineStateInfo&stateInfo);
    ~GraphicsPipeline();
    bool BindPipeline(VkCommandBuffer cmd);
    inline void SetStateInfo(const GraphicsPipelineStateInfo&stateInfo){
        mState = stateInfo;
    }
    inline void PushScissor(uint32_t width, uint32_t height, VkOffset2D offset = {}){
        VkRect2D rect;
        rect.extent.width = width;
        rect.extent.height = height;
        rect.offset = offset;
        mScissor.push_back(rect);
    }
    inline void PushViewport(uint32_t width, uint32_t height, float x = 0, float y = 0, float maxDepth = 1, float minDepth = 0){
        VkViewport viewport;
        viewport.x = x;
        viewport.y = y;
        viewport.width = width;
        viewport.height = height;
        viewport.maxDepth = maxDepth;
        viewport.minDepth = minDepth;
        mViewport.push_back(viewport);
    }
    // virtual bool operator==(const GraphicsPipeline&pipeline);
    //uiSet为0
    // void BindDescriptorSets(VkCommandBuffer cmd, const VkDescriptorSet *pDescriptorSets, uint32_t descriptorSetCount = 1);
    // void BindDescriptorSets(VkCommandBuffer cmd, uint32_t uiSet, const VkDescriptorSet *pDescriptorSets, uint32_t descriptorSetCount = 1);
    // void PushConstant(VkCommandBuffer cmd);
    // void PushConstant(VkCommandBuffer cmd, const std::vector<glm::mat4>&data);

    // virtual void DrawUI();
    // virtual void DrawShaderUI(const ShaderInfo&shader);
    virtual VkResult CreatePipeline(VkDevice device, VkRenderPass&renderPass);
    virtual void IncreaseShader(VkDevice device, const std::vector<uint32_t>&code);
};
#endif