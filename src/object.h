/* object.h defines all classes for object description */
#pragma once
#include"common_include.h"
#include"vertex.h"
#include "tiny_obj_loader.h"

// specify different types of objects  
enum class PrimitiveType{
    // NAME= the number of vertices
    POINT=1,
    LINE=2,
    MESH=3,
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
    /**
     * @brief fill the `vector<Vertex> vertices_` 
     * 
     * @param info 
     * @param attrib 
     */
    virtual void initObject(const tinyobj::shape_t& info,const tinyobj::attrib_t& attrib)=0;
    virtual void printInfo() const=0;
    virtual void clear()=0;

    inline std::vector<Vertex>& getVertices() {
        return vertices_;
    }
    inline const std::vector<uint32_t>& getIndices()const {
        return indices_;
    }

    inline std::string getName()const{
        return name_;
    }

    inline PrimitiveType getPrimitiveType()const{
        return type_;
    }

    inline glm::mat4 getModel()const{
        return mat_model_;
    }


protected:
    std::string name_;

    // all the information of vertices
    std::vector<Vertex> vertices_;  
    // contains indices to `vertices_`,and every three vertices constitude a face
    std::vector<uint32_t> indices_;

    // translation in world space
    glm::mat4 mat_model_=glm::mat4(1.0f);

    PrimitiveType type_;

};

class Mesh:public ObjectDesc{
public:
    Mesh(){
        type_=PrimitiveType::MESH;
    }
  
    // note the obj must be triangulated
    void initObject(const tinyobj::shape_t& info,const tinyobj::attrib_t& attrib) override;

    void clear() override{
        vertices_.clear();
    }
    
    void printInfo() const override;

private:

    // the total number of faces, hence vertices num is trible
    unsigned long long face_num_;   

    
};

class Line:public ObjectDesc{
public:
    Line(){
        type_=PrimitiveType::LINE;
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
        type_=PrimitiveType::POINT;
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

    std::vector<std::unique_ptr<ObjectDesc>>& getObjects(){
        return all_objects_;
    }

    
    void setObject();

    void readObjFile(std::string inputfile="");

private:
    std::string filename_;
    tinyobj::ObjReader reader_;

    unsigned int total_shapes_;

    // move this to `Scene`
    // std::vector<std::unique_ptr<Mesh>> meshes_;
    // std::vector<std::unique_ptr<Line>> lines_;
    std::vector<std::unique_ptr<ObjectDesc>> all_objects_;

};

