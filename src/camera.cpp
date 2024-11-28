#include"camera.h"

void Camera::updateCamera(glm::vec3 pos,glm::vec3 lookat,glm::vec3 right){
	position_=pos;
	front_=glm::normalize(lookat-position_);
	right_=right;
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
	view[3][0]=-position_.x;

	view[0][1]=up_.x;
	view[1][1]=up_.y;
	view[2][1]=up_.z;
	view[3][1]=-position_.y;

	view[0][2]=-front_.x;
	view[1][2]=-front_.y;
	view[2][2]=-front_.z;
	view[3][2]=-position_.z;

	return view;
}

void Camera::setFrastrum(float near,float far){
	if(near<0||far<0||near>far){
		std::cerr<<"Caught an error: err in `setFrastrum`:invalid frastrum!"<<std::endl;
		exit(-1);
	}
	near_flat_z_=near;
	far_flat_z_=far;
}