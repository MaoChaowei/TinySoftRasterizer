/* `Camera` defines the virtual camera and the properties of the final image */
#pragma once
#include"common_include.h"


//@todo: interaction operations, such as move and rotate
class Camera{
public:
    Camera():position_(glm::vec3(0.f)),front_(glm::vec3(0,0,-1)),right_(glm::vec3(1,0,0)),
			up_(glm::vec3(0,1,0)),image_width_(1000),fov_(16.0/9.0){

		image_height_=(int)(image_width_/aspect_ratio_);
		aspect_ratio_=image_width_/(float)image_height_;

		half_near_height_=tan(glm::radians(fov_/2))*near_flat_z_;
		half_near_width_=aspect_ratio_*half_near_height_;
		
	}
	/**
	 * @brief Construct a new camera object
	 * 
	 * @param pos : camera position in world
	 * @param lookat ：any point in camera's lookat direction
	 * @param right ：don't need to normalize
	 * @param fov ：vertical fov in degree
	 * @param ratio
	 * @param image_width 
	 */
	Camera(glm::vec3 pos,glm::vec3 lookat,glm::vec3 right,float fov=60,float ratio=16.0/9.0,int image_width=1000)
	:position_(pos),right_(glm::normalize(right)),fov_(fov),aspect_ratio_(ratio),image_width_(image_width)
	{
		image_height_=(int)(image_width/aspect_ratio_);
		aspect_ratio_=image_width/(float)image_height_;

		half_near_height_=tan(glm::radians(fov/2))*near_flat_z_;
		half_near_width_=aspect_ratio_*half_near_height_;

		front_=glm::normalize(lookat-pos);
		up_=glm::cross(right_,front_);

	}
	
    void updateCamera(glm::vec3 pos,glm::vec3 lookat,glm::vec3 right,float fov=60,float ratio=16.0/9.0,int image_width=1000);

    glm::mat4 getViewMatrix()const;
	glm::mat4 getPerspectiveMatrix()const;
	glm::mat4 getViewportMatrix()const;

    void setFrastrum(float near,float far);

    inline const glm::vec3 getPosition()const{return this->position_;}
    inline const glm::vec3 getFront()const{return this->front_;}
    inline const glm::vec3 getRight()const{return this->right_;}
    inline const glm::vec3 getUp()const{return this->up_;}
    inline const int getImageWidth()const{return this->image_width_;}
    inline const int getImageHeight()const{return this->image_height_;}

private:
	// Camera coordinate system in world coordinate system
	glm::vec3 position_;
	glm::vec3 front_;	// real direction
	glm::vec3 right_;
	glm::vec3 up_;

	// define the image and near flat
	// vertical fov
	float fov_=60;
	float aspect_ratio_=16.0/9.0;
	int image_width_,image_height_;
	float half_near_height_,half_near_width_;

	//for frustrum
	float near_flat_z_=1;
	float far_flat_z_=1000;

};
