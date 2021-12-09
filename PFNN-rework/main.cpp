#define GLM_ENABLE_EXPERIMENTAL


#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <eigen3/Eigen/Dense>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <stdarg.h>
#include <time.h>

#include "PFNNOptions.h"
#include "PFNN.h"
#include "CameraOrbit.h"
#include "LightDirectional.h"
#include "Heightmap.h"
#include "Shader.h"
#include "Character.h"
#include "Trajectory.h"
#include "IK.h"
#include "loadworlds.cpp"
#include "Areas.h"
#include "LeapListener.cpp"

using namespace Eigen;

/* Options */

enum { WINDOW_WIDTH  = 720, WINDOW_HEIGHT = 480 };

enum {
	GAMEPAD_BACK = 5,
	GAMEPAD_START = 4,
	GAMEPAD_A = 10,
	GAMEPAD_B = 11,
	GAMEPAD_X = 12,
	GAMEPAD_Y = 13,
	GAMEPAD_TRIGGER_L  = 4,
	GAMEPAD_TRIGGER_R  = 5,
	GAMEPAD_SHOULDER_L = 8,
	GAMEPAD_SHOULDER_R = 9,
	GAMEPAD_STICK_L_HORIZONTAL = 0,
	GAMEPAD_STICK_L_VERTICAL   = 1,
	GAMEPAD_STICK_R_HORIZONTAL = 2,
	GAMEPAD_STICK_R_VERTICAL   = 3
};

static PFNNOptions* options = NULL;


static PFNN* pfnn = NULL;

/* Joystick */

static SDL_Joystick* stick = NULL;


static CameraOrbit* camera = NULL;

/* Rendering */


static LightDirectional* light = NULL;

/* Heightmap */


static Heightmap* heightmap = NULL;

/* Shader */


static Shader* shader_terrain = NULL;
static Shader* shader_terrain_shadow = NULL;
static Shader* shader_character = NULL;
static Shader* shader_character_shadow = NULL;

/* Character */


static Character* character = NULL;

/* Trajectory */


static Trajectory* trajectory = NULL;

/* IK */


static IK* ik = NULL;

/* Areas */


static Areas* areas = NULL;

/* Leap Listener */

static SampleListener* listener;

/* Helper Functions */

static glm::vec3 mix_directions(glm::vec3 x, glm::vec3 y, float a) {
	glm::quat x_q = glm::angleAxis(atan2f(x.x, x.z), glm::vec3(0,1,0));
	glm::quat y_q = glm::angleAxis(atan2f(y.x, y.z), glm::vec3(0,1,0));
	glm::quat z_q = glm::slerp(x_q, y_q, a);
	return z_q * glm::vec3(0,0,1);
}

static glm::mat4 mix_transforms(glm::mat4 x, glm::mat4 y, float a) {
	glm::mat4 out = glm::mat4(glm::slerp(glm::quat(x), glm::quat(y), a));
	out[3] = mix(x[3], y[3], a);
	return out;
}

static glm::quat quat_exp(glm::vec3 l) {
	float w = glm::length(l);
	glm::quat q = w < 0.01 ? glm::quat(1,0,0,0) : glm::quat(
			cosf(w),
			l.x * (sinf(w) / w),
			l.y * (sinf(w) / w),
			l.z * (sinf(w) / w));
	return q / sqrtf(q.w*q.w + q.x*q.x + q.y*q.y + q.z*q.z); 
}

static glm::vec2 segment_nearest(glm::vec2 v, glm::vec2 w, glm::vec2 p) {
	float l2 = glm::dot(v - w, v - w);
	if (l2 == 0.0) return v;
	float t = glm::clamp(glm::dot(p - v, w - v) / l2, 0.0f, 1.0f);
	return v + t * (w - v);
}

/* Reset */

static void reset(glm::vec2 position) {
	ArrayXf Yp = pfnn->Ymean;

	glm::vec3 root_position = glm::vec3(position.x, heightmap->sample(position), position.y);
	//glm::mat3 root_rotation = glm::identity<glm::mat3>();
	glm::mat3 root_rotation(1.0f); // construct identity

	for (int i = 0; i < Trajectory::LENGTH; i++) {
		trajectory->positions[i] = root_position;
		trajectory->rotations[i] = root_rotation;
		trajectory->directions[i] = glm::vec3(0,0,1);
		trajectory->heights[i] = root_position.y;
		trajectory->gait_stand[i] = 0.0;
		trajectory->gait_walk[i] = 0.0;
		trajectory->gait_jog[i] = 0.0;
		trajectory->gait_crouch[i] = 0.0;
		trajectory->gait_jump[i] = 0.0;
		trajectory->gait_bump[i] = 0.0;
	}

	for (int i = 0; i < Character::JOINT_NUM; i++) {

		int opos = 8+(((Trajectory::LENGTH/2)/10)*4)+(Character::JOINT_NUM*3*0);
		int ovel = 8+(((Trajectory::LENGTH/2)/10)*4)+(Character::JOINT_NUM*3*1);
		int orot = 8+(((Trajectory::LENGTH/2)/10)*4)+(Character::JOINT_NUM*3*2);

		glm::vec3 pos = (root_rotation * glm::vec3(Yp(opos+i*3+0), Yp(opos+i*3+1), Yp(opos+i*3+2))) + root_position;
		glm::vec3 vel = (root_rotation * glm::vec3(Yp(ovel+i*3+0), Yp(ovel+i*3+1), Yp(ovel+i*3+2)));
		glm::mat3 rot = (root_rotation * glm::toMat3(quat_exp(glm::vec3(Yp(orot+i*3+0), Yp(orot+i*3+1), Yp(orot+i*3+2)))));

		character->joint_positions[i]  = pos;
		character->joint_velocities[i] = vel;
		character->joint_rotations[i]  = rot;
	}

	character->phase = 0.0;

	ik->position[IK::HL] = glm::vec3(0,0,0); ik->lock[IK::HL] = 0; ik->height[IK::HL] = root_position.y;
	ik->position[IK::HR] = glm::vec3(0,0,0); ik->lock[IK::HR] = 0; ik->height[IK::HR] = root_position.y;
	ik->position[IK::TL] = glm::vec3(0,0,0); ik->lock[IK::TL] = 0; ik->height[IK::TL] = root_position.y;
	ik->position[IK::TR] = glm::vec3(0,0,0); ik->lock[IK::TR] = 0; ik->height[IK::TR] = root_position.y;

}


static int playerDx = 0;
static int playerDy = 0;
static int playerVelocity = 10000;

static int waypointX[] = {-800, -800, 800, 800};
static int waypointZ[] = {-800, 800, -800, 800};
int waypointIndex = 0;

