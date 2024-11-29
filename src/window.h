/* `Window` use glfw and glad to create a basic window interface */
#pragma once
#include"common_include.h"
#include <GLAD/glad.h>
#include<GLFW/glfw3.h>

class Window{
public:

    // 初始化glfw窗口
    int init(const char* name,int width,int height);
    // 初始化着色器
    void initShaderProgram();
    // 初始化纹理和VAO
    void initData();

    inline bool shouldClose(){return glfwWindowShouldClose(window_);};
    inline void processInput(){return glfwPollEvents();};

    // 更新frameBuffer
    void updateFrame(unsigned char* addr);
    inline void swapBuffer(){glfwSwapBuffers(window_);};


private:
    GLFWwindow* window_;
    unsigned int shaderProgram_;
    unsigned int width_ , height_;
    unsigned int texture_ , VAO_;

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