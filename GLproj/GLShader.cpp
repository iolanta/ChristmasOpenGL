#include "GLShader.h"
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

ShaderData::ShaderData(){}

ShaderData::ShaderData(GLuint sh, GLenum type) {
	ShaderData::shader = sh;         // идентификатор шейдера
	ShaderData::shader_type = type;  // тип шейдера
}

VariableData::VariableData() {}

VariableData::VariableData(GLint  l, std::string n, GLenum t, GLsizei sz) {
	location = l;
	VariableData::name = n;
	VariableData::type = t;
	VariableData::size = sz;
}

GLShader::GLShader()
{
	GLShader::ShaderProgram = 0;
	GLShader::shaders = std::map<int, ShaderData>();
	GLShader::attributes = std::map<std::string, VariableData>();
	shader_count = 0;
	ShaderProgram = glCreateProgram();
}


GLShader::~GLShader()
{
	glUseProgram(0);
	GLint act_cnt;
	glGetProgramiv(ShaderProgram, GL_ATTACHED_SHADERS, &act_cnt);
	GLuint * attached = new GLuint[act_cnt];
	glGetAttachedShaders(ShaderProgram, act_cnt, &act_cnt, attached);
	for (int i = 0; i < act_cnt; ++i) {
		glDetachShader(ShaderProgram, attached[i]);
		
	}
	delete attached;
	glDeleteProgram(ShaderProgram);

	for (int i = 0; i < shaders.size(); ++i) {
		glDeleteShader(shaders[i].shader);
	}
}

int GLShader::load_shader(std::string filename, GLenum shader_type) {
	GLuint s = compileSource(filename, shader_type);
	ShaderData sd = ShaderData(s, shader_type);
	shaders.insert(std::pair<int, ShaderData>(shader_count, sd));
	return shader_count++;
	
}

bool GLShader::setUniformfv4(std::string name, glm::vec4 v)
{
	auto vd = uniforms.find(name);
	if (vd == uniforms.end())
		return false; // no such uniform
	if (vd->second.type != GL_FLOAT_VEC4)
		return false; // type error
	glUniform4fv(vd->second.location, 1, glm::value_ptr(v));
	return true;
}

bool GLShader::setUniformfv3(std::string name, glm::vec3 v)
{
	auto vd = uniforms.find(name);
	if (vd == uniforms.end())
		return false; // no such uniform
	if (vd->second.type != GL_FLOAT_VEC3)
		return false; // type error
	glUniform3fv(vd->second.location, 1, glm::value_ptr(v));
	return true;
}

bool GLShader::setUniformfv2(std::string name, glm::vec2 v)
{
	auto vd = uniforms.find(name);
	if (vd == uniforms.end())
		return false; // no such uniform
	if (vd->second.type != GL_FLOAT_VEC2)
		return false; // type error
	glUniform2fv(vd->second.location, 1, glm::value_ptr(v));
	return true;
}

bool GLShader::setUniformmat4(std::string name, bool transpose, glm::mat4 mat4)
{
	auto vd = uniforms.find(name);
	if (vd == uniforms.end())
		return false; // no such uniform
	if (vd->second.type != GL_FLOAT_MAT4)
		return false; // type error
	glUniformMatrix4fv(vd->second.location, 1,transpose, glm::value_ptr(mat4));
	return true;
}

bool GLShader::setUniformmat3(std::string name, bool transpose, glm::mat3 mat3)
{
	auto vd = uniforms.find(name);
	if (vd == uniforms.end())
		return false; // no such uniform
	if (vd->second.type != GL_FLOAT_MAT3)
		return false; // type error
	glUniformMatrix3fv(vd->second.location, 1, transpose, glm::value_ptr(mat3));
	return true;
}


bool GLShader::setUniform1i(std::string name, const GLint value)
{
	auto vd = uniforms.find(name);
	if (vd == uniforms.end())
		return false; // no such uniform
	if (vd->second.type != GL_INT)
		return false; // type error
	glUniform1i(vd->second.location, value);
	return true;
}

bool GLShader::setUniform1s(std::string name, const GLint value)
{
	auto vd = uniforms.find(name);
	if (vd == uniforms.end())
		return false; // no such uniform
	if (vd->second.type != GL_SAMPLER_2D)
		return false; // type error
	glUniform1i(vd->second.location, value);
	return true;
}

bool GLShader::setUniform1f(std::string name, const GLfloat value)
{
	auto vd = uniforms.find(name);
	if (vd == uniforms.end())
		return false; // no such uniform
	if (vd->second.type != GL_FLOAT)
		return false; // type error
	glUniform1f(vd->second.location, value);
	return true;
}

bool GLShader::setUniform1b(std::string name, const bool value)
{
	auto vd = uniforms.find(name);
	if (vd == uniforms.end())
		return false; // no such uniform
	if (vd->second.type != GL_BOOL)
		return false; // type error
	glUniform1i(vd->second.location, value);
	return true;
}

