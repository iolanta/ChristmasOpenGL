#pragma once
#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/glut.h>
#include <SOIL2.h>
#include <string>
#include "GLShader.h"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

class GLobject
{
	GLushort * indeces;

	GLobject();

public:
	GLuint VBO, VAO, IBO, texture;
	GLsizei stride, CoordOffset, NormalOffset, TexCoordOffset, ColorOffset;
	glm::mat4 object_transformation;
	
	glm::vec4 material_ambient, material_diffuse, material_specular, material_emission;
	GLfloat material_shininess;
	bool use_texture;


	int count_vertex;
	int count_indexes;
	GLobject(std::string path, std::string pathtex, glm::vec3 clr = { 1,1,1 }, glm::vec3 norm = { 0,0,0 });
	
	static GLobject* draw_ground(GLdouble x0, GLdouble x1, GLdouble y0, GLdouble y1, int divx, int divy);

	bool BindAttributesToShader(GLShader& shaderobject);

	void drawObject();

	~GLobject();

};

