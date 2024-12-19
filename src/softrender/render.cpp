#include"render.h"

// once change camera property, we need to update VPV-matrix accordingly
void Render::updateMatrix(){
    mat_view_=camera_.getViewMatrix();
    mat_perspective_=camera_.getPerspectiveMatrix();
    mat_viewport_=camera_.getViewportMatrix();
}

void Render::addObjInstance(std::string filename,glm::mat4& model,ShaderType shader,bool flipn,bool backculling){
    scene_.addObjInstance(filename,model,shader,flipn,backculling); 
}

void Render::pipelineInit(const RenderSetting & setting){
    // init transformation
    updateMatrix();
    setting_=setting;

    // init TLAS
    scene_.buildTLAS();

    // init shader
    sdptr_=std::make_shared<Shader>();
    sdptr_->setShaderSwitch(setting.shader_switch);
    sdptr_->setFrustum(camera_.getNear(),camera_.getFar());
    sdptr_->bindTimer(&timer_);

    is_init_=true;
}

// drawLine in screen space
void Render::drawLine(glm::vec2 t1,glm::vec2 t2){
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
    glm::vec3 t[3];
    for(int i=0;i<3;++i)
        t[i]=sdptr_->getScreenPos(i);
    
    aabb.containTriangel(t[0],t[1],t[2]);
    aabb.clipAABB(box_);
    if(!aabb.valid)
        return;
    
    if(ShaderType::Frame==sdptr_->getType()){
        for(int i=0;i<3;++i){
            drawLine(t[i],t[(i+1)%3]);
        }
    }
    else{
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
}

// in screen space
bool Render::backCulling(const glm::vec3& face_norm,const glm::vec3& dir) const {
    return glm::dot(dir,face_norm) <= 0;
}

void Render::pipelineBegin(){

    if(is_init_==false){
        std::cerr<<"pipelineBegin: didn't use `pipelineInit` to initialize.\n";
        return;
    }
    // if camera is changed, need to update view
    if(camera_.needUpdateView())
        updateViewMatrix();

    // shader might need these to calculate color:
    sdptr_->bindCamera(std::make_shared<Camera>(camera_));          
    sdptr_->bindLights(scene_.getLights());

    // put each instance into the pipeline. TODO: frustrum clipping~
    auto& asinstances=scene_.getAllInstances();
    for(auto& ins:asinstances){
        auto& obj=ins.blas_->object_;
        auto& mat_model=ins.modle_;       
        auto otype=obj->getPrimitiveType();
        auto& objvertices=obj->getVertices();
        auto& objindices=obj->getIndices();
        auto& objfacenorms=obj->getFaceNorms();
        auto& objmtls=obj->getMtls();
        auto& objmtlidx=obj->getMtlIdx();

        // init shader for the current instance
        glm::mat4 mvp=mat_perspective_*mat_view_*mat_model;
        glm::mat4 normal_mat=glm::transpose(glm::inverse(mat_model));
        sdptr_->bindMVP(&mvp);
        sdptr_->bindViewport(&mat_viewport_);
        sdptr_->bindNormalMat(&normal_mat);
        sdptr_->bindModelMat(&mat_model);
        sdptr_->setShaderType(ins.shader_);
        sdptr_->setPrimitiveType(otype);

        /*----- Geometry phrase --------*/
#ifdef TIME_RECORD
        timer_.start("110.MVP");
#endif
        // MVP => clip space
        for(auto& v:objvertices){
            sdptr_->vertexShader(v);
        }

#ifdef TIME_RECORD
        timer_.stop("110.MVP");
#endif
        /*----- Rasterize phrase --------*/
        // for each primitive
#ifdef TIME_RECORD
        timer_.start("120.Rasterize phrase");
#endif
        int face_cnt=0;
        for(auto it=objindices.begin();it!=objindices.end();++face_cnt){
            // do clipping in clip space and reassemble primitives
            std::vector<Vertex> in,out;
            for(int t=0;t<3;++t){
                in.push_back(objvertices[*it++]);
            }
            int primitive_num=pipelineClipping(in,out);
            if(primitive_num==0) 
                continue;
            
            for(int t=0;t<primitive_num;++t){
                Vertex* v1=&out[t*3+0];
                Vertex* v2=&out[t*3+1];
                Vertex* v3=&out[t*3+2];
                // assembly primitive
                sdptr_->assemblePrimitive(v1,v2,v3);
                // clip space => NDC => screen space 
                sdptr_->vertex2Screen(*v1);
                sdptr_->vertex2Screen(*v2);
                sdptr_->vertex2Screen(*v3);

                // back culling
                if(sdptr_->checkFlag(ShaderSwitch::BackCulling)&&obj->isBackCulling()){
                    glm::vec3 norm=normal_mat*glm::vec4(objfacenorms[face_cnt],0.f);
                    glm::vec3 dir=camera_.getPosition()-v1->w_pos_;
                    if(backCulling(norm,dir)==true){
                        continue;
                    }
                }

                // the primitive binds the material if it has one
                int midx=objmtlidx[face_cnt];
                if(midx>=0 && midx<objmtls.size()){
                    sdptr_->bindMaterial(objmtls[midx]);
                }else{
                    sdptr_->bindMaterial(nullptr);
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
            } // end for-primitive_num

        }// end for-objindices

#ifdef TIME_RECORD
        timer_.stop("120.Rasterize phrase");
#endif

    }// end of for-asinstances

    if(setting_.show_tlas){
        showTLAS();
    }
}

void Render::showTLAS(){
    auto&tlas=scene_.getTLAS();
    auto&tlas_tree=(*tlas.tree_);
    if(tlas_tree.size()){
        traverseBVHandDraw(tlas_tree,0,true);
    }
}

void Render::showBLAS(const ASInstance& inst){
    auto&blas=inst.blas_;
    auto&blas_tree=*(blas->nodes_);
    if(blas_tree.size()){
        traverseBVHandDraw(blas_tree,0,false,inst.modle_);
    }
}

/**
 * @brief traverse through BVH from top to bottom and draw each node's AABB.
 * 
 * @param tree : the tree structure
 * @param nodeIdx : current node
 * @param is_TLAS : to support both TLAS and BLAS traversal, this on-off differentiates these two cases.
 * @param model   : specify the model matrix, which is necessary for BLAS without knowing its world position
 */
void Render::traverseBVHandDraw(const std::vector<BVHnode>& tree,uint32_t nodeIdx,bool is_TLAS,const glm::mat4& model){
    if(nodeIdx>=tree.size()){
        std::cerr<<"Render::traverseBVHandDraw:nodeIdx>=tree.size()!\n";
        exit(-1);
    }
    auto& node=tree[nodeIdx];

    glm::vec3 bmin=node.bbox.min;
    glm::vec3 bmax=node.bbox.max;
    glm::vec3 bcenter=(bmin+bmax)*0.5f;
    std::vector<glm::vec3> bboxpoints={bmin,{bmin.x,bmin.y,bmax.z},{bmin.x,bmax.y,bmax.z},{bmin.x,bmax.y,bmin.z},
                        {bmax.x,bmin.y,bmin.z},{bmax.x,bmin.y,bmax.z},                 bmax,{bmax.x,bmax.y,bmin.z}};
    // transform p*v=>/z=>viewport
    bool valid[12]={true};
    glm::mat4 trans=is_TLAS?(mat_perspective_*mat_view_):(mat_perspective_*mat_view_*model);

    bcenter=trans*glm::vec4(bcenter,1.0f);

    for(int i=0;i<bboxpoints.size();++i){
        auto& p=bboxpoints[i];
        glm::vec4 pos=trans*glm::vec4(p,1.0f);
        int bias[3]={0};
        for(int t=0;t<3;++t){
            if(pos[t]<bias[t]) pos[t]-=1e-3;
            else pos[t]+=1e-3;
        }

        if(pos.w>1e-6 ){  
            pos/=pos.w;
            pos=mat_viewport_*pos;
            p=glm::vec3(pos);
            valid[i]=true;
        }
        else{
            valid[i]=false;
        }
    }
    // draw 12 lines
    glm::vec4 color=is_TLAS?glm::vec4(255.0):glm::vec4(0.0,100.0,100.0,1.0);
    for(int i=0;i<4;++i){
        if(valid[i]&&valid[(i+1)%4])
            drawLine3d(bboxpoints[i],bboxpoints[(i+1)%4],color);
        if(valid[i]&&valid[i+4])
            drawLine3d(bboxpoints[i],bboxpoints[i+4],color);
        if(valid[i+4]&&valid[(i+1)%4+4])
            drawLine3d(bboxpoints[i+4],bboxpoints[(i+1)%4+4],color);
    }

    // dive into blas if possible and required
    if(is_TLAS&&node.left==-1&&node.right==-1){
        if(setting_.show_blas){
            auto&tlas=scene_.getTLAS();
            uint32_t idx=node.prmitive_start;
            showBLAS(tlas.all_instances_[idx]);
        }
    }
    
    if(node.left!=-1)
        traverseBVHandDraw(tree,node.left,is_TLAS,model);
    if(node.right!=-1)
        traverseBVHandDraw(tree,node.right,is_TLAS,model);
}

// drawLine in screen space
void Render::drawLine3d(glm::vec3 t1,glm::vec3 t2,const glm::vec4& color) {
    // naive clip: create a aabb2d to intersect with the screen 
    AABB2d aabb;
    aabb.containLine(glm::vec2(t1),glm::vec2(t2));
    aabb.clipAABB(box_);

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
        float dt=(x-t1.x)/float(dx);
        float curz=(1-dt)*t1.z+dt*(t2.z);
        if(swap_flag){
            if(x<=aabb.max.y&&x>=aabb.min.y&&y<=aabb.max.x&&y>=aabb.min.x){
                float curdepth=zbuffer_.getDepth(y,x);
                if(curz<=curdepth+10*srender::EPSILON){
                    colorbuffer_.setPixel(y,x,color);
                    zbuffer_.setDepth(y,x,curz);
                }
            }
        }else{
            if(y<=aabb.max.y&&y>=aabb.min.y&&x<=aabb.max.x&&x>=aabb.min.x){
                float curdepth=zbuffer_.getDepth(x,y);
                if(curz<=curdepth+10*srender::EPSILON){
                    colorbuffer_.setPixel(x,y,color);
                    zbuffer_.setDepth(x,y,curz);
                }
            }
        }
        error2+=delta2;
        if(error2>dx){
            y+=positive_flag?1:-1;
            error2-=dx*2;
        }
    }
}


namespace ClipTools{
// Defines a bit flag for the clipping plane
enum ClipPlaneBit {
    CLIP_LEFT   = 1 << 0, // 000001
    CLIP_RIGHT  = 1 << 1, // 000010
    CLIP_BOTTOM = 1 << 2, // 000100
    CLIP_TOP    = 1 << 3, // 001000
    CLIP_NEAR   = 1 << 4, // 010000
    CLIP_FAR    = 1 << 5  // 100000
};

int computeOutcode(const glm::vec4& pos) {
    int outcode = 0;
    if (pos.x < -pos.w) outcode |= CLIP_LEFT;
    if (pos.x > pos.w)  outcode |= CLIP_RIGHT;
    if (pos.y < -pos.w) outcode |= CLIP_BOTTOM;
    if (pos.y > pos.w)  outcode |= CLIP_TOP;
    if (pos.z < -pos.w) outcode |= CLIP_NEAR;
    if (pos.z > pos.w)  outcode |= CLIP_FAR;
    return outcode;
}

bool isInside(const Vertex& vertex, ClipPlane plane) {
    const glm::vec4& pos = vertex.c_pos_;
    switch (plane) {
        case ClipPlane::Left:
            return pos.x >= -pos.w;
        case ClipPlane::Right:
            return pos.x <= pos.w;
        case ClipPlane::Bottom:
            return pos.y >= -pos.w;
        case ClipPlane::Top:
            return pos.y <= pos.w;
        case ClipPlane::Near:
            return pos.z >= -pos.w;
        case ClipPlane::Far:
            return pos.z <= pos.w;
        default:
            return false;
    }
}

Vertex computeIntersection(const Vertex& v1, const Vertex& v2, ClipPlane plane) {
    float A, B, C, D;
    switch (plane) {
        case ClipPlane::Left:
            A = 1.0f; B = 0.0f; C = 0.0f; D = 1.0f;
            break;
        case ClipPlane::Right:
            A = -1.0f; B = 0.0f; C = 0.0f; D = 1.0f;
            break;
        case ClipPlane::Bottom:
            A = 0.0f; B = 1.0f; C = 0.0f; D = 1.0f;
            break;
        case ClipPlane::Top:
            A = 0.0f; B = -1.0f; C = 0.0f; D = 1.0f;
            break;
        case ClipPlane::Near:
            A = 0.0f; B = 0.0f; C = 1.0f; D = 1.0f;
            break;
        case ClipPlane::Far:
            A = 0.0f; B = 0.0f; C = -1.0f; D = 1.0f;
            break;
        default:
            A = B = C = D = 0.0f;
    }

    float startVal = A * v1.c_pos_.x + B * v1.c_pos_.y + C * v1.c_pos_.z + D * v1.c_pos_.w;
    float endVal = A * v2.c_pos_.x + B * v2.c_pos_.y + C * v2.c_pos_.z + D * v2.c_pos_.w;

    // denom check
    if (fabs(startVal - endVal) < 1e-6f) {
        return v1; 
    }

    float t = startVal / (startVal - endVal);
    t = glm::clamp(t, 0.0f, 1.0f);
    return v1.vertexInterp(v2, t);
}
}


void Render::clipWithPlane(ClipPlane plane, std::vector<Vertex>& in, std::vector<Vertex>& out) {
    if (in.empty()) return;

    std::vector<Vertex> result;
    size_t vnum = in.size();

    for (size_t i = 0; i < vnum; ++i) {
        size_t next = (i + 1) % vnum;
        const Vertex& current = in[i];
        const Vertex& nextPos = in[next];

        bool currentInside = ClipTools::isInside(current, plane);
        bool nextInside = ClipTools::isInside(nextPos, plane);

        if (currentInside && nextInside) {
            // Case 1: Both inside
            result.push_back(nextPos);
        }
        else if (currentInside && !nextInside) {
            // Case 2: Current inside, next outside
            Vertex intersectVertex = ClipTools::computeIntersection(current, nextPos, plane);
            result.push_back(intersectVertex);
        }
        else if (!currentInside && nextInside) {
            // Case 3: Current outside, next inside
            Vertex intersectVertex = ClipTools::computeIntersection(current, nextPos, plane);
            result.push_back(intersectVertex);
            result.push_back(nextPos);
        }
        // Case 4: Both outside - do nothing
    }

    out = std::move(result);
}

// return the number of triangles after clipping
int Render::pipelineClipping(std::vector<Vertex>& vertices, std::vector<Vertex>& out) {
    if (vertices.size() != 3) {
        return 0;
    }

    // get outcode
    int outcode1 = ClipTools::computeOutcode(vertices[0].c_pos_);
    int outcode2 = ClipTools::computeOutcode(vertices[1].c_pos_);
    int outcode3 = ClipTools::computeOutcode(vertices[2].c_pos_);

    int outcode_OR = outcode1 | outcode2 | outcode3;
    int outcode_AND = outcode1 & outcode2 & outcode3;

    // rapid reject
    if (outcode_AND != 0) {
        return 0; // the triangle is totally outside
    }

    // rapid accept
    if (outcode_OR == 0) {
        out = vertices;
        return 1;
    }

    // clip
    std::vector<Vertex> input = vertices;
    std::vector<Vertex> temp;

    // define order
    std::vector<ClipPlane> planes = {
        ClipPlane::Left,
        ClipPlane::Right,
        ClipPlane::Bottom,
        ClipPlane::Top,
        ClipPlane::Near,
        ClipPlane::Far
    };

    for (const auto& plane : planes) {
        temp.clear();
        clipWithPlane(plane, input, temp);
        input = std::move(temp);

        if (input.empty()) {
            out.clear();
            return 0;
        }
    }


    int vnum = input.size();
    out.clear();
    if (vnum < 3) return 0;

    for (int i = 1; i < vnum - 1; ++i) {
        out.push_back(input[0]);
        out.push_back(input[i]);
        out.push_back(input[i + 1]);
    }

    return out.size() / 3;
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
