#pragma once
#include "common/common_include.h"
#include "bvhbuilder.h"

class BLAS
{
public:
    BLAS()=delete;
    BLAS(std::shared_ptr<ObjectDesc> obj,uint32_t leaf_size){
        // bind object
        
        object_ = obj;
        // build its bvh
        // BVHbuilder builder(obj,leaf_size);
        // nodes_=std::make_unique<std::vector<BVHnode>>(std::move(builder.getNodes()));
        // primitives_indices_=std::make_unique<std::vector<uint32_t>>(std::move(builder.getPridices()));
    }

public:
    std::shared_ptr<ObjectDesc> object_;
    std::unique_ptr<std::vector<BVHnode>> nodes_;                   // nodes_ points to  primitives_indices_
    std::unique_ptr<std::vector<uint32_t>> primitives_indices_;     // primitives_indices_ points to object_'s primitive
};

class ASInstance{
public:
    ASInstance(std::shared_ptr<BLAS>blas,glm::mat4 mat){
        blas_=blas;
        modle_=std::make_shared<glm::mat4>(mat);
        AABB3d rootBox=blas_->nodes_->at(0).bbox;
        worldBBox=rootBox.transform(*modle_);
    }
public:
    std::shared_ptr<BLAS> blas_;
    std::shared_ptr<glm::mat4> modle_;
    AABB3d worldBBox;
};


class TLAS
{
public:


public:
    

};