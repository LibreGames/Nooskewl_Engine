#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/shader.h"

using namespace Nooskewl_Engine;

Shader::Shader(bool opengl, std::string vertex_source, std::string fragment_source) :
	opengl(opengl),
	vertex_source(vertex_source),
	fragment_source(fragment_source)
{
	if (opengl) {
		GLint status;

		char *p = (char *)vertex_source.c_str();

		opengl_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		printGLerror("glCreateShader");
		glShaderSource(opengl_vertex_shader, 1, &p, 0);
		printGLerror("glShaderSource");
		glCompileShader(opengl_vertex_shader);
		printGLerror("glCompileShader");
		glGetShaderiv(opengl_vertex_shader, GL_COMPILE_STATUS, &status);
		printGLerror("glGetShaderiv");
		if (status != GL_TRUE) {
			char buffer[512];
			glGetShaderInfoLog(opengl_vertex_shader, 512, 0, buffer);
			errormsg("Vertex shader error: %s\n", buffer);
		}

		p = (char *)fragment_source.c_str();

		opengl_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		printGLerror("glCreateShader");
		glShaderSource(opengl_fragment_shader, 1, &p, 0);
		printGLerror("glShaderSource");
		glCompileShader(opengl_fragment_shader);
		printGLerror("glCompileShader");
		glGetShaderiv(opengl_fragment_shader, GL_COMPILE_STATUS, &status);
		printGLerror("glGetShaderiv");
		if (status != GL_TRUE) {
			char buffer[512];
			glGetShaderInfoLog(opengl_fragment_shader, 512, 0, buffer);
			errormsg("Fragment shader error: %s\n", buffer);
		}

		opengl_shader = glCreateProgram();
		glAttachShader(opengl_shader, opengl_vertex_shader);
		printGLerror("glAttachShader");
		glAttachShader(opengl_shader, opengl_fragment_shader);
		printGLerror("glAttachShader");
		glLinkProgram(opengl_shader);
		printGLerror("glLinkProgram");
	}
#ifdef NOOSKEWL_ENGINE_WINDOWS
	else {
		LPD3DXBUFFER errors;

		std::string shader_source = vertex_source + fragment_source;

		shader_source +=
			"technique TECH"
			"{"
			"		pass p1"
			"		{"
			"				VertexShader = compile vs_2_0 vs_main();"
			"				PixelShader = compile ps_2_0 ps_main();"
			"		}"
			"}";

		DWORD result = D3DXCreateEffect(noo.d3d_device, shader_source.c_str(), shader_source.length(), 0, 0, D3DXSHADER_PACKMATRIX_ROWMAJOR, 0, &d3d_effect, &errors);

		if (result != D3D_OK) {
			char *msg = (char *)errors->GetBufferPointer();
			throw Error("Shader error: " + std::string(msg));
		}

		d3d_technique = d3d_effect->GetTechniqueByName("TECH");
		d3d_effect->ValidateTechnique(d3d_technique);
		d3d_effect->SetTechnique(d3d_technique);
	}
#endif
}

Shader::~Shader()
{
	if (opengl) {
		glDeleteShader(opengl_vertex_shader);
		printGLerror("glDeleteShader");
		glDeleteShader(opengl_fragment_shader);
		printGLerror("glDeleteShader");
		glDeleteProgram(opengl_shader);
		printGLerror("glDeleteProgram");
	}
#ifdef NOOSKEWL_ENGINE_WINDOWS
	else {
		d3d_effect->Release();
	}
#endif
}

void Shader::use()
{
	if (opengl) {
		glUseProgram(opengl_shader);
		printGLerror("glUseProgram");
	}

	noo.update_projection();
}

void Shader::set_texture(std::string name, Image *image)
{
	if (opengl) {
		if (image) {
			glBindTexture(GL_TEXTURE_2D, image->internal->texture);
			printGLerror("glBindTexture");
			glBindVertexArray(image->internal->vao);
			printGLerror("glBindVertexArray");
			glBindBuffer(GL_ARRAY_BUFFER, image->internal->vbo);
			printGLerror("glBindBuffer");
		}
	}
#ifdef NOOSKEWL_ENGINE_WINDOWS
	else {
		if (image != NULL) {
			d3d_effect->SetTexture("tex", image->internal->video_texture);
			noo.d3d_device->SetTexture(0, image->internal->video_texture);
		}
		else {
			noo.d3d_device->SetTexture(0, NULL);	
		}
	}
#endif
}

void Shader::set_matrix(std::string name, const float *matrix)
{
	if (opengl) {
		GLint uni = glGetUniformLocation(opengl_shader, name.c_str());
		printGLerror("glGetUniformLocation");
		glUniformMatrix4fv(uni, 1, GL_FALSE, matrix);
		printGLerror("glUniformMatrix4fv");
	}
	else {
		d3d_effect->SetMatrix(name.c_str(), (D3DXMATRIX *)matrix);
	}
}

void Shader::set_float(std::string name, float value)
{
	if (opengl) {
		GLint uni = glGetUniformLocation(opengl_shader, name.c_str());
		printGLerror("glGetUniformLocation");
		glUniform1f(uni, value);
		printGLerror("glUniform1f");
	}
#ifdef NOOSKEWL_ENGINE_WINDOWS
	else {
		d3d_effect->SetFloat(name.c_str(), value);
	}
#endif
}

void Shader::set_bool(std::string name, bool value)
{
	if (opengl) {
		GLint uni = glGetUniformLocation(opengl_shader, name.c_str());
		printGLerror("glGetUniformLocation");
		glUniform1i(uni, value);
		printGLerror("glUniform1i");
	}
	else {
		d3d_effect->SetBool(name.c_str(), value);
	}
}

GLuint Shader::get_opengl_shader()
{
	return opengl_shader;
}

#ifdef NOOSKEWL_ENGINE_WINDOWS
LPD3DXEFFECT Shader::get_d3d_effect()
{
	return d3d_effect;
}
#endif