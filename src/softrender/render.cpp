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

// implement MVP for each Vertex
void Render::pipeModel2NDC(){
    // for each object's each vertex,transform them to vec4 and implement mvpv
    for(auto& obj:scene_.all_objects_){
        if(obj->type_==objecType::MESH){
            glm::mat4 mat_model=obj->mat_model_;
            auto& screenVertices=ndc_pos_[obj];
            for(auto& v:*(obj->getVertices())){
                // from model to Clip space
                glm::vec4 npos=mat_perspective_*mat_view_*mat_model*glm::vec4(v.position_,1.0f);
                // perspective division
                npos=npos/npos.w;
                // update _sspace
                screenVertices.push_back(glm::vec3(npos));
            }

        }
        else{// other objectype
            // TODO
        }
    }
}



// rastrization
void Render::pipeRasterizeSimple(){
// 剔除视锥体外的片元

// 进入screen空间，组装图元

// 对于屏幕空间中的每一个primitive，按照类型进行光栅化，结果写入colorBuffer

}