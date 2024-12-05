/* define all the implementation of the graph pipeline here */
#pragma once
#include"common_include.h"
#include"../camera.h"
#include"../scene_loader.h"
#include"../buffer.h"
#include"AABB.h"
#include"utils.h"
#include"shader.h"


struct RenderSetting{
    // ShaderType shader_type=ShaderType::Depth;
    ShaderSetting shader_setting;
};

class Render{
public:
    Render():camera_(),colorbuffer_(camera_.getImageWidth(),camera_.getImageHeight()),
            zbuffer_(camera_.getImageWidth(),camera_.getImageHeight()){

        box_.min={0,0};
        box_.max={camera_.getImageWidth()-1,camera_.getImageHeight()-1};
    }

    inline void setCamera(glm::vec3 pos,glm::vec3 lookat,glm::vec3 right,float fov=60,float ratio=16.0/9.0,int image_width=1000){
        camera_.updateCamera(pos,lookat,right);
        colorbuffer_.reSetBuffer(camera_.getImageWidth(),camera_.getImageHeight());
        zbuffer_.reSetBuffer(camera_.getImageWidth(),camera_.getImageHeight());
        updateMatrix();

        box_.min={0,0};
        box_.max={camera_.getImageWidth()-1,camera_.getImageHeight()-1};
    }

    inline void setScene(std::string filename){
        scene_.addScene(filename);
    }


    void pipeModel2Screen();

    void pipeFragmentShader();

    // once change camera property, we need this function to update VPV-matrix
    void updateMatrix();

    void pipelineInit(const RenderSetting & setting=RenderSetting());


    // rastrization each frame, call setTransformation before this
    void pipelineBegin();

    // call this at the end of each frame's rendering!
    void inline cleanFrame(){
        // screen_pos_.clear();
        colorbuffer_.clear();
        zbuffer_.clear();
    }

    // rasterize
    void drawLine();
    void drawTriangle();

    inline const Camera& getCamera()const{ return camera_;}
    inline const ColorBuffer& getColorBuffer()const{ return colorbuffer_;}
    inline const Scene& getScene()const{ return scene_;}

    // debug
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


    // each object's position in Screen space, updated each frame
    // note that the index order of vertex should not change!
    // std::unordered_map<std::shared_ptr<ObjectDesc>,std::vector<glm::vec4>> screen_pos_;

    // screen aabb box
    AABB2d box_;


};