static void pre_render() {
	/* Update Target Direction / Velocity */

	int x_vel = 0;
	int y_vel = 0;
	SDL_PumpEvents();
	const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
	if (keyboardState[SDL_SCANCODE_W]) {
		y_vel += playerVelocity;
	}
	if (keyboardState[SDL_SCANCODE_S]) {
		y_vel -= playerVelocity;
	}
	if (keyboardState[SDL_SCANCODE_A]) {
		x_vel += playerVelocity;
	}
	if (keyboardState[SDL_SCANCODE_D]) {
		x_vel -= playerVelocity;
	}
	{
		glm::vec3 root_position = glm::vec3(
				trajectory->positions[Trajectory::LENGTH/2].x, 
				trajectory->heights[Trajectory::LENGTH/2],
				trajectory->positions[Trajectory::LENGTH/2].z);
		auto currentDirection = trajectory->directions[Trajectory::LENGTH/2];

		int numWaypoints = sizeof(waypointX) / sizeof(waypointX[0]);
		int targetX = waypointX[waypointIndex];
		int targetZ = waypointZ[waypointIndex];

		while (abs(root_position.x - targetX) < 50 && abs(root_position.z - targetZ) < 50) {
			waypointIndex = (waypointIndex + 1) % numWaypoints;
			targetX = waypointX[waypointIndex];
			targetZ = waypointZ[waypointIndex];
		}

		auto target_position = glm::vec3(targetX, 0, targetZ);

		y_vel = -(target_position.x - root_position.x) * 200;
		x_vel = (target_position.z - root_position.z) * 200;
		x_vel = (std::abs(x_vel) < 10000 ? 0 : (x_vel < 0 ? -1 : 1)) * std::max(20000, std::min(30000, std::abs(x_vel)));
		y_vel = (std::abs(y_vel) < 10000 ? 0 : (y_vel < 0 ? -1 : 1)) * std::max(20000, std::min(30000, std::abs(y_vel)));

		//std::cout << "Position[" << root_position.x << ", " << root_position.y << ", " << root_position.z << "], Vel[" << x_vel << ", " << y_vel << "], Target[" << targetX << ", " << targetZ << "]" << "\n";

		// TODO
		auto waypointOffset = target_position - root_position;
		auto waypointOffsetLength = glm::length(waypointOffset);
		waypointOffset = std::min(waypointOffsetLength, 5.0f) / waypointOffsetLength * waypointOffset;
		//trajectory->target_vel = waypointOffset; // "Smart"
		//trajectory->target_vel = 5.0f * glm::normalize(glm::vec3(-y_vel, 0, x_vel)); // Stepped

		auto speed = 5.0f * listener->indexFingerVelocity / 0.1f;
		if (speed < 1.0f) {
			speed = 0.0f;
		}
		if (listener-> numFingersExtended > 0 && listener->numHands > 0) {
			cout << "SPEED: " << speed << "\n";
			trajectory->target_vel = speed * (glm::mat3(glm::rotate(-0.5f * (listener->handYaw - listener->handRoll), glm::vec3(0.0f, 1.0f, 0.0f))) * currentDirection);
		} else {
			trajectory->target_vel = glm::vec3(0.0f, 0.0f, 0.0f);
		}

		trajectory->target_dir = 1.0f/5.0f * trajectory->target_vel;
		//trajectory->target_dir = glm::vec3(-1.0f, 0.0f, 0.0f);
		//camera->yaw = glm::mix(camera->yaw, atan2f(-y_vel, x_vel) + ((float) M_PI) / 2.0f, 0.1f);
		//camera->yaw = glm::mix(camera->yaw, atan2f(currentDirection.y, currentDirection.x), 0.1f);
		auto cameraDirection = glm::mat3(glm::rotate(camera->yaw, glm::vec3(0.0f, 1.0f, 0.0f))) * glm::vec3(1.0f, 0.0f, 0.0f);
		auto newCameraDirection = mix_directions(cameraDirection, -currentDirection, 0.1f);

		camera->yaw = atan2f(-newCameraDirection.z, newCameraDirection.x);


		// trajectory->target_dir, trajectory->target_vel
	}

	character->crouched_amount = glm::mix(character->crouched_amount, character->crouched_target, options->extra_crouched_smooth);

	/* Update Gait */

	if (glm::length(trajectory->target_vel) < 0.1)  {
		// state = stand
		float stand_amount = 1.0f - glm::clamp(glm::length(trajectory->target_vel) / 0.1f, 0.0f, 1.0f);
		trajectory->gait_stand[Trajectory::LENGTH/2]  = glm::mix(trajectory->gait_stand[Trajectory::LENGTH/2],  stand_amount, options->extra_gait_smooth);
		trajectory->gait_walk[Trajectory::LENGTH/2]   = glm::mix(trajectory->gait_walk[Trajectory::LENGTH/2],   0.0f, options->extra_gait_smooth);
		trajectory->gait_jog[Trajectory::LENGTH/2]    = glm::mix(trajectory->gait_jog[Trajectory::LENGTH/2],    0.0f, options->extra_gait_smooth);
		trajectory->gait_crouch[Trajectory::LENGTH/2] = glm::mix(trajectory->gait_crouch[Trajectory::LENGTH/2], 0.0f, options->extra_gait_smooth);
		trajectory->gait_jump[Trajectory::LENGTH/2]   = glm::mix(trajectory->gait_jump[Trajectory::LENGTH/2],   0.0f, options->extra_gait_smooth);
		trajectory->gait_bump[Trajectory::LENGTH/2]   = glm::mix(trajectory->gait_bump[Trajectory::LENGTH/2],   0.0f, options->extra_gait_smooth);
	} else if (character->crouched_amount > 0.1) {
		// state = crouched
		trajectory->gait_stand[Trajectory::LENGTH/2]  = glm::mix(trajectory->gait_stand[Trajectory::LENGTH/2],  0.0f, options->extra_gait_smooth);
		trajectory->gait_walk[Trajectory::LENGTH/2]   = glm::mix(trajectory->gait_walk[Trajectory::LENGTH/2],   0.0f, options->extra_gait_smooth);
		trajectory->gait_jog[Trajectory::LENGTH/2]    = glm::mix(trajectory->gait_jog[Trajectory::LENGTH/2],    0.0f, options->extra_gait_smooth);
		trajectory->gait_crouch[Trajectory::LENGTH/2] = glm::mix(trajectory->gait_crouch[Trajectory::LENGTH/2], character->crouched_amount, options->extra_gait_smooth);
		trajectory->gait_jump[Trajectory::LENGTH/2]   = glm::mix(trajectory->gait_jump[Trajectory::LENGTH/2],   0.0f, options->extra_gait_smooth);
		trajectory->gait_bump[Trajectory::LENGTH/2]   = glm::mix(trajectory->gait_bump[Trajectory::LENGTH/2],   0.0f, options->extra_gait_smooth);
	} else if (glm::length(trajectory->target_vel) > 2) {
		// state = jog
		trajectory->gait_stand[Trajectory::LENGTH/2]  = glm::mix(trajectory->gait_stand[Trajectory::LENGTH/2],  0.0f, options->extra_gait_smooth);
		trajectory->gait_walk[Trajectory::LENGTH/2]   = glm::mix(trajectory->gait_walk[Trajectory::LENGTH/2],   0.0f, options->extra_gait_smooth);
		trajectory->gait_jog[Trajectory::LENGTH/2]    = glm::mix(trajectory->gait_jog[Trajectory::LENGTH/2],    1.0f, options->extra_gait_smooth);
		trajectory->gait_crouch[Trajectory::LENGTH/2] = glm::mix(trajectory->gait_crouch[Trajectory::LENGTH/2], 0.0f, options->extra_gait_smooth);
		trajectory->gait_jump[Trajectory::LENGTH/2]   = glm::mix(trajectory->gait_jump[Trajectory::LENGTH/2],   0.0f, options->extra_gait_smooth);    
		trajectory->gait_bump[Trajectory::LENGTH/2]   = glm::mix(trajectory->gait_bump[Trajectory::LENGTH/2],   0.0f, options->extra_gait_smooth);    
	} else {
		// state = walk
		trajectory->gait_stand[Trajectory::LENGTH/2]  = glm::mix(trajectory->gait_stand[Trajectory::LENGTH/2],  0.0f, options->extra_gait_smooth);
		trajectory->gait_walk[Trajectory::LENGTH/2]   = glm::mix(trajectory->gait_walk[Trajectory::LENGTH/2],   1.0f, options->extra_gait_smooth);
		trajectory->gait_jog[Trajectory::LENGTH/2]    = glm::mix(trajectory->gait_jog[Trajectory::LENGTH/2],    0.0f, options->extra_gait_smooth);
		trajectory->gait_crouch[Trajectory::LENGTH/2] = glm::mix(trajectory->gait_crouch[Trajectory::LENGTH/2], 0.0f, options->extra_gait_smooth);
		trajectory->gait_jump[Trajectory::LENGTH/2]   = glm::mix(trajectory->gait_jump[Trajectory::LENGTH/2],   0.0f, options->extra_gait_smooth);  
		trajectory->gait_bump[Trajectory::LENGTH/2]   = glm::mix(trajectory->gait_bump[Trajectory::LENGTH/2],   0.0f, options->extra_gait_smooth);  
	}

	/* Predict Future Trajectory */

	glm::vec3 trajectory_positions_blend[Trajectory::LENGTH];
	trajectory_positions_blend[Trajectory::LENGTH/2] = trajectory->positions[Trajectory::LENGTH/2];

	for (int i = Trajectory::LENGTH/2+1; i < Trajectory::LENGTH; i++) {

		float bias_pos = character->responsive ? glm::mix(2.0f, 2.0f, character->strafe_amount) : glm::mix(0.5f, 1.0f, character->strafe_amount);
		float bias_dir = character->responsive ? glm::mix(5.0f, 3.0f, character->strafe_amount) : glm::mix(2.0f, 0.5f, character->strafe_amount);

		float scale_pos = (1.0f - powf(1.0f - ((float)(i - Trajectory::LENGTH/2) / (Trajectory::LENGTH/2)), bias_pos));
		float scale_dir = (1.0f - powf(1.0f - ((float)(i - Trajectory::LENGTH/2) / (Trajectory::LENGTH/2)), bias_dir));

		trajectory_positions_blend[i] = trajectory_positions_blend[i-1] + glm::mix(
				trajectory->positions[i] - trajectory->positions[i-1], 
				trajectory->target_vel,
				scale_pos);

		/* Collide with walls */
		for (int j = 0; j < areas->num_walls(); j++) {
			glm::vec2 trjpoint = glm::vec2(trajectory_positions_blend[i].x, trajectory_positions_blend[i].z);
			if (glm::length(trjpoint - ((areas->wall_start[j] + areas->wall_stop[j]) / 2.0f)) > 
					glm::length(areas->wall_start[j] - areas->wall_stop[j])) { continue; }
			glm::vec2 segpoint = segment_nearest(areas->wall_start[j], areas->wall_stop[j], trjpoint);
			float segdist = glm::length(segpoint - trjpoint);
			if (segdist < areas->wall_width[j] + 100.0) {
				glm::vec2 prjpoint0 = (areas->wall_width[j] +   0.0f) * glm::normalize(trjpoint - segpoint) + segpoint; 
				glm::vec2 prjpoint1 = (areas->wall_width[j] + 100.0f) * glm::normalize(trjpoint - segpoint) + segpoint; 
				glm::vec2 prjpoint = glm::mix(prjpoint0, prjpoint1, glm::clamp((segdist - areas->wall_width[j]) / 100.0f, 0.0f, 1.0f));
				trajectory_positions_blend[i].x = prjpoint.x;
				trajectory_positions_blend[i].z = prjpoint.y;
			}
		}

		trajectory->directions[i] = mix_directions(trajectory->directions[i], trajectory->target_dir, scale_dir);

		trajectory->heights[i] = trajectory->heights[Trajectory::LENGTH/2]; 

		trajectory->gait_stand[i]  = trajectory->gait_stand[Trajectory::LENGTH/2]; 
		trajectory->gait_walk[i]   = trajectory->gait_walk[Trajectory::LENGTH/2];  
		trajectory->gait_jog[i]    = trajectory->gait_jog[Trajectory::LENGTH/2];   
		trajectory->gait_crouch[i] = trajectory->gait_crouch[Trajectory::LENGTH/2];
		trajectory->gait_jump[i]   = trajectory->gait_jump[Trajectory::LENGTH/2];  
		trajectory->gait_bump[i]   = trajectory->gait_bump[Trajectory::LENGTH/2];  
	}

	for (int i = Trajectory::LENGTH/2+1; i < Trajectory::LENGTH; i++) {
		trajectory->positions[i] = trajectory_positions_blend[i];
	}

	/* Jumps */
	for (int i = Trajectory::LENGTH/2; i < Trajectory::LENGTH; i++) {
		trajectory->gait_jump[i] = 0.0;
		for (int j = 0; j < areas->num_jumps(); j++) {
			float dist = glm::length(trajectory->positions[i] - areas->jump_pos[j]);
			trajectory->gait_jump[i] = std::max(trajectory->gait_jump[i], 
					1.0f-glm::clamp((dist - areas->jump_size[j]) / areas->jump_falloff[j], 0.0f, 1.0f));
		}
	}

	/* Crouch Area */
	for (int i = Trajectory::LENGTH/2; i < Trajectory::LENGTH; i++) {
		for (int j = 0; j < areas->num_crouches(); j++) {
			float dist_x = abs(trajectory->positions[i].x - areas->crouch_pos[j].x);
			float dist_z = abs(trajectory->positions[i].z - areas->crouch_pos[j].z);
			float height = (sinf(trajectory->positions[i].x/Areas::CROUCH_WAVE)+1.0)/2.0;
			trajectory->gait_crouch[i] = glm::mix(1.0f-height, trajectory->gait_crouch[i], 
					glm::clamp(
						((dist_x - (areas->crouch_size[j].x/2)) + 
						 (dist_z - (areas->crouch_size[j].y/2))) / 100.0f, 0.0f, 1.0f));
		}
	}

	/* Walls */
	for (int i = 0; i < Trajectory::LENGTH; i++) {
		trajectory->gait_bump[i] = 0.0;
		for (int j = 0; j < areas->num_walls(); j++) {
			glm::vec2 trjpoint = glm::vec2(trajectory->positions[i].x, trajectory->positions[i].z);
			glm::vec2 segpoint = segment_nearest(areas->wall_start[j], areas->wall_stop[j], trjpoint);
			float segdist = glm::length(segpoint - trjpoint);
			trajectory->gait_bump[i] = glm::max(trajectory->gait_bump[i], 1.0f-glm::clamp((segdist - areas->wall_width[j]) / 10.0f, 0.0f, 1.0f));
		} 
	}

	/* Trajectory Rotation */
	for (int i = 0; i < Trajectory::LENGTH; i++) {
		trajectory->rotations[i] = glm::mat3(glm::rotate(atan2f(
						trajectory->directions[i].x,
						trajectory->directions[i].z), glm::vec3(0,1,0)));
	}

	/* Trajectory Heights */
	for (int i = Trajectory::LENGTH/2; i < Trajectory::LENGTH; i++) {
		trajectory->positions[i].y = heightmap->sample(glm::vec2(trajectory->positions[i].x, trajectory->positions[i].z));
	}

	trajectory->heights[Trajectory::LENGTH/2] = 0.0;
	for (int i = 0; i < Trajectory::LENGTH; i+=10) {
		trajectory->heights[Trajectory::LENGTH/2] += (trajectory->positions[i].y / ((Trajectory::LENGTH)/10));
	}

	glm::vec3 root_position = glm::vec3(
			trajectory->positions[Trajectory::LENGTH/2].x, 
			trajectory->heights[Trajectory::LENGTH/2],
			trajectory->positions[Trajectory::LENGTH/2].z);

	//std::cout << "Root Position: " << root_position.x << " - " << root_position.y << " - " << root_position.z << "\n";

	glm::mat3 root_rotation = trajectory->rotations[Trajectory::LENGTH/2];

	/* Input Trajectory Positions / Directions */
	for (int i = 0; i < Trajectory::LENGTH; i+=10) {
		int w = (Trajectory::LENGTH)/10;
		glm::vec3 pos = glm::inverse(root_rotation) * (trajectory->positions[i] - root_position);
		glm::vec3 dir = glm::inverse(root_rotation) * trajectory->directions[i];  
		pfnn->Xp((w*0)+i/10) = pos.x; pfnn->Xp((w*1)+i/10) = pos.z;
		pfnn->Xp((w*2)+i/10) = dir.x; pfnn->Xp((w*3)+i/10) = dir.z;
	}

	/* Input Trajectory Gaits */
	for (int i = 0; i < Trajectory::LENGTH; i+=10) {
		int w = (Trajectory::LENGTH)/10;
		pfnn->Xp((w*4)+i/10) = trajectory->gait_stand[i];
		pfnn->Xp((w*5)+i/10) = trajectory->gait_walk[i];
		pfnn->Xp((w*6)+i/10) = trajectory->gait_jog[i];
		pfnn->Xp((w*7)+i/10) = trajectory->gait_crouch[i];
		pfnn->Xp((w*8)+i/10) = trajectory->gait_jump[i];
		pfnn->Xp((w*9)+i/10) = 0.0; // Unused.
	}

	/* Input Joint Previous Positions / Velocities / Rotations */
	glm::vec3 prev_root_position = glm::vec3(
			trajectory->positions[Trajectory::LENGTH/2-1].x, 
			trajectory->heights[Trajectory::LENGTH/2-1],
			trajectory->positions[Trajectory::LENGTH/2-1].z);

	glm::mat3 prev_root_rotation = trajectory->rotations[Trajectory::LENGTH/2-1];

	for (int i = 0; i < Character::JOINT_NUM; i++) {
		int o = (((Trajectory::LENGTH)/10)*10);  
		glm::vec3 pos = glm::inverse(prev_root_rotation) * (character->joint_positions[i] - prev_root_position);
		glm::vec3 prv = glm::inverse(prev_root_rotation) *  character->joint_velocities[i];
		pfnn->Xp(o+(Character::JOINT_NUM*3*0)+i*3+0) = pos.x;
		pfnn->Xp(o+(Character::JOINT_NUM*3*0)+i*3+1) = pos.y;
		pfnn->Xp(o+(Character::JOINT_NUM*3*0)+i*3+2) = pos.z;
		pfnn->Xp(o+(Character::JOINT_NUM*3*1)+i*3+0) = prv.x;
		pfnn->Xp(o+(Character::JOINT_NUM*3*1)+i*3+1) = prv.y;
		pfnn->Xp(o+(Character::JOINT_NUM*3*1)+i*3+2) = prv.z;
	}

	/* Input Trajectory Heights */
	for (int i = 0; i < Trajectory::LENGTH; i += 10) {
		int o = (((Trajectory::LENGTH)/10)*10)+Character::JOINT_NUM*3*2;
		int w = (Trajectory::LENGTH)/10;
		glm::vec3 position_r = trajectory->positions[i] + (trajectory->rotations[i] * glm::vec3( trajectory->width, 0, 0));
		glm::vec3 position_l = trajectory->positions[i] + (trajectory->rotations[i] * glm::vec3(-trajectory->width, 0, 0));
		pfnn->Xp(o+(w*0)+(i/10)) = heightmap->sample(glm::vec2(position_r.x, position_r.z)) - root_position.y;
		pfnn->Xp(o+(w*1)+(i/10)) = trajectory->positions[i].y - root_position.y;
		pfnn->Xp(o+(w*2)+(i/10)) = heightmap->sample(glm::vec2(position_l.x, position_l.z)) - root_position.y;
	}

	/* Perform Regression */

	clock_t time_start = clock();

	pfnn->predict(character->phase);

	clock_t time_end = clock();

	/* Timing */

	enum { TIME_MSAMPLES = 500 };
	static int time_nsamples = 0;
	static float time_samples[TIME_MSAMPLES];

	time_samples[time_nsamples] = (float)(time_end - time_start) / CLOCKS_PER_SEC;
	time_nsamples++;
	if (time_nsamples == TIME_MSAMPLES) {
		float time_avg = 0.0;
		for (int i = 0; i < TIME_MSAMPLES; i++) {
			time_avg += (time_samples[i] / TIME_MSAMPLES);
		}
		printf("PFNN: %0.5f ms\n", time_avg * 1000);
		time_nsamples = 0;
	}

	/* Build Local Transforms */

	for (int i = 0; i < Character::JOINT_NUM; i++) {
		int opos = 8+(((Trajectory::LENGTH/2)/10)*4)+(Character::JOINT_NUM*3*0);
		int ovel = 8+(((Trajectory::LENGTH/2)/10)*4)+(Character::JOINT_NUM*3*1);
		int orot = 8+(((Trajectory::LENGTH/2)/10)*4)+(Character::JOINT_NUM*3*2);

		glm::vec3 pos = (root_rotation * glm::vec3(pfnn->Yp(opos+i*3+0), pfnn->Yp(opos+i*3+1), pfnn->Yp(opos+i*3+2))) + root_position;
		glm::vec3 vel = (root_rotation * glm::vec3(pfnn->Yp(ovel+i*3+0), pfnn->Yp(ovel+i*3+1), pfnn->Yp(ovel+i*3+2)));
		glm::mat3 rot = (root_rotation * glm::toMat3(quat_exp(glm::vec3(pfnn->Yp(orot+i*3+0), pfnn->Yp(orot+i*3+1), pfnn->Yp(orot+i*3+2)))));

		/*
		 ** Blending Between the predicted positions and
		 ** the previous positions plus the velocities 
		 ** smooths out the motion a bit in the case 
		 ** where the two disagree with each other.
		 */

		character->joint_positions[i]  = glm::mix(character->joint_positions[i] + vel, pos, options->extra_joint_smooth);
		character->joint_velocities[i] = vel;
		character->joint_rotations[i]  = rot;

		character->joint_global_anim_xform[i] = glm::transpose(glm::mat4(
					rot[0][0], rot[1][0], rot[2][0], pos[0],
					rot[0][1], rot[1][1], rot[2][1], pos[1],
					rot[0][2], rot[1][2], rot[2][2], pos[2],
					0,         0,         0,      1));
	}

	/* Convert to local space ... yes I know this is inefficient. */

	for (int i = 0; i < Character::JOINT_NUM; i++) {
		if (i == 0) {
			character->joint_anim_xform[i] = character->joint_global_anim_xform[i];
		} else {
			character->joint_anim_xform[i] = glm::inverse(character->joint_global_anim_xform[character->joint_parents[i]]) * character->joint_global_anim_xform[i];
		}
	}

	character->forward_kinematics();

	/* Perform IK (enter this block at your own risk...) */

	if (options->enable_ik) {

		/* Get Weights */

		glm::vec4 ik_weight = glm::vec4(pfnn->Yp(4+0), pfnn->Yp(4+1), pfnn->Yp(4+2), pfnn->Yp(4+3));

		glm::vec3 key_hl = glm::vec3(character->joint_global_anim_xform[Character::JOINT_HEEL_L][3]);
		glm::vec3 key_tl = glm::vec3(character->joint_global_anim_xform[Character::JOINT_TOE_L][3]);
		glm::vec3 key_hr = glm::vec3(character->joint_global_anim_xform[Character::JOINT_HEEL_R][3]);
		glm::vec3 key_tr = glm::vec3(character->joint_global_anim_xform[Character::JOINT_TOE_R][3]);

		key_hl = glm::mix(key_hl, ik->position[IK::HL], ik->lock[IK::HL]);
		key_tl = glm::mix(key_tl, ik->position[IK::TL], ik->lock[IK::TL]);
		key_hr = glm::mix(key_hr, ik->position[IK::HR], ik->lock[IK::HR]);
		key_tr = glm::mix(key_tr, ik->position[IK::TR], ik->lock[IK::TR]);

		ik->height[IK::HL] = glm::mix(ik->height[IK::HL], heightmap->sample(glm::vec2(key_hl.x, key_hl.z)) + ik->heel_height, ik->smoothness);
		ik->height[IK::TL] = glm::mix(ik->height[IK::TL], heightmap->sample(glm::vec2(key_tl.x, key_tl.z)) + ik->toe_height, ik->smoothness);
		ik->height[IK::HR] = glm::mix(ik->height[IK::HR], heightmap->sample(glm::vec2(key_hr.x, key_hr.z)) + ik->heel_height, ik->smoothness);
		ik->height[IK::TR] = glm::mix(ik->height[IK::TR], heightmap->sample(glm::vec2(key_tr.x, key_tr.z)) + ik->toe_height, ik->smoothness);

		key_hl.y = glm::max(key_hl.y, ik->height[IK::HL]);
		key_tl.y = glm::max(key_tl.y, ik->height[IK::TL]);
		key_hr.y = glm::max(key_hr.y, ik->height[IK::HR]);
		key_tr.y = glm::max(key_tr.y, ik->height[IK::TR]);

		/* Rotate Hip / Knee */

		{
			glm::vec3 hip_l  = glm::vec3(character->joint_global_anim_xform[Character::JOINT_HIP_L][3]);
			glm::vec3 knee_l = glm::vec3(character->joint_global_anim_xform[Character::JOINT_KNEE_L][3]);
			glm::vec3 heel_l = glm::vec3(character->joint_global_anim_xform[Character::JOINT_HEEL_L][3]);

			glm::vec3 hip_r  = glm::vec3(character->joint_global_anim_xform[Character::JOINT_HIP_R][3]);
			glm::vec3 knee_r = glm::vec3(character->joint_global_anim_xform[Character::JOINT_KNEE_R][3]);
			glm::vec3 heel_r = glm::vec3(character->joint_global_anim_xform[Character::JOINT_HEEL_R][3]);

			ik->two_joint(hip_l, knee_l, heel_l, key_hl, 1.0,
					character->joint_global_anim_xform[Character::JOINT_ROOT_L],
					character->joint_global_anim_xform[Character::JOINT_HIP_L],
					character->joint_global_anim_xform[Character::JOINT_HIP_L],
					character->joint_global_anim_xform[Character::JOINT_KNEE_L],
					character->joint_anim_xform[Character::JOINT_HIP_L],
					character->joint_anim_xform[Character::JOINT_KNEE_L]);

			ik->two_joint(hip_r, knee_r, heel_r, key_hr, 1.0, 
					character->joint_global_anim_xform[Character::JOINT_ROOT_R],
					character->joint_global_anim_xform[Character::JOINT_HIP_R],
					character->joint_global_anim_xform[Character::JOINT_HIP_R],
					character->joint_global_anim_xform[Character::JOINT_KNEE_R],
					character->joint_anim_xform[Character::JOINT_HIP_R],
					character->joint_anim_xform[Character::JOINT_KNEE_R]);

			character->forward_kinematics();
		}

		/* Rotate Heel */

		{
			const float heel_max_bend_s = 4;
			const float heel_max_bend_u = 4;
			const float heel_max_bend_d = 4;

			glm::vec4 ik_toe_pos_blend = glm::clamp(ik_weight * 2.5f, 0.0f, 1.0f);

			glm::vec3 heel_l = glm::vec3(character->joint_global_anim_xform[Character::JOINT_HEEL_L][3]);
			glm::vec4 side_h0_l = character->joint_global_anim_xform[Character::JOINT_HEEL_L] * glm::vec4( 10,0,0,1);
			glm::vec4 side_h1_l = character->joint_global_anim_xform[Character::JOINT_HEEL_L] * glm::vec4(-10,0,0,1);
			glm::vec3 side0_l = glm::vec3(side_h0_l) / side_h0_l.w;
			glm::vec3 side1_l = glm::vec3(side_h1_l) / side_h1_l.w;
			glm::vec3 floor_l = key_tl;

			side0_l.y = glm::clamp(heightmap->sample(glm::vec2(side0_l.x, side0_l.z)) + ik->toe_height, heel_l.y - heel_max_bend_s, heel_l.y + heel_max_bend_s);
			side1_l.y = glm::clamp(heightmap->sample(glm::vec2(side1_l.x, side1_l.z)) + ik->toe_height, heel_l.y - heel_max_bend_s, heel_l.y + heel_max_bend_s);
			floor_l.y = glm::clamp(floor_l.y, heel_l.y - heel_max_bend_d, heel_l.y + heel_max_bend_u);

			glm::vec3 targ_z_l = glm::normalize(floor_l - heel_l);
			glm::vec3 targ_x_l = glm::normalize(side0_l - side1_l);
			glm::vec3 targ_y_l = glm::normalize(glm::cross(targ_x_l, targ_z_l));
			targ_x_l = glm::cross(targ_z_l, targ_y_l);

			character->joint_anim_xform[Character::JOINT_HEEL_L] = mix_transforms(
					character->joint_anim_xform[Character::JOINT_HEEL_L],
					glm::inverse(character->joint_global_anim_xform[Character::JOINT_KNEE_L]) * glm::mat4(
						glm::vec4( targ_x_l, 0),
						glm::vec4(-targ_y_l, 0),
						glm::vec4( targ_z_l, 0),
						glm::vec4( heel_l, 1)), ik_toe_pos_blend.y);

			glm::vec3 heel_r = glm::vec3(character->joint_global_anim_xform[Character::JOINT_HEEL_R][3]);
			glm::vec4 side_h0_r = character->joint_global_anim_xform[Character::JOINT_HEEL_R] * glm::vec4( 10,0,0,1);
			glm::vec4 side_h1_r = character->joint_global_anim_xform[Character::JOINT_HEEL_R] * glm::vec4(-10,0,0,1);
			glm::vec3 side0_r = glm::vec3(side_h0_r) / side_h0_r.w;
			glm::vec3 side1_r = glm::vec3(side_h1_r) / side_h1_r.w;
			glm::vec3 floor_r = key_tr;

			side0_r.y = glm::clamp(heightmap->sample(glm::vec2(side0_r.x, side0_r.z)) + ik->toe_height, heel_r.y - heel_max_bend_s, heel_r.y + heel_max_bend_s);
			side1_r.y = glm::clamp(heightmap->sample(glm::vec2(side1_r.x, side1_r.z)) + ik->toe_height, heel_r.y - heel_max_bend_s, heel_r.y + heel_max_bend_s);
			floor_r.y = glm::clamp(floor_r.y, heel_r.y - heel_max_bend_d, heel_r.y + heel_max_bend_u);

			glm::vec3 targ_z_r = glm::normalize(floor_r - heel_r);
			glm::vec3 targ_x_r = glm::normalize(side0_r - side1_r);
			glm::vec3 targ_y_r = glm::normalize(glm::cross(targ_z_r, targ_x_r));
			targ_x_r = glm::cross(targ_z_r, targ_y_r);

			character->joint_anim_xform[Character::JOINT_HEEL_R] = mix_transforms(
					character->joint_anim_xform[Character::JOINT_HEEL_R],
					glm::inverse(character->joint_global_anim_xform[Character::JOINT_KNEE_R]) * glm::mat4(
						glm::vec4(-targ_x_r, 0),
						glm::vec4( targ_y_r, 0),
						glm::vec4( targ_z_r, 0),
						glm::vec4( heel_r, 1)), ik_toe_pos_blend.w);

			character->forward_kinematics();
		}

		/* Rotate Toe */

		{
			const float toe_max_bend_d = 0;
			const float toe_max_bend_u = 10;

			glm::vec4 ik_toe_rot_blend = glm::clamp(ik_weight * 2.5f, 0.0f, 1.0f);

			glm::vec3 toe_l     = glm::vec3(character->joint_global_anim_xform[Character::JOINT_TOE_L][3]);
			glm::vec4 fwrd_h_l  = character->joint_global_anim_xform[Character::JOINT_TOE_L] * glm::vec4(  0, 0, 10, 1);
			glm::vec4 side_h0_l = character->joint_global_anim_xform[Character::JOINT_TOE_L] * glm::vec4( 10, 0,  0, 1);
			glm::vec4 side_h1_l = character->joint_global_anim_xform[Character::JOINT_TOE_L] * glm::vec4(-10, 0,  0, 1);
			glm::vec3 fwrd_l  = glm::vec3(fwrd_h_l) / fwrd_h_l.w;
			glm::vec3 side0_l = glm::vec3(side_h0_l) / side_h0_l.w;
			glm::vec3 side1_l = glm::vec3(side_h1_l) / side_h1_l.w;

			fwrd_l.y  = glm::clamp(heightmap->sample(glm::vec2(fwrd_l.x, fwrd_l.z))   + ik->toe_height, toe_l.y - toe_max_bend_d, toe_l.y + toe_max_bend_u);
			side0_l.y = glm::clamp(heightmap->sample(glm::vec2(side0_l.x, side0_l.z)) + ik->toe_height, toe_l.y - toe_max_bend_d, toe_l.y + toe_max_bend_u);
			side1_l.y = glm::clamp(heightmap->sample(glm::vec2(side0_l.x, side1_l.z)) + ik->toe_height, toe_l.y - toe_max_bend_d, toe_l.y + toe_max_bend_u);

			glm::vec3 side_l = glm::normalize(side0_l - side1_l);
			fwrd_l = glm::normalize(fwrd_l - toe_l);
			glm::vec3 upwr_l = glm::normalize(glm::cross(side_l, fwrd_l));
			side_l = glm::cross(fwrd_l, upwr_l);

			character->joint_anim_xform[Character::JOINT_TOE_L] = mix_transforms(
					character->joint_anim_xform[Character::JOINT_TOE_L],
					glm::inverse(character->joint_global_anim_xform[Character::JOINT_HEEL_L]) * glm::mat4(
						glm::vec4( side_l, 0),
						glm::vec4(-upwr_l, 0),
						glm::vec4( fwrd_l, 0),
						glm::vec4( toe_l, 1)), ik_toe_rot_blend.y);

			glm::vec3 toe_r     = glm::vec3(character->joint_global_anim_xform[Character::JOINT_TOE_R][3]);
			glm::vec4 fwrd_h_r  = character->joint_global_anim_xform[Character::JOINT_TOE_R] * glm::vec4( 0,0,10,1);
			glm::vec4 side_h0_r = character->joint_global_anim_xform[Character::JOINT_TOE_R] * glm::vec4( 10,0, 0,1);
			glm::vec4 side_h1_r = character->joint_global_anim_xform[Character::JOINT_TOE_R] * glm::vec4(-10,0, 0,1);
			glm::vec3 fwrd_r  = glm::vec3(fwrd_h_r) / fwrd_h_r.w;
			glm::vec3 side0_r = glm::vec3(side_h0_r) / side_h0_r.w;
			glm::vec3 side1_r = glm::vec3(side_h1_r) / side_h1_r.w;

			fwrd_r.y  = glm::clamp(heightmap->sample(glm::vec2(fwrd_r.x, fwrd_r.z))   + ik->toe_height, toe_r.y - toe_max_bend_d, toe_r.y + toe_max_bend_u);
			side0_r.y = glm::clamp(heightmap->sample(glm::vec2(side0_r.x, side0_r.z)) + ik->toe_height, toe_r.y - toe_max_bend_d, toe_r.y + toe_max_bend_u);
			side1_r.y = glm::clamp(heightmap->sample(glm::vec2(side1_r.x, side1_r.z)) + ik->toe_height, toe_r.y - toe_max_bend_d, toe_r.y + toe_max_bend_u);

			glm::vec3 side_r = glm::normalize(side0_r - side1_r);
			fwrd_r = glm::normalize(fwrd_r - toe_r);
			glm::vec3 upwr_r = glm::normalize(glm::cross(side_r, fwrd_r));
			side_r = glm::cross(fwrd_r, upwr_r);

			character->joint_anim_xform[Character::JOINT_TOE_R] = mix_transforms(
					character->joint_anim_xform[Character::JOINT_TOE_R],
					glm::inverse(character->joint_global_anim_xform[Character::JOINT_HEEL_R]) * glm::mat4(
						glm::vec4( side_r, 0),
						glm::vec4(-upwr_r, 0),
						glm::vec4( fwrd_r, 0),
						glm::vec4( toe_r, 1)), ik_toe_rot_blend.w);

			character->forward_kinematics();
		}

		/* Update Locks */

		if ((ik->lock[IK::HL] == 0.0) && (ik_weight.y >= ik->threshold)) {
			ik->lock[IK::HL] = 1.0; ik->position[IK::HL] = glm::vec3(character->joint_global_anim_xform[Character::JOINT_HEEL_L][3]);
			ik->lock[IK::TL] = 1.0; ik->position[IK::TL] = glm::vec3(character->joint_global_anim_xform[Character::JOINT_TOE_L][3]);
		}

		if ((ik->lock[IK::HR] == 0.0) && (ik_weight.w >= ik->threshold)) {
			ik->lock[IK::HR] = 1.0; ik->position[IK::HR] = glm::vec3(character->joint_global_anim_xform[Character::JOINT_HEEL_R][3]);
			ik->lock[IK::TR] = 1.0; ik->position[IK::TR] = glm::vec3(character->joint_global_anim_xform[Character::JOINT_TOE_R][3]);
		}

		if ((ik->lock[IK::HL] > 0.0) && (ik_weight.y < ik->threshold)) {
			ik->lock[IK::HL] = glm::clamp(ik->lock[IK::HL] - ik->fade, 0.0f, 1.0f);
			ik->lock[IK::TL] = glm::clamp(ik->lock[IK::TL] - ik->fade, 0.0f, 1.0f);
		}

		if ((ik->lock[IK::HR] > 0.0) && (ik_weight.w < ik->threshold)) {
			ik->lock[IK::HR] = glm::clamp(ik->lock[IK::HR] - ik->fade, 0.0f, 1.0f);
			ik->lock[IK::TR] = glm::clamp(ik->lock[IK::TR] - ik->fade, 0.0f, 1.0f);
		}

	}

}

