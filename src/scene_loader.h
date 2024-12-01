/* scene_loader defines `Scene` to control all the objects and accelerate structures */
#pragma once
#include"common_include.h"
#include"object.h"


class Scene{
public:
    Scene(){};
    Scene(std::string filename){
        addScene(filename);
    }

    void addScene(std::string filename){
        // read from objfile
        ObjLoader objloader(filename);

        // update all_objects_
        auto& objmesh=objloader.getMeshes();
        auto& objline=objloader.getLines();

        for(auto& m:objmesh){
            all_objects_.push_back(std::move(m));
        }
        for(auto& li:objline){
            all_objects_.push_back(std::move(li));
        }
    }

    inline std::vector<std::unique_ptr<ObjectDesc>>& getObjects(){
        return all_objects_;
    }
    
private:
    
    // 管理所有的ObjectDesc对象
    std::vector<std::unique_ptr<ObjectDesc>> all_objects_;
    // BVH root node

};


