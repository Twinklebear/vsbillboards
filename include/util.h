#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <array>
#include <string>
#include <glm/glm.hpp>
#include "gl_core_3_3.h"

namespace util {
	//Convert degrees to radians
	constexpr float deg_to_rad(float deg){
		return deg * 0.01745f;
	}
	/*
	 * Get the resource path for resources located in res/sub_dir
	 */
	std::string get_resource_path(const std::string &sub_dir = "");
	/*
	* Read the entire contents of a file into a string, if an error occurs
	* the string will be empty
	*/
	std::string read_file(const std::string &fName);
	/*
	 * Load a GLSL shader from some file, returns -1 if loading failed
	 */
	GLint load_shader(GLenum type, const std::string &file);
	/*
	 * Build a shader program from the list of shaders passed
	 */
	GLint load_program(const std::vector<std::tuple<GLenum, std::string>> &shaders);
	/*
	 * Load an image into an OpenGL texture. SDL is used to read the image into
	 * a surface which is then passed to OpenGL. A new texture id is created
	 * and returned if successful. The texture unit desired for this texture
	 * should be set active before loading the texture as it will be bound during
	 * the loading process
	 * Note: To lazy to setup a FindSDL2_Image for my windows machine so
	 * just BMP support for now
	 */
	GLuint load_texture(const std::string &file);
	/*
	 * Check for an OpenGL error and log it along with the message passed
	 * if an error occured. Will return true if an error occured & was logged
	 */
	bool log_glerror(const std::string &msg);
	/*
	 * A debug callback for the GL_ARB_debug_out extension
	 */
#ifdef _WIN32
	void APIENTRY gldebug_callback(GLenum src, GLenum type, GLuint id, GLenum severity,
		GLsizei len, const GLchar *msg, const GLvoid *user);
#else
	void gldebug_callback(GLenum src, GLenum type, GLuint id, GLenum severity,
		GLsizei len, const GLchar *msg, const GLvoid *user);
#endif
}

#endif
