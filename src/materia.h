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
    void setTexture(MltMember mtype){
        if(mtype==MltMember::Diffuse){

        }
        else{

        }
    }

    const glm::vec3 getAmbient()const{ return ambient_; }
    const glm::vec3 getDiffuse()const{ return diffuse_; }
    const glm::vec3 getSpecular()const{ return specular_; }

private:
    glm::vec3 ambient_;
    glm::vec3 diffuse_;
    glm::vec3 specular_;

    std::string ambient_textname_;
    std::string diffuse_textname_;
    std::string specular_textname_;
    
};