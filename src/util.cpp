#include <vector>
#include <array>
#include <map>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <tuple>
#include <glm/glm.hpp>
#include <SDL.h>
#include "gl_core_3_3.h"
#include "util.h"

std::string util::get_resource_path(const std::string &sub_dir){
#ifdef _WIN32
	const char PATH_SEP = '\\';
#else
	const char PATH_SEP = '/';
#endif
	static std::string base_res;
	if (base_res.empty()){
		char *base_path = SDL_GetBasePath();
		if (base_path){
			base_res = base_path;
			SDL_free(base_path);
		}
		else {
			std::cerr << "Error getting resource path: " << SDL_GetError() << std::endl;
			return "";
		}
		//The final part of the string should be bin/ so replace it with res/
		size_t pos = base_res.find_last_of("bin") - 2;
		base_res = base_res.substr(0, pos) + "res" + PATH_SEP;
	}
	if (sub_dir.empty()){
		return base_res;
	}
	return base_res + sub_dir + PATH_SEP;
}
std::string util::read_file(const std::string &fName){
	std::ifstream file(fName);
	if (!file.is_open()){
		std::cout << "Failed to open file: " << fName << std::endl;
		return "";
	}
	return std::string((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
}
GLint util::load_shader(GLenum type, const std::string &file){
	GLuint shader = glCreateShader(type);
	std::string src = read_file(file);
	const char *csrc = src.c_str();
	glShaderSource(shader, 1, &csrc, 0);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE){
		std::cerr << "load_shader: ";
		switch (type){
		case GL_VERTEX_SHADER:
			std::cerr << "Vertex shader: ";
			break;
		case GL_FRAGMENT_SHADER:
			std::cerr << "Fragment shader: ";
			break;
		case GL_GEOMETRY_SHADER:
			std::cerr << "Geometry shader: ";
			break;
		default:
			std::cerr << "Other shader type: ";
		}
		std::cerr << file << " failed to compile. Compilation log:\n";
		GLint len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
		char *log = new char[len];
		glGetShaderInfoLog(shader, len, 0, log);
		std::cerr << log << "\n";
		delete[] log;
		glDeleteShader(shader);
		return -1;
	}
	return shader;
}
GLint util::load_program(const std::vector<std::tuple<GLenum, std::string>> &shaders){
	std::vector<GLuint> glshaders;
	for (const std::tuple<GLenum, std::string> &s : shaders){
		GLint h = load_shader(std::get<0>(s), std::get<1>(s));
		if (h == -1){
			std::cerr << "load_program: A required shader failed to compile, aborting\n";
			for (GLuint g : glshaders){
				glDeleteShader(g);
			}
			return -1;
		}
		glshaders.push_back(h);
	}
	GLuint program = glCreateProgram();
	for (GLuint s : glshaders){
		glAttachShader(program, s);
	}
	glLinkProgram(program);
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE){
		std::cerr << "load_program: Program failed to link, log:\n";
		GLint len;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
		char *log = new char[len];
		glGetProgramInfoLog(program, len, 0, log);
		std::cerr << log << "\n";
		delete[] log;
	}
	for (GLuint s : glshaders){
		glDetachShader(program, s);
		glDeleteShader(s);
	}
	if (status == GL_FALSE){
		glDeleteProgram(program);
		return -1;
	}
	return program;
}
GLuint util::load_texture(const std::string &file){
	SDL_Surface *surf = SDL_LoadBMP(file.c_str());
	//TODO: Throw an error?
	if (!surf){
		std::cout << "Failed to load bmp: " << file
			<< " SDL_error: " << SDL_GetError() << "\n";
		return 0;
	}
	//Assume 4 or 3 bytes per pixel
	GLenum format, internal;
	if (surf->format->BytesPerPixel == 4){
		internal = GL_RGBA;
		if (surf->format->Rmask == 0x000000ff){
			format = GL_RGBA;
		}
		else {
			format = GL_BGRA;
		}
	}
	else {
		internal = GL_RGB;
		if (surf->format->Rmask == 0x000000ff){
			format = GL_RGB;
		}
		else {
			format = GL_BGR;
		}
	}
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexImage2D(GL_TEXTURE_2D, 0, internal, surf->w, surf->h, 0, format,
		GL_UNSIGNED_BYTE, surf->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	SDL_FreeSurface(surf);
	return tex;
}
bool util::log_glerror(const std::string &msg){
	GLenum err = glGetError();
	if (err != GL_NO_ERROR){
		std::cerr << "OpenGL Error: ";
		switch (err){
		case GL_INVALID_ENUM:
			std::cerr << "Invalid enum";
			break;
		case GL_INVALID_VALUE:
			std::cerr << "Invalid value";
			break;
		case GL_INVALID_OPERATION:
			std::cerr << "Invalid operation";
			break;
		case GL_OUT_OF_MEMORY:
			std::cerr << "Out of memory";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			std::cerr << "Invalid FrameBuffer operation";
			break;
		default:
			std::cerr << std::hex << err << std::dec;
		}
		std::cerr << " - " << msg << "\n";
		return true;
	}
	return false;
}
#if _MSC_VER
void APIENTRY util::gldebug_callback(GLenum src, GLenum type, GLuint id, GLenum severity,
	GLsizei len, const GLchar *msg, const GLvoid *user)
#else
void util::gldebug_callback(GLenum src, GLenum type, GLuint id, GLenum severity,
	GLsizei len, const GLchar *msg, const GLvoid *user)
#endif
{
	//Print a time stamp for the message
	float sec = SDL_GetTicks() / 1000.f;
	int min = static_cast<int>(sec / 60.f);
	sec -= sec / 60.f;
	std::cerr << "[" << min << ":"
		<< std::setprecision(3) << sec << "] OpenGL Debug -";
	switch (severity){
	case GL_DEBUG_SEVERITY_HIGH_ARB:
		std::cerr << " High severity";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM_ARB:
		std::cerr << " Medium severity";
		break;
	case GL_DEBUG_SEVERITY_LOW_ARB:
		std::cerr << " Low severity";
	}
	switch (src){
	case GL_DEBUG_SOURCE_API_ARB:
		std::cerr << " API";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
		std::cerr << " Window system";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
		std::cerr << " Shader compiler";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
		std::cerr << " Third party";
		break;
	case GL_DEBUG_SOURCE_APPLICATION_ARB:
		std::cerr << " Application";
		break;
	default:
		std::cerr << " Other";
	}
	switch (type){
	case GL_DEBUG_TYPE_ERROR_ARB:
		std::cerr << " Error";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
		std::cerr << " Deprecated behavior";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
		std::cerr << " Undefined behavior";
		break;
	case GL_DEBUG_TYPE_PORTABILITY_ARB:
		std::cerr << " Portability";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE_ARB:
		std::cerr << " Performance";
		break;
	default:
		std::cerr << " Other";
	}
	std::cerr << ":\n\t" << msg << "\n";
}

