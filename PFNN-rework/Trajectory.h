#pragma once



struct Trajectory {

	enum { LENGTH = 120 };

	float width;

	glm::vec3 positions[LENGTH];
	glm::vec3 directions[LENGTH];
	glm::mat3 rotations[LENGTH];
	float heights[LENGTH];

	float gait_stand[LENGTH];
	float gait_walk[LENGTH];
	float gait_jog[LENGTH];
	float gait_crouch[LENGTH];
	float gait_jump[LENGTH];
	float gait_bump[LENGTH];

	glm::vec3 target_dir, target_vel;

	Trajectory()
		: width(25)
		  , target_dir(glm::vec3(0,0,1))
		  , target_vel(glm::vec3(0)) {}

};
