#include"render.h"


void Render::updateMatrix(){
    mat_view_=camera_.getViewMatrix();
    mat_perspective_=camera_.getPerspectiveMatrix();
    mat_viewport_=camera_.getViewportMatrix();
}

void Render::pipelineInit(const RenderSetting & setting){
    // init transformation
    updateMatrix();
    setting_=setting;

    // init shader
    sdptr_=std::make_shared<Shader>();
    sdptr_->setShaderSetting(setting.shader_setting);

    is_init_=true;
}

// TODO: make sure points are legal
void Render::drawLine(){
    glm::vec2 t1=sdptr_->getPoint2d(0);
    glm::vec2 t2=sdptr_->getPoint2d(1);
    glm::vec4 color(255.0f);
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


// go through all the pixels inside the AABB, that means I didn't use coherence here
void Render::drawTriangle(){
    AABB2d aabb;
    aabb.containTriangel(sdptr_->getPoint2d(0),sdptr_->getPoint2d(1),sdptr_->getPoint2d(2));
    aabb.clipAABB(box_);
    if(!aabb.valid)
        return;
    
    for(int y=aabb.min.y;y<=aabb.max.y;++y){
        for(int x=aabb.min.x;x<=aabb.max.x;++x){
            bool passZtest=sdptr_->fragmentShader(x,y,zbuffer_.getDepth(x,y));
            if(passZtest){
                colorbuffer_.setPixel(x,y,sdptr_->getColor());
                zbuffer_.setDepth(x,y,sdptr_->getDepth());
            }
        }
    }
}



void Render::pipelineBegin(){
    if(is_init_==false){
        std::cout<<"Fail: didn't use `pipelineInit` to initialize.\n";
        return;
    }
    // for each object's each vertex
    auto& objects=scene_.getObjects();
    for(auto& obj:objects){
        sdptr_->clear();                            // clean the context of the shader

        glm::mat4 mat_model=obj->getModel();        // debugMatrix();
        auto otype=obj->getPrimitiveType();
        auto& objvertices=obj->getVertices();
        auto& objindices=obj->getIndices();

        // calculate all the matrix operations and send to shader
        glm::mat4 transform=mat_viewport_*mat_perspective_*mat_view_*mat_model;
        glm::mat4 normal_mat=glm::transpose(glm::inverse(mat_model));
        sdptr_->bindTransform(&transform);
        sdptr_->bindNormalMat(&normal_mat);

        glm::vec4 npos;
        int ver_num=int(otype);
        sdptr_->setPrimitiveType(otype);

        for(auto it=objindices.begin();it!=objindices.end();){
            /*----- Geometry phrase --------*/
            for(int i=0;i<ver_num;++i){
                // vertex shader(MVP) => perspective division => viewport transformation
                sdptr_->vertexShader(i,objvertices[*it]);
                ++it;
            }
            // back culling
            if(sdptr_->checkFlag(ShaderSwitch::BackCulling)){
                
            }

            /*----- Rasterize phrase --------*/
            switch(otype){
                case PrimitiveType::LINE:
                    // drawLine();
                    break;
                case PrimitiveType::MESH:
                    drawTriangle();
                    break;
                default:
                    break;
            }

        }
    }
}

/*
// implement MVP for each Vertex
void Render::pipeModel2Screen(){
    // for each object's each vertex,transform them to vec4 and implement mvpv
    auto& objects=scene_.getObjects();
    for(auto& obj:objects){
        glm::mat4 mat_model=obj->getModel();
        // debugMatrix();
        auto& SVertices=screen_pos_[obj];
        auto& objvertices=obj->getVertices();

        // vertex shader(MVP) => perspective division => viewport transformation
        for( auto& v:objvertices){
            // from model to Screen space, note Z is in the range of [0,1]
            glm::vec4 npos=mat_viewport_*mat_perspective_*mat_view_*mat_model*glm::vec4(v.pos_,1.0f);
            npos=npos/npos.w;
            // update _sspace
            SVertices.push_back(npos);
        }
    }
}

void Render::pipeFragmentShader(){
    // assemble primitives and shade them
    auto& objects=scene_.getObjects();
    for(auto& obj:objects){
        auto otype=obj->getPrimitiveType();
        auto& SVertices=screen_pos_[obj];
        auto& objvertices=obj->getVertices();
        auto& objindices=obj->getIndices();

        // => fragement shader
        if(otype==PrimitiveType::LINE){
            int v_num=objindices.size();
            //MUST TODO: Line Clipping
            glm::vec2 t1,t2;
            t1=SVertices[0];
            for(int i=1;i<v_num;++i){
                t2=SVertices[i];
                drawLine(t1,t2,glm::vec4(255));
                t1=t2;
            }
        }
        else if(otype==PrimitiveType::MESH){
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

*/



// go through all the pixels inside the AABB, that means didn't use coherence here
/*void Render::drawTriangle(TriangelRecord& triangle){
    AABB2d aabb;
    aabb.containTriangel(triangle.p[0],triangle.p[1],triangle.p[2]);
    aabb.clipAABB(box_);
    if(!aabb.valid)
        return;
    
    for(int y=aabb.min.y;y<=aabb.max.y;++y){
        for(int x=aabb.min.x;x<=aabb.max.x;++x){
            // get barycentric coordinate~
            glm::vec3 bary=utils::getBaryCenter(triangle.p[0],triangle.p[1],triangle.p[2],glm::vec2(x,y));
            if(bary.x<0||bary.y<0||bary.z<0)
                continue;

            // depth test
            float depth=glm::dot(glm::vec3(triangle.p[0].z,triangle.p[1].z,triangle.p[2].z),bary);
            if(zbuffer_.zTest(x,y,depth)==false)
                continue;

            // get uv or color
            if(setting_.shader_type==ShaderType::Ordinary){
                if(triangle.v[0]->uv_[0]>=0){
                    glm::vec2 uv;
                    for(int i=0;i<2;++i){
                        uv[i]=glm::dot(glm::vec3(triangle.v[0]->uv_[i],triangle.v[1]->uv_[i],
                        triangle.v[2]->uv_[i]),bary);
                    }
                    // glm::vec4 color=;
                    // colorbuffer_.setPixel(x,y,color);
                }
                else if(triangle.v[0]->color_[0]>=0){
                    glm::vec4 color;
                    for(int i=0;i<4;++i){
                        color[i]=glm::dot(glm::vec3(triangle.v[0]->color_[i],triangle.v[1]->color_[i],
                        triangle.v[2]->color_[i]),bary);
                    }
                    colorbuffer_.setPixel(x,y,color);
                }
                else{   
                    // default color : White  
                    colorbuffer_.setPixel(x,y,glm::vec4(255.0,255.0,255.0,1));
                }
            }
            else if(setting_.shader_type==ShaderType::Depth){
                    colorbuffer_.setPixel(x,y,glm::vec4(depth*255.f,depth*255.f,depth*255.f,1));
            }
            else{
                std::cout<<"unknown `setting_.shader_type`...tobedone\n";
            }
        }
    }
}*/
