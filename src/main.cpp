#include<iostream>
#include"window.h"
#include <random> // 包含随机数库
#include <chrono>
#include <thread>

#include"render.h"


const int TARGET_FPS = 60;
const double FRAME_DURATION = 1000.0 / TARGET_FPS; // 单位：ms


int main(void) {
    
    // init my render
    Render render;
    render.setViewport(1600,16.0/9.0,60.0);

    render.loadDemoScene("Bunny",ShaderType::BlinnPhone|ShaderType::ORDER);
    // render.loadDemoScene("Brickwall",ShaderType::Normal);

    auto& camera=render.getCamera();
    camera.setMovement(0.1,0.1);
    camera.setFrastrum(1.0,1000.0);

    auto& colorbuffer=render.getColorBuffer();
    auto& scene=render.getScene();
    auto objs=scene.getObjects();

    int width=camera.getImageWidth();
    int height=camera.getImageHeight();
    
    RenderSetting setting;
    setting.shader_switch=ShaderSwitch::ALL_ON;
    // setting.shader_setting.flags=ShaderSwitch::ALL_ON^ShaderSwitch::BackCulling;


    // init glfw window and glad shader
    Window window;
    window.init("SRender", width, height);
    window.bindRender(&render);

    int cnt=0;
    auto lastTime=std::chrono::high_resolution_clock::now();
    auto curTime=lastTime;

    // gameloop
    render.pipelineInit(setting);
    while (!window.shouldClose()) {
       
        // if(1){// 简单的模型移动
           
        //     static int angle=0;
        //     // angle=(++angle)%360;
        //     glm::vec3 model_position1{100,-100,-300};
        //     // glm::vec3 model_position1{0,0,0};
        //     // M=T*R*S
        //     glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(50));
        //     glm::mat4 translation = glm::translate(glm::mat4(1.0f),model_position1);
        //     glm::mat4 rotate=glm::rotate(glm::mat4(1.0f), glm::radians((float)angle), glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));
        //     glm::mat4 model_matrix=translation*rotate*scale;

        //     objs[0]->setModel2World(model_matrix);

        //     glm::vec3 model_position2{100,-100,-100};
        //     // M=T*R*S
        //     glm::mat4 translation2 = glm::translate(glm::mat4(1.0f),model_position2);
        //     glm::mat4 model_matrix2=translation2*rotate*scale;

        //     objs[1]->setModel2World(model_matrix2);

        // }
        
        /* RENDERING */
        render.moveCamera();
        render.pipelineBegin();
        
        // update frameBuffer
        window.updateFrame(colorbuffer.getAddr());

         // wait some time
        curTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - lastTime).count();
        if (duration < FRAME_DURATION) {
            std::this_thread::sleep_for(std::chrono::milliseconds((long)(FRAME_DURATION - duration)));
            curTime = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - lastTime).count();
        }
        std::cout<<"frame : "<<cnt++<<" ,duration:"<<duration <<std::endl;

        // postrender events
        window.swapBuffer();
        render.cleanFrame();

        lastTime=curTime;
        render.setDeltaTime(float(duration));

         // process input
        window.processInput();
       
    }

    glfwTerminate();    
    return 0;
}


    /*
    Scene scene(std::string("assets/model/line_dot.obj"));// cornell_box
    if(0){
        int t=0;
        for(auto& p:scene.getObjects()){
            std::cout<<"---------------Obj [ "<<t++<<" ] INFO---------------"<<std::endl;
            p->printInfo();
        }
    }*/
