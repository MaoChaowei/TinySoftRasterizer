#include<iostream>
#include<common/gl_init.h>
#include <random> // 包含随机数库

#define DIM 700

// globals needed by the update routine
struct DataBlock {
    unsigned char* bitmap;
    int x, y;
    DataBlock(int width, int height) {
        bitmap = new unsigned char[width * height * 4];
        x = width;
        y = height;
    }
};

int main(void) {
    GLFWwindow* window;
    init(window, "SR", DIM, DIM);

    unsigned int texture, VAO;
    initData(texture, VAO);

    // 初始化着色器
    unsigned int shaderProgram = initShaderProgram();


    DataBlock data(DIM, DIM);
    int bitmap_size = 4 * DIM * DIM;
       // 设定随机数种子（基于时间）
    std::random_device rd; 
    std::mt19937 gen(rd()); // 使用Mersenne Twister引擎
    std::uniform_int_distribution<> dis(1, 100); // 定义随机数范围 [1, 100]

    int cnt=0;
    while (!glfwWindowShouldClose(window)) {
        // process input
        processInput(window);

        /* RENDERING */
        std::cout<<"-frame-"<<cnt++<<std::endl;
        std::fill(data.bitmap,data.bitmap+sizeof(data.bitmap)/sizeof(char),dis(gen)%255);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, DIM, DIM, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.bitmap);// 更新纹理数据

        // clear
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // 绘制纹理
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // postrender events
        glfwPollEvents();
        glfwSwapBuffers(window);// 刷新


    }

    return 0;
}