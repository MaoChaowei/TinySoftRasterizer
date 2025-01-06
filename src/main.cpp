#include<iostream>
#include"window.h"
#include"render.h"

// const int TARGET_FPS = 60;
// const double FRAME_DURATION = 1000.0 / TARGET_FPS; // ms
ImGuiIO* Window::io=nullptr;                       // initialize static member

int main(void) {

    Render render;
    render.setBVHLeafSize(12);
    render.setViewport(1024,1.0/1.0,60.0);

    auto& camera=render.getCamera();
    camera.setMovement(0.05,0.1);
    camera.setFrastrum(1.0,1000.0);
    // render.loadDemoScene("SingleBox",ShaderType::Normal);
    // render.loadDemoScene("Bunnys",ShaderType::BlinnPhone|ShaderType::ORDER);
    
    render.pipelineInit();

    render.GameLoop();
    
    return 0;
}



/*
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

        window.processInput();
        window.newImGuiFrame();

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

        window.renderImGuiFrame();

        // postrender events
        window.swapBuffer();
        render.cleanFrame();

        lastTime=curTime;
        render.setDeltaTime(float(duration));


#ifdef TIME_RECORD
        if(!(cnt%10)){
            render.timer_.report();
            render.timer_.reset();
        }
#endif
    }// game loop

    window.shutdownImGui();
    glfwTerminate();    

*/