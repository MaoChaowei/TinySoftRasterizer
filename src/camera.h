/* `Camera` defines the virtual camera and the properties of the final image */
#pragma once
#include"common_include.h"

enum class CameraMovement{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	REFRESH,
};

class Camera{
public:
    Camera():position_(glm::vec3(0.f)),front_(glm::vec3(0,0,-1)),right_(glm::vec3(1,0,0)),
			up_(glm::vec3(0,1,0)),image_width_(1000),fov_(60){
		image_width_+=image_width_%2;
		image_height_=(int)(image_width_/aspect_ratio_);
		image_height_+=image_height_%2;
		aspect_ratio_=image_width_/(float)image_height_;

		half_near_height_=tan(glm::radians(fov_/2))*near_flat_z_;
		half_near_width_=aspect_ratio_*half_near_height_;

		yaw_   = -90.0f; 
    	pitch_ = 0.0f;
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
		image_width_+=image_width_%2;
		image_height_=(int)(image_width/aspect_ratio_);
		image_height_+=image_height_%2;
		aspect_ratio_=image_width/(float)image_height_;

		half_near_height_=tan(glm::radians(fov/2))*near_flat_z_;
		half_near_width_=aspect_ratio_*half_near_height_;

		front_=glm::normalize(lookat-pos);
		up_=glm::cross(right_,front_);

		pitch_ = glm::degrees(asin(front_.y));
		yaw_   = glm::degrees(atan2(front_.z, front_.x));
		if (pitch_ > 89.0f)  pitch_ = 89.0f;
		if (pitch_ < -89.0f) pitch_ = -89.0f;

	}
	void setViewport(uint32_t width,float ratio,float fov){
		image_width_=width;
		image_width_+=image_width_%2;
		image_height_=(int)(image_width_/ratio);
		image_height_+=image_height_%2;
		aspect_ratio_=image_width_/(float)image_height_;
		fov_=fov;

		half_near_height_=tan(glm::radians(fov_/2))*near_flat_z_;
		half_near_width_=aspect_ratio_*half_near_height_;
	}
	
    void updateCamera(glm::vec3 pos,glm::vec3 lookat,glm::vec3 right,float fov=60,float ratio=1.0,int image_width=1024);

    glm::mat4 getViewMatrix()const;
	glm::mat4 getPerspectiveMatrix()const;
	glm::mat4 getViewportMatrix()const;
	float getNear()const{return near_flat_z_;}
	float getFar()const{return far_flat_z_;}

    void setFrastrum(float near,float far);

    inline const glm::vec3 getPosition()const{return this->position_;}
    inline const glm::vec3 getFront()const{return this->front_;}
    inline const glm::vec3 getRight()const{return this->right_;}
    inline const glm::vec3 getUp()const{return this->up_;}
    inline const int getImageWidth()const{return this->image_width_;}
    inline const int getImageHeight()const{return this->image_height_;}
	inline const bool needUpdateView(){
		if(update_flag_){
			update_flag_=false;
			return true;
		}
		return false;
	}

	// ui
	void setMovement(float spd,float sensi);
	void processKeyboard(CameraMovement type,float delta);
	void processMouseMovement(float xoffset, float yoffset);
	void updateCameraVectors();


private:
	// Camera coordinate system in world coordinate system
	glm::vec3 position_;
	glm::vec3 front_;	// real direction
	glm::vec3 right_;
	glm::vec3 up_;

	bool update_flag_=false;		// turn true when camera moves and turn false when checked by `needUpdateMatrices`

	float speed_=0.8f;
	float sensitivity_=0.1f;
	float yaw_=0.f;    // horizental
    float pitch_=0.f;  // vertical

	// define the image and near flat
	
	float fov_=60;	// vertical fov
	float aspect_ratio_=16.0/9.0;
	int image_width_,image_height_;
	float half_near_height_,half_near_width_;

	//for frustrum
	float near_flat_z_=1;
	float far_flat_z_=1000;

};
