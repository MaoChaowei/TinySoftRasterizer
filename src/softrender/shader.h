#pragma once
#include"common/common_include.h"
#include"common/cputimer.h"
#include"vertex.h"
#include"object.h"
#include"material.h"
#include"light.h"
#include"camera.h"

// specify object's shader type
enum class ShaderType{
    // decide how to fetch color
    Texture       =1<<0,
    Color         =1<<1,         
    Depth         =1<<2,
    Normal        =1<<3,
    Frame         =1<<4,
    
    // decide shader's type
    BlinnPhone    =1<<5,
    Light         =1<<6,

    ORDER=Texture|Color|Normal|Frame,
    LIGHTSHADER=BlinnPhone,

};
inline ShaderType operator|(const ShaderType& s1,const ShaderType& s2){
    return (ShaderType)((int)(s1)|(int)(s2));
}
inline ShaderType operator&(const ShaderType& s1,const ShaderType& s2){
    return (ShaderType)((int)(s1)&(int)(s2));
}

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


// per-primitive design,make sure each fragment knows its' context in the pipeline
struct ShaderContentRecord{

    // vertex shader output
    Vertex* v[3];             // all attribute of vetices
    PrimitiveType primitive_type;   

    // fragment shader input
    float depth;
    glm::vec3 normal;
    glm::vec3 vbary;        // barycenter in view space
    

    // fragment shader output
    glm::vec4 color;
    
};

class Shader{
public:
    Shader(){}
    
    inline void bindMVP(glm::mat4* m){ mvp_=m;}
    inline void bindViewport(glm::mat4* m){ viewport_=m;}
    inline void bindNormalMat(glm::mat4* m){ normal_mat_=m;}
    inline void bindModelMat(glm::mat4* m){ model_mat_=m;}
    inline void bindMaterial(std::shared_ptr<Material> mp){ material_=mp; }
    inline void bindCamera(std::shared_ptr<Camera> cp){ camera_=cp; }
    inline void bindLights(const std::vector<std::shared_ptr<Light>>& light){ lights_=light; }
    inline void bindTimer(CPUTimer* t){ timer_=t; }

    inline void setShaderType(ShaderType st){type_=st;}
    inline void setPrimitiveType( PrimitiveType t){ content_.primitive_type=t; }
    inline void setShaderSwitch(const ShaderSwitch& set){ flags_=set; }
    inline void setFrustum(int near,int far){
        near_plane_=near;
        far_plane_=far;
    }
    
    inline ShaderType getType()const{ return type_;}
    inline glm::vec4 getColor()const { return content_.color; }
    inline float getDepth() const { return content_.depth; }
    inline glm::vec3 getScreenPos(uint32_t idx) const { return content_.v[idx]->s_pos_; }
 
    inline bool checkFlag(ShaderSwitch flag){
        return (bool)(flag&flags_);
    }
    inline bool checkShader(ShaderType flag){
        return (bool)(flag&type_);
    }

    inline void assemblePrimitive(Vertex* v1, Vertex* v2, Vertex* v3){
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
    void vertex2Screen(Vertex& v );
    /**
     * @brief shading the fragment with properties set in `ShaderType`.
     * 
     * @param xy : refer to the position in screen-space
     * @param cur_depth : refer to the current z-buffer(x,y) so that I can finish Early-Z Test here.
     * @return if the fragment can be accepted for now, than return `true`, ortherwise return `false`.
     */
    bool fragmentShader(uint32_t x,uint32_t y,float cur_depth);
    bool fragmentInterp(uint32_t x,uint32_t y,float cur_depth);

    // different shaders
    void blinnphoneShader(Material& mtl);
    void textureShader();
    void colorShader();
    void depthShader();
    void normalShader();
    void lightShader();

    // local light calculation 
    void shadeDirectLight(const DirLight& light,const glm::vec3& normal,const glm::vec3& eyepos,const glm::vec3& fpos,const Material& mtl);
    void shadePointLight(const PointLight& light,const glm::vec3& normal,const glm::vec3& eyepos,const glm::vec3& fpos,const Material& mtl);


private:
    ShaderContentRecord content_;
    ShaderType type_;
    ShaderSwitch flags_;

    glm::mat4 * mvp_;
    glm::mat4 * viewport_;     // mvpv
    glm::mat4 * normal_mat_;    // m-1T
    glm::mat4 * model_mat_;     // m
    std::shared_ptr<Material> material_;
    std::vector<std::shared_ptr<Light>> lights_;
    std::shared_ptr<Camera> camera_;

    float far_plane_;
    float near_plane_;

    CPUTimer* timer_;

};





