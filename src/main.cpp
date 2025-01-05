#include<iostream>
#include"window.h"
#include <chrono>
#include <thread>

#include"render.h"


const int TARGET_FPS = 60;
const double FRAME_DURATION = 1000.0 / TARGET_FPS; // 单位：ms


int main(void) {

    Render render;
    render.setBVHLeafSize(12);
    render.setViewport(1024,1.0/1.0,60.0);

    auto& camera=render.getCamera();
    camera.setMovement(0.05,0.1);
    camera.setFrastrum(1.0,1000.0);

    render.loadDemoScene("Bunny",ShaderType::BlinnPhone|ShaderType::ORDER);
    // render.loadDemoScene("CornellBox",ShaderType::Depth);
    // render.loadDemoScene("SingleBox",ShaderType::Normal);
    
    
    RenderSetting setting;
    setting.back_culling=true;
    setting.earlyz_test=true;

    setting.easy_hzb=false;
    setting.scan_convert=false;
    setting.bvh_hzb=true;

    setting.show_tlas=true;
    setting.show_blas=false;
    setting.profile_report=true;
    
    render.pipelineInit(setting);


    // init glfw window and glad
    int width=camera.getImageWidth();
    int height=camera.getImageHeight();

    Window window;
    window.init("SRender", width, height);
    window.bindRender(&render);

    int cnt=0;
    auto lastTime=std::chrono::high_resolution_clock::now();
    auto curTime=lastTime;

    // gameloop

    auto& colorbuffer=render.getColorBuffer();
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
        std::cout<<"frame : "<<cnt++<<" ,duration:"<<duration <<" ms "<<std::endl;

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
