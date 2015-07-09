#include "Nooskewl_Engine/error.h"
#include "Nooskewl_Engine/log.h"
#include "Nooskewl_Engine/vertex_accel.h"
#include "Nooskewl_Engine/video.h"

// FIXME: private
SDL_Window *window;
SDL_GLContext context;
GLuint vertexShader;
GLuint fragmentShader;
GLuint current_shader;

int screen_w;
int screen_h;

void flip()
{
	SDL_GL_SwapWindow(window);
}

void init_video()
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	screen_w = 285;
	screen_h = 160;

	window = SDL_CreateWindow("SS", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_w * 4, screen_h * 4, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (window == NULL) {
		throw Error("SDL_CreateWindow failed");
	}

	context = SDL_GL_CreateContext(window);

	SDL_GL_SetSwapInterval(1); // vsync

	glewExperimental = 1;
	glewInit();

	const char *vertexSource =
		"#version 150 core\n"
		"in vec3 position;"
		"in vec4 color;"
		"in vec2 texcoord;"
		"out vec4 Colour;"
		"out vec2 Texcoord;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 proj;"
		"void main() {"
		"	Colour = color;"
		"	Texcoord = texcoord;"
		"	gl_Position = proj * view * model * vec4(position, 1.0);"
		"}";
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	GLint status;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		char buffer[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
		errormsg("Vertex shader error: %s\n", buffer);
	}

	const char *fragmentSource =
		"#version 150 core\n"
		"in vec4 Colour;"
		"in vec2 Texcoord;"
		"out vec4 outColour;"
		"uniform sampler2D tex;"
		"uniform bool use_tex;"
		"void main()"
		"{"
		"	if (use_tex)"
		"		outColour = texture(tex, Texcoord) * Colour;"
		"	else"
		"		outColour = Colour;"
		"}";
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		char buffer[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
		errormsg("Fragment shader error: %s\n", buffer);
	}

	current_shader = glCreateProgram();
	glAttachShader(current_shader, vertexShader);
	glAttachShader(current_shader, fragmentShader);
	glBindFragDataLocation(current_shader, 0, "outColour");
	glLinkProgram(current_shader);
	glUseProgram(current_shader);

	set_default_projection();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	vertex_accel = new Vertex_Accel();
	vertex_accel->init();
}

void shutdown_video()
{
	delete vertex_accel;

	glDeleteProgram(current_shader);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void set_default_projection()
{
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	glm::mat4 proj = glm::ortho(0.0f, (float)w, (float)h, 0.0f);
	GLint uniTrans = glGetUniformLocation(current_shader, "proj");
	glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(proj));

	glm::mat4 view = glm::scale(glm::mat4(), glm::vec3(4.0f, 4.0f, 4.0f));
	uniTrans = glGetUniformLocation(current_shader, "view");
	glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(view));

	uniTrans = glGetUniformLocation(current_shader, "model");
	glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(glm::mat4()));
}

void set_map_transition_projection(float angle)
{
	GLint uni;

	uni = glGetUniformLocation(current_shader, "model");
    glm::mat4 model = glm::rotate(glm::mat4(), angle, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(angle >= PI/2 ? -4.0f : 4.0f, 4.0f, 4.0f));
    glUniformMatrix4fv(uni, 1, GL_FALSE, glm::value_ptr(model));

	uni = glGetUniformLocation(current_shader, "view");
	glm::mat4 view = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -2.0f));
    glUniformMatrix4fv(uni, 1, GL_FALSE, glm::value_ptr(view));

	uni = glGetUniformLocation(current_shader, "proj");
	glm::mat4 proj = glm::frustum(1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(uni, 1, GL_FALSE, glm::value_ptr(proj));
}