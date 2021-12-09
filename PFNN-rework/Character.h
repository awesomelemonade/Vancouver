#pragma once


struct Character {

	enum { JOINT_NUM = 31 };

	GLuint vbo, tbo;
	int ntri, nvtx;
	float phase;
	float strafe_amount;
	float strafe_target;
	float crouched_amount;
	float crouched_target;
	float responsive;

	glm::vec3 joint_positions[JOINT_NUM];
	glm::vec3 joint_velocities[JOINT_NUM];
	glm::mat3 joint_rotations[JOINT_NUM];

	glm::mat4 joint_anim_xform[JOINT_NUM];
	glm::mat4 joint_rest_xform[JOINT_NUM];
	glm::mat4 joint_mesh_xform[JOINT_NUM];
	glm::mat4 joint_global_rest_xform[JOINT_NUM];
	glm::mat4 joint_global_anim_xform[JOINT_NUM];

	int joint_parents[JOINT_NUM];

	enum {
		JOINT_ROOT_L = 1,
		JOINT_HIP_L  = 2,
		JOINT_KNEE_L = 3,  
		JOINT_HEEL_L = 4,
		JOINT_TOE_L  = 5,  

		JOINT_ROOT_R = 6,  
		JOINT_HIP_R  = 7,  
		JOINT_KNEE_R = 8,  
		JOINT_HEEL_R = 9,
		JOINT_TOE_R  = 10  
	};

	Character()
		: vbo(0)
		  , tbo(0)
		  , ntri(66918)
		  , nvtx(11200)
		  , phase(0)
		  , strafe_amount(0)
		  , strafe_target(0)
		  , crouched_amount(0) 
		  , crouched_target(0) 
		  , responsive(0) {}

	~Character() {
		if (vbo != 0) { glDeleteBuffers(1, &vbo); vbo = 0; }
		if (tbo != 0) { glDeleteBuffers(1, &tbo); tbo = 0; }
	}

	void load(const char* filename_v, const char* filename_t, const char* filename_p, const char* filename_r) {

		printf("Read Character '%s %s'\n", filename_v, filename_t);

		if (vbo != 0) { glDeleteBuffers(1, &vbo); vbo = 0; }
		if (tbo != 0) { glDeleteBuffers(1, &tbo); tbo = 0; }

		glGenBuffers(1, &vbo);
		glGenBuffers(1, &tbo);

		FILE *f;

		f = fopen(filename_v, "rb");
		float *vbo_data = (float*)malloc(sizeof(float) * 15 * nvtx);
		int result = fread(vbo_data, sizeof(float) * 15 * nvtx, 1, f);
		if (result != 1) {
			std::cout << "Reading Error\n";
		}
		fclose(f);

		f = fopen(filename_t, "rb");
		uint32_t *tbo_data = (uint32_t*)malloc(sizeof(uint32_t) * ntri);  
		result = fread(tbo_data, sizeof(uint32_t) * ntri, 1, f);
		if (result != 1) {
			std::cout << "Reading Error\n";
		}
		fclose(f);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 15 * nvtx, vbo_data, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * ntri, tbo_data, GL_STATIC_DRAW);

		free(vbo_data);
		free(tbo_data);

		f = fopen(filename_p, "rb");
		float fparents[JOINT_NUM];
		result = fread(fparents, sizeof(float) * JOINT_NUM, 1, f);
		if (result != 1) {
			std::cout << "Reading Error\n";
		}
		for (int i = 0; i < JOINT_NUM; i++) { joint_parents[i] = (int)fparents[i]; }
		fclose(f);

		f = fopen(filename_r, "rb");
		result = fread(glm::value_ptr(joint_rest_xform[0]), sizeof(float) * JOINT_NUM * 4 * 4, 1, f);
		if (result != 1) {
			std::cout << "Reading Error\n";
		}
		for (int i = 0; i < JOINT_NUM; i++) { joint_rest_xform[i] = glm::transpose(joint_rest_xform[i]); }
		fclose(f);

	}

	void forward_kinematics() {

		for (int i = 0; i < JOINT_NUM; i++) {
			joint_global_anim_xform[i] = joint_anim_xform[i];
			joint_global_rest_xform[i] = joint_rest_xform[i];
			int j = joint_parents[i];
			while (j != -1) {
				joint_global_anim_xform[i] = joint_anim_xform[j] * joint_global_anim_xform[i];
				joint_global_rest_xform[i] = joint_rest_xform[j] * joint_global_rest_xform[i];
				j = joint_parents[j];
			}
			joint_mesh_xform[i] = joint_global_anim_xform[i] * glm::inverse(joint_global_rest_xform[i]);
		}

	}

};
