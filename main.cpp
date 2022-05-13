#include <time.h>
#ifdef __linux
#include <unistd.h>
#include <dirent.h>
#endif
#include <iostream>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Pipeline.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "vulkanFrame.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#define MAXBYTE 0xff
#define MAX_PROGRESS_RATE 360.0f
// #define ARRAY_COUNT(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))
struct FourierTransform{
    bool available;
    double phase;
    double amplitude;
    double frequency;
    FourierTransform(){
        available = true;
        phase = 0;
        amplitude = 1;
        frequency = 1;
    }
};
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Color;
};

extern VkQueue	g_Graphics;
extern VkDescriptorPool	g_DescriptorPool;
extern QueueFamilyIndices g_QueueFamilyIndices;

vulkanBasicInfo                 g_VulkanBasic;
vulkanWindowInfo                g_VulkanWindow;
//---------------------
const uint32_t			g_WindowWidth = 800;//上下分屏。一边显示图形。一边显示波形
const uint32_t			g_WindowHeight = g_WindowWidth;

VkCommandBuffer    g_CommandBuffers;

GraphicsPipeline g_GraphicsPipeline;
std::vector<VkDescriptorSet>g_DescriptorSets;
std::vector<VkDescriptorSetLayout>g_DescriptorSetLayouts;

std::vector<FourierTransform>g_FourierTransform;

glm::vec3 g_LineColor = glm::vec3(1.0f, .0f, .0f);
bufferInfo g_Position;
uint32_t g_VertexCount;
std::vector<bufferInfo>g_Vertex(2);//0是图形, 1是波形

