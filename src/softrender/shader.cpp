#include"shader.h"
#include"common/utils.h"

#define INSIDE(x,y,z,w) ((x)<(w)&&(x)>(-w))&&\
                       ((y)<(w)&&(y)>(-w))&&\
                       ((z)<(w)&&(z)>(-w))
/**
 * @brief vertex shader: convert vertex from model-space to screen-space 
 *        => x and y in [width,height], z in [-1,1];
 *        convert normals into world space
 * @param v : the vertex information holder
 */
void Shader::vertexShader(Vertex& v ){

    v.c_pos_=(*mvp_)*glm::vec4(v.pos_,1.0f);

    if(v.c_pos_.w>1e-6 && INSIDE(v.c_pos_.x,v.c_pos_.y,v.c_pos_.z,v.c_pos_.w)){
        v.discard=false;
        // record word postion and normal to shade color.
        v.w_pos_=(*model_mat_)*glm::vec4(v.pos_,1.0f);      
        v.w_norm_=(*normal_mat_)*glm::vec4(v.norm_,0);   
    }
    else{   
        // need to be clipped
        v.discard=true;
    }
}

void Shader::vertex2Screen(Vertex& v ){
    // perspective division and viewport transformation
    v.s_pos_=(*viewport_)*(v.c_pos_/v.c_pos_.w);     
}

