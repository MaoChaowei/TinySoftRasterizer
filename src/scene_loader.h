/* scene_loader defines `Scene` to control all the objects and accelerate structures */
#pragma once
#include"common_include.h"
#include"object.h"


class Scene{
public:

    // 管理所有的ObjectDesc对象
    std::vector<std::shared_ptr<ObjectDesc>> all_objects_;
    // BVH root node


};


