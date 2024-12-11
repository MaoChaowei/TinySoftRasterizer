#include"utils.h"

// some small functions
namespace utils{

void printvec3(glm::vec3 v,std::string str=""){
    if(str.size()) std::cout<<str;
    std::cout<<": { "<<v.x<<", "<<v.y<<", "<<v.z<<"}\n";
}


}