bool g_PlayAnimation;
double g_ProgressRate = MAX_PROGRESS_RATE;
bool g_ShowIncreaseFourierTransform;
void FourierTransformGraphics(double radians, glm::vec3&pos){
    double sinVal = 0, cosVal = 0;
    for (size_t uiFt = 0; uiFt < g_FourierTransform.size(); ++uiFt){
        if(g_FourierTransform[uiFt].available){
            //                                                                                            因为imgui可以输入负数。所以这里直接+(当然也可以直接-)
            sinVal += g_FourierTransform[uiFt].amplitude * glm::sin(g_FourierTransform[uiFt].frequency * radians + g_FourierTransform[uiFt].phase);
            cosVal += g_FourierTransform[uiFt].amplitude * glm::cos(g_FourierTransform[uiFt].frequency * radians + g_FourierTransform[uiFt].phase);
        }
    }
    pos.x = sinVal;
    pos.y = cosVal;
    pos.z = .0f;
}
void FourierTransformSin(double radians, glm::vec3&pos){
    double sinVal = 0, cosVal = 0;
    for (size_t uiFt = 0; uiFt < g_FourierTransform.size(); ++uiFt){
        if(g_FourierTransform[uiFt].available){
            //                                                                                            目前因为imgui可以输入负数。所以这里直接+(当然也可以直接-)
            sinVal += g_FourierTransform[uiFt].amplitude * glm::sin(g_FourierTransform[uiFt].frequency * radians + g_FourierTransform[uiFt].phase);
        }
    }
    pos.x = radians;
    pos.y = sinVal;
    pos.z = .0f;
}
void getVertex(std::vector<Vertex>&vertices, void(*callback)(double, glm::vec3&), double fEnd = MAX_PROGRESS_RATE){
    vertices.clear();
    Vertex v;
    v.Color = g_LineColor;
    for (double i = 0; i < fEnd; i += .1f){
        callback(glm::radians(i), v.Position);
        vertices.push_back(v);
    }
    g_VertexCount = vertices.size();
}
void updateVertexData(double fEnd = MAX_PROGRESS_RATE){
    std::vector<Vertex>vertices;
    getVertex(vertices, FourierTransformGraphics, fEnd);
    bufferData(g_VulkanBasic.device, vertices.size() * sizeof(Vertex), vertices.data(), g_Vertex[0].memory);
    getVertex(vertices, FourierTransformSin, fEnd);
    bufferData(g_VulkanBasic.device, vertices.size() * sizeof(Vertex), vertices.data(), g_Vertex[1].memory);
}
void playAnimation(){
    g_ProgressRate += .1f;
    if(g_ProgressRate > MAX_PROGRESS_RATE){
        g_ProgressRate = MAX_PROGRESS_RATE;
        g_PlayAnimation = false;
        return;
    }
    updateVertexData(g_ProgressRate);
}
void updateDescriptorSet(uint32_t uiSet, VkDescriptorSet&dsSet, const std::vector<ShaderInfo>::const_iterator&begin, const std::vector<ShaderInfo>::const_iterator&end){
    VkDescriptorImageInfo imageInfo{};
    VkDescriptorBufferInfo bufferInfo{};
    std::vector<VkWriteDescriptorSet>writeDescriptorSets;
    bufferInfo.buffer = g_Position.buffer;
    bufferInfo.range = g_Position.size;
    VkWriteDescriptorSet info{};
    info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    info.descriptorCount = 1;
    info.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    info.dstSet = dsSet;
    info.pBufferInfo = &bufferInfo;
    writeDescriptorSets.push_back(info);
    vkUpdateDescriptorSets(g_VulkanBasic.device, writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
    //如果资源有多种。就可能需要多个set。例如多张图片。那么可以一个set一张图片
}
void updateDescriptorSet(){
    //遍历描述符集、从着色器得到创建buffer或image的信息、创建、更新
    for (uint32_t uiSet = 0; uiSet < g_DescriptorSets.size(); ++uiSet){
        updateDescriptorSet(uiSet, g_DescriptorSets[uiSet], g_GraphicsPipeline.ShaderBegin(), g_GraphicsPipeline.ShaderEnd());
    }
}
void drawFourierTransform(FourierTransform&ft){
    if(ImGui::BeginTable("圆表格相关", 3)){
        ImGui::TableNextColumn();ImGui::InputDouble("振幅", &ft.amplitude);
        ImGui::TableNextColumn();ImGui::InputDouble("相位", &ft.phase);
        ImGui::TableNextColumn();ImGui::InputDouble("频率", &ft.frequency);
        ImGui::EndTable();
    }
}
void drawFourierTransform(std::vector<FourierTransform>::iterator&ft){
    drawFourierTransform(*ft);
}
// void drawFourierTransform(std::vector<FourierTransform>&ft){
//     for (size_t i = 0; i < ft.size(); ++i){
//         drawFourierTransform(ft[i]);
//     }    
// }
void updateImguiWidget(){
    // static bool checkbuttonstatu;//检查框的状态。这个值传给imgui会影响到检查框
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // static double test = 0;
    // ImGui::DragInt("拖动条", &test);//确实可以拖动。但不是类似一条线中间有个圆的样子
    ImGui::Begin("傅里叶变换");
        //ImGui::ProgressBar(90);
        // ImGui::SameLine();
        // ImGui::Sliderdouble("滑块", &test, .0f, 1.0f);
        char buffer[MAXBYTE] = {0};
        static float lineColor[3] = { g_LineColor.x, g_LineColor.y, g_LineColor.z };
        ImGui::ColorEdit3("线颜色", lineColor);
        if(lineColor[0] != g_LineColor.x || lineColor[1] != g_LineColor.y || lineColor[2] != g_LineColor.z){
            g_LineColor.x = lineColor[0];
            g_LineColor.y = lineColor[1];
            g_LineColor.z = lineColor[2];
            updateVertexData(g_ProgressRate);
        }
        uint32_t index = 0;
        for (auto it = g_FourierTransform.begin(); it != g_FourierTransform.end(); ++it){
            sprintf(buffer, "第%d个圆", index++ + 1);
            if(ImGui::TreeNode(buffer)){
                double val[3] = { it->amplitude, it->frequency, it->phase };
                drawFourierTransform(it);
                if(it->amplitude != val[0] || it->frequency != val[1] || it->phase != val[2]){
                    updateVertexData(g_ProgressRate);
                }
                if(ImGui::BeginTable("启用或删除", 2)){
                    ImGui::TableNextColumn();ImGui::Checkbox("启用", &it->available);
                    ImGui::TableNextColumn();
                    if(ImGui::Button("删除")){
                        g_FourierTransform.erase(it);
                        ImGui::EndTable();
                        ImGui::TreePop();
                        break;
                    }
                    // updateVertexData(g_ProgressRate);//播放动画时。或许不应该继续更新和删除
                    ImGui::EndTable();
                }
                ImGui::TreePop();
            }
        }
        if(ImGui::BeginTable("操作按钮", 2)){
            ImGui::TableNextColumn();
            if(ImGui::Button("添加圆")){
                g_ShowIncreaseFourierTransform = true;
            }
            ImGui::TableNextColumn();
            if(ImGui::Button("播放")){
                g_PlayAnimation = !g_PlayAnimation;
                if(g_PlayAnimation){
                    
                    g_ProgressRate = .0f;
                }
                else{
                    g_ProgressRate = MAX_PROGRESS_RATE;
                    updateVertexData();
                }
            }
            ImGui::EndTable();
        }
    ImGui::End();
    if(g_ShowIncreaseFourierTransform){
        ImGui::Begin("添加圆");
            static FourierTransform ft;
            // ft.available = true;
            drawFourierTransform(ft);
            static int count = 1;
            ImGui::InputInt("个数", &count);
            if(ImGui::BeginTable("操作按钮表格", 2)){
                ImGui::TableNextColumn();
                if(ImGui::Button("确定")){
                    for (size_t i = 0; i < count; ++i){
                        g_FourierTransform.push_back(ft);
                    }                
                    updateVertexData(g_ProgressRate);
                    g_ShowIncreaseFourierTransform = false;
                }
                ImGui::TableNextColumn();
                if(ImGui::Button("取消")){
                    g_ShowIncreaseFourierTransform = false;
                }
                ImGui::EndTable();
            }
        ImGui::End();
    }
    ImGui::Render();
    ImDrawData *draw_data = ImGui::GetDrawData();
    
    const bool isMinimized = (draw_data->DisplaySize.x <=.0f || draw_data->DisplaySize.y <= .0f);
    if(!isMinimized)ImGui_ImplVulkan_RenderDrawData(draw_data, g_CommandBuffers);
}
void recodeCommand(uint32_t currentFrame){
    std::vector<VkClearValue> clearValues(2);
    clearValues[0].color = { 0.1f , 0.1f , 0.1f , 1.0f };
    clearValues[1].depthStencil = { 1.0f , 0 };
    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.clearValueCount = clearValues.size();
    renderPassInfo.pClearValues = clearValues.data();
    renderPassInfo.renderArea = { 0, 0, g_WindowWidth, g_WindowHeight };
    renderPassInfo.renderPass = g_VulkanWindow.renderPass;
    renderPassInfo.framebuffer = g_VulkanWindow.framebuffers[currentFrame];
    VkCommandBufferBeginInfo beginInfo;
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;//ﬂags 成员变量用于指定我们将要怎样使用指令缓冲:VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT： 指令缓冲在执行一次后，就被用来记录新的指令。VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT： 在指令缓冲等待执行时，仍然可以提交这一指令缓冲。
    beginInfo.pInheritanceInfo = nullptr;
    vkDeviceWaitIdle(g_VulkanBasic.device);
    vkResetCommandBuffer(g_CommandBuffers, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    if(VK_SUCCESS != vkBeginCommandBuffer(g_CommandBuffers, &beginInfo)){
        std::cout << "vkBeginCommandBuffer failed" << std::endl;
        return;
    }
    std::vector<VkViewport> view(2);
    view[1].y = g_WindowHeight * .5f;
    std::vector<VkRect2D> scissor(2);
    scissor[1].offset = { 0, g_WindowHeight / 2 };
    vkCmdBeginRenderPass(g_CommandBuffers, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    if(g_GraphicsPipeline.BindPipeline(g_CommandBuffers)){
        static const VkDeviceSize vertexOffset = 0;
        static const glm::mat4 projection = glm::ortho(0.0f, (float)g_WindowWidth, 0.0f, (float)g_WindowHeight, -1.0f, 1.0f);//glm::mat4(1.0f);
        for (size_t uiScreen = 0; uiScreen < 2; ++uiScreen){
            view[uiScreen].maxDepth = 1;
            view[uiScreen].width = g_WindowWidth;
            view[uiScreen].height = g_WindowHeight * .5f;
            scissor[uiScreen].extent = { g_WindowWidth, g_WindowHeight };
            vkCmdSetViewport(g_CommandBuffers, 0, 1, &view[uiScreen]);
            vkCmdSetScissor(g_CommandBuffers, 0, 1, &scissor[uiScreen]);
            vkCmdPushConstants(g_CommandBuffers, g_GraphicsPipeline.GetLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &projection);
            vkCmdBindVertexBuffers(g_CommandBuffers, 0, 1, &g_Vertex[uiScreen].buffer, &vertexOffset);
            for (size_t i = 0; i < g_DescriptorSets.size(); ++i){
                vkCmdBindDescriptorSets(g_CommandBuffers, VK_PIPELINE_BIND_POINT_GRAPHICS, g_GraphicsPipeline.GetLayout(), i, 1, &g_DescriptorSets[i], 0, nullptr);
            }
            vkCmdDraw(g_CommandBuffers, g_VertexCount, 1, 0, 0);            
        }
    }
    updateImguiWidget();
    vkCmdEndRenderPass(g_CommandBuffers);
    if(VK_SUCCESS != vkEndCommandBuffer(g_CommandBuffers)){
        std::cout << "vkEndCommandBuffer failed" << std::endl; 
        return;
    }
}
/*}}}*/
/*{{{*/
void setup(GLFWwindow *window){//不能直接修改，应该考虑以后测试更容易写的方式
    FourierTransform ft;
    // ft.phase = 0;
    // ft.amplitude = 1;
    // ft.frequency = 1;
    // ft.available = true;
    g_FourierTransform.push_back(ft);
    std::vector<Vertex>vertices;
    getVertex(vertices, FourierTransformGraphics);
    createBuffer(g_VulkanBasic.device,  vertices.size() * sizeof(Vertex), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, g_Vertex[0]);
    bufferData(g_VulkanBasic.device, vertices.size() * sizeof(Vertex), vertices.data(), g_Vertex[0].memory);
    
    getVertex(vertices, FourierTransformSin);
    createBuffer(g_VulkanBasic.device,  vertices.size() * sizeof(Vertex), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, g_Vertex[1]);
    bufferData(g_VulkanBasic.device, vertices.size() * sizeof(Vertex), vertices.data(), g_Vertex[1].memory);
    
    createBuffer(g_VulkanBasic.device, sizeof(glm::mat4), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, g_Position);
    glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(200.0f, 300.0f, 0)), glm::vec3(50.0f, 100.0f, 1));
    // glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(.25f, .25f, 1));
    bufferData(g_VulkanBasic.device, sizeof(glm::mat4(1.0f)), &model, g_Position.memory);

    std::vector<uint32_t>cacheData;
    GetFileContent("GraphicsPipelineCache", cacheData);
    std::string shader[] = { "shader/baseVert.spv", "shader/baseFrag.spv" };
    for (size_t i = 0; i < 2; ++i){
        std::vector<uint32_t>code;
        GetFileContent(shader[i], code);
        g_GraphicsPipeline.IncreaseShader(g_VulkanBasic.device, code);
    }
    GraphicsPipelineStateInfo pipelineState;
    // pipelineState.mInputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    // pipelineState.mInputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    pipelineState.mInputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
    g_GraphicsPipeline.SetStateInfo(pipelineState);
    // g_GraphicsPipeline.PushScissor(g_WindowWidth, g_WindowHeight);
    // g_GraphicsPipeline.PushViewport(g_WindowWidth, g_WindowHeight);
    VK_CHECK(g_GraphicsPipeline.CreateCache(g_VulkanBasic.device, cacheData));
    VK_CHECK(g_GraphicsPipeline.CreateDescriptorSetLayout(g_VulkanBasic.device, g_DescriptorSetLayouts));
    VK_CHECK(g_GraphicsPipeline.CreateLayout(g_VulkanBasic.device, g_DescriptorSetLayouts));
    VK_CHECK(g_GraphicsPipeline.CreatePipeline(g_VulkanBasic.device, g_VulkanWindow.renderPass));
    g_GraphicsPipeline.GetCacheData(g_VulkanBasic.device, cacheData);
    writeFileContent("GraphicsPipelineCache", cacheData.data(), cacheData.size() * sizeof(uint32_t));

    g_DescriptorSets.resize(g_DescriptorSetLayouts.size());
    for (size_t i = 0; i < g_DescriptorSets.size(); ++i){
        allocateDescriptorSets(g_VulkanBasic.device, 1, g_DescriptorSetLayouts[i], g_DescriptorSets[i]);
    }
    updateDescriptorSet();

    allocateCommandBuffers(g_VulkanBasic.device, 1, &g_CommandBuffers);
    //imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO&io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;//启用手柄
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo initInfo = {};
    initInfo.Instance = g_VulkanBasic.instance;
    initInfo.PhysicalDevice = g_VulkanBasic.physicalDevice;
    initInfo.Device = g_VulkanBasic.device;
    initInfo.QueueFamily = g_QueueFamilyIndices.graphicsFamily.value();
    initInfo.Queue = g_Graphics;
    initInfo.PipelineCache = VK_NULL_HANDLE;//g_GraphicsPipline.getCache();
    initInfo.DescriptorPool = g_DescriptorPool;
    initInfo.MinImageCount = 3;
    initInfo.ImageCount = g_VulkanWindow.framebuffers.size();
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    initInfo.Allocator = nullptr;
    initInfo.CheckVkResultFn = nullptr;
    ImGui_ImplVulkan_Init(&initInfo, g_VulkanWindow.renderPass);

    io.Fonts->AddFontFromFileTTF("fonts/SourceHanSerifCN-Bold.otf", 20, NULL, io.Fonts->GetGlyphRangesChineseFull());

    VkCommandBuffer cmd;
    beginSingleTimeCommands(g_VulkanBasic.device, cmd);
    ImGui_ImplVulkan_CreateFontsTexture(cmd);
    endSingleTimeCommands(g_VulkanBasic.device, cmd);
    ImGui_ImplVulkan_DestroyFontUploadObjects();
}
/*}}}*/
/*{{{*/
void cleanup(GLFWwindow* window){
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    g_GraphicsPipeline.DestrotyShader(g_VulkanBasic.device);
    g_GraphicsPipeline.DestroyLayout(g_VulkanBasic.device);
    g_GraphicsPipeline.DestroyPipeline(g_VulkanBasic.device);
    std::vector<uint32_t>cacheData;
    g_GraphicsPipeline.DestroyCache(g_VulkanBasic.device, cacheData);
    writeFileContent("GraphicsPipelineCache", cacheData.data(), cacheData.size() * sizeof(uint32_t));
    for (size_t i = 0; i < g_Vertex.size(); ++i){
        destroyBuffer(g_VulkanBasic.device, g_Vertex[i]);
    }    
    destroyBuffer(g_VulkanBasic.device, g_Position);
    for (size_t i = 0; i < g_DescriptorSetLayouts.size(); ++i){
        vkDestroyDescriptorSetLayout(g_VulkanBasic.device, g_DescriptorSetLayouts[i], nullptr);g_DescriptorSetLayouts[i] = nullptr;
    }
}
/*{{{*/
void recreateSwapchain(void *userData) {
    int width = 0, height = 0;
    GLFWwindow *window = (GLFWwindow *)userData;
    while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
    }
    vkDeviceWaitIdle(g_VulkanBasic.device);
    cleanupVulkanWindow(g_VulkanBasic.device, g_VulkanWindow);
    setupVulkanWindow(g_VulkanBasic.physicalDevice, g_VulkanBasic.device, g_VulkanWindow);
    freeCommandBufferMemory(g_VulkanBasic.device, 1, &g_CommandBuffers);
    allocateCommandBuffers(g_VulkanBasic.device, 1, &g_CommandBuffers);
    // recodeCommand();
}
/*}}}*/
/*{{{*/
void display(GLFWwindow* window){
    // processInput(window);
    static size_t currentFrame;
    if(g_PlayAnimation)playAnimation();
    recodeCommand(currentFrame);
    drawFrame(g_VulkanBasic.device, currentFrame, g_CommandBuffers, g_VulkanWindow, recreateSwapchain, window);
    currentFrame = (currentFrame + 1) % g_VulkanWindow.framebuffers.size();
}
/*}}}*/
void createSurface(VkInstance instance, VkSurfaceKHR&surface, void* userData){
    glfwCreateWindowSurface(instance, (GLFWwindow *)userData, nullptr, &surface);
}
int main(int32_t argc, char *argv[]){
    if (GLFW_FALSE == glfwInit()) {
            std::cout << "initialize glfw error" << std::endl;
            return 1;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(g_WindowWidth, g_WindowHeight, "FourierTransformDemo", NULL, NULL);
    // Setup Vulkan
    if (!glfwVulkanSupported()){
        printf("GLFW: Vulkan Not Supported\n");
        return 1;
    }
    uint32_t count;
    const char** instanceExtension = glfwGetRequiredInstanceExtensions(&count);
    std::vector<const char*> extensions(instanceExtension, instanceExtension + count);
    setupVulkan(extensions, { }, g_VulkanBasic, createSurface, window);
    setupVulkanWindow(g_VulkanBasic.physicalDevice, g_VulkanBasic.device, g_VulkanWindow, VK_TRUE);
    createPool(g_VulkanBasic.device, 2, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    setup(window);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        display(window);
    }
    vkDeviceWaitIdle(g_VulkanBasic.device);
    cleanup(window);
    cleanupPool(g_VulkanBasic.device);
    cleanupVulkanWindow(g_VulkanBasic.device, g_VulkanWindow);
    cleanupVulkan(g_VulkanBasic.instance, g_VulkanBasic.device);
}
