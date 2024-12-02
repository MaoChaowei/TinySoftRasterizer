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
    render.setScene(std::string("assets/model/tri.obj"));

    auto& camera=render.getCamera();
    auto& colorbuffer=render.getColorBuffer();
    auto& scene=render.getScene();
    auto objs=scene.getObjects();

    int width=camera.getImageWidth();
    int height=camera.getImageHeight();
    
    render.setTransformation();
    
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
        if(0){// 简单的模型移动
            // Step 1: 自转（绕模型局部坐标系的 rotation_axis 旋转）
            glm::mat4 model_matrix=glm::rotate(objs[0]->getModel(), glm::radians(10.f), glm::vec3(0,0,1));
            // Step 2: 平移（将模型移动到新的位置）
            // model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 1.0f, 1.0f));

            objs[0]->setModel2World(model_matrix);
        }
        
        /* RENDERING */
        //TODO: pipline的入口函数
        render.pipelineDemo();

        std::cout<<"frame : "<<cnt++<<std::endl;
        
        // update frameBuffer
        window.updateFrame(colorbuffer.getAddr());

         // 等待刷新
        auto curTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - startTime).count();
        if (duration < FRAME_DURATION) {
            std::this_thread::sleep_for(std::chrono::milliseconds((long)(FRAME_DURATION - duration)));
        }

        // postrender events
        window.swapBuffer();
        render.cleanFrame();
       
    }
    glfwTerminate();    
    return 0;
}