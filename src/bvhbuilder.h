#pragma once
#include "common/common_include.h"
#include "common/AABB.h"
#include"object.h"
#include<algorithm>

struct BVHnode
{
    int left;
    int right;
    AABB3d bbox;
    // indices of `primitives_` in class BVHbuilder
    // specify which primitives do this node control.
    unsigned int prmitive_start;
    unsigned int primitive_num ;

    BVHnode():left(-1),right(-1),prmitive_start(0),primitive_num(0){}
};

class BVHbuilder
{
public:
    BVHbuilder()=delete;
    BVHbuilder(std::shared_ptr<ObjectDesc> obj,uint32_t leaf_size):vertices_(obj->getconstVertices()),indices_(obj->getIndices()){
        leaf_size_=leaf_size;
        // reserve enough space to avoid frequent capacity expansion
        int facenum=indices_.size()/3;
        nodes_.reserve(2*facenum/leaf_size);
        pridices_.resize(facenum);
        for(int i=0;i<facenum;++i){
            pridices_[i]=i;
            AABB3d box(vertices_[indices_[i*3+0]].pos_,vertices_[indices_[i*3+1]].pos_,vertices_[indices_[i*3+2]].pos_);
            priboxes_.emplace_back(box);
        }
        buildBVH(0,facenum-1);
    }

    int buildBVH(uint32_t start,uint32_t end){
        if(start>end) 
            return -1;

        // set current node
        nodes_.emplace_back(BVHnode());
        int nodeidx=nodes_.size()-1;
        auto& current=nodes_.back();
        current.primitive_num=end-start+1;
        current.prmitive_start=start;
        for(int i=start;i<=end;++i){
            current.bbox.expand(priboxes_[pridices_[i]]);
        }
        int lenx=current.bbox.extent(0);
        int leny=current.bbox.extent(1);
        int lenz=current.bbox.extent(2);
        
        int axis=0;
        if(leny>lenx&&leny>lenz) axis=1;
        else if(lenz>lenx&&lenz>leny) axis=2;

        // leaf?
        if(end-start+1<=leaf_size_)
            return nodeidx;

        // sort to find the middle face
        sort(pridices_.begin()+start,pridices_.end()+end+1,[this,axis](uint32_t a, uint32_t b){
            return this->cmp(a,b,axis);
        });
        uint32_t mid=(start+end)/2;

        // recursive 
        current.left=buildBVH(start,mid);
        current.right=buildBVH(mid+1,end);

        return nodeidx;
    }

    bool cmp(uint32_t a, uint32_t b,int axis){
        if(axis==0)
            return (priboxes_[a].min.x+priboxes_[a].max.x<priboxes_[b].min.x+priboxes_[b].max.x);
        else if(axis==1)
            return (priboxes_[a].min.y+priboxes_[a].max.y<priboxes_[b].min.y+priboxes_[b].max.y);
        else if(axis==2)
            return (priboxes_[a].min.z+priboxes_[a].max.z<priboxes_[b].min.z+priboxes_[b].max.z);
        
        return true;
    }

    std::vector<BVHnode>& getNodes(){ return nodes_; }
    std::vector<uint32_t>& getPridices(){ return pridices_; }



public:
    std::vector<BVHnode> nodes_;        // root node is the nodes_[0].
    std::vector<uint32_t> pridices_;    // primitives_indices_

    const std::vector<Vertex>& vertices_;  
    const std::vector<uint32_t>& indices_;
    std::vector<AABB3d> priboxes_;      // bbox for each primitive
    uint32_t leaf_size_=4;
};