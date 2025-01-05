#pragma once
#include "common/common_include.h"
#include "common/AABB.h"
#include"object.h"
#include<algorithm>

// forward declair
class ASInstance;

struct BVHnode
{
    int left;
    int right;

    AABB3d bbox;    // box in world or local space, pre-calculated when obj file is loaded.
    AABB3d sbox;    // box in screen space, calculated each frame.

    // specify which elements do this node control.
    unsigned int prmitive_start;
    unsigned int primitive_num ;

    BVHnode():left(-1),right(-1),prmitive_start(0),primitive_num(0){}
};


class BVHbuilder
{
public:
    BVHbuilder()=delete;

    // building bvh tree for BLAS
    BVHbuilder(std::shared_ptr<ObjectDesc> obj,uint32_t leaf_size);

    // building bvh tree for TLAS
    BVHbuilder(const std::vector<ASInstance>& instances);

    // building implemention
    int buildBVH(uint32_t start,uint32_t end);
    bool cmp(uint32_t a, uint32_t b,int axis);

    std::unique_ptr<std::vector<BVHnode>> moveNodes(){ return std::move(nodes_); }
    std::vector<uint32_t>& getPridices(){ return pridices_; }


public:
    std::unique_ptr<std::vector<BVHnode>> nodes_;        // root node is the nodes_[0].

    std::vector<uint32_t> pridices_;    // primitives_indices_
    std::vector<AABB3d> priboxes_;      // bbox for each element
    uint32_t leaf_size_=4;
};