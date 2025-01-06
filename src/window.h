/* `Window` use glfw and glad to create a basic window interface */
#pragma once
#include <GLAD/glad.h>
#include<GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include"interface.h"
#include"common_include.h"

// forward declair
class Render;

class Window{
public:
    Window(){}
    Window(const char* name,int width,int height){
        init(name,width,height);
    }

    // 初始化glfw窗口
    int init(const char* name,int width,int height);

    void bindRender(Render* rptr){
        render_=rptr;
    }

    void bindRenderIOInfo(RenderIOInfo* info){
        info_=info;
    }

    // 初始化着色器
    void initShaderProgram();
    // 初始化纹理和VAO
    void initData();

    inline bool shouldClose(){return glfwWindowShouldClose(window_);};
    inline void processInput(){return glfwPollEvents();};

    // 更新frameBuffer
    void updateFrame(unsigned char* addr);
    inline void swapBuffer(){glfwSwapBuffers(window_);};

    // imGui
    void initImGui(){
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        io = &ImGui::GetIO();
        io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window_, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
        ImGui_ImplOpenGL3_Init();
    }

    void newImGuiFrame(){
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        showMyImGuiWindow();

    }

    void showMyImGuiWindow() {

        RenderSetting &setting = info_->setting_;
        PerfCnt &profile = info_->profile_;


        if (ImGui::Begin("Render Settings and Performance Metrics")) {

            ImGui::Text("Render Settings");

            // show_tlas
            ImGui::Checkbox("Show TLAS", &setting.show_tlas);

            // show_blas
            ImGui::Checkbox("Show BLAS", &setting.show_blas);

            // demo_scene 
            setting.scene_change=false;
            const std::vector<std::string> demoScenes = {"SingleBox", "Boxes", "Bunny","Bunnys_mutilights","CornellBox"};
            static int currentSceneIndex = 4;
            if (ImGui::BeginCombo("Demo Scene", demoScenes[currentSceneIndex].c_str())) {
                for (int i = 0; i < demoScenes.size(); ++i) {
                    bool isSelected = (currentSceneIndex == i);
                    if (ImGui::Selectable(demoScenes[i].c_str(), isSelected)) {
                        currentSceneIndex = i;
                        setting.scene_filename = demoScenes[i];
                        setting.scene_change=true;
                    }
                }
                ImGui::EndCombo();
            }

            const std::vector<std::string> rasterizeTypes = {"Naive" ,"Bvh_hzb", "Easy_hzb" ,"Scan_convert"};
            const std::vector<RasterizeType> rasterizeValues = {RasterizeType::Naive,RasterizeType::Bvh_hzb,RasterizeType::Easy_hzb,RasterizeType::Scan_convert};
            auto findIdx=[&setting,&rasterizeValues](){
                int idx=0;
                while(idx<rasterizeValues.size()){
                    if(rasterizeValues[idx]==setting.rasterize_type)
                        return idx;
                    ++idx;
                }
                std::cerr<<"unknown RasterizeType!\n";
                return 0;
            };

            static int currentRasterizeIndex = findIdx();
            if (ImGui::BeginCombo("Rasterize Type", rasterizeTypes[currentRasterizeIndex].c_str())) {
                for (int i = 0; i < rasterizeTypes.size(); ++i) {
                    bool isSelected = (currentRasterizeIndex == i);
                    if (ImGui::Selectable(rasterizeTypes[i].c_str(), isSelected)) {
                        currentRasterizeIndex = i;
                        setting.rasterize_type=rasterizeValues[currentRasterizeIndex];
                    }
                }
                ImGui::EndCombo();
            }

            const std::vector<std::string> shaderTypes = {"Depth" ,"BlinnPhone", "Normal" ,"Frame"};
            const std::vector<ShaderType> shaderValues = {ShaderType::Depth ,ShaderType::BlinnPhone|ShaderType::ORDER, ShaderType::Normal ,ShaderType::Frame};
            auto findIdx2=[&setting,&shaderValues](){
                int idx=0;
                while(idx<shaderValues.size()){
                    if(shaderValues[idx]==setting.shader_type)
                        return idx;
                    ++idx;
                }
                std::cerr<<"unknown ShaderType!\n";
                return 0;
            };

            static int currentShaderType = findIdx2();
            setting.shader_change=false;
            if (ImGui::BeginCombo("   Shader Type", shaderTypes[currentShaderType].c_str())) {
                for (int i = 0; i < shaderTypes.size(); ++i) {
                    bool isSelected = (currentShaderType == i);
                    if (ImGui::Selectable(shaderTypes[i].c_str(), isSelected)) {
                        currentShaderType = i;
                        setting.shader_type=shaderValues[currentShaderType];
                        setting.shader_change=true;
                    }
                }
                ImGui::EndCombo();
            }

            // back_culling
            ImGui::Checkbox("Backface Culling", &setting.back_culling);

            // profile_report
            ImGui::Checkbox("Profile Report", &setting.profile_report);

            // 输出部分：PerfCnt 数据展示
            ImGui::Separator();
            ImGui::Text("Performance Metrics");

            if (setting.profile_report) {
                ImGui::Text("Total Faces: %d", profile.total_face_num_);
                ImGui::Text("Shaded Faces: %d", profile.shaded_face_num_);
                ImGui::Text("Back Culled Faces: %d", profile.back_culled_face_num_);
                ImGui::Text("Clipped Faces: %d", profile.clipped_face_num_);
                ImGui::Text("HZB Culled Faces: %d", profile.hzb_culled_face_num_);
            } else {
                ImGui::Text("Enable 'Profile Report' to view metrics.");
            }
        }

        ImGui::End();

    }

    void renderImGuiFrame(){
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void shutdownImGui(){
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }



private:
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    GLFWwindow* window_;
    Render* render_;
    RenderIOInfo* info_;
    static ImGuiIO* io;
    unsigned int shaderProgram_;
    unsigned int width_ , height_;
    unsigned int texture_ , VAO_;
    
    float lastX_, lastY_; // 鼠标位置
    bool firstMouse_=true;     

    // 顶点着色器源代码
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        out vec2 TexCoord;
        void main() {
            gl_Position = vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";

    // 片段着色器源代码
    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 TexCoord;
        uniform sampler2D screenTexture;
        void main() {
            FragColor = texture(screenTexture, TexCoord);
        }
    )";



};