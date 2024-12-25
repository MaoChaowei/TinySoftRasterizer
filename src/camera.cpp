#include"camera.h"

void Camera::updateCamera(glm::vec3 pos,glm::vec3 lookat,glm::vec3 right,float fov,float ratio,int image_width){
	position_=pos;
	front_=glm::normalize(lookat-position_);
	right_=right;
	fov_=fov;
	aspect_ratio_=ratio;

	up_=glm::cross(right_,front_);
	
	setViewport(image_width,ratio,fov);

}

void Camera::setMovement(float spd,float sensi){
	speed_=spd;
	sensitivity_=sensi;
}

void Camera::processKeyboard(CameraMovement type,float delta){
	float velocity=speed_*delta;
	switch(type){
		case CameraMovement::FORWARD:
			position_+=glm::vec3(velocity)*front_;
			update_flag_=true;
			break;
		case CameraMovement::BACKWARD:
			position_-=glm::vec3(velocity)*front_;
			update_flag_=true;
			break;
		case CameraMovement::LEFT:
			position_-=glm::vec3(velocity)*right_;
			update_flag_=true;
			break;
		case CameraMovement::RIGHT:
			position_+=glm::vec3(velocity)*right_;
			update_flag_=true;
			break;
		case CameraMovement::REFRESH:
			position_=glm::vec3(0.f);
			front_=glm::vec3(0,0,-1);
			right_=glm::vec3(1,0,0);
			up_=glm::vec3(0,1,0);
			update_flag_=true;
			break;
		default:
			break;
	}
}


void Camera::processMouseMovement(float xoffset, float yoffset) {

    xoffset *= sensitivity_;
    yoffset *= sensitivity_;

    yaw_ += xoffset;
    pitch_ += yoffset;

    // in case of turning over
    if (pitch_ > 89.0f) pitch_ = 89.0f;
    if (pitch_ < -89.0f) pitch_ = -89.0f;

    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    direction.y = sin(glm::radians(pitch_));
    direction.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_ = glm::normalize(direction);

    right_ = glm::normalize(glm::cross(front_, glm::vec3(0.0f, 1.0f, 0.0f)));
    up_ = glm::normalize(glm::cross(right_, front_));

	update_flag_=true;
}

/**
 * @brief Calculate the view matrix. Aligns `right_` to x-axis, `up_` to y-axis,
 *        `front_` to negative z-axis, and sets `position_` to the origin.
 * 
 * @return glm::mat4 The calculated view matrix.
 */
glm::mat4 Camera::getViewMatrix()const{
	glm::mat4 view(1.0);
	view[0][0]=right_.x;
	view[1][0]=right_.y;
	view[2][0]=right_.z;
	view[3][0]=-glm::dot(position_,right_);

	view[0][1]=up_.x;
	view[1][1]=up_.y;
	view[2][1]=up_.z;
	view[3][1]=-glm::dot(position_,up_);

	view[0][2]=-front_.x;
	view[1][2]=-front_.y;
	view[2][2]=-front_.z;
	view[3][2]=glm::dot(position_,front_);

	return view;
}

glm::mat4 Camera::getPerspectiveMatrix()const{
	glm::mat4 p(0.f);
	p[0][0]=(1.0f*near_flat_z_)/half_near_width_;
	p[1][1]=(1.0f*near_flat_z_)/half_near_height_;
	p[2][2]=-(near_flat_z_+far_flat_z_)/(far_flat_z_-near_flat_z_);
	p[2][3]=-1;
	p[3][2]=-(2.0*far_flat_z_*near_flat_z_)/(far_flat_z_-near_flat_z_);
	
	return p;
}

glm::mat4 Camera::getViewportMatrix()const{
	glm::mat4 v(1.0f);
	v[0][0]=(image_width_-1)/2.0;
	v[3][0]=(image_width_-1)/2.0;
	v[1][1]=(image_height_-1)/2.0;
	v[3][1]=(image_height_-1)/2.0;

	return v;
}

void Camera::setFrastrum(float near,float far){
	if(near<0||far<0||near>far){
		std::cerr<<"Caught an error: err in `setFrastrum`:invalid frastrum!"<<std::endl;
		exit(-1);
	}
	near_flat_z_=near;
	far_flat_z_=far;

	half_near_height_=tan(glm::radians(fov_/2))*near_flat_z_;
	half_near_width_=aspect_ratio_*half_near_height_;
}