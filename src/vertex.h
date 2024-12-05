#pragma once
#include"common_include.h"


struct Vertex{
    glm::vec3 pos_;      // 模型局部坐标系下的位置
    glm::vec3 norm_;    // 需要根据(（model）^-1)T 矩阵进行变换
    glm::vec4 color_;
    glm::vec2 uv_;

    Vertex():pos_(glm::vec3(0.f)),norm_(glm::vec3(0.f)),color_(glm::vec4(-1.f)),uv_(glm::vec2(-1.f)){};
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