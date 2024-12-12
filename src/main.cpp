#include<iostream>
#include"window.h"
#include <chrono>
#include <thread>

#include"render.h"


const int TARGET_FPS = 60;
const double FRAME_DURATION = 1000.0 / TARGET_FPS; // 单位：ms


int main(void) {
    // init my render
    Render render;
    render.loadDemoScene("Bunny",ShaderType::Depth);//ShaderType::BlinnPhone|ShaderType::ORDER
    
    render.setViewport(1600,16.0/9.0,60.0);
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


    // init glfw window and glad
    Window window;
    window.init("SRender", width, height);
    window.bindRender(&render);

    int cnt=0;
    auto lastTime=std::chrono::high_resolution_clock::now();
    auto curTime=lastTime;

    // gameloop
    render.pipelineInit(setting);
    while (!window.shouldClose()) {
        /* RENDERING */
        render.moveCamera();

#ifdef TIME_RECORD
    render.timer_.start("100.Render::piplineBegin");
#endif
        render.pipelineBegin();
#ifdef TIME_RECORD
    render.timer_.stop("100.Render::piplineBegin");
#endif
        
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

#ifdef TIME_RECORD
        if(!(cnt%10)){
            render.timer_.report();
            render.timer_.reset();
        }
#endif
    }// game loop

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
