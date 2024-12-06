#include"render.h"

// once change camera property, we need to update VPV-matrix accordingly
void Render::updateMatrix(){
    mat_view_=camera_.getViewMatrix();
    mat_perspective_=camera_.getPerspectiveMatrix();
    mat_viewport_=camera_.getViewportMatrix();
}

void Render::addScene(std::string filename){
    scene_.addScene(filename); 
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

// in screen space
bool Render::backCulling(const glm::vec3& face_norm,const glm::vec3& dir) const {
    return glm::dot(dir,face_norm) <= 0;
}


void Render::pipelineBegin(){
    if(is_init_==false){
        std::cout<<"Fail: didn't use `pipelineInit` to initialize.\n";
        return;
    }
    // if camera is changed, need to update view
    if(camera_.needUpdateView())
        updateViewMatrix();
    
    auto& objects=scene_.getObjects();
    for(auto& obj:objects){

        glm::mat4 mat_model=obj->getModel();        // debugMatrix();
        auto otype=obj->getPrimitiveType();
        auto& objvertices=obj->getVertices();
        auto& objindices=obj->getIndices();
        auto& objfacenorms=obj->getFaceNorms();

        // calculate all the matrix operations and send to shader
        glm::mat4 transform=mat_viewport_*mat_perspective_*mat_view_*mat_model;
        glm::mat4 normal_mat=glm::transpose(glm::inverse(mat_model));
        sdptr_->bindTransform(&transform);
        sdptr_->bindNormalMat(&normal_mat);
        sdptr_->bindModelMat(&mat_model);

        glm::vec4 npos;
        int ver_num=int(otype);
        sdptr_->setPrimitiveType(otype);

        /*----- Geometry phrase --------*/
        // for each objects' each vetices, go through VS
        for(auto& v:objvertices){
            sdptr_->vertexShader(v);
        }

        /*----- Rasterize phrase --------*/
        // for each primitive's each fragment, go through FS
        int face_cnt=0;
        for(auto it=objindices.begin();it!=objindices.end();){
            // assembly primitive
            const Vertex* v1=&objvertices[*it++];
            const Vertex* v2=&objvertices[*it++];
            const Vertex* v3=&objvertices[*it++];

            sdptr_->assemblePrimitive(v1,v2,v3);

            // back culling
            if(sdptr_->checkFlag(ShaderSwitch::BackCulling)){
                glm::vec3 norm=normal_mat*glm::vec4(objfacenorms[face_cnt++],0.f);
                glm::vec3 dir=camera_.getPosition()-v1->w_pos_;
                if(backCulling(norm,dir)==true){
                    // std::cout<<"culling:";
                    // utils::printvec3(norm,"norm");
                    continue;
                }
                // std::cout<<"showing:";
                // utils::printvec3(norm,"norm");
            }

            // render
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

void Render::afterCameraUpdate(){
    colorbuffer_.reSetBuffer(camera_.getImageWidth(),camera_.getImageHeight());
    zbuffer_.reSetBuffer(camera_.getImageWidth(),camera_.getImageHeight());
    updateMatrix();

    box_.min={0,0};
    box_.max={camera_.getImageWidth()-1,camera_.getImageHeight()-1};
}

void Render::handleKeyboardInput(int key, int action) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
        keys_[key]=true;
    else if(action==GLFW_RELEASE)
        keys_[key]=false;
}

void Render::moveCamera(){
    if (keys_[GLFW_KEY_W]) camera_.processKeyboard(CameraMovement::FORWARD,delta_time_);
    if (keys_[GLFW_KEY_S]) camera_.processKeyboard(CameraMovement::BACKWARD,delta_time_);
    if (keys_[GLFW_KEY_A]) camera_.processKeyboard(CameraMovement::LEFT,delta_time_);
    if (keys_[GLFW_KEY_D]) camera_.processKeyboard(CameraMovement::RIGHT,delta_time_);
    if (keys_[GLFW_KEY_TAB]) camera_.processKeyboard(CameraMovement::REFRESH,delta_time_);
}

void Render::handleMouseInput(double xoffset, double yoffset) {
    camera_.processMouseMovement(static_cast<float>(xoffset), static_cast<float>(yoffset));
}
