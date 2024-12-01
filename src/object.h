/* object.h defines all classes for object description */
#pragma once
#include"common_include.h"
#include "tiny_obj_loader.h"

struct Vertex{
    glm::vec3 pos_;
    glm::vec3 norm_;
    glm::vec4 color_;
    glm::vec2 uv_;
    Vertex():pos_(glm::vec3(0.f)),norm_(glm::vec3(-1.f)),color_(glm::vec4(-1.f)),uv_(glm::vec2(-1.f)){};
};

// specify different types of objects  
enum class objecType{
    MESH,
    POINT,
    LINE,
    // SPHERE,
};

//@Todo: struct Materia

// the base class for all kinds of objects to be rendered
class ObjectDesc{
public:
    void setModel2World(glm::mat4& model){
        mat_model_=model;
    }
    void addTransform(glm::mat4& model){
        mat_model_=model*mat_model_;
    }

    virtual void initObject(const tinyobj::shape_t& info,const tinyobj::attrib_t& attrib)=0;
    virtual void printInfo() const=0;
    virtual void clear()=0;

    inline const std::vector<Vertex>& getVertices()const {
        return vertices_;
    }
    inline std::string getName()const{
        return name_;
    }

protected:
    std::string name_;

    // all the information of vertices
    std::vector<Vertex> vertices_;  

    // translation in world space
    glm::mat4 mat_model_=glm::mat4(1.0f);

    objecType type_;

};

class Mesh:public ObjectDesc{
public:
    Mesh(){
        type_=objecType::MESH;
    }

    // note the obj must be triangulated
    void initObject(const tinyobj::shape_t& info,const tinyobj::attrib_t& attrib) override;

    void clear() override{
        vertices_.clear();
    }
    
    // for debug use
    void setTriangleDemo();
    void printInfo() const override;

private:

    // the total number of faces, hence vertices num is trible
    unsigned long long face_num_;   

    
};

class Line:public ObjectDesc{
public:
    Line(){
        type_=objecType::LINE;
    }
    void initObject(const tinyobj::shape_t& info,const tinyobj::attrib_t& attrib) override;

    void clear() override{
        vertices_.clear();
    }

    // for debug use
    // void setLineDemo();
    void printInfo() const override;

private:
    // the total number of lines, hence vertices num is line_num_+1
    unsigned long long line_num_;   

};

class Point:public ObjectDesc{
public:
    Point(){
        type_=objecType::POINT;
    }
//TODO
    // void initObject(const tinyobj::shape_t& info,const tinyobj::attrib_t& attrib) override;

    // void clear() override;

    // // for debug use
    // void setPointDemo();
    // void printPointInfo() const;

private:
    // the total number of lines, hence vertices num is line_num_+1
    unsigned long long point_num_;   


};


// a single read from .obj results in a `ObjLoader`
class ObjLoader{
public:
    ObjLoader()=delete;

    ObjLoader(std::string str){
        filename_=str;
        readObjFile(str);
        setObject();
    }

    void setFileAndRead(std::string str){
        filename_=str;
        readObjFile(str);
    }
    
    inline std::string getFilename()const{
        return filename_;
    }

    inline const tinyobj::ObjReader& getReader() const{
        return reader_; // read only
    }

    std::vector<std::unique_ptr<Mesh>>& getMeshes(){
        return meshes_;
    }

    std::vector<std::unique_ptr<Line>>& getLines(){
        return lines_;
    }
    

private:
    
    void readObjFile(std::string inputfile="");

    void setObject();

private:
    std::string filename_;
    tinyobj::ObjReader reader_;

    unsigned int total_shapes_;

    // move this to `Scene`
    std::vector<std::unique_ptr<Mesh>> meshes_;
    std::vector<std::unique_ptr<Line>> lines_;

};
