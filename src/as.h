#pragma once
#include "common/common_include.h"
#include "common/utils.h"
#include "bvhbuilder.h"
#include"softrender/shader.h"

class BLAS
{
public:
    BLAS()=delete;
    BLAS(std::shared_ptr<ObjectDesc> obj,uint32_t leaf_size){
        // bind object
        object_ = obj;
        // build its bvh
        BVHbuilder builder(obj,leaf_size);
        tree_=builder.moveNodes();
        primitives_indices_=std::make_unique<std::vector<uint32_t>>(std::move(builder.getPridices()));
    }

    
public:
    std::shared_ptr<ObjectDesc> object_;
    std::unique_ptr<std::vector<BVHnode>> tree_;                   // nodes_ points to  primitives_indices_
    std::unique_ptr<std::vector<uint32_t>> primitives_indices_;     // primitives_indices_ points to object_'s primitive
};

struct PrimitiveHolder{         // because of the neccessity of clipping, each frame updates all the primitives of the instance.
    ClipFlag clipflag_;         // 0: accepted; 1: clipped; 2: refused;
    int32_t mtlidx_;            // point to its material in blas_
    int32_t vertex_start_pos_;  // point to vertices_
    int32_t vertex_num_;        // specify the range starting from vertex_start_pos_

    PrimitiveHolder(ClipFlag cf, int32_t mtl, int32_t startpos, int32_t num)
    : clipflag_(cf),
        mtlidx_(mtl),
        vertex_start_pos_(startpos),
        vertex_num_(num)
    {}
};

class ASInstance{
public:
    ASInstance(std::shared_ptr<BLAS>blas,const glm::mat4& mat,ShaderType shader):blas_(blas),modle_(mat),shader_(shader){
        AABB3d rootBox=blas_->tree_->at(0).bbox;
        worldBBox_=rootBox.transform(modle_);

        vertices_=std::make_unique<std::vector<Vertex>>();
        mtlidx_=std::make_unique<std::vector<int>>();
        primitives_buffer_=std::make_unique<std::vector<PrimitiveHolder>>();
        blas_sboxes_=std::make_unique<std::vector<AABB3d>>(blas_->tree_->size());
    }

    void refreshVertices(){
        vertices_=std::make_unique<std::vector<Vertex>>();
        mtlidx_=std::make_unique<std::vector<int>>();
        primitives_buffer_=std::make_unique<std::vector<PrimitiveHolder>>();
        blas_sboxes_=std::make_unique<std::vector<AABB3d>>(blas_->tree_->size());
    }

    void BLASupdateSBox(){
        auto& blas_tree=*blas_->tree_;
        auto& primitive_indices=*blas_->primitives_indices_;
        updateScreenBox(0,blas_tree,primitive_indices);
    }

    void updateScreenBox(int32_t node_idx,std::vector<BVHnode>&blas_tree,std::vector<uint32_t>& primitive_indices){

        int32_t left_idx=blas_tree[node_idx].left;
        int32_t right_idx=blas_tree[node_idx].right;

        auto& sbox=blas_sboxes_->at(node_idx);
        sbox.reset();

        if(left_idx==-1&&right_idx==-1){

            int st_primitive=blas_tree[node_idx].prmitive_start;

            for(int i=0;i<blas_tree[node_idx].primitive_num;++i){

                uint32_t idx=primitive_indices[st_primitive+i];

                auto cflag=primitives_buffer_->at(idx).clipflag_;
                if(cflag==ClipFlag::accecpted||cflag==ClipFlag::clipped){
                    int32_t st_ver=primitives_buffer_->at(idx).vertex_start_pos_;

                    for(int v=0;v<primitives_buffer_->at(idx).vertex_num_;++v){ // >=3 vertivces
                        sbox.addPoint(this->vertices_->at(st_ver+v).s_pos_);
                    }
                }
            }
            return;
        }

        if(left_idx!=-1){
            updateScreenBox(left_idx,blas_tree,primitive_indices);
        }
        if(right_idx!=-1){
            updateScreenBox(right_idx,blas_tree,primitive_indices);
        }

        sbox.expand(blas_sboxes_->at(left_idx));
        sbox.expand(blas_sboxes_->at(right_idx));

        return;
    }


public:
    std::shared_ptr<BLAS> blas_;

    // preserve properties for each frame after clipping.
    std::unique_ptr<std::vector<Vertex>> vertices_;
    std::unique_ptr<std::vector<int>> mtlidx_;
    std::unique_ptr<std::vector<PrimitiveHolder>> primitives_buffer_;
    std::unique_ptr<std::vector<AABB3d>> blas_sboxes_;

    glm::mat4 modle_;
    AABB3d worldBBox_;
    ShaderType shader_;
};

class TLAS
{
public:
    TLAS():tree_(std::make_unique<std::vector<BVHnode>>()),tlas_sboxes_(std::make_unique<std::vector<AABB3d>>()){}

    void buildTLAS(){
        if(all_instances_.size()){
            BVHbuilder builder(all_instances_);
            tree_=builder.moveNodes();
        }
        tlas_sboxes_->resize(tree_->size());
    }

    void TLASupdateSBox(){
        for(auto& inst:all_instances_){
            inst.BLASupdateSBox();
            auto& blas_sbox=inst.blas_sboxes_->at(0);
        }
        updateScreenBox(0);
    }

    
    void updateScreenBox(int32_t node_idx){
        
        int32_t left_idx=tree_->at(node_idx).left;
        int32_t right_idx=tree_->at(node_idx).right;
        auto& sbox=tlas_sboxes_->at(node_idx);

        if(left_idx==-1&&right_idx==-1){
            int st=tree_->at(node_idx).prmitive_start;
            sbox=all_instances_[st].blas_sboxes_->at(0);
            return;
        }

        if(left_idx!=-1){
            updateScreenBox(left_idx);
        }
        if(right_idx!=-1){
            updateScreenBox(right_idx);
        }

        sbox.reset();
        sbox.expand(tlas_sboxes_->at(left_idx));
        sbox.expand(tlas_sboxes_->at(right_idx));

        return;
    }
    

public:
    std::vector<ASInstance> all_instances_;
    std::unique_ptr<std::vector<BVHnode>> tree_;

    std::unique_ptr<std::vector<AABB3d>> tlas_sboxes_;

};