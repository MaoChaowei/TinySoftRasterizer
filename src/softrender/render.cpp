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
    // 0. init transformation
    updateMatrix();
    setting_=setting;

    // 1. init scene as
    scene_.buildTLAS();

    // 2. init shader
    sdptr_=std::make_shared<Shader>();
    sdptr_->setFrustum(camera_.getNear(),camera_.getFar());
    sdptr_->bindTimer(&timer_);

    // 3. init rastertizer
    tri_scanliner_=std::make_shared<PerTriangleScanLiner>();
    tri_scanliner_->initialize(box_,colorbuffer_,zbuffer_,sdptr_);

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
            colorbuffer_->setPixel(y,x,color);
        }else{
            colorbuffer_->setPixel(x,y,color);
        }
        error2+=delta2;
        if(error2>dx){
            y+=positive_flag?1:-1;
            error2-=dx*2;
        }
    }
}


// go through all the pixels inside the AABB, that means I didn't use coherence here
void Render::drawTriangleNaive(){
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
                if(depthTest(x,y)){
                    sdptr_->fragmentShader(x,y);
                    colorbuffer_->setPixel(x,y,sdptr_->getColor());
                    zbuffer_->setDepth(x,y,sdptr_->getDepth());
                }
            }
        }
    }
}

bool Render::depthTest(uint32_t x,uint32_t y){
    float depth=sdptr_->fragmentDepth(x,y);
    if(setting_.hzb_flag==true){
        exit(-1);
    }
    else{
        if(depth<zbuffer_->getDepth(x,y))
            return true;
        
        return false;
    }
}

