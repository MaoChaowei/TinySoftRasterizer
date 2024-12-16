#pragma once
#include<iostream>
#include"common_include.h"
#include"algorithm"

struct AABB2d{
    glm::vec2 min;
    glm::vec2 max;
    bool valid=true;
    AABB2d():min(srender::INF,srender::INF),max(-srender::INF,-srender::INF){}
    
    void containTriangel(const glm::vec2& p1,const glm::vec2& p2,const glm::vec2& p3){
        min.x=std::min(std::min(p1.x,p2.x),p3.x);
        min.y=std::min(std::min(p1.y,p2.y),p3.y);
        max.x=std::max(std::max(p1.x,p2.x),p3.x);
        max.y=std::max(std::max(p1.y,p2.y),p3.y);
        if(min.x>=max.x||min.y>=max.y)
            valid=false;
    }

    void clipAABB(const AABB2d& box){
        max.y=std::min(max.y,box.max.y);
        max.x=std::min(max.x,box.max.x);

        min.y=std::max(min.y,box.min.y);
        min.x=std::max(min.x,box.min.x);

        if(min.x>=max.x||min.y>=max.y)
            valid=false;
    }
};

struct AABB3d{
    glm::vec3 min;
    glm::vec3 max;
    // bool valid=true;
    AABB3d():min(srender::INF,srender::INF,srender::INF),max(-srender::INF,-srender::INF,-srender::INF){}
    AABB3d(const glm::vec3& p1,const glm::vec3& p2,const glm::vec3& p3){
        min.x=std::min(std::min(p1.x,p2.x),p3.x);
        min.y=std::min(std::min(p1.y,p2.y),p3.y);
        min.z=std::min(std::min(p1.z,p2.z),p3.z);
        max.x=std::max(std::max(p1.x,p2.x),p3.x);
        max.y=std::max(std::max(p1.y,p2.y),p3.y);
        max.z=std::max(std::max(p1.z,p2.z),p3.z);

    }

    void expand(const AABB3d& box){
        min.x=std::min(min.x,box.min.x);
        min.y=std::min(min.y,box.min.y);
        min.z=std::min(min.z,box.min.z);
        
        max.x=std::max(max.x,box.max.x);
        max.y=std::max(max.y,box.max.y);
        max.z=std::max(max.z,box.max.z);
    }

    float extent(int axis) const{
        switch(axis){
            case 0: return max.x-min.x;
            case 1: return max.y-min.y;
            case 2: return max.z-min.z;
        }
        return 0;
    }

    AABB3d transform(const glm::mat4& m){
        min=m*glm::vec4(min,1);
        max=m*glm::vec4(max,1);
        return *this;
    }
    

};