#include<iostream>
#include"window.h"
#include"render.h"

 // initialize static member for Window
ImGuiIO* Window::io=nullptr;                      

int main(void) {

    // define my render
    Render render;

    // these setting are not open to users yet..
    render.setViewport(1024,1.0/1.0,60.0);
    auto& camera=render.getCamera();
    camera.setMovement(0.05,0.1);
    camera.setFrastrum(1.0,1000.0);
    
    // init and loop
    render.pipelineInit();
    render.GameLoop();
    
    return 0;
}
