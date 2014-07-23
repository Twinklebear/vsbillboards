#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "util.h"
#include "camera.h"

Camera::Camera(const glm::vec3 &eye, const glm::vec3 &center, const glm::vec3 &up)
	: eye(eye), center(center), up(up), view(glm::lookAt(eye, center, up))
{}
void Camera::zoom(float dist){
	glm::vec3 dir = view_dir();
	eye += dist * dir;
	center += dist * dir;
	view = glm::lookAt(eye, center, up);
}
void Camera::strafe_horiz(float dist){
	glm::vec3 horiz = glm::cross(view_dir(), up);
	eye += dist * horiz;
	center += dist * horiz;
	view = glm::lookAt(eye, center, up);
}
void Camera::strafe_vert(float dist){
	glm::vec3 horiz = glm::cross(view_dir(), up);
	glm::vec3 vert = glm::cross(horiz, view_dir());
	eye += dist * vert;
	center += dist * vert;
	view = glm::lookAt(eye, center, up);
}
void Camera::pitch(float deg){
	glm::vec3 horiz = glm::cross(view_dir(), up);
	glm::mat4 rot = glm::rotate(util::deg_to_rad(deg), horiz);
	glm::vec3 dir = glm::normalize(glm::vec3{rot * glm::vec4{view_dir(), 0}});
	center = eye + dir;
	up = glm::normalize(glm::vec3{rot * glm::vec4(up, 0)});
	view = glm::lookAt(eye, center, up);
}
void Camera::roll(float deg){
	glm::mat4 rot = glm::rotate(util::deg_to_rad(deg), view_dir());
	up = glm::normalize(glm::vec3{rot * glm::vec4{up, 0}});
	view = glm::lookAt(eye, center, up);
}
void Camera::yaw(float deg){
	glm::vec3 horiz = glm::cross(view_dir(), up);
	glm::vec3 vert = glm::cross(horiz, view_dir());
	glm::mat4 rot = glm::rotate(util::deg_to_rad(deg), vert);
	glm::vec3 dir = glm::normalize(glm::vec3{rot * glm::vec4{view_dir(), 0}});
	center = eye + dir;
	view = glm::lookAt(eye, center, up);
}
const glm::mat4& Camera::view_mat() const {
	return view;
}
const glm::vec3& Camera::eye_pos() const {
	return eye;
}
glm::vec3 Camera::view_dir() const {
	return glm::normalize(center - eye);
}