void post_render() {

	/* Update Past Trajectory */

	for (int i = 0; i < Trajectory::LENGTH/2; i++) {
		trajectory->positions[i]  = trajectory->positions[i+1];
		trajectory->directions[i] = trajectory->directions[i+1];
		trajectory->rotations[i] = trajectory->rotations[i+1];
		trajectory->heights[i] = trajectory->heights[i+1];
		trajectory->gait_stand[i] = trajectory->gait_stand[i+1];
		trajectory->gait_walk[i] = trajectory->gait_walk[i+1];
		trajectory->gait_jog[i] = trajectory->gait_jog[i+1];
		trajectory->gait_crouch[i] = trajectory->gait_crouch[i+1];
		trajectory->gait_jump[i] = trajectory->gait_jump[i+1];
		trajectory->gait_bump[i] = trajectory->gait_bump[i+1];
	}

	/* Update Current Trajectory */

	float stand_amount = powf(1.0f-trajectory->gait_stand[Trajectory::LENGTH/2], 0.25f);

	glm::vec3 trajectory_update = (trajectory->rotations[Trajectory::LENGTH/2] * glm::vec3(pfnn->Yp(0), 0, pfnn->Yp(1)));
	trajectory->positions[Trajectory::LENGTH/2]  = trajectory->positions[Trajectory::LENGTH/2] + stand_amount * trajectory_update;
	trajectory->directions[Trajectory::LENGTH/2] = glm::mat3(glm::rotate(stand_amount * -pfnn->Yp(2), glm::vec3(0,1,0))) * trajectory->directions[Trajectory::LENGTH/2];
	trajectory->rotations[Trajectory::LENGTH/2] = glm::mat3(glm::rotate(atan2f(
					trajectory->directions[Trajectory::LENGTH/2].x,
					trajectory->directions[Trajectory::LENGTH/2].z), glm::vec3(0,1,0)));

	/* Collide with walls */

	for (int j = 0; j < areas->num_walls(); j++) {
		glm::vec2 trjpoint = glm::vec2(trajectory->positions[Trajectory::LENGTH/2].x, trajectory->positions[Trajectory::LENGTH/2].z);
		glm::vec2 segpoint = segment_nearest(areas->wall_start[j], areas->wall_stop[j], trjpoint);
		float segdist = glm::length(segpoint - trjpoint);
		if (segdist < areas->wall_width[j] + 100.0) {
			glm::vec2 prjpoint0 = (areas->wall_width[j] +   0.0f) * glm::normalize(trjpoint - segpoint) + segpoint; 
			glm::vec2 prjpoint1 = (areas->wall_width[j] + 100.0f) * glm::normalize(trjpoint - segpoint) + segpoint; 
			glm::vec2 prjpoint = glm::mix(prjpoint0, prjpoint1, glm::clamp((segdist - areas->wall_width[j]) / 100.0f, 0.0f, 1.0f));
			trajectory->positions[Trajectory::LENGTH/2].x = prjpoint.x;
			trajectory->positions[Trajectory::LENGTH/2].z = prjpoint.y;
		}
	}

	/* Update Future Trajectory */

	for (int i = Trajectory::LENGTH/2+1; i < Trajectory::LENGTH; i++) {
		int w = (Trajectory::LENGTH/2)/10;
		float m = fmod(((float)i - (Trajectory::LENGTH/2)) / 10.0, 1.0);
		trajectory->positions[i].x  = (1-m) * pfnn->Yp(8+(w*0)+(i/10)-w) + m * pfnn->Yp(8+(w*0)+(i/10)-w+1);
		trajectory->positions[i].z  = (1-m) * pfnn->Yp(8+(w*1)+(i/10)-w) + m * pfnn->Yp(8+(w*1)+(i/10)-w+1);
		trajectory->directions[i].x = (1-m) * pfnn->Yp(8+(w*2)+(i/10)-w) + m * pfnn->Yp(8+(w*2)+(i/10)-w+1);
		trajectory->directions[i].z = (1-m) * pfnn->Yp(8+(w*3)+(i/10)-w) + m * pfnn->Yp(8+(w*3)+(i/10)-w+1);
		trajectory->positions[i]    = (trajectory->rotations[Trajectory::LENGTH/2] * trajectory->positions[i]) + trajectory->positions[Trajectory::LENGTH/2];
		trajectory->directions[i]   = glm::normalize((trajectory->rotations[Trajectory::LENGTH/2] * trajectory->directions[i]));
		trajectory->rotations[i]    = glm::mat3(glm::rotate(atan2f(trajectory->directions[i].x, trajectory->directions[i].z), glm::vec3(0,1,0)));
	}

	/* Update Phase */

	character->phase = fmod(character->phase + (stand_amount * 0.9f + 0.1f) * 2*M_PI * pfnn->Yp(3), 2*M_PI);

	/* Update Camera */

	camera->target = glm::mix(camera->target, glm::vec3(
				trajectory->positions[Trajectory::LENGTH/2].x, 
				trajectory->heights[Trajectory::LENGTH/2] + 100, 
				trajectory->positions[Trajectory::LENGTH/2].z), 0.1);

}

