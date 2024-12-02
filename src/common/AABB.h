#pragma once
#include<iostream>
#include"common_include.h"
#include"algorithm"

struct Point2d{
    int x;
    int y;
    Point2d():x(0),y(0){}
    Point2d(int x,int y):x(x),y(y){}
    Point2d(glm::vec4& t){
        x=t.x;
        y=t.y;
    }
};

struct AABB2d{
    Point2d min;
    Point2d max;
    bool valid=true;
    AABB2d():min(srender::INF,srender::INF),max(-srender::INF,-srender::INF){}
    
    void containTriangel(Point2d p1,Point2d p2,Point2d p3){
        min.x=std::min(std::min(p1.x,p2.x),p3.x);
        min.y=std::min(std::min(p1.y,p2.y),p3.y);
        max.x=std::max(std::max(p1.x,p2.x),p3.x);
        max.y=std::max(std::max(p1.y,p2.y),p3.y);
        if(min.x>=max.x||min.y>=max.y)
            valid=false;
    }

    void clipAABB(AABB2d box){
        max.y=std::min(max.y,box.max.y);
        max.x=std::min(max.x,box.max.x);

        min.y=std::max(min.y,box.min.y);
        min.x=std::max(min.x,box.min.x);

        if(min.x>=max.x||min.y>=max.y)
            valid=false;
    }
};