/* scene_loader defines `Scene` to control all the objects and accelerate structures */
#pragma once
#include"common_include.h"
#include"object.h"
#include"texture.h"
#include"light.h"
#include"as.h"
#include<unordered_map>


class Scene{
public:
    Scene(){};
    void addLight(std::shared_ptr<Light> light){
        all_lights_.emplace_back(light);
    }

    void addObjInstance(std::string filename, glm::mat4& model,ShaderType shader,bool flipn=false,bool backculling=true){
        // create BLAS for obj if it hasn't been built.
        if(blas_map_.find(filename)==blas_map_.end()){
            // read from objfile
            ObjLoader objloader(filename,flipn,backculling);
            std::shared_ptr<ObjectDesc> obj=std::move(objloader.getObjects());
            // create blas
            blas_map_[filename]=std::make_shared<BLAS>(obj,leaf_num_);

            objloader.updateNums(vertex_num_,face_num_);
            std::cout<<"Current vertex num: "<<vertex_num_<<std::endl;
            std::cout<<"Current face num: "<<face_num_<<std::endl;
        }
        // create ASInstance for obj
        all_instances_.emplace_back( ASInstance(blas_map_[filename],model,shader) );
    }

    void buildTLAS(){
        // TODO: building TLAS
    }

    inline const  std::vector<ASInstance>& getAllInstances()const{
        return all_instances_;
    }

    inline const std::vector<std::shared_ptr<Light>>& getLights()const{
        return all_lights_;
    } 

    inline void clearScene(){
        all_lights_.clear();
        all_instances_.clear();
        vertex_num_=0;
        face_num_=0;
    }

    
private:
    
    // AS for objects
    std::unordered_map<std::string,std::shared_ptr<BLAS> > blas_map_;
    std::vector<ASInstance> all_instances_;         // AS->BLAS->objectdesc
    int leaf_num_=4;
    int vertex_num_;
    int face_num_;

    // lights
    std::vector<std::shared_ptr<Light>> all_lights_;

};


