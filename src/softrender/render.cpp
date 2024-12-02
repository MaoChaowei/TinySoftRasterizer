#include"render.h"


namespace tools{
#define IN_NDC(x) (((x)>=-1.0f)&&((x)<=1.0f))

    inline bool outNDC(const glm::vec4& pos){
        return IN_NDC(pos.x)&&IN_NDC(pos.y)&&IN_NDC(pos.z);
    }

}

void Render::setTransformation(){

    mat_view_=camera_.getViewMatrix();
    mat_perspective_=camera_.getPerspectiveMatrix();
    mat_viewport_=camera_.getViewportMatrix();

}

// TODO: make sure points are legal
void Render::drawLine( Point2d t1, Point2d t2,const glm::vec4 color){
    // make sure: x-axis is less steep and t1 is the left point
    bool swap_flag=0;
    if(std::abs(t1.x-t2.x)<std::abs(t1.y-t2.y)){
        std::swap(t1.x,t1.y);
        std::swap(t2.x,t2.y);
        swap_flag=1;
    }
    if(t1.x>t2.x){
        std::swap(t1,t2);
    }
    bool positive_flag=1;
    int dx=t2.x-t1.x;
    int dy=t2.y-t1.y;
    if(dy<0){
        dy=-dy;
        positive_flag=0;
    }

    int delta2=dy*2;
    int error2=0;
    int y=t1.y;
    for(int x=t1.x;x<=t2.x;++x){
        // TODO: interpolate the color if need
        if(swap_flag){
            colorbuffer_.setPixel(y,x,color);
        }else{
            colorbuffer_.setPixel(x,y,color);
        }
        error2+=delta2;
        if(error2>dx){
            y+=positive_flag?1:-1;
            error2-=dx*2;
        }
    }
}


// implement MVP for each Vertex
void Render::pipeModel2NDC(){
    debugMatrix();
    // for each object's each vertex,transform them to vec4 and implement mvpv
    auto& objects=scene_.getObjects();
    for(auto& obj:objects){
        glm::mat4 mat_model=obj->getModel();
        auto& NDCVertices=screen_pos_[obj];

        for( auto& v:obj->getVertices()){
            // from model to Clip space
            glm::vec4 npos=mat_perspective_*mat_view_*mat_model*glm::vec4(v.pos_,1.0f);
            // perspective division into NDC
            npos=npos/npos.w;
            // update _sspace
            NDCVertices.push_back(npos);
        }
    
    }
}

void Render::pipeClip2Screen(){
    auto& objects=scene_.getObjects();
    for(auto& obj:objects){
        auto otype=obj->getType();
        auto& NDCVertices=screen_pos_[obj];

        if(otype==objecType::LINE){
            int v_num=NDCVertices.size();
            // TODO: Clipping

            Point2d t1,t2;
            NDCVertices[0]=mat_viewport_*NDCVertices[0];
            t1=NDCVertices[0];
            for(int i=1;i<v_num;++i){
                NDCVertices[i]=mat_viewport_*NDCVertices[i];
                t2=NDCVertices[i];
                drawLine(t1,t2,glm::vec4(255));
                t1=t2;
            }
        }
    }
}

