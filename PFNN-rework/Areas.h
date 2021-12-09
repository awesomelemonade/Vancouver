#pragma once



struct Areas {

	std::vector<glm::vec3> crouch_pos;
	std::vector<glm::vec2> crouch_size;
	static constexpr float CROUCH_WAVE = 50;

	std::vector<glm::vec3> jump_pos;
	std::vector<float> jump_size;
	std::vector<float> jump_falloff;

	std::vector<glm::vec2> wall_start;
	std::vector<glm::vec2> wall_stop;
	std::vector<float> wall_width;

	void clear() {
		crouch_pos.clear();
		crouch_size.clear();
		jump_pos.clear();
		jump_size.clear();
		jump_falloff.clear();
		wall_start.clear();
		wall_stop.clear();
		wall_width.clear();
	}

	void add_wall(glm::vec2 start, glm::vec2 stop, float width) {
		wall_start.push_back(start);
		wall_stop.push_back(stop);
		wall_width.push_back(width);
	}

	void add_crouch(glm::vec3 pos, glm::vec2 size) {
		crouch_pos.push_back(pos);
		crouch_size.push_back(size);
	}

	void add_jump(glm::vec3 pos, float size, float falloff) {
		jump_pos.push_back(pos);
		jump_size.push_back(size);
		jump_falloff.push_back(falloff);
	}

	int num_walls() { return wall_start.size(); }
	int num_crouches() { return crouch_pos.size(); }
	int num_jumps() { return jump_pos.size(); }

};
