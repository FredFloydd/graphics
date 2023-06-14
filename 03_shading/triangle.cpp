#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <math.h>
using namespace std;

/* Use glew.h instead of gl.h to get all the GL prototypes declared */
#include <GL/glew.h>
/* Using SDL2 for the base window and OpenGL context init */
#include <SDL2/SDL.h>

#include "../common/shader_utils.h"

GLuint program;
GLuint vbo_triangle;
GLint attribute_coord2d, attribute_v_color;
GLint uniform_fade;

struct attributes {
	GLfloat coord2d[2];
	GLfloat v_color[3];
};

bool init_resources() {	
	struct attributes triangle_attributes[] = {
		{{ 0.0,  0.8}, {1.0, 1.0, 0.0}},
		{{-0.8, -0.8}, {0.0, 0.0, 1.0}},
		{{ 0.8, -0.8}, {1.0, 0.0, 0.0}},
	};
	glGenBuffers(1, &vbo_triangle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_attributes), triangle_attributes, GL_STATIC_DRAW);

	GLint link_ok = GL_FALSE;
	
	GLuint vs, fs;
	if ((vs = create_shader("triangle.v.glsl", GL_VERTEX_SHADER))   == 0) return false;
	if ((fs = create_shader("triangle.f.glsl", GL_FRAGMENT_SHADER)) == 0) return false;
	
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
	
	const char* attribute_name = "coord2d";
	attribute_coord2d = glGetAttribLocation(program, attribute_name);
	if (attribute_coord2d == -1) {
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
	uniform_name = "fade";
	uniform_fade = glGetUniformLocation(program, uniform_name);
	if (uniform_fade == -1) {
		cerr << "Could not bind uniform " << uniform_name << endl;
		return false;
	}
	return true;
}

void logic() {
	/* Cycle transparency every 5 seconds */
	float cur_fade = sinf(SDL_GetTicks() / 1000.0 * 6.28 / 5) / 2 + 0.5;
	glUseProgram(program);
	glUniform1f(uniform_fade, cur_fade);
}

void render(SDL_Window* window) {
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program);

	glEnableVertexAttribArray(attribute_coord2d);
	glEnableVertexAttribArray(attribute_v_color);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);
	glVertexAttribPointer(
		attribute_coord2d,         // attribute
		2,                         // number of elements per vertex, here (x,y)
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

	/* Push each element in buffer_vertices to the vertex shader */
	glDrawArrays(GL_TRIANGLES, 0, 3);
	
	glDisableVertexAttribArray(attribute_coord2d);
	glDisableVertexAttribArray(attribute_v_color);
	
	SDL_GL_SwapWindow(window);
}

void free_resources() {
	glDeleteProgram(program);
	glDeleteBuffers(1, &vbo_triangle);
}

void mainLoop(SDL_Window* window) {
	while (true) {
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_QUIT)
				return;
		}
		logic();
		render(window);
	}
}

int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("My Second Triangle",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		640, 480,
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
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	mainLoop(window);
	
	free_resources();
	return EXIT_SUCCESS;
}