void render() {

	/* Render Shadows */

	glm::mat4 light_view = glm::lookAt(camera->target + light->position, camera->target, glm::vec3(0,1,0));
	glm::mat4 light_proj = glm::ortho(-500.0f, 500.0f, -500.0f, 500.0f, 10.0f, 10000.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, light->fbo);

	glViewport(0, 0, 1024, 1024);
	glClearDepth(1.0f);  
	glClear(GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glUseProgram(shader_character_shadow->program);

	glUniformMatrix4fv(glGetUniformLocation(shader_character_shadow->program, "light_view"), 1, GL_FALSE, glm::value_ptr(light_view));
	glUniformMatrix4fv(glGetUniformLocation(shader_character_shadow->program, "light_proj"), 1, GL_FALSE, glm::value_ptr(light_proj));
	glUniformMatrix4fv(glGetUniformLocation(shader_character_shadow->program, "joints"), Character::JOINT_NUM, GL_FALSE, (float*)character->joint_mesh_xform);

	glBindBuffer(GL_ARRAY_BUFFER, character->vbo);

	glEnableVertexAttribArray(glGetAttribLocation(shader_character_shadow->program, "vPosition"));  
	glEnableVertexAttribArray(glGetAttribLocation(shader_character_shadow->program, "vWeightVal"));
	glEnableVertexAttribArray(glGetAttribLocation(shader_character_shadow->program, "vWeightIds"));

	glVertexAttribPointer(glGetAttribLocation(shader_character_shadow->program, "vPosition"),  3, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (void*)(sizeof(float) *  0));
	glVertexAttribPointer(glGetAttribLocation(shader_character_shadow->program, "vWeightVal"), 4, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (void*)(sizeof(float) *  7));
	glVertexAttribPointer(glGetAttribLocation(shader_character_shadow->program, "vWeightIds"), 4, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (void*)(sizeof(float) * 11));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, character->tbo);
	glDrawElements(GL_TRIANGLES, character->ntri, GL_UNSIGNED_INT, (void*)0);

	glDisableVertexAttribArray(glGetAttribLocation(shader_character_shadow->program, "vPosition"));  
	glDisableVertexAttribArray(glGetAttribLocation(shader_character_shadow->program, "vWeightVal"));
	glDisableVertexAttribArray(glGetAttribLocation(shader_character_shadow->program, "vWeightIds"));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/* Render Terrain */

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glClearDepth(1.0);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec3 light_direction = glm::normalize(light->target - light->position);

	glUseProgram(shader_terrain->program);

	glUniformMatrix4fv(glGetUniformLocation(shader_terrain->program, "view"), 1, GL_FALSE, glm::value_ptr(camera->view_matrix()));
	glUniformMatrix4fv(glGetUniformLocation(shader_terrain->program, "proj"), 1, GL_FALSE, glm::value_ptr(camera->proj_matrix()));
	glUniform3f(glGetUniformLocation(shader_terrain->program, "light_dir"), light_direction.x, light_direction.y, light_direction.z);

	glUniformMatrix4fv(glGetUniformLocation(shader_terrain->program, "light_view"), 1, GL_FALSE, glm::value_ptr(light_view));
	glUniformMatrix4fv(glGetUniformLocation(shader_terrain->program, "light_proj"), 1, GL_FALSE, glm::value_ptr(light_proj));

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, light->tex);
	glUniform1i(glGetUniformLocation(shader_terrain->program, "shadows"), 0);


	glBindBuffer(GL_ARRAY_BUFFER, heightmap->vbo);

	glEnableVertexAttribArray(glGetAttribLocation(shader_terrain->program, "vPosition"));  
	glEnableVertexAttribArray(glGetAttribLocation(shader_terrain->program, "vNormal"));
	glEnableVertexAttribArray(glGetAttribLocation(shader_terrain->program, "vAO"));

	glVertexAttribPointer(glGetAttribLocation(shader_terrain->program, "vPosition"), 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (void*)(sizeof(float) * 0));
	glVertexAttribPointer(glGetAttribLocation(shader_terrain->program, "vNormal"),   3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (void*)(sizeof(float) * 3));
	glVertexAttribPointer(glGetAttribLocation(shader_terrain->program, "vAO"), 1, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (void*)(sizeof(float) * 6));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, heightmap->tbo);
	glDrawElements(GL_TRIANGLES, ((heightmap->data.size()-1)/2) * ((heightmap->data[0].size()-1)/2) * 2 * 3, GL_UNSIGNED_INT, (void*)0);  
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(glGetAttribLocation(shader_terrain->program, "vPosition"));  
	glDisableVertexAttribArray(glGetAttribLocation(shader_terrain->program, "vNormal"));
	glDisableVertexAttribArray(glGetAttribLocation(shader_terrain->program, "vAO"));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(0);

	/* Render Character */

	glUseProgram(shader_character->program);

	glUniformMatrix4fv(glGetUniformLocation(shader_character->program, "view"), 1, GL_FALSE, glm::value_ptr(camera->view_matrix()));
	glUniformMatrix4fv(glGetUniformLocation(shader_character->program, "proj"), 1, GL_FALSE, glm::value_ptr(camera->proj_matrix()));
	glUniform3f(glGetUniformLocation(shader_character->program, "light_dir"), light_direction.x, light_direction.y, light_direction.z);

	glUniformMatrix4fv(glGetUniformLocation(shader_character->program, "light_view"), 1, GL_FALSE, glm::value_ptr(light_view));
	glUniformMatrix4fv(glGetUniformLocation(shader_character->program, "light_proj"), 1, GL_FALSE, glm::value_ptr(light_proj));

	glUniformMatrix4fv(glGetUniformLocation(shader_character->program, "joints"), Character::JOINT_NUM, GL_FALSE, (float*)character->joint_mesh_xform);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, light->tex);
	glUniform1i(glGetUniformLocation(shader_character->program, "shadows"), 0);

	glBindBuffer(GL_ARRAY_BUFFER, character->vbo);

	glEnableVertexAttribArray(glGetAttribLocation(shader_character->program, "vPosition"));  
	glEnableVertexAttribArray(glGetAttribLocation(shader_character->program, "vNormal"));
	glEnableVertexAttribArray(glGetAttribLocation(shader_character->program, "vAO"));
	glEnableVertexAttribArray(glGetAttribLocation(shader_character->program, "vWeightVal"));
	glEnableVertexAttribArray(glGetAttribLocation(shader_character->program, "vWeightIds"));

	glVertexAttribPointer(glGetAttribLocation(shader_character->program, "vPosition"),  3, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (void*)(sizeof(float) *  0));
	glVertexAttribPointer(glGetAttribLocation(shader_character->program, "vNormal"),    3, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (void*)(sizeof(float) *  3));
	glVertexAttribPointer(glGetAttribLocation(shader_character->program, "vAO"),        1, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (void*)(sizeof(float) *  6));
	glVertexAttribPointer(glGetAttribLocation(shader_character->program, "vWeightVal"), 4, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (void*)(sizeof(float) *  7));
	glVertexAttribPointer(glGetAttribLocation(shader_character->program, "vWeightIds"), 4, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (void*)(sizeof(float) * 11));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, character->tbo);
	glDrawElements(GL_TRIANGLES, character->ntri, GL_UNSIGNED_INT, (void*)0);

	glDisableVertexAttribArray(glGetAttribLocation(shader_character->program, "vPosition"));  
	glDisableVertexAttribArray(glGetAttribLocation(shader_character->program, "vNormal"));  
	glDisableVertexAttribArray(glGetAttribLocation(shader_character->program, "vAO"));  
	glDisableVertexAttribArray(glGetAttribLocation(shader_character->program, "vWeightVal"));
	glDisableVertexAttribArray(glGetAttribLocation(shader_character->program, "vWeightIds"));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(0);

	/* Render the Rest */

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(glm::value_ptr(camera->view_matrix()));

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(camera->proj_matrix()));

	/* Render Crouch Area */

	if (options->display_debug) {
		for (int i = 0; i < areas->num_crouches(); i++) {

			glm::vec3 c0 = areas->crouch_pos[i] + glm::vec3( areas->crouch_size[i].x/2, 0,  areas->crouch_size[i].y/2);
			glm::vec3 c1 = areas->crouch_pos[i] + glm::vec3(-areas->crouch_size[i].x/2, 0,  areas->crouch_size[i].y/2);
			glm::vec3 c2 = areas->crouch_pos[i] + glm::vec3( areas->crouch_size[i].x/2, 0, -areas->crouch_size[i].y/2);
			glm::vec3 c3 = areas->crouch_pos[i] + glm::vec3(-areas->crouch_size[i].x/2, 0, -areas->crouch_size[i].y/2);

			glColor3f(0.0, 0.0, 1.0);
			glLineWidth(options->display_scale * 2.0);
			glBegin(GL_LINES);
			glVertex3f(c0.x, c0.y, c0.z); glVertex3f(c1.x, c1.y, c1.z);
			glVertex3f(c0.x, c0.y, c0.z); glVertex3f(c2.x, c2.y, c2.z);
			glVertex3f(c3.x, c3.y, c3.z); glVertex3f(c1.x, c1.y, c1.z);
			glVertex3f(c3.x, c3.y, c3.z); glVertex3f(c2.x, c2.y, c2.z);

			for (float j = 0; j < 1.0; j+=0.05) {
				glm::vec3 cm_a = glm::mix(c0, c1, j     );
				glm::vec3 cm_b = glm::mix(c0, c1, j+0.05);
				glm::vec3 cm_c = glm::mix(c3, c2, j     );
				glm::vec3 cm_d = glm::mix(c3, c2, j+0.05);
				float cmh_a = ((sinf(cm_a.x/Areas::CROUCH_WAVE)+1.0)/2.0) * 50 + 130;        
				float cmh_b = ((sinf(cm_b.x/Areas::CROUCH_WAVE)+1.0)/2.0) * 50 + 130;
				float cmh_c = ((sinf(cm_c.x/Areas::CROUCH_WAVE)+1.0)/2.0) * 50 + 130;        
				float cmh_d = ((sinf(cm_d.x/Areas::CROUCH_WAVE)+1.0)/2.0) * 50 + 130;
				glVertex3f(cm_a.x, cmh_a, cm_a.z);
				glVertex3f(cm_b.x, cmh_b, cm_b.z);
				glVertex3f(cm_c.x, cmh_c, cm_c.z);
				glVertex3f(cm_d.x, cmh_d, cm_d.z);
				glVertex3f(cm_a.x, cmh_a, cm_a.z);
				glVertex3f(cm_a.x, cmh_a,   c2.z);
				if (j + 0.05 >= 1.0) {
					glVertex3f(cm_b.x, cmh_b, cm_b.z);
					glVertex3f(cm_b.x, cmh_b,   c2.z);
				}
			}

			float c0h = ((sinf(c0.x/Areas::CROUCH_WAVE)+1.0)/2.0) * 50 + 130;
			float c1h = ((sinf(c1.x/Areas::CROUCH_WAVE)+1.0)/2.0) * 50 + 130;
			float c2h = ((sinf(c2.x/Areas::CROUCH_WAVE)+1.0)/2.0) * 50 + 130;
			float c3h = ((sinf(c3.x/Areas::CROUCH_WAVE)+1.0)/2.0) * 50 + 130;

			glVertex3f(c0.x, c0.y + c0h, c0.z); glVertex3f(c0.x, c0.y, c0.z);
			glVertex3f(c1.x, c1.y + c1h, c1.z); glVertex3f(c1.x, c1.y, c1.z);
			glVertex3f(c2.x, c2.y + c2h, c2.z); glVertex3f(c2.x, c2.y, c2.z);
			glVertex3f(c3.x, c3.y + c3h, c3.z); glVertex3f(c3.x, c3.y, c3.z);

			glEnd();
			glLineWidth(1.0);
			glColor3f(1.0, 1.0, 1.0);
		}
	}

	/* Render Jump Areas */

	if (options->display_debug && options->display_areas_jump) {
		for (int i = 0; i < areas->num_jumps(); i++) {
			glColor3f(1.0, 0.0, 0.0);
			glLineWidth(options->display_scale * 2.0);
			glBegin(GL_LINES);
			for (float r = 0; r < 1.0; r+=0.01) {
				glVertex3f(areas->jump_pos[i].x + areas->jump_size[i] * sin((r+0.00)*2*M_PI), areas->jump_pos[i].y, areas->jump_pos[i].z + areas->jump_size[i] * cos((r+0.00)*2*M_PI));
				glVertex3f(areas->jump_pos[i].x + areas->jump_size[i] * sin((r+0.01)*2*M_PI), areas->jump_pos[i].y, areas->jump_pos[i].z + areas->jump_size[i] * cos((r+0.01)*2*M_PI));
			}
			glEnd();
			glLineWidth(1.0);
			glColor3f(1.0, 1.0, 1.0);
		}
	}

	/* Render Walls */

	if (options->display_debug && options->display_areas_walls) {
		for (int i = 0; i < areas->num_walls(); i++) {
			glColor3f(0.0, 1.0, 0.0);
			glLineWidth(options->display_scale * 2.0);
			glBegin(GL_LINES);
			for (float r = 0; r < 1.0; r+=0.1) {
				glm::vec2 p0 = glm::mix(areas->wall_start[i], areas->wall_stop[i], r    );
				glm::vec2 p1 = glm::mix(areas->wall_start[i], areas->wall_stop[i], r+0.1);
				glVertex3f(p0.x, heightmap->sample(p0) + 5, p0.y);
				glVertex3f(p1.x, heightmap->sample(p1) + 5, p1.y);
			}
			glEnd();
			glLineWidth(1.0);
			glColor3f(1.0, 1.0, 1.0);
		}

	}

	/* Render Trajectory */

	if (options->display_debug) {
		glPointSize(1.0 * options->display_scale);
		glBegin(GL_POINTS);
		for (int i = 0; i < Trajectory::LENGTH-10; i++) {
			glm::vec3 position_c = trajectory->positions[i];
			glColor3f(trajectory->gait_jump[i], trajectory->gait_bump[i], trajectory->gait_crouch[i]);
			glVertex3f(position_c.x, position_c.y + 2.0, position_c.z);
		}
		glEnd();
		glColor3f(1.0, 1.0, 1.0);
		glPointSize(1.0);


		glPointSize(4.0 * options->display_scale);
		glBegin(GL_POINTS);
		for (int i = 0; i < Trajectory::LENGTH; i+=10) {
			glm::vec3 position_c = trajectory->positions[i];
			glColor3f(trajectory->gait_jump[i], trajectory->gait_bump[i], trajectory->gait_crouch[i]);
			glVertex3f(position_c.x, position_c.y + 2.0, position_c.z);
		}
		glEnd();
		glColor3f(1.0, 1.0, 1.0);
		glPointSize(1.0);

		if (options->display_debug_heights) {
			glPointSize(2.0 * options->display_scale);
			glBegin(GL_POINTS);
			for (int i = 0; i < Trajectory::LENGTH; i+=10) {
				glm::vec3 position_r = trajectory->positions[i] + (trajectory->rotations[i] * glm::vec3( trajectory->width, 0, 0));
				glm::vec3 position_l = trajectory->positions[i] + (trajectory->rotations[i] * glm::vec3(-trajectory->width, 0, 0));
				glColor3f(trajectory->gait_jump[i], trajectory->gait_bump[i], trajectory->gait_crouch[i]);
				glVertex3f(position_r.x, heightmap->sample(glm::vec2(position_r.x, position_r.z)) + 2.0, position_r.z);
				glVertex3f(position_l.x, heightmap->sample(glm::vec2(position_l.x, position_l.z)) + 2.0, position_l.z);
			}
			glEnd();
			glColor3f(1.0, 1.0, 1.0);
			glPointSize(1.0);
		}

		glLineWidth(1.0 * options->display_scale);
		glBegin(GL_LINES);
		// trajectory
		for (int i = 0; i < Trajectory::LENGTH; i+=10) {
			glm::vec3 base = trajectory->positions[i] + glm::vec3(0.0, 2.0, 0.0);
			glm::vec3 side = glm::normalize(glm::cross(trajectory->directions[i], glm::vec3(0.0, 1.0, 0.0)));
			glm::vec3 fwrd = base + 15.0f * trajectory->directions[i];
			fwrd.y = heightmap->sample(glm::vec2(fwrd.x, fwrd.z)) + 2.0;
			glm::vec3 arw0 = fwrd +  4.0f * side + 4.0f * -trajectory->directions[i];
			glm::vec3 arw1 = fwrd -  4.0f * side + 4.0f * -trajectory->directions[i];
			glColor3f(trajectory->gait_jump[i], trajectory->gait_bump[i], trajectory->gait_crouch[i]);
			glVertex3f(base.x, base.y, base.z);
			glVertex3f(fwrd.x, fwrd.y, fwrd.z);
			glVertex3f(fwrd.x, fwrd.y, fwrd.z);
			glVertex3f(arw0.x, fwrd.y, arw0.z);
			glVertex3f(fwrd.x, fwrd.y, fwrd.z);
			glVertex3f(arw1.x, fwrd.y, arw1.z);
		}
		// Waypoint render
		// TODO 2
		auto currentPosition = glm::vec3(
				trajectory->positions[Trajectory::LENGTH/2].x, 
				trajectory->heights[Trajectory::LENGTH/2],
				trajectory->positions[Trajectory::LENGTH/2].z);
		auto x = waypointX[waypointIndex];
		auto z = waypointZ[waypointIndex];
		auto waypointPosition = glm::vec3(x, heightmap->sample(glm::vec2(x, z)) + 1.0f, z);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(currentPosition.x, currentPosition.y, currentPosition.z);
		glVertex3f(waypointPosition.x, waypointPosition.y, waypointPosition.z);

		// Control render
		auto targetVelocity = currentPosition + 50.0f * trajectory->target_vel;

		glLineWidth(5.0);
		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex3f(currentPosition.x, currentPosition.y + 1.1f, currentPosition.z);
		glVertex3f(targetVelocity.x, targetVelocity.y + 1.1f, targetVelocity.z);

		// Direction render
		auto currentDirection = currentPosition + 50.0f * trajectory->directions[Trajectory::LENGTH/2];

		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(currentPosition.x, currentPosition.y + 1.2f, currentPosition.z);
		glVertex3f(currentDirection.x, currentDirection.y + 1.2f, currentDirection.z);

		// END


		glEnd();
		glLineWidth(1.0);
		glColor3f(1.0, 1.0, 1.0);
	}

	/* Render Joints */

	if (options->display_debug && options->display_debug_joints) {
		glDisable(GL_DEPTH_TEST);
		glPointSize(3.0 * options->display_scale);
		glColor3f(0.6, 0.3, 0.4);      
		glBegin(GL_POINTS);
		for (int i = 0; i < Character::JOINT_NUM; i++) {
			glm::vec3 pos = character->joint_positions[i];
			glVertex3f(pos.x, pos.y, pos.z);
		}
		glEnd();
		glPointSize(1.0);

		glLineWidth(1.0 * options->display_scale);
		glColor3f(0.6, 0.3, 0.4);      
		glBegin(GL_LINES);
		for (int i = 0; i < Character::JOINT_NUM; i++) {
			glm::vec3 pos = character->joint_positions[i];
			glm::vec3 vel = pos - 5.0f * character->joint_velocities[i];
			glVertex3f(pos.x, pos.y, pos.z);
			glVertex3f(vel.x, vel.y, vel.z);
		}
		glEnd();
		glLineWidth(1.0);
		glEnable(GL_DEPTH_TEST);
	}

	/* UI Elements */

	glm::mat4 ui_view = glm::mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
	glm::mat4 ui_proj = glm::ortho(0.0f, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, 0.0f, 0.0f, 1.0f);  

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(ui_proj));

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(glm::value_ptr(ui_view));

	/* PFNN Visual */

	if (options->display_debug && options->display_debug_pfnn) {

		glColor3f(0.0, 0.0, 0.0);  

		glLineWidth(5);
		glBegin(GL_LINES);
		for (float i = 0; i < 2*M_PI; i+=0.01) {
			glVertex3f(WINDOW_WIDTH-125+50*cos(i     ),100+50*sin(i     ),0);    
			glVertex3f(WINDOW_WIDTH-125+50*cos(i+0.01),100+50*sin(i+0.01),0);
		}
		glEnd();
		glLineWidth(1);

		glPointSize(20);
		glBegin(GL_POINTS);
		glVertex3f(WINDOW_WIDTH-125+50*cos(character->phase), 100+50*sin(character->phase), 0);
		glEnd();
		glPointSize(1);

		glColor3f(1.0, 1.0, 1.0); 

		int pindex_1 = (int)((character->phase / (2*M_PI)) * 50);
		MatrixXf W0p = pfnn->W0[pindex_1];

		glPointSize(1);
		glBegin(GL_POINTS);

		for (int x = 0; x < W0p.rows(); x++)
			for (int y = 0; y < W0p.cols(); y++) {
				float v = (W0p(x, y)+0.5)/2.0;
				glm::vec3 col = v > 0.5 ? glm::mix(glm::vec3(1,0,0), glm::vec3(0,0,1), v-0.5) : glm::mix(glm::vec3(0,1,0), glm::vec3(0,0,1), v*2.0);
				glColor3f(col.x, col.y, col.z); 
				glVertex3f(WINDOW_WIDTH-W0p.cols()+y-25, x+175, 0);
			}

		glEnd();
		glPointSize(1);

	}

	/* Display UI */

	if (options->display_debug && options->display_hud_options) {
		glLineWidth(3);
		glBegin(GL_LINES);
		glColor3f(0.0, 0.0, 0.0); 
		glVertex3f(125+20,20,0); glVertex3f(125+20,40,0); /* I */
		glVertex3f(125+25,20,0); glVertex3f(125+25,40,0); /* K */
		glVertex3f(125+25,30,0); glVertex3f(125+30,40,0);
		glVertex3f(125+25,30,0); glVertex3f(125+30,20,0);
		glEnd();
		glLineWidth(1);

		if (options->enable_ik) { glColor3f(0.0, 1.0, 0.0); } else { glColor3f(1.0, 0.0, 0.0); }
		glPointSize(10);
		glBegin(GL_POINTS);
		glVertex3f(125+60, 30, 0);
		glEnd();
		glPointSize(1); 
		glColor3f(1.0, 1.0, 1.0); 
	}

	if (options->display_debug && options->display_hud_options) {
		glLineWidth(3);
		glBegin(GL_LINES);
		glColor3f(0.0, 0.0, 0.0); 
		glVertex3f(125+20,50,0); glVertex3f(125+20,70,0); /* D */
		glVertex3f(125+20,50,0); glVertex3f(125+25,55,0);
		glVertex3f(125+20,70,0); glVertex3f(125+25,65,0);
		glVertex3f(125+25,65,0); glVertex3f(125+25,55,0);
		glVertex3f(125+30,50,0); glVertex3f(125+30,70,0); /* I */
		glVertex3f(125+35,50,0); glVertex3f(125+35,70,0); /* R */
		glVertex3f(125+35,60,0); glVertex3f(125+40,70,0);
		glVertex3f(125+35,50,0); glVertex3f(125+40,55,0);
		glVertex3f(125+35,60,0); glVertex3f(125+40,55,0);
		glEnd();
		glLineWidth(1);

		glColor3f(1.0-character->strafe_amount, character->strafe_amount, 0.0);
		glPointSize(10);
		glBegin(GL_POINTS);
		glVertex3f(125+60, 60, 0);
		glEnd();
		glPointSize(1); 
		glColor3f(1.0, 1.0, 1.0); 
	}

	if (options->display_debug && options->display_hud_options) {
		glLineWidth(3);
		glBegin(GL_LINES);
		glColor3f(0.0, 0.0, 0.0); 
		glVertex3f(125+20,80,0); glVertex3f(125+20,100,0); /* R */
		glVertex3f(125+20,90,0); glVertex3f(125+25,100,0);
		glVertex3f(125+20,80,0); glVertex3f(125+25,85,0);
		glVertex3f(125+20,90,0); glVertex3f(125+25,85,0);
		glVertex3f(125+30,80,0); glVertex3f(125+30,100,0); /* E */
		glVertex3f(125+30,80,0); glVertex3f(125+35,80,0); 
		glVertex3f(125+30,90,0); glVertex3f(125+35,90,0); 
		glVertex3f(125+30,100,0); glVertex3f(125+35,100,0); 
		glVertex3f(125+40,80,0); glVertex3f(125+40,90,0); /* S */
		glVertex3f(125+45,90,0); glVertex3f(125+45,100,0);
		glVertex3f(125+40,80,0); glVertex3f(125+45,80,0); 
		glVertex3f(125+40,90,0); glVertex3f(125+45,90,0); 
		glVertex3f(125+40,100,0); glVertex3f(125+45,100,0); 
		glEnd();
		glLineWidth(1);

		glColor3f(1.0-character->responsive, character->responsive, 0.0);
		glPointSize(10);
		glBegin(GL_POINTS);
		glVertex3f(125+60, 90, 0);
		glEnd();
		glPointSize(1); 
		glColor3f(1.0, 1.0, 1.0); 
	}


	if (options->display_debug && options->display_hud_stick) {
		glColor3f(0.0, 0.0, 0.0); 
		glPointSize(1 * options->display_scale);
		glBegin(GL_POINTS);
		for (float i = 0; i < 1.0; i+=0.025) {
			glVertex3f(60+40*cos(2*M_PI*(i     )),60+40*sin(2*M_PI*(i     )), 0);    
		}
		glEnd();

		int x_vel = -SDL_JoystickGetAxis(stick, GAMEPAD_STICK_L_HORIZONTAL);
		int y_vel = -SDL_JoystickGetAxis(stick, GAMEPAD_STICK_L_VERTICAL); 
		if (abs(x_vel) + abs(y_vel) < 10000) { x_vel = 0; y_vel = 0; };  

		glm::vec2 direction = glm::vec2((-x_vel) / 32768.0f, (-y_vel) / 32768.0f);
		glLineWidth(1 * options->display_scale);    
		glBegin(GL_LINES);
		glVertex3f(60, 60, 0);
		glVertex3f(60+direction.x*40, 60+direction.y*40, 0);
		glEnd();
		glLineWidth(1.0);    

		glPointSize(3 * options->display_scale);
		glBegin(GL_POINTS);
		glVertex3f(60, 60, 0);
		glVertex3f(60+direction.x*40, 60+direction.y*40, 0);
		glEnd();
		glPointSize(1);

		float speed0 = ((SDL_JoystickGetAxis(stick, 5) / 32768.0) + 1.0) / 2.0;

		glPointSize(1 * options->display_scale);
		glBegin(GL_POINTS);
		for (float i = 0; i < 1.0; i+=0.09) {
			glVertex3f(120, 100 - i * 80, 0);    
		}
		glEnd();

		glLineWidth(1 * options->display_scale);    
		glBegin(GL_LINES);
		glVertex3f(120, 100, 0);
		glVertex3f(120, 100 - speed0 * 80, 0);
		glEnd();
		glLineWidth(1.0); 

		glPointSize(3 * options->display_scale);
		glBegin(GL_POINTS);
		glVertex3f(120, 100, 0);
		glVertex3f(120, 100 - speed0 * 80, 0);
		glEnd();
		glPointSize(1);

		glColor3f(1.0, 1.0, 1.0); 
	}

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);


}



