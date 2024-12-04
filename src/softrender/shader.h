#pragma once
#include"common_include.h"
#include"vertex.h"

enum class ShaderType{
    Texture,
    Color,
    Depth,
    Normal,
};

// // encapsulate info for drawTriangle
// struct TriangelRecord{
//     // position in screen space
//     glm::vec3 p[3];
//     // pointer to attribute of vertex         
//     const Vertex* v[3];
// };

// per-primitive design
struct ShaderContentRecord{
    
    // vertex shader output
    glm::vec3 p[3];                 // x,y in screen-space and z in [0,1] where 0 is the near-flat 
    const Vertex* v[3];             // I can get all attribute of vetices~
    PrimitiveType primitive_type;   // get this after assembling of primitive

    // fragment shader output
    glm::vec4 color;
    float depth;
    
};

class Shader{
public:
    Shader():type_(ShaderType::Depth){}
    
    inline void bindTransform(glm::mat4* m){ transform_=m;}
    inline void setPrimitiveType( PrimitiveType t){ content_.primitive_type=t; }
    inline void setShaderType(ShaderType s){ type_=s; }

    inline ShaderType getType()const{ return type_;}
    inline glm::vec4 getColor()const { return content_.color; }
    inline float getDepth() const { return content_.depth; }
    inline glm::vec3 getPoint2d(uint32_t idx) const { return content_.p[idx]; }
    inline void clear(){
        vertex_map_.clear();
    }

    /**
     * @brief vertex shader: convert vertex from model-space to screen-space 
     *        => x and y in [width,height], z in [0,1]
     * @param idx : vertex shader is per-fragment, and I use `idx` to define which vertex it is in its Primitive 
     * @param v : the original vertex information
     */
    inline void vertexShader(uint32_t idx, Vertex& v ){
        auto it=std::make_shared<Vertex>(v);
        if(vertex_map_.find(it)==vertex_map_.end()){
            // haven't calculate this before,then:
            glm::vec4 npos=(*transform_)*glm::vec4(v.pos_,1.0f);
            vertex_map_[it]=npos/npos.w;
        }
        // update corresponding context of `ShaderContentRecord`
        content_.p[idx]=vertex_map_[it];
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
            if(cur_depth<content_.depth)    // TODO: add early z test on-off
                return false;

            // shading~
            if(ShaderType::Color==type_){
                for(int i=0;i<4;++i)
                    content_.color[i]=glm::dot(glm::vec3(content_.v[0]->color_[i],content_.v[1]->color_[i],content_.v[2]->color_[i]),bary);
            }
            else if(ShaderType::Depth==type_){
                content_.color=glm::vec4(content_.depth*255.0f);
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
    ShaderType type_;
    glm::mat4 * transform_;
    // for memerized search: in a single pass, I use this to record the screen-space position of each vertex
    std::unordered_map<std::shared_ptr<Vertex>,glm::vec3> vertex_map_;

};





