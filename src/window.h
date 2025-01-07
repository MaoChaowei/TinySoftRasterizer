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
        static bool first_load=true;
        if(first_load){
            ImGui::SetNextWindowPos(ImVec2(0, height_-300)); 
            ImGui::SetNextWindowSize(ImVec2(width_/3, 300)); 
            first_load=false;
        }

        if (ImGui::Begin("Render Settings")) {
            ImGui::Dummy(ImVec2(0.0f, 10.0f));

            // show_tlas
            ImGui::Checkbox("Show TLAS", &setting.show_tlas);

            // show_blas
            ImGui::Checkbox("Show BLAS", &setting.show_blas);

            ImGui::Text("Leaf size of BLAS");
            ImGui::SameLine();
            setting.leaf_num_change=false;
            if (ImGui::SliderInt("##Leaf size of BLAS", &setting.bvh_leaf_num, 4, 36)) {
                setting.leaf_num_change=true;
            }

            // demo_scene 
            setting.scene_change=false;
            const std::vector<std::string> demoScenes = {"person", "Boxes", "Bunny_with_wall","Bunnys_mutilights","High_Depth_Complexity"};
            auto findIdx0=[&setting,&demoScenes](){
                int idx=0;
                while(idx<demoScenes.size()){
                    if(demoScenes[idx]==setting.scene_filename)
                        return idx;
                    ++idx;
                }
                std::cerr<<"unknown demoScenes!\n";
                return 0;
            };

            static int currentSceneIndex = findIdx0();
            ImGui::Text("    Demo Scene");
            ImGui::SameLine();
            if (ImGui::BeginCombo("##   Demo Scene", demoScenes[currentSceneIndex].c_str())) {
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
            setting.rasterize_change=false;
            ImGui::Text("Rasterize Type");
            ImGui::SameLine();
            if (ImGui::BeginCombo("##Rasterize Type", rasterizeTypes[currentRasterizeIndex].c_str())) {
                for (int i = 0; i < rasterizeTypes.size(); ++i) {
                    bool isSelected = (currentRasterizeIndex == i);
                    if (ImGui::Selectable(rasterizeTypes[i].c_str(), isSelected)) {
                        currentRasterizeIndex = i;
                        setting.rasterize_type=rasterizeValues[currentRasterizeIndex];
                        setting.rasterize_change=true;
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
            ImGui::Text("   Shader Type");
            ImGui::SameLine();
            if (ImGui::BeginCombo("##   Shader Type", shaderTypes[currentShaderType].c_str())) {
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


            if (setting.profile_report) {
                showProfileReport();
            } else {
                ImGui::Text("Enable 'Profile Report' to view metrics.");
            }
           
        }
        ImGui::End();

    }

    void showProfileReport(){ 
        static bool first_load=true;
        if(first_load){
            ImGui::SetNextWindowPos(ImVec2(width_/3, height_-300)); 
            ImGui::SetNextWindowSize(ImVec2(width_*2.0/3, 300)); 
            first_load=false;
        }

        if (ImGui::Begin("Performance Metrics")) {

            ImGui::Columns(2, nullptr, true);// 开启 2 列布局

            PerfCnt &profile = info_->profile_;
            CPUTimer& timer=info_->timer_;

            ImGui::Text("* Face(Triangle) Distribution");
            ImGui::Text("· Total Faces: %d", profile.total_face_num_);
            ImGui::Text("· Shaded Faces: %d", profile.shaded_face_num_);
            ImGui::Text("· Back Culled Faces: %d", profile.back_culled_face_num_);
            ImGui::Text("· Clipped Faces: %d", profile.clipped_face_num_);
            ImGui::Text("· HZB Culled Faces: %d", profile.hzb_culled_face_num_);
 
            ImGui::Dummy(ImVec2(0.0f, 10.0f)); 
            ImGui::NextColumn();
#ifdef TIME_RECORD
            ImGui::Text("* Time for Each Stage");
            for (const auto &[type, timer] : timer.timers_)
            {
                ImGui::Text("· %s : %.1f ms",type.c_str(),(timer.elapsed_time) / 1000.0);
            }
#endif
            ImGui::Columns(1);// 回到默认的单列布局
            ImGui::Separator(); 
            ImGui::Dummy(ImVec2(0.0f, 5.0f)); 

            showFPSGraph();
        }
        ImGui::End();
    }

    void showFPSGraph() {
        
        static std::vector<float> fps(50, 0.0f); // 存储最近 100 帧的帧率
        static int currentIndex = 0;

        // 获取当前帧的帧时间 (ms)
        float currentFPS=ImGui::GetIO().Framerate;
        float currentFrameTime = 1000.0f / currentFPS;

        // 显示帧率
        ImGui::Text("FPS: %.1f fps", ImGui::GetIO().Framerate);
        ImGui::Text("Frame Time: %.3f ms/frame", currentFrameTime);
        fps[currentIndex] = currentFPS;
        currentIndex = (currentIndex + 1) % fps.size();

        // 绘制帧时间折线图
        ImGui::PlotLines(
            "##FPS Line Chart",           // 图表标题
            fps.data(),               // 数据指针
            fps.size(),               // 数据数量
            currentIndex,                    // 当前数据偏移
            nullptr,                         // 可选标签（显示在图表左上角）
            0.0f,                            // 最小值
            30.f,  // 最大值
            ImVec2(0, 80)                    // 图表大小
        );
        ImGui::Text("FPS Line Chart");
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