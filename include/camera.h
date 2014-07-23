#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

class Camera {
	glm::vec3 eye, center, up;
	glm::mat4 view;

public:
	Camera(const glm::vec3 &eye, const glm::vec3 &center, const glm::vec3 &up);
	void zoom(float dist);
	void strafe_horiz(float dist);
	void strafe_vert(float dist);
	void pitch(float deg);
	void roll(float deg);
	void yaw(float deg);
	const glm::mat4& view_mat() const;
	const glm::vec3& eye_pos() const;
	glm::vec3 view_dir() const;
};

#endif

