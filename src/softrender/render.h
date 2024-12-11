/* define all the implementation of the graph pipeline here */
#pragma once
#include"common_include.h"
#include"../camera.h"
#include"../scene_loader.h"
#include"../buffer.h"
#include"AABB.h"
#include"utils.h"
#include"shader.h"
#include"../light.h"
#include <GLFW/glfw3.h>

// 定义裁剪平面
enum class ClipPlane {
    Left,
    Right,
    Bottom,
    Top,
    Near,
    Far
};

// 定义裁剪平面的位标志
enum ClipPlaneBit {
    CLIP_LEFT   = 1 << 0, // 000001
    CLIP_RIGHT  = 1 << 1, // 000010
    CLIP_BOTTOM = 1 << 2, // 000100
    CLIP_TOP    = 1 << 3, // 001000
    CLIP_NEAR   = 1 << 4, // 010000
    CLIP_FAR    = 1 << 5  // 100000
};



struct RenderSetting{
    ShaderSwitch shader_switch;
};

class Render{
public:
    Render():camera_(),colorbuffer_(camera_.getImageWidth(),camera_.getImageHeight()),
            zbuffer_(camera_.getImageWidth(),camera_.getImageHeight()){
        box_.min={0,0};
        box_.max={camera_.getImageWidth()-1,camera_.getImageHeight()-1};
    }

    // MEMBER SETTING

    inline void setCamera(glm::vec3 pos,glm::vec3 lookat,glm::vec3 right,float fov=60,float ratio=16.0/9.0,int image_width=1000){
        camera_.updateCamera(pos,lookat,right);
        afterCameraUpdate();
    }
    inline void setViewport(uint32_t width,float ratio,float fov){
        camera_.setViewport(width,ratio,fov);
        afterCameraUpdate();
    }
    void addScene(std::string filename,bool flipn=false,bool backculling=true);

    inline void setDeltaTime(float t){delta_time_=t;}

    void updateMatrix();
    inline void updateViewMatrix(){ mat_view_=camera_.getViewMatrix(); }

     // call this at the end of each frame's rendering!
    inline void cleanFrame(){
        colorbuffer_.clear();
        zbuffer_.clear();
    }

    inline  Camera& getCamera(){ return camera_;}
    inline const ColorBuffer& getColorBuffer()const{ return colorbuffer_;}
    inline const Scene& getScene()const{ return scene_;}

    // PIPELINE

    void pipelineInit(const RenderSetting & setting=RenderSetting());
    void pipelineBegin();

    int pipelineClipping(std::vector<Vertex>& v,std::vector<Vertex>& out);
    void clipWithPlane(ClipPlane plane,std::vector<Vertex>&in,std::vector<Vertex>&out);
    bool backCulling(const glm::vec3& face_norm,const glm::vec3& dir)const;


    // UI
    void handleKeyboardInput(int key, int action);
    void handleMouseInput(double xoffset, double yoffset);
    void moveCamera();

    // DEBUG
    void loadDemoScene(std::string name,ShaderType shader);

    void printMatrix(const glm::mat4& mat,const std::string name)const{
        std::cout<<name<<std::endl;
        for(int i=0;i<4;++i){
            for(int j=0;j<4;++j)
                std::cout<<mat[j][i]<<" ";
            std::cout<<std::endl;
        }
    }
    void debugMatrix()const{
        printMatrix(mat_view_,"mat_view_");
        printMatrix(mat_perspective_,"mat_perspective_");
        printMatrix(mat_viewport_,"mat_viewport_");
    }

private:
    // rasterize
    void drawLine(glm::vec2 t1,glm::vec2 t2);
    void drawTriangle();

    // update members accrordingly after camera's update
    void afterCameraUpdate();

private:
    bool is_init_=false;
    RenderSetting setting_;
    std::shared_ptr<Shader> sdptr_;
    Camera camera_;
    ColorBuffer colorbuffer_;
    DepthBuffer zbuffer_;
    Scene scene_;
    
    glm::mat4 mat_view_;        // model to world
    glm::mat4 mat_perspective_; // world to NDC
    glm::mat4 mat_viewport_;    // NDC to screen

    // screen aabb box
    AABB2d box_;

public:
    // for ui
    float delta_time_;          // time spent to render last frame; (ms)
    bool keys_[1024]={0}; 

};