#include"common_include.h"

// struct Color{
//     char r,g,b,a;
//     Color():r(0),g(0),b(0),a(1){}
// };

class ColorBuffer{
public:
    
    ColorBuffer()=delete;
    ColorBuffer(const ColorBuffer& other)=delete;
    ColorBuffer& operator=(const ColorBuffer& other)=delete;
    ColorBuffer(ColorBuffer&&) = delete;
    ColorBuffer& operator=(ColorBuffer&&) = delete;

    ColorBuffer(int width,int height):width_(width),height_(height){
        addr_=new unsigned char[width*height*4];
        pixel_num_=width*height;
    }

    inline void cleanBuffer(const unsigned char num=0){
        std::fill(addr_,addr_+width_*height_*4,num);
    }
    inline void reSetBuffer(int width,int height){
        width_=width;
        height_=height;
        delete []addr_;
        addr_=new unsigned char[width*height*4];
        pixel_num_=width*height;
    }
    inline unsigned char* getAddr()const{return addr_;}

    inline void setPixel(int x,int y,glm::vec4 color)const{
        int idx=y*width_+x;
        if(idx>=pixel_num_||idx<0) 
            return;

        for(int i=0;i<4;++i)
            addr_[idx*4+i]=color[i];
    }

    ~ColorBuffer(){
        delete[]addr_;
    }

private:
    int width_;
    int height_;
    // addr_:帧缓存起始地址，采用rgba四字节连续存储的方式表示一个像素的颜色，
    // viewport的地址（x，y）按照：左下角为（0,0），右上角为（width,height）
    unsigned char* addr_;
    int pixel_num_=0;

}; 


class DepthBuffer{
public:
    DepthBuffer(int width,int height){
        zbuffer_.resize(width*height);
    }
    inline void reSize(int width,int height){
        zbuffer_.resize(width*height);
    }

    void cleanZbuffer(){
        // -1.0F is the farest in NDC space
        std::fill(zbuffer_.begin(),zbuffer_.end(),-1.0f);
    }

    void setDepth(int x,int y,float depth){
        zbuffer_[y*width_+x]=depth;
    }

    inline std::shared_ptr<std::vector<float>> getZbuffer(){
        return std::make_shared<std::vector<float>>(zbuffer_);
    }

private:
    int width_;
    int height_;
    std::vector<float> zbuffer_;

};
 