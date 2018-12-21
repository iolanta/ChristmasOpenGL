#pragma once
#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/glut.h>
#include <SOIL2.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <glm/mat4x4.hpp>

class ShaderData {
public:
	GLuint shader;
	GLenum shader_type;

	ShaderData();
	ShaderData(GLuint sh, GLenum type);

};

class VariableData {
public:
	std::string name;
	GLint location;
	GLenum type;
	GLsizei size;

	VariableData();
	VariableData(GLint  l,std::string n, GLenum t, GLsizei sz);
};

class GLShader {
public:
	GLShader();
	~GLShader();

	int load_shader(std::string filename, GLenum shader_type);

	bool setUniformfv4(std::string name, glm::vec4 v);
	bool setUniformfv3(std::string name,  glm::vec3 v);
	bool setUniformfv2(std::string name, glm::vec2 v);
	bool setUniformmat4(std::string name, bool transpose, glm::mat4 mat4);
	bool setUniformmat3(std::string name, bool transpose, glm::mat3 mat3);
	bool setUniform1i(std::string name,  const GLint value);
	bool setUniform1s(std::string name, const GLint value);
	bool setUniform1f(std::string name, const GLfloat value);
	bool setUniform1b(std::string name, const bool value);


	GLint getAttributeLocation(std::string name);

	void linkProgram(int vertex_id, int fragment_id);
	GLuint ShaderProgram;

	void checkOpenGLerror();
private:
	int shader_count;

	std::map<int, ShaderData> shaders;
	std::map<std::string, VariableData> attributes;
	std::map<std::string, VariableData> uniforms;

	std::string loadSourceFile(const std::string& source_file_name);

	GLuint compileSource(std::string filename, GLenum shader_type);

	void printInfoLogShader(GLuint shader);

	
	void printInfoLogProgram();
	



};

