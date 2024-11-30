/* define all the implementation of the graph pipeline here */
#pragma once
#include"common_include.h"
#include"../camera.h"
#include"../scene_loader.h"
#include"../buffer.h"

class Render{
public:
    Render():camera_(),colorbuffer_(camera_.getImageWidth(),camera_.getImageHeight()){}

    inline void setCamera(glm::vec3 pos,glm::vec3 lookat,glm::vec3 right,float fov=60,float ratio=16.0/9.0,int image_width=1000){
        camera_.updateCamera(pos,lookat,right);
        colorbuffer_.reSetBuffer(camera_.getImageWidth(),camera_.getImageHeight());
    }

    // calculate transformation from wold-space to viewport-space
    // x and y in the range of [image]^[image],z in the range of [-1,1]; 
    void setTransformation();

    // implement MVP for each Vertex
    void pipeModel2NDC();

    // build Accelerate structure in Screen space
    // void createBVH();

    // rastrization
    void pipeRasterizeSimple();




private:
    Camera camera_;
    ColorBuffer colorbuffer_;
    
    glm::mat4 mat_view_;        // model to world
    glm::mat4 mat_perspective_; // world to NDC
    glm::mat4 mat_viewport_;    // NDC to screen
    Scene scene_;


    // each object's position in Screen space, updated each frame
    // note that the index order of vertex should not change!
    std::unordered_map<std::shared_ptr<ObjectDesc>,std::vector<glm::vec3>> ndc_pos_;

};