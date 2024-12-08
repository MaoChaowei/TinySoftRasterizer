#include"shader.h"
#include"common/utils.h"

/**
 * @brief vertex shader: convert vertex from model-space to screen-space 
 *        => x and y in [width,height], z in [-1,1];
 *        convert normals into world space
 * @param v : the vertex information holder
 */
void Shader::vertexShader(Vertex& v ){
    glm::vec4 npos=(*transform_)*glm::vec4(v.pos_,1.0f);
    if(npos.w>0){
        v.c_pos_w=npos.w;
        v.s_pos_=npos/npos.w;

        v.w_pos_=(*model_mat_)*glm::vec4(v.pos_,1.0f);
        v.w_norm_=(*normal_mat_)*glm::vec4(v.norm_,0);
        v.discard=false;
    }
    else{   // need to be clipped
        v.discard=true;
    }
}

/**
 * @brief shading the fragment with properties set in `ShaderType`.
 * 
 * @param xy : refer to the position in screen-space
 * @param cur_depth : refer to the current z-buffer(x,y) so that I can finish Early-Z Test here.
 * @return if the fragment can be accepted for now, than return `true`, ortherwise return `false`.
 */
bool Shader::fragmentShader(uint32_t x,uint32_t y,float cur_depth){
    auto& otype=content_.primitive_type;
    auto& v1=content_.v[0];
    auto& v2=content_.v[1];
    auto& v3=content_.v[2];

    if(otype==PrimitiveType::MESH){
        // TODO: clipping
        if(v1->discard||v2->discard||v3->discard)
            return false;

        // getBaryCenter
        glm::vec3 bary=utils::getBaryCenter(v1->s_pos_,v2->s_pos_,v3->s_pos_,glm::vec2(x,y));
        glm::vec3 correct_bary;
        if(bary.x<0||bary.y<0||bary.z<0)
            return false;
        
        // perspective correct interpolation
        for(int i=0;i<3;++i)
            correct_bary[i]=bary[i]/content_.v[i]->c_pos_w;

        float z_n=1.0/(correct_bary[0]+correct_bary[1]+correct_bary[2]);

        for(int i=0;i<3;++i)
            correct_bary[i]*=z_n;

        // depth
        content_.depth=glm::dot(glm::vec3(v1->s_pos_.z,v2->s_pos_.z,v3->s_pos_.z),correct_bary);
        if(checkFlag(ShaderSwitch::EarlyZtest)&&cur_depth<content_.depth)
            return false;

        // normal , has a bug here..
        for(int i=0;i<3;++i)
            content_.normal[i]=glm::dot(glm::vec3(v1->w_norm_[i],v2->w_norm_[i],v3->w_norm_[i]),correct_bary);

        // shading
        if   (checkShader(ShaderType::Texture)&&material_){
            auto ami=material_->getTexture(MltMember::Ambient);
            auto diff=material_->getTexture(MltMember::Diffuse);
            auto spec=material_->getTexture(MltMember::Specular);
            float u,v;
            u=glm::dot(glm::vec3(v1->uv_[0],v2->uv_[0],v3->uv_[0]),correct_bary);
            v=glm::dot(glm::vec3(v1->uv_[1],v2->uv_[1],v3->uv_[1]),correct_bary);
            if(ami){
                ami->getColorBilinear(u,v, content_.color);
                return true;
            }
            else if(diff){
                diff->getColorBilinear(u,v, content_.color);
                return true;
            }
            else if(spec){
                spec->getColorBilinear(u,v, content_.color);
                return true;
            }
        }
        if(checkShader(ShaderType::Color)){
            for(int i=0;i<4;++i)
                content_.color[i]=glm::dot(glm::vec3(v1->color_[i],v2->color_[i],v3->color_[i]),correct_bary);
            return true;
        }
        if(checkShader(ShaderType::Depth)){
            // from ndc Zn to view space -Ze(since Zn is non-linear!), that is dist below
            float dist=(2.0*far_plane_*near_plane_)/(-content_.depth*(far_plane_-near_plane_)+(far_plane_+near_plane_));
            content_.color=glm::vec4( (255.0*dist-far_plane_)/(near_plane_-far_plane_));
            return true;
        }
        if(checkShader(ShaderType::Normal)){
            content_.color=glm::vec4(content_.normal[2]*255.0f);
            return true;
        }
        else{
            content_.color=glm::vec4(0,0,155,1);
            return true;
        }
        
        return true;
    }
    else if(otype==PrimitiveType::LINE){
        std::cout<<"fragmentShader: I haven't supported LINE rendering yet~\n";
    }
    else{
        std::cout<<"fragmentShader:Actually, I haven't supported anything except MESH rendering...\n";
    }
    
    return true;
}

