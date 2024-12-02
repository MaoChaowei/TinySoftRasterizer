/* object.h defines all classes for object description */
#pragma once
#include"common_include.h"
#include "tiny_obj_loader.h"

struct Vertex{
    glm::vec3 pos_;      
    glm::vec3 norm_;    // 需要根据(（model）^-1)T 矩阵进行变换
    glm::vec4 color_;
    glm::vec2 uv_;
    Vertex():pos_(glm::vec3(0.f)),norm_(glm::vec3(-1.f)),color_(glm::vec4(-1.f)),uv_(glm::vec2(-1.f)){};
};

// 为 Vertex 结构体重载 operator<<
inline std::ostream& operator<<(std::ostream& os, const Vertex& vertex) {
    os << "Position(" << vertex.pos_.x << ", " << vertex.pos_.y << ", " << vertex.pos_.z << "), "
       << "Normal(" << vertex.norm_.x << ", " << vertex.norm_.y << ", " << vertex.norm_.z << "), "
       << "Color(" << vertex.color_.r << ", " << vertex.color_.g << ", " << vertex.color_.b << ", " << vertex.color_.a << "), "
       << "UV(" << vertex.uv_.x << ", " << vertex.uv_.y << ")";
    return os;
}
// 近似比较函数
inline bool AlmostEqual(float a, float b, float epsilon = srender::EPSILON) {
    return std::fabs(a - b) < epsilon;
}

inline bool AlmostEqual(const glm::vec2& a, const glm::vec2& b, float epsilon = srender::EPSILON) {
    return AlmostEqual(a.x, b.x, epsilon) && AlmostEqual(a.y, b.y, epsilon);
}

inline bool AlmostEqual(const glm::vec3& a, const glm::vec3& b, float epsilon = srender::EPSILON) {
    return AlmostEqual(a.x, b.x, epsilon) && AlmostEqual(a.y, b.y, epsilon) && AlmostEqual(a.z, b.z, epsilon);
}

inline bool AlmostEqual(const glm::vec4& a, const glm::vec4& b, float epsilon = srender::EPSILON) {
    return AlmostEqual(a.x, b.x, epsilon) && AlmostEqual(a.y, b.y, epsilon) &&
           AlmostEqual(a.z, b.z, epsilon) && AlmostEqual(a.w, b.w, epsilon);
}

// 重载 Vertex operator==
inline bool operator==(const Vertex& lhs, const Vertex& rhs) {
    return AlmostEqual(lhs.pos_, rhs.pos_) &&
           AlmostEqual(lhs.norm_, rhs.norm_) &&
           AlmostEqual(lhs.color_, rhs.color_) &&
           AlmostEqual(lhs.uv_, rhs.uv_);
}


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
    /**
     * @brief fill the `vector<Vertex> vertices_` 
     * 
     * @param info 
     * @param attrib 
     * @param offset : to move the `Vertex.pos_idx_` to the right position
     */
    virtual void initObject(const tinyobj::shape_t& info,const tinyobj::attrib_t& attrib)=0;
    virtual void printInfo() const=0;
    virtual void clear()=0;

    inline const std::vector<Vertex>& getVertices()const {
        return vertices_;
    }
    inline const std::vector<uint32_t>& getIndices()const {
        return indices_;
    }

    inline std::string getName()const{
        return name_;
    }

    inline objecType getType()const{
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
    
    void setObject();

    void readObjFile(std::string inputfile="");

private:
    std::string filename_;
    tinyobj::ObjReader reader_;

    unsigned int total_shapes_;

    // move this to `Scene`
    std::vector<std::unique_ptr<Mesh>> meshes_;
    std::vector<std::unique_ptr<Line>> lines_;

};


// 为 glm::vec2, glm::vec3, glm::vec4 和 Vertex 定义 hash 函数
namespace std {
    template<>
    struct hash<glm::vec2> {
        size_t operator()(const glm::vec2& v) const {
            return hash<float>()(v.x) ^ (hash<float>()(v.y) << 1);
        }
    };

    template<>
    struct hash<glm::vec3> {
        size_t operator()(const glm::vec3& v) const {
            return ((hash<float>()(v.x) ^ (hash<float>()(v.y) << 1)) >> 1) ^ (hash<float>()(v.z) << 1);
        }
    };

    template<>
    struct hash<glm::vec4> {
        size_t operator()(const glm::vec4& v) const {
            size_t h1 = hash<float>()(v.x);
            size_t h2 = hash<float>()(v.y);
            size_t h3 = hash<float>()(v.z);
            size_t h4 = hash<float>()(v.w);
            return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
        }
    };

    template<>
    struct hash<Vertex> {
        size_t operator()(const Vertex& vertex) const {
            size_t h1 = hash<glm::vec3>()(vertex.pos_);
            size_t h2 = hash<glm::vec3>()(vertex.norm_);
            size_t h3 = hash<glm::vec4>()(vertex.color_);
            size_t h4 = hash<glm::vec2>()(vertex.uv_);
            return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
        }
    };
}
