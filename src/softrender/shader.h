#pragma once
#include"common_include.h"
#include"vertex.h"

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

struct ShaderSetting{
    ShaderType type;
    ShaderSwitch flags;
};


// per-primitive design
struct ShaderContentRecord{
    
    // vertex shader output
    glm::vec3 p[3];                 // x,y in screen-space and z in [0,1] where 0 is the near-flat 
    glm::vec3 n[4];                 // n[0~2]:normals of vertices in world space, that is norm_world=Model_mat^-1^T*norm_model.n[3]is the face normal
    const Vertex* v[3];             // I can get all attribute of vetices~
    PrimitiveType primitive_type;   // get this after assembling of primitive

    // fragment shader output
    glm::vec4 color;
    float depth;
    glm::vec3 normal;
    
};

// vertex's single pass holder
struct VertexSPHolder{
    glm::vec3 pos;                  // position in screen space  
    glm::vec3 norm;                 // normal in world space
};

class Shader{
public:
    Shader(){}
    
    inline void bindTransform(glm::mat4* m){ transform_=m;}
    inline void bindNormalMat(glm::mat4* m){ normal_mat_=m;}
    inline void setPrimitiveType( PrimitiveType t){ content_.primitive_type=t; }
    inline void setShaderSetting(const ShaderSetting& set){ setting_=set; }
    

    inline ShaderType getType()const{ return setting_.type;}
    inline glm::vec4 getColor()const { return content_.color; }
    inline float getDepth() const { return content_.depth; }
    inline glm::vec3 getPoint2d(uint32_t idx) const { return content_.p[idx]; }
    inline void clear(){ vertex_map_.clear(); }
    bool checkFlag(ShaderSwitch flag){
        return (bool)(flag|setting_.flags);
    }


    /**
     * @brief vertex shader: convert vertex from model-space to screen-space 
     *        => x and y in [width,height], z in [0,1]
     * @param idx : vertex shader is per-fragment, and I use `idx` to define which vertex it is in its Primitive 
     * @param v : the original vertex information
     */
    inline void vertexShader(uint32_t idx, Vertex& v ){
        auto it=std::make_shared<Vertex>(v);
        if(vertex_map_.find(it)==vertex_map_.end()){// haven't calculate this before,then:
            glm::vec4 npos=(*transform_)*glm::vec4(v.pos_,1.0f);
            vertex_map_[it].pos=npos/npos.w;
            // map vertex's normal to world space
            vertex_map_[it].norm=(*normal_mat_)*glm::vec4(v.norm_,0.0f);
        }
        // update corresponding context of `ShaderContentRecord`
        content_.p[idx]=vertex_map_[it].pos;
        content_.n[idx]=vertex_map_[it].norm;
        content_.v[idx]=&v;
    }

    /**
     * @brief shading the fragment with properties set in `ShaderType`.
     * 
     * @param xy : refer to the position in screen-space
     * @param cur_depth : refer to the current z-buffer(x,y) so that I can finish Early-Z Test here.
     * @return if the fragment can be accepted for now, than return `true`, ortherwise return `false`.
     */
    bool fragmentShader(uint32_t x,uint32_t y,float cur_depth){
        auto& otype=content_.primitive_type;
        if(otype==PrimitiveType::MESH){
            // getBaryCenter
            glm::vec3 bary=utils::getBaryCenter(content_.p[0],content_.p[1],content_.p[2],glm::vec2(x,y));
            if(bary.x<0||bary.y<0||bary.z<0)
                return false;

            // depth
            content_.depth=glm::dot(glm::vec3(content_.p[0].z,content_.p[1].z,content_.p[2].z),bary);
            if(checkFlag(ShaderSwitch::EarlyZtest)&&cur_depth<content_.depth)
                return false;

            // normal 
            for(int i=0;i<3;++i)
                content_.normal[i]=glm::dot(glm::vec3(content_.n[0][i],content_.n[1][i],content_.n[2][i]),bary);

            // shading
            switch(setting_.type){
                case ShaderType::Color:
                    for(int i=0;i<4;++i)
                        content_.color[i]=glm::dot(glm::vec3(content_.v[0]->color_[i],content_.v[1]->color_[i],content_.v[2]->color_[i]),bary);
                    break;

                case ShaderType::Depth:
                    content_.color=glm::vec4(content_.depth*255.0f);
                    break;

                case ShaderType::Normal:
                // 法向量在z轴方向上的可视化
                    content_.color=glm::vec4(content_.normal[2]*255.0f);
                    break;

                case ShaderType::Frame:
                    if(bary.x==0||bary.y==0||bary.z==0)
                        content_.color=glm::vec4(255.0f);
                    break;

                case ShaderType::Texture:
                    break;   

                default:
                    break;                                           
            }
            
            
            return true;
        }
        else if(otype==PrimitiveType::LINE){
            std::cout<<"fragmentShader: I haven't supported LINE rendering yet~\n";
        }
        else{
            std::cout<<"fragmentShader:Actually, I haven't supported anything except MESH rendering...\n";
        }
        
        return true;
    }

protected:
    ShaderContentRecord content_;
    ShaderSetting setting_;
    glm::mat4 * transform_;
    glm::mat4 * normal_mat_;
    // for memerized search: in a single pass, I use this to record the screen-space position of each vertex
    std::unordered_map<std::shared_ptr<Vertex>,VertexSPHolder> vertex_map_;

};





