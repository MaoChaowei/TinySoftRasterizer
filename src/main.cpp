#include<iostream>
#include"window.h"
#include"render.h"

 // initialize static member for Window
ImGuiIO* Window::io=nullptr;                      

int main(void) {

    Render render;

    // these setting are not open to users yet..
    render.setViewport(1024,1.0/1.0,60.0);
    auto& camera=render.getCamera();
    camera.setMovement(0.05,0.1);
    camera.setFrastrum(1.0,1000.0);
    
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