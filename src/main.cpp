#include <iostream>
#include <string>
#include <tuple>
#include <functional>
#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <lfwatch.h>
#include "gl_core_3_3.h"
#include "util.h"
#include "camera.h"

const int WIN_WIDTH = 1280;
const int WIN_HEIGHT = 720;

void run(SDL_Window *win);
//Handle input events to the camera, returns true if the camera was moved
bool move_camera(Camera &camera, const SDL_Event &e);

int main(int argc, char **argv){
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		std::cerr << "SDL_Init error: " << SDL_GetError() << "\n";
		return 1;
	}
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	SDL_GL_SetSwapInterval(1);
	SDL_SetRelativeMouseMode(SDL_TRUE);

	SDL_Window *win = SDL_CreateWindow("Fast Billboards", SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(win);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED){
		std::cerr << "ogl load failed\n";
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return 1;
	}
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClearDepth(1.f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << "\n"
		<< "OpenGL Vendor: " << glGetString(GL_VENDOR) << "\n"
		<< "OpenGL Renderer: " << glGetString(GL_RENDERER) << "\n"
		<< "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";

	std::cout << "CONTROLS:\n"
		<< "\tw/s - forward/back\n" << "\ta/d - strafe left/right\n"
		<< "\tq/e - strafe up/down\n" << "\tr/f - roll clockwise/counterclockwise\n"
		<< "\tclick + drag - move camera look direction\n";

	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
	glDebugMessageCallbackARB(util::gldebug_callback, NULL);
	glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
		NULL, GL_TRUE);

	run(win);

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}
void run(SDL_Window *win){
	std::string res_path = util::get_resource_path();
	GLint shader = util::load_program({std::make_tuple(GL_VERTEX_SHADER, res_path + "vertex.glsl"),
		std::make_tuple(GL_FRAGMENT_SHADER, res_path + "fragment.glsl")});
	assert(shader != -1);
	glUseProgram(shader);

	Camera camera{glm::vec3{0, 0, 5}, glm::vec3{0, 0, 0}, glm::vec3{0, 1, 0}};

	//Setup our viewing matrix buffer to pass viewing information to the shaders
	//as a uniform block
	GLuint viewing_buf;
	glGenBuffers(1, &viewing_buf);
	glBindBuffer(GL_UNIFORM_BUFFER, viewing_buf);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4) + 1 * sizeof(glm::vec4), NULL, GL_STREAM_DRAW);
	{
		char *buf = static_cast<char*>(glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY));
		glm::mat4 *m = reinterpret_cast<glm::mat4*>(buf);
		m[0] = camera.view_mat();
		m[1] = glm::perspective<GLfloat>(util::deg_to_rad(75.f),
			static_cast<float>(WIN_WIDTH) / WIN_HEIGHT, 1, 100);
		glm::vec4 *v = reinterpret_cast<glm::vec4*>(buf + 2 * sizeof(glm::mat4));
		v[0] = glm::vec4{camera.eye_pos(), 0};
		glUnmapBuffer(GL_UNIFORM_BUFFER);
	}
	GLuint viewing_block = glGetUniformBlockIndex(shader, "Viewing");
	glUniformBlockBinding(shader, viewing_block, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, viewing_buf);

	//Setup our uniform color data for the sprites (here you'd instead pass uv data or whatever)
	//This will be indexed by the sprite id instance attribute
	GLuint color_buf;
	glGenBuffers(1, &color_buf);
	glBindBuffer(GL_UNIFORM_BUFFER, color_buf);
	glBufferData(GL_UNIFORM_BUFFER, 16 * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);
	{
		glm::vec4 *color = static_cast<glm::vec4*>(glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY));
		color[0] = glm::vec4{1, 0, 0, 1};
		color[1] = glm::vec4{0, 1, 0, 1};
		color[2] = glm::vec4{0, 0, 1, 1};
		color[3] = glm::vec4{1, 1, 1, 1};

		color[4] = glm::vec4{1, 1, 0, 1};
		color[5] = glm::vec4{1, 0, 1, 1};
		color[6] = glm::vec4{0, 1, 1, 1};
		color[7] = glm::vec4{0.5, 0.5, 1, 1};

		color[8] = glm::vec4{0.5, 0, 0, 1};
		color[9] = glm::vec4{0, 0.5, 0, 1};
		color[10] = glm::vec4{0, 0, 0.5, 1};
		color[11] = glm::vec4{0.5, 1, 0.5, 1};

		color[12] = glm::vec4{0.5, 0.5, 0, 1};
		color[13] = glm::vec4{0.5, 0, 0.5, 1};
		color[14] = glm::vec4{0, 0.5, 0.5, 1};
		color[15] = glm::vec4{1, 0.5, 0.5, 1};
		glUnmapBuffer(GL_UNIFORM_BUFFER);
	}
	GLuint color_block = glGetUniformBlockIndex(shader, "Colors");
	glUniformBlockBinding(shader, color_block, 1);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, color_buf);

	/*
	 * IMPORTANT NOTE: I use two buffers here for simplicity but you'd really want to compact all
	 * the instance data (positions, ids, etc) into a single buffer to be more efficient
	 */
	
	int n_billboards = 4;
	//Setup the buffer containing our billboard positions
	GLuint pos_buf;
	glGenBuffers(1, &pos_buf);
	glBindBuffer(GL_ARRAY_BUFFER, pos_buf);
	glBufferData(GL_ARRAY_BUFFER, n_billboards * sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
	{
		glm::vec3 *pos = static_cast<glm::vec3*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
		pos[0] = glm::vec3{-2, -2, 0};
		pos[1] = glm::vec3{2, -2, 0};
		pos[2] = glm::vec3{-2, 2, 0};
		pos[3] = glm::vec3{2, 2, 0};
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	/*
	 * Setup other per-instance data, eg. sprite ids to be used to select the
	 * proper uv coordinates (and optionally texture array index) to draw
	 * the appropriate sprite texture
	 * In this demo it's used to look up colors for the vertices
	 */
	GLuint extra_buf;
	glGenBuffers(1, &extra_buf);
	glBindBuffer(GL_ARRAY_BUFFER, extra_buf);
	glBufferData(GL_ARRAY_BUFFER, n_billboards * sizeof(GLint), NULL, GL_STATIC_DRAW);
	{
		int *indices = static_cast<int*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 3;
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	//Setup our vao bindings for the billboards
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, pos_buf);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribDivisor(0, 1);

	glBindBuffer(GL_ARRAY_BUFFER, extra_buf);
	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(1, 1, GL_INT, 0, 0);
	glVertexAttribDivisor(1, 1);

	//Monitor the shaders for changes and reload them if they're updated
	//This isn't required for the billboard rendering but does make it
	//easier to work on the shaders since you get hot reloading
	lfw::Watcher file_watcher;
	file_watcher.watch(res_path, lfw::Notify::FILE_MODIFIED,
		[&shader, res_path](const lfw::EventData &e){
			if (e.fname == "vertex.glsl" || e.fname == "fragment.glsl"){
				GLint new_shader = util::load_program({std::make_tuple(GL_VERTEX_SHADER, res_path + "vertex.glsl"),
					std::make_tuple(GL_FRAGMENT_SHADER, res_path + "fragment.glsl")});
				if (new_shader == -1){
					std::cerr << "Error compiling reloaded shader, aborting...\n";
				}
				else {
					glDeleteProgram(shader);
					shader = new_shader;
					glUseProgram(shader);
					//Re-hook up the uniform bindings
					GLuint viewing_block = glGetUniformBlockIndex(shader, "Viewing");
					glUniformBlockBinding(shader, viewing_block, 0);
					GLuint color_block = glGetUniformBlockIndex(shader, "Colors");
					glUniformBlockBinding(shader, color_block, 1);
				}
			}
		});

	bool update_view = false;
	bool quit = false;
	while (!quit){
		SDL_Event e;
		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)){
				quit = true;
			}
			else if (e.type == SDL_KEYDOWN
				|| (e.type == SDL_MOUSEMOTION && (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))))
			{
				update_view = move_camera(camera, e);
			}
		}
		if (update_view){
			update_view = false;
			glBindBuffer(GL_UNIFORM_BUFFER, viewing_buf);
			char *buf = static_cast<char*>(glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY));
			glm::mat4 *m = reinterpret_cast<glm::mat4*>(buf);
			m[0] = camera.view_mat();
			glm::vec4 *v = reinterpret_cast<glm::vec4*>(buf + 2 * sizeof(glm::mat4));
			v[0] = glm::vec4{camera.eye_pos(), 0};
			glUnmapBuffer(GL_UNIFORM_BUFFER);
		}
		file_watcher.update();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Draw our four billboard instances
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, n_billboards);

		SDL_GL_SwapWindow(win);
		GLenum err = glGetError();
		if (err != GL_NO_ERROR){
			std::cerr << "OpenGL Error: " << std::hex << err << std::dec << "\n";
		}
	}
	glDeleteProgram(shader);
	glDeleteBuffers(1, &viewing_buf);
	glDeleteBuffers(1, &color_buf);
	glDeleteBuffers(1, &pos_buf);
	glDeleteBuffers(1, &extra_buf);
	glDeleteVertexArrays(1, &vao);
}
bool move_camera(Camera &camera, const SDL_Event &e){
	if (e.type == SDL_KEYDOWN){
		switch (e.key.keysym.sym){
			case SDLK_d:
				camera.strafe_horiz(0.3);
				return true;
			case SDLK_a:
				camera.strafe_horiz(-0.3);
				return true;
			case SDLK_w:
				camera.zoom(0.3);
				return true;
			case SDLK_s:
				camera.zoom(-0.3);
				return true;
			case SDLK_q:
				camera.strafe_vert(0.3);
				return true;
			case SDLK_e:
				camera.strafe_vert(-0.3);
				return true;
			case SDLK_r:
				camera.roll(5);
				return true;
			case SDLK_f:
				camera.roll(-5);
				return true;
			default:
				break;
		}
	}
	else if (e.type == SDL_MOUSEMOTION){
		camera.pitch(-e.motion.yrel / 10.f);
		camera.yaw(-e.motion.xrel / 10.f);
		return true;
	}
	return false;
}

