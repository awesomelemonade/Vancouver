#pragma once

struct PFNNOptions {

	bool invert_y;

	bool enable_ik;

	bool display_debug;
	bool display_debug_heights;
	bool display_debug_joints;
	bool display_debug_pfnn;
	bool display_hud_options;
	bool display_hud_stick;
	bool display_hud_speed;

	bool display_areas_jump;
	bool display_areas_walls;

	float display_scale;

	float extra_direction_smooth;
	float extra_velocity_smooth;
	float extra_strafe_smooth;
	float extra_crouched_smooth;
	float extra_gait_smooth;
	float extra_joint_smooth;

	PFNNOptions()
		: invert_y(false)
		  , enable_ik(true)
		  , display_debug(true)
		  , display_debug_heights(true)
		  , display_debug_joints(false)
		  , display_debug_pfnn(false)
		  , display_hud_options(true)
		  , display_hud_stick(true)
		  , display_hud_speed(true)
		  , display_areas_jump(false)
		  , display_areas_walls(false)
			  , display_scale(2.0)
			  , extra_direction_smooth(0.9)
			  , extra_velocity_smooth(0.9)
			  , extra_strafe_smooth(0.9)
			  , extra_crouched_smooth(0.9)
			  , extra_gait_smooth(0.1)
			  , extra_joint_smooth(0.5)
			  {}
};
