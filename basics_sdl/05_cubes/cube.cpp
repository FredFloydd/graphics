#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <math.h>
using namespace std;

/* Use glew.h instead of gl.h to get all the GL prototypes declared */
#include <GL/glew.h>
/* Using SDL2 for the base window and OpenGL context init */
#include <SDL2/SDL.h>
/* GLM for matrix manipulation */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../common/shader_utils.h"

int screen_width=800, screen_height=600;

GLuint program;
GLuint vbo_cube, ibo_cube;
GLint attribute_coord3d, attribute_v_color;
GLint uniform_mvp;

struct attributes {
	GLfloat coord3d[3];
	GLfloat v_color[3];
};

bool init_resources() {	
	/* Cube vertices and colors */
	struct attributes cube_attributes[] = {
		{{-1.0, -1.0,  1.0}, {1.0, 0.0, 0.0}},
		{{ 1.0, -1.0,  1.0}, {0.0, 1.0, 0.0}},
		{{ 1.0,  1.0,  1.0}, {0.0, 0.0, 1.0}},
		{{-1.0,  1.0,  1.0}, {1.0, 1.0, 1.0}},
		
		{{-1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
		{{ 1.0, -1.0, -1.0}, {0.0, 1.0, 0.0}},
		{{ 1.0,  1.0, -1.0}, {0.0, 0.0, 1.0}},
		{{-1.0,  1.0, -1.0}, {1.0, 1.0, 1.0}},
		
	};
	glGenBuffers(1, &vbo_cube);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_attributes), cube_attributes, GL_STATIC_DRAW);
	/* Definition of cube elements */
	GLushort cube_elements[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};
	glGenBuffers(1, &ibo_cube);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);

	GLint link_ok = GL_FALSE;
	
	GLuint vs, fs;
	if ((vs = create_shader("cube.v.glsl", GL_VERTEX_SHADER))   == 0) return false;
	if ((fs = create_shader("cube.f.glsl", GL_FRAGMENT_SHADER)) == 0) return false;
	
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		cerr << "glLinkProgram:";
		print_log(program);
		return false;
	}
	
	const char* attribute_name = "coord3d";
	attribute_coord3d = glGetAttribLocation(program, attribute_name);
	if (attribute_coord3d == -1) {
		cerr << "Could not bind attribute " << attribute_name << endl;
		return false;
	}
	attribute_name = "v_color";
	attribute_v_color = glGetAttribLocation(program, attribute_name);
	if (attribute_v_color == -1) {
		cerr << "Could not bind attribute " << attribute_name << endl;
		return false;
	}
	const char* uniform_name;
	uniform_name = "mvp";
	uniform_mvp = glGetUniformLocation(program, uniform_name);
	if (uniform_mvp == -1) {
		cerr << "Could not bind uniform " << uniform_name << endl;
		return false;
	}
	return true;
}

/* Aspect ratio for calculating field of view */
float aspectaxis() {
	float outputzoom = 1.0f;
	float aspectorigin = 16.0f / 9.0f;
	int aspectconstraint = 1;
	switch (aspectconstraint) {
		case 1: 
			if (((float) screen_width / screen_height) < aspectorigin) {
				outputzoom *= (((float) screen_width / screen_height) / aspectorigin);
			}
			else {
				outputzoom *= ((float) aspectorigin / aspectorigin);
			}
		break;
		case 2:
			outputzoom *= (((float) screen_width / screen_height) / aspectorigin);
		break;
		default:
			outputzoom *= ((float) aspectorigin / aspectorigin);
	}
	return outputzoom;
}

float recalculatefov() {
	return 2.0f * glm::atan(glm::tan(glm::radians(45.0f / 2.0f)) / aspectaxis());
}

/* Resets screen width and height upon resizing */
void onResize(int width, int height) {
	screen_width = width;
	screen_height = height;
	glViewport(0, 0, screen_width, screen_height);
}

/* Calculate transforms and animate cube */
void logic() {
	float angle = SDL_GetTicks() / 1000.0 * 45;
	glm::vec3 axis_y(0, 1, 0);
	glm::mat4 anim = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis_y);

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -4.0));
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 2.0, 0.0), 
			      glm::vec3(0.0, 0.0, -4.0), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 projection = glm::perspective(recalculatefov(), 
					 1.0f * screen_width / screen_height, 0.1f, 10.0f);
	
	glm::mat4 mvp = projection * view * model * anim;	

	glUseProgram(program);
	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
}

void render(SDL_Window* window) {
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);

	glEnableVertexAttribArray(attribute_coord3d);
	glEnableVertexAttribArray(attribute_v_color);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube);
	glVertexAttribPointer(
		attribute_coord3d,         // attribute
		3,                         // number of elements per vertex, here (x,y)
		GL_FLOAT,                  // the type of each element
		GL_FALSE,                  // take our values as-is
		sizeof(struct attributes), // no extra data between each position
		0                          // offset of first element
	);
	glVertexAttribPointer(
		attribute_v_color,         
		3,		           
		GL_FLOAT,	           
		GL_FALSE,	           
		sizeof(struct attributes), 
		(GLvoid*) offsetof(struct attributes, v_color)
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube);
	int size; glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
	
	glDisableVertexAttribArray(attribute_coord3d);
	glDisableVertexAttribArray(attribute_v_color);

	SDL_GL_SwapWindow(window);
}

void free_resources() {
	glDeleteProgram(program);
	glDeleteBuffers(1, &vbo_cube);
}

void mainLoop(SDL_Window* window) {
	while (true) {
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_QUIT)
				return;
			if (ev.type == SDL_WINDOWEVENT && ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
				onResize(ev.window.data1, ev.window.data2);
			}
		}
		logic();
		render(window);
	}
}

int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("My First Cube",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		screen_width, screen_height,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	if (window == NULL) {
		cerr << "Error: can't create window: " << SDL_GetError() << endl;
		return EXIT_FAILURE;
	}
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1);
	if (SDL_GL_CreateContext(window) == NULL) {
		cerr << "Error: SDL_GL_CreateContext: " << SDL_GetError() << endl;
		return EXIT_FAILURE;
	}

	GLenum glew_status = glewInit();
	if (glew_status != GLEW_OK) {
		cerr << "Error: glewInit: " << glewGetErrorString(glew_status) << endl;
		return EXIT_FAILURE;
	}
	if (!GLEW_VERSION_2_0) {
		cerr << "Error: your graphic card does not support OpenGL 2.0" << endl;
		return EXIT_FAILURE;
	}

	if (!init_resources())
		return EXIT_FAILURE;

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	mainLoop(window);
	
	free_resources();
	return EXIT_SUCCESS;
}

