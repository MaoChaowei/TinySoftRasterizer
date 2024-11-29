/* object.h defines all classes for object description */
#pragma once
#include"common_include.h"


struct Vertex{
    glm::vec3 position_;
    glm::vec3 normal_;
    glm::vec4 color_;
    glm::vec2 uv_;
};

//@Todo: struct Materia

// the base class for all kinds of objects to be rendered
class ObjectDesc{
public:
    void setModel2World(glm::mat4& model){
        mat_model_=model;
    }
    virtual void setObjectData(std::string filename="")=0;
    
protected:
    glm::mat4 mat_model_=glm::mat4(1.0f);

};

class Mesh:public ObjectDesc{
public:
    Mesh(){};
    inline void setVertices( std::vector<Vertex>& vertices){
        vertices_=vertices;
    };
    inline void setVIdx(std::vector<int>& vidx){
        vidx_arr_=vidx;
    };
    inline void setVertices( const std::vector<Vertex>& vertices){
        vertices_=vertices;
    };
    inline void setVIdx(const std::vector<int>& vidx){
        vidx_arr_=vidx;
    };
    void setTriangleDemo();

    void printMeshInfo() const;

    void clear();

   void setObjectData(std::string filename="")override;

private:
    std::vector<int> vidx_arr_;
    std::vector<Vertex> vertices_;

};