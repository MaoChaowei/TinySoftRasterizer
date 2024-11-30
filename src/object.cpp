#include"object.h"

//---------------------Mesh--------------------------//

void Mesh::initObject(const tinyobj::shape_t& info,const tinyobj::attrib_t& attrib){
    name_=info.name;
    face_num_=info.mesh.num_face_vertices.size();
    vertices_.resize(face_num_*3);   // each face is a triangle

    int vnum=vertices_.size();
    for(int i=0;i<vnum;++i){
        // access to vertex
        tinyobj::index_t idx = info.mesh.indices[i];
        tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
        tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
        tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];
        vertices_[i].pos_=glm::vec3(vx,vy,vz);


        // negative = no normal data
        if (idx.normal_index >= 0) {
            tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
            tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
            tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];
            vertices_[i].norm_=glm::vec3(nx,ny,nz);
        }

        // negative = no texcoord data
        if (idx.texcoord_index >= 0) {
            tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
            tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
            vertices_[i].uv_=glm::vec2(tx,ty);
        }

        //vertex colors
        tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
        tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
        tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
        vertices_[i].color_=glm::vec4(red,green,blue,1.0f);
    }

}

void Mesh::setTriangleDemo() {
    // 清空现有数据
    vertices_.clear();
    face_num_ = 1; // 1 个三角形

    // 设置三个顶点的属性
    vertices_.resize(3);

    // 顶点 1
    vertices_[0].pos_ = glm::vec3(0.0f, 0.0f, 0.0f);    // 坐标
    vertices_[0].norm_ = glm::vec3(0.0f, 0.0f, 1.0f);   // 法线
    vertices_[0].color_ = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // 红色
    vertices_[0].uv_ = glm::vec2(0.0f, 0.0f);           // 纹理坐标

    // 顶点 2
    vertices_[1].pos_ = glm::vec3(1.0f, 0.0f, 0.0f);    // 坐标
    vertices_[1].norm_ = glm::vec3(0.0f, 0.0f, 1.0f);   // 法线
    vertices_[1].color_ = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f); // 绿色
    vertices_[1].uv_ = glm::vec2(1.0f, 0.0f);           // 纹理坐标

    // 顶点 3
    vertices_[2].pos_ = glm::vec3(0.0f, 1.0f, 0.0f);    // 坐标
    vertices_[2].norm_ = glm::vec3(0.0f, 0.0f, 1.0f);   // 法线
    vertices_[2].color_ = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); // 蓝色
    vertices_[2].uv_ = glm::vec2(0.0f, 1.0f);           // 纹理坐标
}

void Mesh::printMeshInfo() const {
    std::cout << "Mesh Name: " << name_ << "\n";
    std::cout << "Number of Faces: " << face_num_ << "\n";
    std::cout << "Number of Vertices: " << vertices_.size() << "\n";

    for (size_t i = 0; i < vertices_.size(); ++i) {
        const auto& v = vertices_[i];
        std::cout << "Vertex " << i + 1 << ":\n";
        std::cout << "  Position: (" << v.pos_.x << ", " << v.pos_.y << ", " << v.pos_.z << ")\n";
        std::cout << "  Normal:   (" << v.norm_.x << ", " << v.norm_.y << ", " << v.norm_.z << ")\n";
        std::cout << "  Color:    (" << v.color_.r << ", " << v.color_.g << ", " << v.color_.b << ", " << v.color_.a << ")\n";
        std::cout << "  UV:       (" << v.uv_.x << ", " << v.uv_.y << ")\n";
    }
}

//---------------------Line--------------------------//
void Line::initObject(const tinyobj::shape_t& info,const tinyobj::attrib_t& attrib){
    name_=info.name;
    line_num_=info.lines.num_line_vertices.size()+1;
    vertices_.resize(line_num_-1);  

    int vnum=vertices_.size();
    for(int i=0;i<vnum;++i){
        // access to vertex
        tinyobj::index_t idx = info.lines.indices[i];
        tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
        tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
        tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];
        vertices_[i].pos_=glm::vec3(vx,vy,vz);

        // negative = no normal data
        if (idx.normal_index >= 0) {
            tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
            tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
            tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];
            vertices_[i].norm_=glm::vec3(nx,ny,nz);
        }

        // negative = no texcoord data
        if (idx.texcoord_index >= 0) {
            tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
            tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
            vertices_[i].uv_=glm::vec2(tx,ty);
        }

        //vertex colors
        tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
        tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
        tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
        vertices_[i].color_=glm::vec4(red,green,blue,1.0f);
    }
}


//---------------------ObjLoader--------------------------//


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

// void  ObjLoader::setMesh(){
//     auto& attrib = reader_.GetAttrib();
//     auto& shapes = reader_.GetShapes();

//     total_shapes_=shapes.size();
//     meshes_.resize(total_shapes_);

//     for(int i=0;i<total_shapes_;++i){
//         meshes_[i] = std::make_unique<Mesh>(); // 初始化 unique_ptr
//         meshes_[i]->initObject(shapes[i],attrib);
//     }
// }

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
