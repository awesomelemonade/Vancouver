#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>


/* Camera */

struct CameraOrbit {

	glm::vec3 target;
	float pitch, yaw;
	float distance;
	float width, height;

	CameraOrbit(float width, float height)
		: target(glm::vec3(0))
		  , pitch(M_PI/6)
		  , yaw(0)
		  , distance(300)
			, width(width)
			, height(height) {}

	glm::vec3 position() {
		glm::vec3 posn = glm::mat3(glm::rotate(yaw, glm::vec3(0,1,0))) * glm::vec3(distance, 0, 0);
		glm::vec3 axis = glm::normalize(glm::cross(posn, glm::vec3(0,1,0)));
		return glm::mat3(glm::rotate(pitch, axis)) * posn + target;
	}

	glm::vec3 direction() {
		return glm::normalize(target - position());
	}

	glm::mat4 view_matrix() {
		return glm::lookAt(position(), target, glm::vec3(0,1,0));
	}

	glm::mat4 proj_matrix() {
		return glm::perspective(45.0f, width / height, 10.0f, 10000.0f);
	}

};
