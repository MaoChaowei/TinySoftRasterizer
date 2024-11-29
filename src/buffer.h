#include"common_include.h"


class ColorBuffer{
public:
    
    ColorBuffer()=delete;
    ColorBuffer(const ColorBuffer& other)=delete;
    ColorBuffer& operator=(const ColorBuffer& other)=delete;
    ColorBuffer(ColorBuffer&&) = delete;
    ColorBuffer& operator=(ColorBuffer&&) = delete;

    ColorBuffer(int width,int height):width_(width),height_(height){
        addr_=new unsigned char[width*height*4];
    }

    inline void cleanBuffer(const unsigned char num=0){
        std::fill(addr_,addr_+width_*height_*4,num);
    }
    inline void reSetBuffer(int width,int height){
        width_=width;
        height_=height;
        delete []addr_;
        addr_=new unsigned char[width*height*4];
    }
    inline unsigned char* getAddr()const{return addr_;}

    inline void setPixel(int x,int y,glm::vec4 color)const{
        int idx=y*width_+x;
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
    // viewport的地址（x，y）按照：左上角为（0,0），右下角为（width,height）
    unsigned char* addr_;

};