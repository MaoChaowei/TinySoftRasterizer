#pragma once
#include"common/common_include.h"
#include"texture.h"

enum class MltMember{
    Ambient,
    Diffuse,
    Specular,
};

class Material{
public:
    void setADS(glm::vec3 am,glm::vec3 di,glm::vec3 sp){
        ambient_=am;
        diffuse_=di;
        specular_=sp;
    }
    void setName(std::string name){ name_= name; }
    void setTexture(MltMember mtype,std::string path){
        switch(mtype){
            case MltMember::Ambient:
                ambient_path_=path;
                amb_texture_=std::make_shared<Texture>(path);
                break;
            case MltMember::Diffuse:
                diffuse_path_=path;
                dif_texture_=std::make_shared<Texture>(path);
                break;
            case MltMember::Specular:
                specular_path_=path;
                spe_texture_=std::make_shared<Texture>(path);
                break;            
        }
    }

    std::shared_ptr<Texture> getTexture(MltMember mtype)const{
        switch(mtype){
            case MltMember::Ambient:
                return amb_texture_;
            case MltMember::Diffuse:
                return dif_texture_;
            case MltMember::Specular:
                return spe_texture_;       
        }
        std::cout<<"std::shared_ptr<Texture> getTexture(MltMember mtype): unknow input!\n";
        return nullptr;
    }



    const glm::vec3 getAmbient()const{ return ambient_; }
    const glm::vec3 getDiffuse()const{ return diffuse_; }
    const glm::vec3 getSpecular()const{ return specular_; }
    const std::string getName()const{ return name_; }

private:
    std::string name_;

    glm::vec3 ambient_;
    glm::vec3 diffuse_;
    glm::vec3 specular_;

    std::string ambient_path_;
    std::string diffuse_path_;
    std::string specular_path_;

    std::shared_ptr<Texture> amb_texture_=nullptr;
    std::shared_ptr<Texture> dif_texture_=nullptr;
    std::shared_ptr<Texture> spe_texture_=nullptr;
    
};