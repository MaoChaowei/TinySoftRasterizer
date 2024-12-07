#pragma once
#include"common/common_include.h"
#include"../vertex.h"
#include"../object.h"
#include"../texture.h"

enum class ShaderType{
    Default       ,
    Texture       ,
    Color         ,          
    Depth         ,
    Normal        ,
    Frame         ,
};

enum class ShaderSwitch{
    EarlyZtest      =1<<0,
    BackCulling     =1<<1,

    ALL_ON          = EarlyZtest|BackCulling,
};
inline ShaderSwitch operator|(const ShaderSwitch& s1,const ShaderSwitch& s2){
    return (ShaderSwitch)((int)(s1)|(int)(s2));
}
inline ShaderSwitch operator&(const ShaderSwitch& s1,const ShaderSwitch& s2){
    return (ShaderSwitch)((int)(s1)&(int)(s2));
}
inline ShaderSwitch operator^(const ShaderSwitch& s1,const ShaderSwitch& s2){
    return (ShaderSwitch)((int)(s1)^(int)(s2));
}

struct ShaderSetting{
    ShaderType type;
    ShaderSwitch flags;
};


// per-primitive design
struct ShaderContentRecord{

    // vertex shader output
    const Vertex* v[3];             // I can get all attribute of vetices~
    PrimitiveType primitive_type;   // get this after assembling of primitive

    // fragment shader output
    glm::vec4 color;
    float depth;
    glm::vec3 normal;
    
};

class Shader{
public:
    Shader(){}
    
    inline void bindTransform(glm::mat4* m){ transform_=m;}
    inline void bindNormalMat(glm::mat4* m){ normal_mat_=m;}
    inline void bindModelMat(glm::mat4* m){ model_mat_=m;}
    inline void setPrimitiveType( PrimitiveType t){ content_.primitive_type=t; }
    inline void setShaderSetting(const ShaderSetting& set){ setting_=set; }
    inline void setFrustum(int near,int far){
        near_plane_=near;
        far_plane_=far;
    }
    
    inline ShaderType getType()const{ return setting_.type;}
    inline glm::vec4 getColor()const { return content_.color; }
    inline float getDepth() const { return content_.depth; }
    inline glm::vec3 getScreenPos(uint32_t idx) const { return content_.v[idx]->s_pos_; }
 
    inline bool checkFlag(ShaderSwitch flag){
        return (bool)(flag&setting_.flags);
    }

    inline void assemblePrimitive(const Vertex* v1,const Vertex* v2,const Vertex* v3){
        content_.v[0]=v1;
        content_.v[1]=v2;
        content_.v[2]=v3;
    }

    /**
     * @brief vertex shader: convert vertex from model-space to screen-space 
     *        => x and y in [width,height], z in [-1,1];
     *        convert normals into world space
     * @param v : the vertex information holder
     */
    void vertexShader(Vertex& v );
    
    /**
     * @brief shading the fragment with properties set in `ShaderType`.
     * 
     * @param xy : refer to the position in screen-space
     * @param cur_depth : refer to the current z-buffer(x,y) so that I can finish Early-Z Test here.
     * @return if the fragment can be accepted for now, than return `true`, ortherwise return `false`.
     */
    bool fragmentShader(uint32_t x,uint32_t y,float cur_depth);


protected:
    ShaderContentRecord content_;
    ShaderSetting setting_;
    glm::mat4 * transform_;     // mvpv
    glm::mat4 * normal_mat_;    // m-1T
    glm::mat4 * model_mat_;     // m
    std::shared_ptr<Texture> texture_;

    float far_plane_;
    float near_plane_;

};





