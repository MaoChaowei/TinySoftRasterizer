#include"object.h"


 void Mesh::clear() {
    vertices_.clear();
    vidx_arr_.clear();
}

// 设置一个简单的三角形示例
void Mesh::setTriangleDemo() {
    vertices_ = {
        {{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.5f, 1.0f}}, // 顶点 0
        {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // 顶点 1
        {{1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 0.0f}}  // 顶点 2
    };

    vidx_arr_ = {0, 1, 2};
}

// 打印网格数据
void Mesh::printMeshInfo() const {
    std::cout << "Vertices:\n";
    for (const auto& vertex : vertices_) {
        std::cout << "Position: (" << vertex.position_.x << ", " << vertex.position_.y << ", " << vertex.position_.z
                    << "), Normal: (" << vertex.normal_.x << ", " << vertex.normal_.y << ", " << vertex.normal_.z
                    << "), Color: (" << vertex.color_.r << ", " << vertex.color_.g << ", " << vertex.color_.b << ", " << vertex.color_.a
                    << "), UV: (" << vertex.uv_.x << ", " << vertex.uv_.y << ")\n";
    }

    std::cout << "Indices:\n";
    for (size_t i = 0; i < vidx_arr_.size(); i += 3) {
        std::cout << "Triangle: " << vidx_arr_[i] << ", " << vidx_arr_[i + 1] << ", " << vidx_arr_[i + 2] << "\n";
    }
}

void Mesh::setObjectData(std::string filename){

}