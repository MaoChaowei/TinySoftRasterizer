#include"render.h"


namespace tools{

#define IN_NDC(x) (((x)>=-1.0f)&&((x)<=1.0f))

inline bool outNDC(const glm::vec4& pos){
    return IN_NDC(pos.x)&&IN_NDC(pos.y)&&IN_NDC(pos.z);
}

// get the barycenter of goal_p in the p1-p2-p3 triangle
inline glm::vec3 getBaryCenter(Point2d p1, Point2d p2, Point2d p3, Point2d goal_p) {
    float denom = (p2.y - p3.y) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.y - p3.y);
    if (std::abs(denom) < 1e-6) {
        throw std::runtime_error("Triangle is degenerate, denominator is zero.");
    }
    float lambda1 = ((p2.y - p3.y) * (goal_p.x - p3.x) + (p3.x - p2.x) * (goal_p.y - p3.y)) / denom;
    float lambda2 = ((p3.y - p1.y) * (goal_p.x - p3.x) + (p1.x - p3.x) * (goal_p.y - p3.y)) / denom;
    float lambda3 = 1.0f - lambda1 - lambda2;

    return glm::vec3(lambda1, lambda2, lambda3);
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
            Point2d t1,t2;
            NDCVertices[0]=mat_viewport_*NDCVertices[0];
            t1=NDCVertices[0];
            for(int i=1;i<v_num;++i){
                //MUST TODO: Line Clipping
                NDCVertices[i]=mat_viewport_*NDCVertices[i];
                t2=NDCVertices[i];
                drawLine(t1,t2,glm::vec4(255));
                t1=t2;
            }
        }
    }
}


// go through all the pixels inside the AABB, that means didn't use coherence here
void Render::drawTriangle(TriangelRecord& triangle){
    AABB2d aabb;
    aabb.containTriangel(triangle.p[0],triangle.p[1],triangle.p[2]);
    aabb.clipAABB(box_);
    if(!aabb.valid)
        return;
    
    for(int y=aabb.min.y;y<=aabb.max.y;++y){
        for(int x=aabb.min.x;x<=aabb.max.x;++x){
            // get barycentric coordinate~
            glm::vec3 bary=tools::getBaryCenter(triangle.p[0],triangle.p[1],triangle.p[2],Point2d(x,y));
            if(bary.x<0||bary.y<0||bary.z<0)
                continue;
            // get color
            glm::vec4 color;
            for(int i=0;i<4;++i){
                color[i]=glm::dot(glm::vec3(triangle.v[0]->color_[i],triangle.v[1]->color_[i],
                triangle.v[2]->color_[i]),bary);
            }
            colorbuffer_.setPixel(x,y,color);
        }
    }
}

// implement MVP for each Vertex
void Render::pipeModel2Screen(){
    // for each object's each vertex,transform them to vec4 and implement mvpv
    auto& objects=scene_.getObjects();
    for(auto& obj:objects){
        glm::mat4 mat_model=obj->getModel();
        auto otype=obj->getType();
        auto& SVertices=screen_pos_[obj];
        auto& objvertices=obj->getVertices();
        auto& objindices=obj->getIndices();

        // vertex shader(MVP) => perspective division => viewport transformation
        for( auto& v:objvertices){
            // from model to Screen space
            glm::vec4 npos=mat_viewport_*mat_perspective_*mat_view_*mat_model*glm::vec4(v.pos_,1.0f);
            npos=npos/npos.w;
            // update _sspace
            SVertices.push_back(npos);
        }

        // => fragement shader
        if(otype==objecType::LINE){
            int v_num=objindices.size();
            //MUST TODO: Line Clipping
            Point2d t1,t2;
            t1=SVertices[0];
            for(int i=1;i<v_num;++i){
                t2=SVertices[i];
                drawLine(t1,t2,glm::vec4(255));
                t1=t2;
            }
        }
        else if(otype==objecType::MESH){
            int v_num=objindices.size();

            for(int i=0;i<v_num;i+=3){
                TriangelRecord triangle;
                for(int t=0;t<3;++t){
                    uint32_t idx=objindices[i+t];
                    triangle.p[t]=SVertices[idx];
                    triangle.v[t]=&objvertices[idx];
                }
                drawTriangle(triangle);
            }
        }
    }
}
