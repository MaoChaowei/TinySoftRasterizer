/* scene_loader defines `Scene` to control all the objects and accelerate structures */
#pragma once
#include"common_include.h"
#include"object.h"
#include"texture.h"
#include"light.h"
#include"as.h"
#include<unordered_map>

struct ObjectInstance{
    std::shared_ptr<ObjectDesc> object;
    glm::mat4 model;
    ShaderType shader;
    ObjectInstance(std::shared_ptr<ObjectDesc> ptr,const glm::mat4& m, ShaderType& s){
        object=std::move(ptr);
        model=m;
        shader=s;
    }
};


class Scene{
public:
    Scene(){};
    void addLight(std::shared_ptr<Light> light){
        all_lights_.emplace_back(light);
    }

    void addScene(std::string filename,const glm::mat4& model,ShaderType shader,bool flipn=false,bool backculling=true){
        std::shared_ptr<ObjectDesc> obj;
        if(blas_map_.find(filename)==blas_map_.end()){
            // read from objfile
            ObjLoader objloader(filename,flipn,backculling);
            obj=std::move(objloader.getObjects());
            // create blas
            blas_map_[filename]=std::make_shared<BLAS>(obj,leaf_num_);

            objloader.updateNums(vertex_num_,face_num_);
            std::cout<<"Current vertex num: "<<vertex_num_<<std::endl;
            std::cout<<"Current face num: "<<face_num_<<std::endl;
        }
        else{
            obj=blas_map_[filename]->object_;
        }
        // update all_objects and asinstance
        all_objects_.emplace_back( ObjectInstance(obj,model,shader) );
        // asinstance_.emplace_back(std::make_shared<ASInstance>(blas_map_[filename],model));
    }

    void buildTLAS(){
        // TODO: building TLAS
    }

    inline const  std::vector<ObjectInstance>& getObjects()const{
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
    std::vector<ObjectInstance> all_objects_;
    int vertex_num_;
    int face_num_;

    // lights
    std::vector<std::shared_ptr<Light>> all_lights_;
    
    // AS
    std::unordered_map<std::string,std::shared_ptr<BLAS> > blas_map_;
    int leaf_num_=4;
    std::vector<std::shared_ptr<ASInstance>> asinstance_;

};


