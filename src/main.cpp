#include<iostream>
#include"gl_init.h"
#include <random> // 包含随机数库
#include <chrono>
#include <thread>
#include"camera.h"
#include"buffer.h"

const int TARGET_FPS = 10;
const double FRAME_DURATION = 1000.0 / TARGET_FPS; // 单位：ms


int main(void) {
    // prepare camera and window
    Camera camera(glm::vec3(10,10,10),glm::vec3(0,0,0),glm::vec3(0,0,1));
    int width=camera.getImageWidth();
    int height=camera.getImageHeight();

    GLFWwindow* window;
    init(window, "SRender", width, height);

    unsigned int texture, VAO;
    initData(texture, VAO);

    // 初始化着色器
    unsigned int shaderProgram = initShaderProgram();

    // init color buffer
    ColorBuffer colorbuffer(width,height);

    // 设定随机数种子（基于时间）
    std::random_device rd; 
    std::mt19937 gen(rd()); // 使用Mersenne Twister引擎
    std::uniform_int_distribution<> dis(0, 255); // 定义随机数范围 [1, 100]

    int cnt=0;
     // 初始化计时器
    auto lastTime = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window)) {
         
        auto startTime = std::chrono::high_resolution_clock::now();// 当前帧开始时间
        // process input
        glfwPollEvents();

        /* RENDERING */
        // pipline的入口函数

        int temp=dis(gen)%255;
        std::cout<<"frame "<<cnt++<<" : temp = "<<temp<<std::endl;
        colorbuffer.cleanBuffer(temp);

        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,colorbuffer.getAddr());// 更新纹理数据

        // clear
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // 绘制纹理
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

         // 等待刷新
        auto curTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - startTime).count();
        if (duration < FRAME_DURATION) {
            std::this_thread::sleep_for(std::chrono::milliseconds((long)(FRAME_DURATION - duration)));
        }

        // postrender events
        glfwSwapBuffers(window);// 刷新
       
    }
    glfwTerminate();    
    return 0;
}