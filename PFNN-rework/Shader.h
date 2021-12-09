#pragma once


struct Shader {

	GLuint program;
	GLuint vs, fs;

	Shader()
		: program(0)
		  , vs(0)
		  , fs(0) { }

	~Shader() {
		if (vs != 0) { glDeleteShader(vs); vs = 0; }
		if (fs != 0) { glDeleteShader(fs); fs = 0; }
		if (program != 0) { glDeleteShader(program); program = 0; }
	}

	void load_shader(const char* filename, GLenum type, GLuint *shader) {

		SDL_RWops* file = SDL_RWFromFile(filename, "r");
		if(file == NULL) {
			fprintf(stderr, "Cannot load file %s\n", filename);
			exit(1);
		}

		long size = SDL_RWseek(file,0,SEEK_END);
		char* contents = (char*)malloc(size+1);
		contents[size] = '\0';

		SDL_RWseek(file, 0, SEEK_SET);
		SDL_RWread(file, contents, size, 1);
		SDL_RWclose(file);

		*shader = glCreateShader(type);

		glShaderSource(*shader, 1, (const char**)&contents, NULL);
		glCompileShader(*shader);

		free(contents);

		char log[2048];
		int i;
		glGetShaderInfoLog(*shader, 2048, &i, log);
		log[i] = '\0';
		if (strcmp(log, "") != 0) { printf("%s\n", log); }

		int compile_error = 0;
		glGetShaderiv(*shader, GL_COMPILE_STATUS, &compile_error);
		if (compile_error == GL_FALSE) {
			fprintf(stderr, "Compiler Error on Shader %s.\n", filename);
			exit(1);
		}
	}


	void load(const char* vertex, const char* fragment) {

		if (vs != 0) { glDeleteShader(vs); vs = 0; }
		if (fs != 0) { glDeleteShader(fs); fs = 0; }
		if (program != 0) { glDeleteShader(program); program = 0; }

		program = glCreateProgram();
		load_shader(vertex, GL_VERTEX_SHADER, &vs);
		load_shader(fragment, GL_FRAGMENT_SHADER, &fs);
		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);

		char log[2048];
		int i;
		glGetProgramInfoLog(program, 2048, &i, log);
		log[i] = '\0';
		if (strcmp(log, "") != 0) { printf("%s\n", log); }    
	}

};
