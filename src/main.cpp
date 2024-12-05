#include<iostream>
#include"window.h"
#include <random> // 包含随机数库
#include <chrono>
#include <thread>
// #include"camera.h"
// #include"buffer.h"
// #include"scene_loader.h"
// #include"object.h"

#include"render.h"
#ifndef TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#endif
#include "tiny_obj_loader.h"

const int TARGET_FPS = 10;
const double FRAME_DURATION = 1000.0 / TARGET_FPS; // 单位：ms


int main(void) {
    
    Render render;
    render.setScene(std::string("assets/model/cube.obj"));

    auto& camera=render.getCamera();
    auto& colorbuffer=render.getColorBuffer();
    auto& scene=render.getScene();
    auto objs=scene.getObjects();

    int width=camera.getImageWidth();
    int height=camera.getImageHeight();
    
    RenderSetting setting;
    setting.shader_setting.type=ShaderType::Color;
    setting.shader_setting.flags=ShaderSwitch::ALL_ON;

    render.pipelineInit(setting);
    
    /*
    Scene scene(std::string("assets/model/line_dot.obj"));// cornell_box
    if(0){
        int t=0;
        for(auto& p:scene.getObjects()){
            std::cout<<"---------------Obj [ "<<t++<<" ] INFO---------------"<<std::endl;
            p->printInfo();
        }
    }*/


    // init glfw window and glad shader
    Window window;
    window.init("SRender", width, height);


    int cnt=0;
     // 初始化计时器
    auto lastTime = std::chrono::high_resolution_clock::now();
    while (!window.shouldClose()) {
         
        auto startTime = std::chrono::high_resolution_clock::now();// 当前帧开始时间
        // process input
        window.processInput();
        if(1){// 简单的模型移动
           
            static int angle=0;
            angle=(++angle)%360;
            glm::vec3 model_position{20,20,-100};
            // M=T*R*S
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(10));
            glm::mat4 translation = glm::translate(glm::mat4(1.0f),model_position);
            glm::mat4 rotate=glm::rotate(glm::mat4(1.0f), glm::radians((float)angle), glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));
            glm::mat4 model_matrix=translation*rotate*scale;

            objs[0]->setModel2World(model_matrix);
        }
        
        /* RENDERING */
        //TODO: pipline的入口函数
        render.pipelineBegin();
        
        // update frameBuffer
        window.updateFrame(colorbuffer.getAddr());

         // 等待刷新
        auto curTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - startTime).count();
        // if (duration < FRAME_DURATION) {
        //     std::this_thread::sleep_for(std::chrono::milliseconds((long)(FRAME_DURATION - duration)));
        // }
        std::cout<<"frame : "<<cnt++<<" ,duration:"<<duration <<std::endl;

        // postrender events
        window.swapBuffer();
        render.cleanFrame();
       
    }
    glfwTerminate();    
    return 0;
}