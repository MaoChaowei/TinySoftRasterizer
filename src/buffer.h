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
        pixel_num_=width*height;
    }

    inline void clear(const unsigned char num=0){
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
        width_=width;
        height_=height;
        pixel_num_=width*height;
        zbuffer_.resize(width*height);
        std::fill(zbuffer_.begin(),zbuffer_.end(),1.0f);
    }
    inline void reSetBuffer(int width,int height){
        width_=width;
        height_=height;
        pixel_num_=width*height;
        zbuffer_.resize(width*height);
        std::fill(zbuffer_.begin(),zbuffer_.end(),1.0f);
    }

    void clear(){
        // 1.0F is the farest in NDC space
        std::fill(zbuffer_.begin(),zbuffer_.end(),1.0f);
    }

    void setDepth(int x,int y,float depth){
        int idx=y*width_+x;
        if(idx>=pixel_num_){
            std::cerr<<"setDepth: (x,y) out of range!x="<<x<<",y="<<y<<std::endl;
            exit(-1);
        }
        zbuffer_[idx]=depth;
    }

    inline std::shared_ptr<std::vector<float>> getZbuffer(){
        return std::make_shared<std::vector<float>>(zbuffer_);
    }
    /**
     * depth test on (x,y): check wheather current z-depth of (x,y) is closer than `depth`. if so,
     * fail the depth test and return False, on the contrary, update z-buffer and return True. 
     */
    inline bool zTest(int x,int y,float depth){
        int idx=y*width_+x;
        if(idx>=pixel_num_){
            std::cerr<<"zTest: (x,y) out of range!x="<<x<<",y="<<y<<std::endl;
            exit(-1);
        }

        if(depth>zbuffer_[idx]) 
            return false;

        zbuffer_[idx]=depth;
        return true;
    }

    inline float getDepth(int x,int y){
        int idx=y*width_+x;
        if(idx>=pixel_num_){
            std::cerr<<"zTest: (x,y) out of range!x="<<x<<",y="<<y<<std::endl;
            exit(-1);
        }
        return zbuffer_[idx];
    }

private:
    int width_;
    int height_;
    int pixel_num_;
    std::vector<float> zbuffer_;

};
 