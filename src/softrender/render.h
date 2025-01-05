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
    bool bvh_hzb=false;
    bool easy_hzb=false;
    bool scan_convert=false;

    bool back_culling=true;
    bool earlyz_test=true;
    bool profile_report=true;
};

class Render{
public:
    Render():camera_(),colorbuffer_(std::make_shared<ColorBuffer>(camera_.getImageWidth(),camera_.getImageHeight())),
            zbuffer_(std::make_shared<DepthBuffer>(camera_.getImageWidth(),camera_.getImageHeight())){
        box2d_.min={0,0};
        box2d_.max={camera_.getImageWidth()-1,camera_.getImageHeight()-1};
        box3d_.min={0,0,-1};
        box3d_.max={camera_.getImageWidth()-1,camera_.getImageHeight()-1,1};
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
    void setBVHLeafSize(uint32_t num){scene_.setBVHsize(num);}
    void addObjInstance(std::string filename,glm::mat4& model,ShaderType shader,bool flipn=false,bool backculling=true);

    inline void setDeltaTime(float t){delta_time_=t;}

    void updateMatrix();
    inline void updateViewMatrix(){ mat_view_=camera_.getViewMatrix(); }

    inline void cleanFrame(){
        colorbuffer_->clear();
        zbuffer_->clear();
        if(setting_.easy_hzb||setting_.bvh_hzb)
            hzb_->clear();
        
        if(setting_.profile_report){
            total_face_num_=scene_.getFaceNum();
            shaded_face_num_=0;
            back_culled_face_num_=0;
            hzb_culled_face_num_=0;
            clipped_face_num_=0;
        }
    }

    inline  Camera& getCamera(){ return camera_;}
    inline const ColorBuffer& getColorBuffer()const{ return *colorbuffer_;}
    inline const Scene& getScene()const{ return scene_;}

    // PIPELINE
    void pipelineInit(const RenderSetting & setting=RenderSetting());
    void pipelineBegin();

    void pipelineGeometryPhase();

    void pipelineNaive();
    void pipelineRasterizePhasePerInstance();

    void pipelineHZB_BVH();
    void pipelineRasterizePhaseHZB_BVH();
    
    void cullingTriangleInstance(ASInstance& instance,const glm::mat4 normal_mat);

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
    void printProfile();


private:
    // rasterize
    void drawPoint(const glm::vec2 p, float radius,const glm::vec4 color);
    void drawLine(glm::vec2 t1,glm::vec2 t2);
    void drawLine3d(glm::vec3 t1,glm::vec3 t2,const glm::vec4& color=glm::vec4(255));
    void drawTriangleNaive();
    void drawTriangleHZB();
    void drawTriangleScanLine();

    void traverseBVHandDraw(const std::vector<BVHnode>& tree,uint32_t nodeIdx,bool is_TLAS,const glm::mat4& model=glm::mat4(1.0));
    void DfsTlas_BVHwithHZB(const std::vector<BVHnode>& tree,const std::vector<ASInstance>& instances,uint32_t nodeIdx);
    void DfsBlas_BVHwithHZB(const ASInstance& inst,int32_t nodeIdx);
    // bool mapWorldBox2ScreenBox(const BVHnode& node,const glm::mat4& vp,AABB3d& sbox);

    // update members accrordingly after camera's update
    void afterCameraUpdate();

private:
    bool is_init_=false;
    RenderSetting setting_;
    std::shared_ptr<Shader> sdptr_;
    std::shared_ptr<ScanLine::PerTriangleScanLiner> tri_scanliner_;
    Camera camera_;
    std::shared_ptr<ColorBuffer> colorbuffer_;
    std::shared_ptr<DepthBuffer> zbuffer_;
    std::shared_ptr<HZbuffer> hzb_;
    Scene scene_;
    
    glm::mat4 mat_view_;        // model to world
    glm::mat4 mat_perspective_; // world to NDC
    glm::mat4 mat_viewport_;    // NDC to screen

    AABB2d box2d_;                // screen aabb box
    AABB3d box3d_;              

public:
    // for ui
    float delta_time_;          // time spent to render last frame; (ms)
    bool keys_[1024]={0}; 
    
    CPUTimer timer_;            // (us)

    int total_face_num_=0;
    int shaded_face_num_=0;
    int back_culled_face_num_=0;
    int clipped_face_num_=0;
    int hzb_culled_face_num_=0;

};