GLint GLShader::getAttributeLocation(std::string name)
{
	auto vd = attributes.find(name);
	if (vd == attributes.end())
		return -1; // no such attribute
	return vd->second.location;
}

std::string GLShader::loadSourceFile(const std::string& source_file_name) {
	std::ifstream f(source_file_name);
	std::stringstream ss;
	ss << f.rdbuf(); 
	return ss.str();
}

GLuint GLShader::compileSource(std::string filename, GLenum shader_type) {
	GLuint shader = glCreateShader(shader_type);
	std::string str = loadSourceFile(filename);
	const char * c = str.c_str();
	int len = str.length();
	glShaderSource(shader,1,&c,&len);    // загружается исходный код на идентификатор шейдера
	glCompileShader(shader);
	printInfoLogShader(shader);
	return shader;
}

void GLShader::printInfoLogShader(GLuint shader) {
	int infologLen = 0;
	int charsWritten = 0;
	char *infoLog;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
	if (infologLen > 1)
	{
		infoLog = new char[infologLen];
		if (infoLog == NULL)
		{
			std::cout << "ERROR: Could not allocate InfoLog buffer\n";
			exit(1);
		}
		glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog);
		std::cout << "InfoLog: " << infoLog << "\n\n\n";
		checkOpenGLerror();
		delete[] infoLog;
	}
}

void GLShader::printInfoLogProgram() {
	int infologLen = 0;
	int charsWritten = 0;
	char *infoLog;
	glGetProgramiv(ShaderProgram, GL_INFO_LOG_LENGTH, &infologLen);
	if (infologLen > 1)
	{
		infoLog = new char[infologLen];
		if (infoLog == NULL)
		{
			std::cout << "ERROR: Could not allocate InfoLog buffer\n";
			exit(1);
		}
		glGetProgramInfoLog(ShaderProgram, infologLen, &charsWritten, infoLog);
		std::cout << "InfoLog: " << infoLog << "\n\n\n";
		checkOpenGLerror();
		delete[] infoLog;
	}
}

void GLShader::linkProgram(int vertex_id, int fragment_id) {
	GLint act_cnt;
	glGetProgramiv(ShaderProgram, GL_ATTACHED_SHADERS, &act_cnt);
	GLuint * attached = new GLuint[act_cnt];
	glGetAttachedShaders(ShaderProgram, act_cnt, &act_cnt, attached);
	for (int i = 0; i < act_cnt; ++i)
		glDetachShader(ShaderProgram, attached[i]);
	delete attached;

	glAttachShader(ShaderProgram, shaders[vertex_id].shader);   // шейдер прикрепляется к программе 
	checkOpenGLerror();
	printInfoLogProgram();
	glAttachShader(ShaderProgram, shaders[fragment_id].shader);
	checkOpenGLerror();
	printInfoLogProgram();
	glLinkProgram(ShaderProgram);     // объединяет шейдеры между собой (in/out переменные должны сооответствовать)
	
	int link_ok;
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &link_ok);
	if (!link_ok)
	{
		std::cout << "error attach shaders \n";
		checkOpenGLerror();
		printInfoLogProgram();
		return;
	}

	int count_attributes, max_length;
	attributes.clear();
	uniforms.clear();
	glGetProgramiv(ShaderProgram, GL_ACTIVE_ATTRIBUTES, &count_attributes);
	glGetProgramiv(ShaderProgram, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_length);   // макс длина имени атрибута
	char* name = new char[max_length]; 
	for (int i= 0; i < count_attributes; ++i) {
		int size;
		GLenum type;
		glGetActiveAttrib(ShaderProgram, i, max_length, NULL, &size, &type, name);    // возвращает информацию об активном атрибуте в шейдерной программе
		auto Loc = glGetAttribLocation(ShaderProgram, name);
		std::string sname = std::string(name);
		VariableData vb(Loc,sname, type, size);
		attributes.insert(std::pair<std::string, VariableData>(sname, vb));
	}
	delete name;


	glGetProgramiv(ShaderProgram, GL_ACTIVE_UNIFORMS, &count_attributes);
	glGetProgramiv(ShaderProgram, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_length);
	name = new char[max_length];
	for (int i = 0; i < count_attributes; ++i) {
		int size;
		GLenum type;
		glGetActiveUniform(ShaderProgram, i, max_length, NULL, &size, &type, name);
		auto Loc = glGetUniformLocation(ShaderProgram, name);
		std::string sname = std::string(name);
		VariableData vb(Loc,sname, type, size);
		uniforms.insert(std::pair<std::string, VariableData>(sname, vb));
	}
	delete name;


}

void GLShader::checkOpenGLerror()
{
	GLenum errCode;
	if ((errCode = glGetError()) != GL_NO_ERROR)
		std::cout << "OpenGl error! - " << gluErrorString(errCode);
}