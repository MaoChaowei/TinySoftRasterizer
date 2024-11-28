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

    inline void cleanBuffer(const unsigned char num){

        std::fill(addr_,addr_+width_*height_*4,num);
    }
    void reSetBuffer(int width,int height){
        width_=width;
        height_=height;
        delete []addr_;
        addr_=new unsigned char[width*height*4];
    }
    inline unsigned char* getAddr()const{return addr_;}

    ~ColorBuffer(){
        delete[]addr_;
    }

private:
    int width_;
    int height_;
    unsigned char* addr_;

};