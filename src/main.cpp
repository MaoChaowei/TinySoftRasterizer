#include<iostream>
#include"window.h"
#include <random> // 包含随机数库
#include <chrono>
#include <thread>
#include"camera.h"
#include"buffer.h"
#include"scene_loader.h"
#include"object.h"
#ifndef TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#endif
#include "tiny_obj_loader.h"

const int TARGET_FPS = 10;
const double FRAME_DURATION = 1000.0 / TARGET_FPS; // 单位：ms


int main(void) {

    // prepare model
    Scene scene;
    {
        ObjLoader objloader(std::string("assets/model/line_dot.obj"));// cornell_box// line_dot
        auto& objmesh=objloader.getMeshes();
        auto& objline=objloader.getLines();

        for(auto& m:objmesh){
            scene.all_objects_.push_back(std::move(m));
        }
        for(auto& li:objline){
            scene.all_objects_.push_back(std::move(li));
        }
        
        if(1){
            int t=0;
            for(auto& p:scene.all_objects_){
                std::cout<<"Obj["<<t++<<"] INFO---------------"<<std::endl;
                p->printInfo();
            }
        }
    }   // clean objloader,hahaha
    

    // prepare camera
    Camera camera(glm::vec3(10,10,10),glm::vec3(0,0,0),glm::vec3(0,0,1));
    int width=camera.getImageWidth();
    int height=camera.getImageHeight();

    // init color buffer
    ColorBuffer colorbuffer(width,height);


    // 创建一个测试模型矩阵（例如：平移 + 旋转 + 缩放）
    glm::mat4 testMatrix = glm::mat4(1.0f);
    testMatrix = glm::translate(testMatrix, glm::vec3(1.0f, 2.0f, 3.0f)); // 平移
    testMatrix = glm::rotate(testMatrix, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // 绕Y轴旋转45度
    testMatrix = glm::scale(testMatrix, glm::vec3(2.0f, 2.0f, 2.0f)); // 缩放


    // init glfw window and glad shader
    Window window;
    window.init("SRender", width, height);


    // delete this
    std::random_device rd; 
    std::mt19937 gen(rd()); // 使用Mersenne Twister引擎
    std::uniform_int_distribution<> dis(0, 255); // 定义随机数范围 [1, 100]

    int cnt=0;
     // 初始化计时器
    auto lastTime = std::chrono::high_resolution_clock::now();
    while (!window.shouldClose()) {
         
        auto startTime = std::chrono::high_resolution_clock::now();// 当前帧开始时间
        // process input
        window.processInput();

        /* RENDERING */
        //TODO: pipline的入口函数

        int temp=dis(gen)%255;
        // std::cout<<"frame "<<cnt++<<" : temp = "<<temp<<std::endl;
        colorbuffer.cleanBuffer(temp);

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
       
    }
    glfwTerminate();    
    return 0;
}