bool Shader::fragmentInterp(uint32_t x,uint32_t y,float cur_depth){
    auto& v1=content_.v[0];
    auto& v2=content_.v[1];
    auto& v3=content_.v[2];
    /*--------------------- Interpolation ---------------------*/
    glm::vec3 bary=utils::getBaryCenter(v1->s_pos_,v2->s_pos_,v3->s_pos_,glm::vec2(x,y));
    glm::vec3 correct_bary;
    if(bary.x<0||bary.y<0||bary.z<0)
        return false;
    
    // perspective correct interpolation
    for(int i=0;i<3;++i)
        correct_bary[i]=bary[i]/content_.v[i]->c_pos_.w;

    float z_n=1.0/(correct_bary[0]+correct_bary[1]+correct_bary[2]);

    for(int i=0;i<3;++i)
        correct_bary[i]*=z_n;
    
    content_.vbary=correct_bary;
    
    // depth-interpolation
    content_.depth=glm::dot(glm::vec3(v1->s_pos_.z,v2->s_pos_.z,v3->s_pos_.z),correct_bary);
    if(checkFlag(ShaderSwitch::EarlyZtest)&&cur_depth<content_.depth)
        return false;

    // normal-interpolation.Since view matrix didn't scale the space, 
    // so I can interpolate world normals in View space~~it's the same thing as interpolating in world space
    for(int i=0;i<3;++i)
        content_.normal[i]=glm::dot(glm::vec3(v1->w_norm_[i],v2->w_norm_[i],v3->w_norm_[i]),correct_bary);
    content_.normal=glm::normalize(content_.normal);

    return true;
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
        // if(v1->discard||v2->discard||v3->discard)
        //     return false;

        bool pass=fragmentInterp(x,y,cur_depth);
        if(pass==false)
            return false;

        /*--------------------- Get materials ---------------------*/
        Material temp;  // record materials of this fragment
        if  (checkShader(ShaderType::Texture)&&material_){                                        
            auto ami=material_->getTexture(MltMember::Ambient);
            auto diff=material_->getTexture(MltMember::Diffuse);
            auto spec=material_->getTexture(MltMember::Specular);
            float u,v;
            u=glm::dot(glm::vec3(v1->uv_[0],v2->uv_[0],v3->uv_[0]),content_.vbary);
            v=glm::dot(glm::vec3(v1->uv_[1],v2->uv_[1],v3->uv_[1]),content_.vbary);

            if(diff){// get diffuse color
                temp.diffuse_= diff->getColorBilinear(u,v);
            }else{
                temp.diffuse_= (255.0f)*material_->diffuse_;
            }
            //  NOLIGHT: diffuse color is enough
            if(!checkShader(ShaderType::LIGHTSHADER)){
                content_.color=glm::vec4(temp.diffuse_,1.0f);
                return true;
            }

            if(ami){// get ambient color
                temp.ambient_= ami->getColorBilinear(u,v);
            }else{
                temp.ambient_= (255.0f)*material_->ambient_;
            }
            if(spec){// get specular color
                temp.specular_= spec->getColorBilinear(u,v);
            }else{
                temp.specular_= (255.0f)*material_->specular_;
            }
        }
        else if(checkShader(ShaderType::Color)){
            if(!checkShader(ShaderType::LIGHTSHADER)){
                for(int i=0;i<4;++i)
                    content_.color[i]=glm::dot(glm::vec3(v1->color_[i],v2->color_[i],v3->color_[i]),content_.vbary);
                return true;
            }
             
            for(int i=0;i<3;++i)
                temp.diffuse_[i]=glm::dot(glm::vec3(v1->color_[i],v2->color_[i],v3->color_[i]),content_.vbary);

            temp.ambient_=temp.diffuse_;
            temp.specular_=(0.2f)*temp.diffuse_;
        }
        else if(checkShader(ShaderType::Depth)){
            
            float dist=(2.0*far_plane_*near_plane_)/(-content_.depth*(far_plane_-near_plane_)+(far_plane_+near_plane_));        // from ndc Zn to view space -Ze(since Zn is non-linear!)
            content_.color=glm::vec4( (255.0*dist-far_plane_)/(near_plane_-far_plane_));
            return true;
        }
        else if(checkShader(ShaderType::Normal)){
            content_.color= glm::vec4((content_.normal * 0.5f + 0.5f)*255.0f, 1.0); 
            return true;
        }
        else if(checkShader(ShaderType::Light)){
            content_.color=glm::vec4(255,255,255,1);
            return true;
        }

         /*--------------------- LIGHT shading ---------------------*/
        assert(checkShader(ShaderType::LIGHTSHADER));

        content_.color=glm::vec4(0.f);
        if(checkShader(ShaderType::BlinnPhone)){
            // position in world space
            glm::vec3 fragpos;  
            for(int i=0;i<3;++i)
                fragpos[i]=glm::dot(glm::vec3(v1->w_pos_[i],v2->w_pos_[i],v3->w_pos_[i]),content_.vbary);

            // multiple lights shading by blinn-phong shader
            for(auto light:lights_){
                if(LightType::Dirction==light->type_){
                    std::shared_ptr<DirLight> ptr=std::dynamic_pointer_cast<DirLight>(light);
                    if(ptr){
                        shadeDirectLight(*ptr,content_.normal,camera_->getPosition(),fragpos,temp); 
                    }else{
                        std::cerr<<"fail to get ptr!\n";
                        exit(-1);
                    }
                }else{
                    assert(LightType::Point==light->type_);
                    std::shared_ptr<PointLight> ptr=std::dynamic_pointer_cast<PointLight>(light);
                    if(ptr){
                        shadePointLight(*ptr,content_.normal,camera_->getPosition(),fragpos,temp);
                    }else{
                        std::cerr<<"fail to get ptr!\n";
                        exit(-1);
                    }
                }
            }
            // color processing...
            for(int i=0;i<3;++i){
                content_.color[i]=std::min(255.f,content_.color[i]);
            }
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


void Shader::shadePointLight(const PointLight& light,const glm::vec3& normal,const glm::vec3& eyepos,const glm::vec3& fpos,const Material& mtl){
    // glm::vec3 n_normal=glm::normalize(normal);  
    glm::vec3 toLight=glm::vec3(light.pos_-fpos);
    float dist_squred=glm::dot(toLight,toLight);
    toLight=glm::normalize(toLight);
    glm::vec3 toEye=glm::normalize(glm::vec3(eyepos-fpos));

    glm::vec3 halfh=glm::normalize(toLight+toEye);
    float cosa=glm::dot(toLight,normal);

    float speccoef=std::pow(std::max(0.f,glm::dot(halfh,normal)),mtl.getShininess());

    glm::vec3 ambientcolor=light.ambient_*mtl.getAmbient();
    glm::vec3 diffcolor=light.diffuse_*mtl.getDiffuse()*std::max(0.f,cosa);
    glm::vec3 specolor=light.specular_*mtl.getSpecular()*speccoef;

    float dist_attenuation=light.quadratic_*dist_squred;
    float attenuation=dist_attenuation>1?1.0/dist_attenuation:1.0;

    content_.color+=glm::vec4((ambientcolor+diffcolor+specolor)*attenuation,1.0f);
}

void Shader::shadeDirectLight(const DirLight& light,const glm::vec3& normal,const glm::vec3& eyepos,const glm::vec3& fpos,const Material& mtl){
    // glm::vec3 n_normal=glm::normalize(normal);  
    glm::vec3 toLight=glm::normalize(-light.dir_);
    glm::vec3 toEye=glm::normalize(glm::vec3(eyepos-fpos));

    glm::vec3 halfh=glm::normalize(toLight+toEye);
    float cosa=glm::dot(toLight,normal);
    float speccoef=std::pow(std::max(0.f,glm::dot(halfh,normal)),mtl.getShininess());

    glm::vec3 ambientcolor=light.ambient_*mtl.getAmbient();
    glm::vec3 diffcolor=light.diffuse_*mtl.getDiffuse()*std::max(0.f,cosa);
    glm::vec3 specolor=light.specular_*mtl.getSpecular()*speccoef;


    content_.color+=glm::vec4((ambientcolor+diffcolor+specolor),1.0f);
}