void Render::drawTriangleScanLine(){
    if(ShaderType::Frame==sdptr_->getType()){
        AABB2d aabb;
        glm::vec3 t[3];
        for(int i=0;i<3;++i)
            t[i]=sdptr_->getScreenPos(i);
        
        aabb.containTriangel(t[0],t[1],t[2]);
        aabb.clipAABB(box_);
        if(!aabb.valid)
            return;

        for(int i=0;i<3;++i){
            drawLine(t[i],t[(i+1)%3]);
        }
    }
    else{
        auto v0=sdptr_->getVertices(0);
        auto v1=sdptr_->getVertices(1);
        auto v2=sdptr_->getVertices(2);
        std::vector<const Vertex*> vs={v0,v1,v2};
        tri_scanliner_->scanConvert(vs);
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
                if(setting_.back_culling&&obj->isBackCulling()){
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
                        if(setting_.scan_convert)
                            drawTriangleScanLine();
                        else
                            drawTriangleNaive();
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
    }else if(setting_.show_blas){
        for(auto& ins:asinstances)
            showBLAS(ins);
    }
}

void Render::pipelineHZBtraverseBVH(const std::vector<BVHnode>& tree,uint32_t nodeIdx,bool is_TLAS){

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
    if(setting_.show_tlas&&is_TLAS||setting_.show_blas&&!is_TLAS){
        for(int i=0;i<4;++i){
            if(valid[i]&&valid[(i+1)%4])
                drawLine3d(bboxpoints[i],bboxpoints[(i+1)%4],color);
            if(valid[i]&&valid[i+4])
                drawLine3d(bboxpoints[i],bboxpoints[i+4],color);
            if(valid[i+4]&&valid[(i+1)%4+4])
                drawLine3d(bboxpoints[i+4],bboxpoints[(i+1)%4+4],color);
        }
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
                float curdepth=zbuffer_->getDepth(y,x);
                if(curz<=curdepth+10*srender::EPSILON){
                    colorbuffer_->setPixel(y,x,color);
                    zbuffer_->setDepth(y,x,curz);
                }
            }
        }else{
            if(y<=aabb.max.y&&y>=aabb.min.y&&x<=aabb.max.x&&x>=aabb.min.x){
                float curdepth=zbuffer_->getDepth(x,y);
                if(curz<=curdepth+10*srender::EPSILON){
                    colorbuffer_->setPixel(x,y,color);
                    zbuffer_->setDepth(x,y,curz);
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

// drawPoint in screen space with specified radius
void Render::drawPoint(const glm::vec2 p, float radius,const glm::vec4 color){
    int center_x = static_cast<int>(std::round(p.x));
    int center_y = static_cast<int>(std::round(p.y));

    float radius_sq = radius * radius;
    glm::vec2 minp=p-radius;
    glm::vec2 maxp=p+radius;
    AABB2d box;
    box.containLine(minp,maxp);
    box.clipAABB(box_);

    for(int y = box.min.y; y <= box.max.y; ++y){
        for(int x = box.min.x; x <= box.max.x; ++x){

            float dx = static_cast<float>(x) - p.x;
            float dy = static_cast<float>(y) - p.y;
            float distance_sq = dx * dx + dy * dy;

            if(distance_sq <= radius_sq){
                colorbuffer_->setPixel(x, y, color);
            }
        }
    }
}

void Render::afterCameraUpdate(){
    colorbuffer_->reSetBuffer(camera_.getImageWidth(),camera_.getImageHeight());
    zbuffer_->reSetBuffer(camera_.getImageWidth(),camera_.getImageHeight());
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


/*

void Render::scanLineConvert(std::vector<const Vertex*> vertices,std::vector<FragmentHolder>& edge_points){
    assert(vertices.size()==3);

    std::sort(vertices.begin(),vertices.end(),[this](const Vertex*& v1,const Vertex*& v2){
        return (int)v1->s_pos_.y<(int)v2->s_pos_.y;
    });

    auto v1=vertices[0];
    auto v2=vertices[1];
    auto v3=vertices[2];
    int y1=(int)v1->s_pos_.y;
    int y2=(int)v2->s_pos_.y;
    int y3=(int)v3->s_pos_.y;
    
    // each endpoint was added only once
    if(y1<y2&&y2<y3){
        rasterizeEdge(v1,v2,true,false,edge_points);
        rasterizeEdge(v2,v3,true,true,edge_points);
        rasterizeEdge(v3,v1,true,true,edge_points);
    }
    else if(y1==y2&&y1==y3){
        return;
    }
    else if(y1==y2){
        rasterizeEdge(v2,v3,true,true,edge_points);
        rasterizeEdge(v3,v1,true,true,edge_points);
    }
    else if(y2==y3){
        rasterizeEdge(v1,v2,true,true,edge_points);
        rasterizeEdge(v3,v1,true,true,edge_points);
    }

    // sort by Y as first order and X as second order
    std::sort(edge_points.begin(),edge_points.end(),[](FragmentHolder& v1,FragmentHolder& v2){
        if(v1.screenY_!=v2.screenY_)
            return v1.screenY_<v2.screenY_;
        return v1.screenX_<v2.screenX_;
    });

}
// rasterize an edge of triangle for scan line algotirhm, and interplote attributes of edge points and reserve in a vector
// note that each scan line should intersect with the primitive at even points. so I rasterize by 'y'
void Render::rasterizeEdge(const Vertex* v1,const Vertex* v2,bool in1,bool in2,std::vector<FragmentHolder>& edge_points){
    if(v1->s_pos_.y>v2->s_pos_.y){ 
        std::swap(v1,v2);
        std::swap(in1,in2);
    }
    float miny = v1->s_pos_.y;
    float maxy = v2->s_pos_.y;
    
    float dy=maxy-miny;
    if(dy<srender::EPSILON) return; // don't rasterize horizental edge
    float dx=v2->s_pos_.x-v1->s_pos_.x;
    float dx_dy=dx/dy;


    int minx=v1->s_pos_.x;
    FragmentHolder st={(int)v1->s_pos_.x,(int) v1->s_pos_.y,v1->s_pos_.z,v1->color_,v1->uv_,v1->w_pos_,v1->c_pos_,v1->w_norm_};
    FragmentHolder ed={(int)v2->s_pos_.x,(int) v2->s_pos_.y,v2->s_pos_.z,v2->color_,v2->uv_,v2->w_pos_,v2->c_pos_,v2->w_norm_};

    // delete endpoint if necessary

    float k=(ed.screenY_==st.screenY_)?1:1.0/(ed.screenY_-st.screenY_);
    FragmentHolder fdelta(st,ed);

    for(int y=miny+(!in1);y<=maxy-(!in2);++y){
        float t=(y-st.screenY_)*k;
        float tmp=t/st.c_pos_.w;
        float vt=tmp/(tmp+(1-t)/ed.c_pos_.w); // perspective correction
        FragmentHolder curp(st+fdelta*vt);

        curp.screenX_=dx_dy*(y-miny)+minx;
        curp.screenY_=y;

        edge_points.push_back(curp);
        drawPoint(glm::vec2(curp.screenX_,curp.screenY_),2,glm::vec4(255,0,0,1));

    }
}
// use scan-line algorithm to draw triangle

void Render::drawTriangleScanLine()
{
    auto& v1=sdptr_->getVertices(0);
    auto& v2=sdptr_->getVertices(1);
    auto& v3=sdptr_->getVertices(2);

    AABB2d aabb;
    aabb.containTriangel(v1.s_pos_,v2.s_pos_,v3.s_pos_);
    aabb.clipAABB(box_);
    if(!aabb.valid)
        return;

    // rasterize each edge into points
    std::vector<FragmentHolder> edge_points;
    std::vector<const Vertex*> vertices{&v1,&v2,&v3};
    scanLineConvert(vertices,edge_points);

    // for each scanline
    int num=edge_points.size();
    assert(num%2==0);
    for(int i=0;i<num-1;i+=2){
        auto& cur=edge_points[i];
        auto& next=edge_points[i+1];
        assert(cur.screenY_==next.screenY_);
        int y=cur.screenY_;
        if(y>box_.max.y||y<box_.min.y)  
            continue;

        float k=(next.screenX_==cur.screenX_)?1:1/(next.screenX_-cur.screenX_);
        FragmentHolder fdelta(cur,next);

        for(int x=cur.screenX_;x<=next.screenX_;++x){
            if(x>box_.max.x||x<box_.min.x)  
                continue;

            float t=(x-cur.screenX_)*k;
            float tmp=t/cur.c_pos_.w;
            float vt=tmp/(tmp+(1-t)/next.c_pos_.w); // perspective correction
            FragmentHolder point(cur+fdelta*vt);

            // // depth test and update z buffer
            if(zbuffer_.zTest(x,y,point.depth_)){
                // shader
                sdptr_->fragmentShader(point);
                // update color buffer
                colorbuffer_->setPixel(x,y,sdptr_->getColor());
            }
        }
    }
    
}

*/