#pragma once
#include "common/common_include.h"
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
        nodes_=builder.moveNodes();
        primitives_indices_=std::make_unique<std::vector<uint32_t>>(std::move(builder.getPridices()));
    }

public:
    std::shared_ptr<ObjectDesc> object_;
    std::unique_ptr<std::vector<BVHnode>> nodes_;                   // nodes_ points to  primitives_indices_
    std::unique_ptr<std::vector<uint32_t>> primitives_indices_;     // primitives_indices_ points to object_'s primitive
};

class ASInstance{
public:
    ASInstance(std::shared_ptr<BLAS>blas,const glm::mat4& mat,ShaderType shader):blas_(blas),modle_(mat),shader_(shader){
        AABB3d rootBox=blas_->nodes_->at(0).bbox;
        worldBBox_=rootBox.transform(modle_);
    }
public:
    std::shared_ptr<BLAS> blas_;
    glm::mat4 modle_;
    AABB3d worldBBox_;
    ShaderType shader_;
};

class TLAS
{
public:
    TLAS():tree_(std::make_unique<std::vector<BVHnode>>()){}

    void buildTLAS(){
        if(all_instances_.size()){
            BVHbuilder builder(all_instances_);
            tree_=builder.moveNodes();
        }
    }
    

public:
    std::vector<ASInstance> all_instances_;
    std::unique_ptr<std::vector<BVHnode>> tree_;

};