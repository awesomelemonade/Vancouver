#pragma once


#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>


struct LightDirectional {

	glm::vec3 target;
	glm::vec3 position;

	GLuint fbo;
	GLuint buf;
	GLuint tex;

	LightDirectional()
		: target(glm::vec3(0))
		  , position(glm::vec3(3000, 3700, 1500))
		  , fbo(0)
		  , buf(0)
		  , tex(0) {

			  glGenFramebuffers(1, &fbo);
			  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			  glDrawBuffer(GL_NONE);
			  glReadBuffer(GL_NONE);

			  glGenRenderbuffers(1, &buf);
			  glBindRenderbuffer(GL_RENDERBUFFER, buf);
#ifdef HIGH_QUALITY
			  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 2048, 2048);
#else
			  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 1024);
#endif
			  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buf);  

			  glGenTextures(1, &tex);
			  glBindTexture(GL_TEXTURE_2D, tex);
#ifdef HIGH_QUALITY
			  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 2048, 2048, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
#else
			  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
#endif
			  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			  glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
			  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex, 0);

			  glBindTexture(GL_TEXTURE_2D, 0);
			  glBindRenderbuffer(GL_RENDERBUFFER, 0);
			  glBindFramebuffer(GL_FRAMEBUFFER, 0);

		  }

	~LightDirectional() {
		glDeleteBuffers(1, &fbo);
		glDeleteBuffers(1, &buf);
		glDeleteTextures(1, &tex);
	}

};
