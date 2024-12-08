/* scene_loader defines `Scene` to control all the objects and accelerate structures */
#pragma once
#include"common_include.h"
#include"object.h"
#include"texture.h"

class Scene{
public:
    Scene(){};
    Scene(std::string filename){
        addScene(filename);
    }

    void addScene(std::string filename,bool flipn=false,bool backculling=true){
        // read from objfile
        ObjLoader objloader(filename,flipn,backculling);

        // update all_objects_
        auto& objs=objloader.getObjects();
        
        for(auto& item:objs){
            all_objects_.push_back(std::move(item));
        }

        objloader.getNums(vertex_num_,face_num_);
        std::cout<<"Current vertex num: "<<vertex_num_<<std::endl;
        std::cout<<"Current face num: "<<face_num_<<std::endl;
    }

    inline const std::vector<std::shared_ptr<ObjectDesc>>& getObjects()const{
        return all_objects_;
    }

    
private:
    
    // 管理所有的ObjectDesc对象
    std::vector<std::shared_ptr<ObjectDesc>> all_objects_;
    int vertex_num_;
    int face_num_;
    
    // BVH root node

};


