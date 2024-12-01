#include"object.h"
#include <iomanip>
#include<glm/gtx/hash.hpp>

namespace object_tools{

Vertex extractVertex(const tinyobj::attrib_t& attrib,const tinyobj::index_t idx){
    Vertex temp;
    // access to vertex
    tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
    tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
    tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];
    temp.pos_=glm::vec3(vx,vy,vz);


    // negative = no normal data
    if (idx.normal_index >= 0) {
        tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
        tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
        tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];
        temp.norm_=glm::vec3(nx,ny,nz);
    }

    // negative = no texcoord data
    if (idx.texcoord_index >= 0) {
        tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
        tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
        temp.uv_=glm::vec2(tx,ty);
    }

    //vertex colors
    tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
    tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
    tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
    temp.color_=glm::vec4(red,green,blue,1.0f);

    return temp;
 }

}



//---------------------Mesh--------------------------//

void Mesh::initObject(const tinyobj::shape_t& info,const tinyobj::attrib_t& attrib){
    name_=info.name;
    face_num_=info.mesh.num_face_vertices.size();

    std::unordered_map<Vertex,uint32_t> ver2idx;

    for(auto& idx:info.mesh.indices){
        Vertex v=object_tools::extractVertex(attrib,idx);
        if(ver2idx.find(v)!=ver2idx.end()){
            // already preserved
            indices_.push_back(ver2idx[v]);
        }
        else{
            uint32_t t=vertices_.size();
            vertices_.push_back(v);
            indices_.push_back(t);
            ver2idx[v]=t;
        }
    }
}

void Mesh::setTriangleDemo() {
    // TODO
}


void Mesh::printInfo() const {
    std::cout << "Mesh Name: " << name_ << std::endl;
    std::cout << "Number of Vertices: " << vertices_.size() << std::endl;
    std::cout << "Number of Indices: " << indices_.size() << std::endl;
    std::cout << "Number of Faces (Triangles): " << indices_.size() / 3 << std::endl;

    // 打印顶点信息
    std::cout << "\nVertices:" << std::endl;
    for (size_t i = 0; i < vertices_.size(); ++i) {
        std::cout << "Vertex " << i << ": " << vertices_[i] << std::endl;
    }

    // 打印索引信息并还原三角形面
    std::cout << "\nTriangles:" << std::endl;
    size_t triangleCount = indices_.size() / 3;
    for (size_t i = 0; i < triangleCount; ++i) {
        uint32_t index0 = indices_[i * 3 + 0];
        uint32_t index1 = indices_[i * 3 + 1];
        uint32_t index2 = indices_[i * 3 + 2];

        // 输出三角形的索引
        std::cout << "Triangle " << i << ": Indices(" << index0 << ", " << index1 << ", " << index2 << ")" << std::endl;

        // 输出组成三角形的顶点信息
        std::cout << "    Vertex 0: " << vertices_[index0] << std::endl;
        std::cout << "    Vertex 1: " << vertices_[index1] << std::endl;
        std::cout << "    Vertex 2: " << vertices_[index2] << std::endl;
    }
}




//---------------------Line--------------------------//

void Line::initObject(const tinyobj::shape_t& info,const tinyobj::attrib_t& attrib){
    name_=info.name;
    line_num_=info.lines.indices.size()+1;

    std::unordered_map<Vertex,uint32_t> ver2idx;

    for(auto& idx:info.lines.indices){
        Vertex v=object_tools::extractVertex(attrib,idx);
        if(ver2idx.find(v)!=ver2idx.end()){
            // already preserved
            indices_.push_back(ver2idx[v]);
        }
        else{
            uint32_t t=vertices_.size();
            vertices_.push_back(v);
            indices_.push_back(t);
            ver2idx[v]=t;
        }
    }
}

void Line::printInfo() const {
    std::cout << "Line Name: " << name_ << std::endl;
    std::cout << "Number of Vertices: " << vertices_.size() << std::endl;
    std::cout << "Number of Indices: " << indices_.size() << std::endl;
    std::cout << "Number of Lines: " << indices_.size()+1 << std::endl;

    // 打印顶点信息
    std::cout << "\nVertices:" << std::endl;
    for (size_t i = 0; i < vertices_.size(); ++i) {
        std::cout << "Vertex " << i << ": " << vertices_[i] << std::endl;
    }
}

//---------------------ObjLoader--------------------------//

/**
 * @brief banking on `tiny-objloader` to finish obj-file resolving job~
 * everything is conserved in `reader_`
 * 
 * @param inputfile : relative searching route to obj file.
 */
void ObjLoader::readObjFile(std::string inputfile){

    if(inputfile.empty())
        inputfile=filename_;

    std::cout<<"Obj-Loader: Searching "<< inputfile <<std::endl;

    if (!reader_.ParseFromFile(inputfile)) {

        if (!reader_.Error().empty()) {
            std::cerr << "Obj-Loader: " << reader_.Error();
            std::cout<<"try to search ../assets/model/"<<std::endl;
        }

        inputfile = "../assets/model/cornell_box.obj";
        if (!reader_.ParseFromFile(inputfile)) {
            if (!reader_.Error().empty()) {
                std::cerr << "Obj-Loader: " << reader_.Error();
            }
            exit(1);
        }
    }
    if (!reader_.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader_.Warning();
    }

    std::cout<<"Obj-Loader: Successfully found .obj "<<std::endl;
}

/**
 * @brief retrive everything in `reader_` to my structure for further usage
 * 
 */
void ObjLoader::setObject(){
    auto& attrib = reader_.GetAttrib();
    auto& shapes = reader_.GetShapes();

    total_shapes_=shapes.size();
    
    for(int i=0;i<total_shapes_;++i){
        if(shapes[i].mesh.indices.size()){
            auto mesh = std::make_unique<Mesh>();
            mesh->initObject(shapes[i],attrib);
            meshes_.push_back(std::move(mesh)); 
        }
        if(shapes[i].lines.indices.size()){
            auto line = std::make_unique<Line>();
            line->initObject(shapes[i],attrib);
            lines_.push_back(std::move(line)); 
        }
        // todo : dot
    }
}
