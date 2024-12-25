#pragma once
#include"common/common_include.h"
#include"common/AABB.h"
#include"buffer.h"

class HZbuffer{
public:
    HZbuffer(){};
    HZbuffer(uint32_t w,uint32_t h){
        assert(w%2==0&&h%2==0);

        fineset_level_=std::log2(std::min((int)width_,(int)height_));
        hzb_.resize(fineset_level_+1);
        for(int i=fineset_level_;i>=0;--i){
            hzb_[i].reSetBuffer(w,h);
            w/=2;
            h/=2;
        }
    }

    void updateDepth(uint32_t x,uint32_t y,float depth){
        for(int i=fineset_level_;i>=0;--i){
            if(i<fineset_level_&&depth<hzb_[i].getDepth(x,y)){
                return;
            }
            hzb_[i].setDepth(x,y,depth);
            x/=2;
            y/=2;
        }
    }

    bool rapidRefuseBox(const AABB2d& box,float nearest_depth){
        int box_length=std::max(box.max.y-box.min.y,box.max.x-box.min.x);
        int cur_level=getLevel(box_length);
        float inv_unit=1.0/(std::pow(2,fineset_level_-cur_level));

        int left=std::floor(box.min.x*inv_unit);
        int right=std::floor(box.max.x*inv_unit);
        int top=std::ceil(box.max.y*inv_unit);
        int down=std::ceil(box.min.y*inv_unit);

        for(int r=top;r>=down;--r){
            for(int c=left;c<=right;++c){
                float depth=hzb_[cur_level].getDepth(c,r);

                if(depth<nearest_depth){
                    return false;
                }
            }
        }

        return  true;
    }

    // map coarsest_length from the coarsest level to a higher level, where there are 4 depthes at most covering coarsest_length.
    int getLevel(float coarsest_length){
        return std::min((int)std::ceil(std::log2(std::max(coarsest_length,1.f))),(int)fineset_level_);
    }


private:
    uint32_t width_;
    uint32_t height_;
    uint32_t fineset_level_;
    std::vector<DepthBuffer> hzb_;
    
};