/* define all the implementation of the graph pipeline here */
#pragma once
#include"common/common_include.h"
#include"common/cputimer.h"
#include"softrender/shader.h"
#include"camera.h"
#include"scene_loader.h"
#include"buffer.h"
#include"hzb.h"
#include"AABB.h"
#include"utils.h"
#include"light.h"
#include"scanline.h"
#include <GLFW/glfw3.h>

enum class ClipPlane {
    Left,
    Right,
    Bottom,
    Top,
    Near,
    Far
};

struct RenderSetting{
    bool show_tlas=false;
    bool show_blas=false;
    bool hzb_flag=false;
    bool scan_convert=false;

    bool back_culling=true;
    bool earlyz_test=true;
};

class Render{
public:
    Render():camera_(),colorbuffer_(std::make_shared<ColorBuffer>(camera_.getImageWidth(),camera_.getImageHeight())),
            zbuffer_(std::make_shared<DepthBuffer>(camera_.getImageWidth(),camera_.getImageHeight())){
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
    void addObjInstance(std::string filename,glm::mat4& model,ShaderType shader,bool flipn=false,bool backculling=true);

    inline void setDeltaTime(float t){delta_time_=t;}

    void updateMatrix();
    inline void updateViewMatrix(){ mat_view_=camera_.getViewMatrix(); }

    inline void cleanFrame(){
        colorbuffer_->clear();
        zbuffer_->clear();
    }

    inline  Camera& getCamera(){ return camera_;}
    inline const ColorBuffer& getColorBuffer()const{ return *colorbuffer_;}
    inline const Scene& getScene()const{ return scene_;}

    // PIPELINE
    void pipelineInit(const RenderSetting & setting=RenderSetting());
    void pipelineBegin();
    bool depthTest(uint32_t x,uint32_t y);
    
    // void pipelineNaive();

    int pipelineClipping(std::vector<Vertex>& v,std::vector<Vertex>& out);
    void clipWithPlane(ClipPlane plane,std::vector<Vertex>&in,std::vector<Vertex>&out);
    bool backCulling(const glm::vec3& face_norm,const glm::vec3& dir)const;


    // UI
    void handleKeyboardInput(int key, int action);
    void handleMouseInput(double xoffset, double yoffset);
    void moveCamera();

    // DEBUG
    void showTLAS();
    void showBLAS(const ASInstance& inst);
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
    void drawPoint(const glm::vec2 p, float radius,const glm::vec4 color);
    void drawLine(glm::vec2 t1,glm::vec2 t2);
    void drawLine3d(glm::vec3 t1,glm::vec3 t2,const glm::vec4& color=glm::vec4(255));
    void drawTriangleNaive();
    void drawTriangleScanLine();
    void scanLineConvert(std::vector<const Vertex*> vertices,std::vector<FragmentHolder>& edge_points);
    void rasterizeEdge(const Vertex* v1,const Vertex* v2,bool in1,bool in2,std::vector<FragmentHolder>& edge_points);

    void traverseBVHandDraw(const std::vector<BVHnode>& tree,uint32_t nodeIdx,bool is_TLAS,const glm::mat4& model=glm::mat4(1.0));
    void pipelineHZBtraverseBVH(const std::vector<BVHnode>& tree,uint32_t nodeIdx,bool is_TLAS);

    // update members accrordingly after camera's update
    void afterCameraUpdate();

private:
    bool is_init_=false;
    RenderSetting setting_;
    std::shared_ptr<Shader> sdptr_;
    std::shared_ptr<PerTriangleScanLiner> tri_scanliner_;
    Camera camera_;
    std::shared_ptr<ColorBuffer> colorbuffer_;
    std::shared_ptr<DepthBuffer> zbuffer_;
    std::shared_ptr<HZbuffer> hzb_;
    Scene scene_;
    
    glm::mat4 mat_view_;        // model to world
    glm::mat4 mat_perspective_; // world to NDC
    glm::mat4 mat_viewport_;    // NDC to screen

    AABB2d box_;                // screen aabb box

public:
    // for ui
    float delta_time_;          // time spent to render last frame; (ms)
    bool keys_[1024]={0}; 
    
    CPUTimer timer_;            // (us)

};