/* scene_loader defines `Scene` to control all the objects and accelerate structures */
#pragma once
#include"common_include.h"
#include"object.h"
#include"texture.h"
#include"light.h"

class Scene{
public:
    Scene(){};
    Scene(std::string filename){
        addScene(filename);
    }
    void addLight(std::shared_ptr<Light> light){
        all_lights_.push_back(light);
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
    inline const std::vector<std::shared_ptr<Light>>& getLights()const{
        return all_lights_;
    } 

    inline void clearScene(){
        all_lights_.clear();
        all_objects_.clear();
        vertex_num_=0;
        face_num_=0;
    }

    
private:
    
    // objects
    std::vector<std::shared_ptr<ObjectDesc>> all_objects_;
    int vertex_num_;
    int face_num_;

    // lights
    std::vector<std::shared_ptr<Light>> all_lights_;
    
    // BVH root node

};


