/* define all the implementation of the graph pipeline here */
#pragma once
#include"common_include.h"
#include"../camera.h"
#include"../scene_loader.h"
#include"../buffer.h"
#include"AABB.h"
#include"utils.h"
#include"shader.h"
#include <GLFW/glfw3.h>


struct RenderSetting{
    ShaderSetting shader_setting;
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
    void addScene(std::string filename);

    inline void setDeltaTime(float t){delta_time_=t;}

    void updateMatrix();
    inline void updateViewMatrix(){ mat_view_=camera_.getViewMatrix(); }

     // call this at the end of each frame's rendering!
    inline void cleanFrame(){
        colorbuffer_.clear();
        zbuffer_.clear();
    }

    // PIPELINE

    void pipelineInit(const RenderSetting & setting=RenderSetting());
    void pipelineBegin();

    bool backCulling(const glm::vec3& face_norm,const glm::vec3& dir)const;

    inline  Camera& getCamera(){ return camera_;}
    inline const ColorBuffer& getColorBuffer()const{ return colorbuffer_;}
    inline const Scene& getScene()const{ return scene_;}

    // UI
    void handleKeyboardInput(int key, int action);
    void handleMouseInput(double xoffset, double yoffset);
    void moveCamera();

    // DEBUG
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
    void drawLine();
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