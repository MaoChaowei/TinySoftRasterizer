/* define all the implementation of the graph pipeline here */
#pragma once
#include"common_include.h"
#include"../camera.h"
#include"../scene_loader.h"
#include"../buffer.h"

// point in screen space. left-bottom is the origin.
struct Point2d{
    int x;
    int y;
    Point2d():x(0),y(0){}
    Point2d(glm::vec4& t){
        x=t.x;
        y=t.y;
    }
};

class Render{
public:
    Render():camera_(),colorbuffer_(camera_.getImageWidth(),camera_.getImageHeight()){}

    inline void setCamera(glm::vec3 pos,glm::vec3 lookat,glm::vec3 right,float fov=60,float ratio=16.0/9.0,int image_width=1000){
        camera_.updateCamera(pos,lookat,right);
        colorbuffer_.reSetBuffer(camera_.getImageWidth(),camera_.getImageHeight());
    }

    inline void setScene(std::string filename){
        scene_.addScene(filename);
    }

    // implement MVP for each Vertex
    void pipeModel2NDC();

    // clip and map to screen
    void pipeClip2Screen();

    // build Accelerate structure in Screen space
    // void createBVH();

     // calculate transformation from wold-space to viewport-space
    // x and y in the range of [image]^[image],z in the range of [-1,1]; 
    // once change camera property, we need this function to update VPV-matrix
    void setTransformation();


    // rastrization each frame, call setTransformation before this
    void pipelineDemo(){
        this->pipeModel2NDC();
        this->pipeClip2Screen();
    }
    // call this at the end of each frame's rendering!
    void inline cleanFrame(){
        screen_pos_.clear();
    }

    // draw line
    void drawLine( Point2d t1, Point2d t2,const glm::vec4 color);

    inline const Camera& getCamera()const{ return camera_;}
    inline const ColorBuffer& getColorBuffer()const{ return colorbuffer_;}
    inline const Scene& getScene()const{ return scene_;}

    // debug
    inline void printMatrix(const glm::mat4& mat,const std::string name)const{
        std::cout<<name<<std::endl;
        for(int i=0;i<4;++i){
            for(int j=0;j<4;++j)
                std::cout<<mat[j][i]<<" ";
            std::cout<<std::endl;
        }
    }
    inline void debugMatrix()const{
        printMatrix(mat_view_,"mat_view_");
        printMatrix(mat_perspective_,"mat_perspective_");
        printMatrix(mat_viewport_,"mat_viewport_");
    }


private:
    Camera camera_;
    ColorBuffer colorbuffer_;
    Scene scene_;
    
    glm::mat4 mat_view_;        // model to world
    glm::mat4 mat_perspective_; // world to NDC
    glm::mat4 mat_viewport_;    // NDC to screen


    // each object's position in Screen space, updated each frame
    // note that the index order of vertex should not change!
    std::unordered_map<std::shared_ptr<ObjectDesc>,std::vector<glm::vec4>> screen_pos_;

};