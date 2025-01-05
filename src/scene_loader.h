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
    Scene():tlas_(std::make_unique<TLAS>()){};

    void addLight(std::shared_ptr<Light> light){
        all_lights_.emplace_back(light);
    }

    void setBVHsize(uint32_t leaf_num){leaf_num_=leaf_num;}

    // create BLAS for obj if it hasn't been built.
    void addObjInstance_SpaceFriendly(std::string filename, glm::mat4& model,ShaderType shader,bool flipn=false,bool backculling=true){
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
        tlas_->all_instances_.emplace_back( blas_map_[filename],model,shader );
    }

    // create BLAS for obj even if it has been built before.
    void addObjInstance(std::string filename, glm::mat4& model,ShaderType shader,bool flipn=false,bool backculling=true){

        // read from objfile
        ObjLoader objloader(filename,flipn,backculling);
        std::shared_ptr<ObjectDesc> obj=std::move(objloader.getObjects());
        // create blas
        std::shared_ptr<BLAS> blas=std::make_shared<BLAS>(obj,leaf_num_);

        objloader.updateNums(vertex_num_,face_num_);
        std::cout<<"Current vertex num: "<<vertex_num_<<std::endl;
        std::cout<<"Current face num: "<<face_num_<<std::endl;
        
        // create ASInstance for obj
        tlas_->all_instances_.emplace_back( blas,model,shader );
    }

    void buildTLAS(){
        tlas_->buildTLAS();
    }

    inline  std::vector<ASInstance>& getAllInstances(){
        if(tlas_->all_instances_.size())
            return tlas_->all_instances_;
        else{
            std::cerr<<"getAllInstances() have no instaces.\n";
            exit(-1);
        }
    }

    inline const std::vector<std::shared_ptr<Light>>& getLights()const{
        return all_lights_;
    } 
    inline TLAS& getTLAS(){
        return *tlas_;
    }

    inline int getFaceNum(){return face_num_;}


    inline void clearScene(){
        tlas_=std::make_unique<TLAS>();
        all_lights_.clear();
        blas_map_.clear();
        vertex_num_=0;
        face_num_=0;
    }

    
private:
    
    // AS for objects
    std::unique_ptr<TLAS> tlas_;            // TLAS->AS->BLAS->objectdesc
    std::unordered_map<std::string,std::shared_ptr<BLAS> > blas_map_;    
    int leaf_num_=4;
    int vertex_num_;
    int face_num_;

    // lights
    std::vector<std::shared_ptr<Light>> all_lights_;

};