int main(int argc, char **argv) {

	// Create a sample listener and controller
	Controller controller;
	listener = new SampleListener();

	// Have the sample listener receive events from the controller
	controller.addListener(*listener);
	/* Init SDL */

	SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_Window *window = SDL_CreateWindow(
			"PFNN",
			SDL_WINDOWPOS_CENTERED, 
			SDL_WINDOWPOS_CENTERED,
			WINDOW_WIDTH, WINDOW_HEIGHT,
			SDL_WINDOW_OPENGL);

	if (!window) {
		printf("Could not create window: %s\n", SDL_GetError());
		return 1;
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);
	SDL_GL_SetSwapInterval(1);

	stick = SDL_JoystickOpen(0);

	/* Init GLEW */

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(1);
	}

	/* Resources */

	options = new PFNNOptions();
	camera = new CameraOrbit(WINDOW_WIDTH, WINDOW_HEIGHT);
	light = new LightDirectional();

	character = new Character();
	character->load(
			"./network/character_vertices.bin", 
			"./network/character_triangles.bin", 
			"./network/character_parents.bin", 
			"./network/character_xforms.bin");

	trajectory = new Trajectory();
	ik = new IK();

	shader_terrain = new Shader();
	shader_terrain_shadow = new Shader();
	shader_character = new Shader();
	shader_character_shadow = new Shader();
	shader_terrain->load("./shaders/terrain.vs", "./shaders/terrain_low.fs");
	shader_terrain_shadow->load("./shaders/terrain_shadow.vs", "./shaders/terrain_shadow.fs");
	shader_character->load("./shaders/character.vs", "./shaders/character_low.fs");
	shader_character_shadow->load("./shaders/character_shadow.vs", "./shaders/character_shadow.fs");

	heightmap = new Heightmap();
	areas = new Areas();

	pfnn = new PFNN(PFNN::MODE_CONSTANT);
	pfnn->load();

	reset(load_world0(*heightmap, *areas));

	/* Game Loop */

	static bool running = true;

	while (running) {

		SDL_Event event;
		playerDx = 0;
		playerDy = 0;
		while (SDL_PollEvent(&event)) {

			if (event.type == SDL_QUIT) { running = false; break; }

			if (event.type == SDL_JOYBUTTONDOWN) {
				if (event.jbutton.button == GAMEPAD_B) {
					character->crouched_target = character->crouched_target ? 0.0 : 1.0;
				}
				if (event.jbutton.button == GAMEPAD_BACK) {
					character->responsive = !character->responsive;
				}
				if (event.jbutton.button == GAMEPAD_X) {
					options->display_debug = !options->display_debug;
				}
				if (event.jbutton.button == GAMEPAD_START) {
					options->enable_ik = !options->enable_ik;
				}
			}

			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE: running = false; break;
					case SDLK_1: reset(load_world0(*heightmap, *areas)); break;
					case SDLK_2: reset(load_world1(*heightmap, *areas)); break;
					case SDLK_3: reset(load_world2(*heightmap, *areas)); break;
					case SDLK_4: reset(load_world3(*heightmap, *areas)); break;
					case SDLK_5: reset(load_world4(*heightmap, *areas)); break;
					case SDLK_6: reset(load_world5(*heightmap, *areas)); break;
					case SDLK_w:
						     playerDy += 20;
						     break;
					case SDLK_s:
						     playerDy -= 20;
						     break;
					case SDLK_a:
						     playerDx += 20;
						     break;
					case SDLK_d:
						     playerDx -= 20;
						     break;
				}

			}

			if (event.type == SDL_MOUSEWHEEL) {
				playerVelocity = std::max(0, playerVelocity + event.wheel.y * 1000);
				std::cout << playerVelocity << "\n";
			}
		}

		pre_render();
		render();
		post_render();

		glFlush();
		glFinish();

		SDL_GL_SwapWindow(window);
	}

	/* Delete Resources */

	delete options;
	delete camera;
	delete light;
	delete character;
	delete trajectory;
	delete ik;
	delete shader_terrain;
	delete shader_terrain_shadow;
	delete shader_character;
	delete shader_character_shadow;
	delete heightmap;
	delete areas;
	delete pfnn;

	SDL_JoystickClose(stick);
	SDL_GL_DeleteContext(context);  
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
