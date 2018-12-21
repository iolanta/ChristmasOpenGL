
#include "OBJ_Loader.h"
#include "GLobject.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

GLobject::GLobject(){}

GLobject::GLobject(std::string path, std::string pathtex, glm::vec3 clr, glm::vec3 norm)
{
	object_transformation = glm::mat4(1.0);
	bool force_normal = norm != glm::vec3{ 0, 0, 0 };
	objl::Loader modelloader;
	modelloader.LoadFile(path);
	count_indexes = modelloader.LoadedIndices.size();
	count_vertex = modelloader.LoadedVertices.size();
	if (pathtex != "")
		texture = SOIL_load_OGL_texture(pathtex.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_MULTIPLY_ALPHA | SOIL_FLAG_INVERT_Y);
	else
		texture = -1;
	use_texture = texture != -1;

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);
	glGenVertexArrays(1, &VAO);
	
	//		   XYZW					Normal XYZ		 TexCoords	UV					Color RGB		
	auto len = sizeof(GLfloat) * 4 + sizeof(GLfloat) * 3 + sizeof(GLfloat) * 2 + sizeof(GLfloat) * 3;
	CoordOffset = 0;
	NormalOffset = sizeof(GLfloat)*4;
	TexCoordOffset = NormalOffset + sizeof(GLfloat) * 3;
	ColorOffset = TexCoordOffset + sizeof(GLfloat) * 2;
	stride = len;

	glBindVertexArray(VAO);

	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	glBufferData(GL_ARRAY_BUFFER, len*modelloader.LoadedVertices.size(), NULL, GL_STATIC_DRAW);
	GLfloat * mapped_data = (GLfloat *)glMapBufferRange(GL_ARRAY_BUFFER, 0, len*modelloader.LoadedVertices.size(), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	int ind = 0;
	for (size_t i = 0; i < count_vertex; ++i)
	{
		mapped_data[ind++] = modelloader.LoadedVertices[i].Position.X;
		mapped_data[ind++] = modelloader.LoadedVertices[i].Position.Y;
		mapped_data[ind++] = modelloader.LoadedVertices[i].Position.Z;
		mapped_data[ind++] = 1;
		if (!force_normal) {
			mapped_data[ind++] = modelloader.LoadedVertices[i].Normal.X;
			mapped_data[ind++] = modelloader.LoadedVertices[i].Normal.Y;
			mapped_data[ind++] = modelloader.LoadedVertices[i].Normal.Z;
		}
		else {
			mapped_data[ind++] = norm.x;
			mapped_data[ind++] = norm.y;
			mapped_data[ind++] = norm.z;
		}
		mapped_data[ind++] = modelloader.LoadedVertices[i].TextureCoordinate.X;
		mapped_data[ind++] = modelloader.LoadedVertices[i].TextureCoordinate.Y;
		mapped_data[ind++] = clr.r;
		mapped_data[ind++] = clr.g;
		mapped_data[ind++] = clr.b;
		
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
	

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	
	indeces = new GLushort[count_indexes];
	for (size_t i = 0; i < count_indexes; i++)
	{
		indeces[i] = modelloader.LoadedIndices[i];
	}
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count_indexes * sizeof(GLushort), indeces, GL_STATIC_DRAW);
	delete indeces;
	glBindVertexArray(0);

	auto mat =  modelloader.LoadedMeshes[0].MeshMaterial;
	material_ambient = {mat.Ka.X,mat.Ka.Y, mat.Ka.Z,1};
	material_diffuse = { mat.Kd.X,mat.Kd.Y, mat.Kd.Z,1 };
	material_specular = { mat.Ks.X,mat.Ks.Y, mat.Ks.Z,1 };
	material_emission = { 0,0,0,1 };
	material_shininess = mat.Ns;


}
GLobject* GLobject::draw_ground(GLdouble x0, GLdouble x1, GLdouble y0, GLdouble y1, int divx, int divy) {
	GLobject * res = new GLobject();
	GLdouble stepx = (x1 - x0) / divx;
	GLdouble stepy = (y1 - y0) / divy;

	GLdouble x, y;
	y = y0;
	x = x0;

	res->count_indexes = divx*divy*6;
	res->count_vertex = divx * divy *6;
	
	std::vector<GLushort> indexs;
	indexs.reserve(res->count_indexes);

	glGenBuffers(1, &(res->VBO));
	glGenBuffers(1, &(res->IBO));
	glGenVertexArrays(1, &(res->VAO));

	//		   XYZW					Normal XYZ		 TexCoords	UV					Color RGB		
	auto len = sizeof(GLfloat) * 4 + sizeof(GLfloat) * 3 + sizeof(GLfloat) * 2 + sizeof(GLfloat) * 3;
	res->CoordOffset = 0;
	res->NormalOffset = sizeof(GLfloat) * 4;
	res->TexCoordOffset = res->NormalOffset + sizeof(GLfloat) * 3;
	res->ColorOffset = res->TexCoordOffset + sizeof(GLfloat) * 2;
	res->stride = len;

	glBindVertexArray(res->VAO);


	glBindBuffer(GL_ARRAY_BUFFER, res->VBO);	glBufferData(GL_ARRAY_BUFFER, len*res->count_vertex, NULL, GL_STATIC_DRAW);
	GLfloat * mapped_data = (GLfloat *)glMapBufferRange(GL_ARRAY_BUFFER, 0, len*res->count_vertex, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	int ind = 0;
	GLushort vertex = 0;
	for (size_t i = 0; i < divx; i++)
	{
		y = y0;
		for (size_t j = 0; j < divy; j++)
		{
			
			// 0
			mapped_data[ind++] = x;
			mapped_data[ind++] = y;
			mapped_data[ind++] = 0;
			mapped_data[ind++] = 1;
			mapped_data[ind++] = 0;
			mapped_data[ind++] = 0;
			mapped_data[ind++] = 1;
			mapped_data[ind++] = 0;
			mapped_data[ind++] = 0;
			mapped_data[ind++] = 1;
			mapped_data[ind++] = 1;
			mapped_data[ind++] = 1;
			
			
			// 1
			mapped_data[ind++] = x + stepx;
			mapped_data[ind++] = y;
			mapped_data[ind++] = 0;
			mapped_data[ind++] = 1;
			mapped_data[ind++] = 0;
			mapped_data[ind++] = 0;
			mapped_data[ind++] = 1;
			mapped_data[ind++] = 1;
			mapped_data[ind++] = 0;
			mapped_data[ind++] = 1;
			mapped_data[ind++] = 1;
			mapped_data[ind++] = 1;

			//2
			mapped_data[ind++] = x;
			mapped_data[ind++] = y + stepy;
			mapped_data[ind++] = 0;
			mapped_data[ind++] = 1;
			mapped_data[ind++] = 0;
			mapped_data[ind++] = 0;
			mapped_data[ind++] = 1;
			mapped_data[ind++] = 0;
			mapped_data[ind++] = 1;
			mapped_data[ind++] = 1;
			mapped_data[ind++] = 1;
			mapped_data[ind++] = 1;
			
			//3
			mapped_data[ind++] = x + stepx;
			mapped_data[ind++] = y + stepy;
			mapped_data[ind++] = 0;
			mapped_data[ind++] = 1;
			mapped_data[ind++] = 0;
			mapped_data[ind++] = 0;
			mapped_data[ind++] = 1;
			mapped_data[ind++] = 1;
			mapped_data[ind++] = 1;
			mapped_data[ind++] = 1;
			mapped_data[ind++] = 1;
			mapped_data[ind++] = 1;
			
			indexs.push_back(vertex);
			indexs.push_back(vertex + 1);
			indexs.push_back(vertex + 2);
			indexs.push_back(vertex + 2);
			indexs.push_back(vertex + 1);
			indexs.push_back(vertex + 3);

			vertex += 4;
			y += stepy;

		}
		x += stepx;
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, res->IBO);


	glBufferData(GL_ELEMENT_ARRAY_BUFFER, res->count_indexes * sizeof(GLushort), indexs.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
	res->texture = -1;
	res->use_texture = false;
	res->object_transformation = glm::mat4(1.0);

	res->material_ambient = {0,0,0,1};
	res->material_diffuse = { 0,0,0,1 };
	res->material_specular = { 0,0,0,1 };
	res->material_emission = { 0,0,0,1 };
	res->material_shininess = 0;

	return res;
}

bool GLobject::BindAttributesToShader(GLShader & shaderobject)
{
	glBindVertexArray(VAO);
	GLint CoordAttrib = shaderobject.getAttributeLocation("coord");
	GLint NormAttrib = shaderobject.getAttributeLocation("norm");

	GLint TexCoordAttrib = shaderobject.getAttributeLocation("texcoord");
	GLint ColorAttrib = shaderobject.getAttributeLocation("color");
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	if (CoordAttrib == -1 || NormAttrib == -1 || TexCoordAttrib == -1 || ColorAttrib == -1)
	{
		//glBindVertexArray(0);
		//return false;
	}
	if (CoordAttrib != -1) {
		glEnableVertexAttribArray(CoordAttrib);
		glVertexAttribPointer(CoordAttrib, 4, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0));
	}
	if (NormAttrib != -1) {
		glEnableVertexAttribArray(NormAttrib);
		glVertexAttribPointer(NormAttrib, 3, GL_FLOAT, GL_TRUE, stride, BUFFER_OFFSET(sizeof(GLfloat)*4));
	}
	if (TexCoordAttrib != -1) {
		glEnableVertexAttribArray(TexCoordAttrib);
		glVertexAttribPointer(TexCoordAttrib, 2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(sizeof(GLfloat)* 7));
	}
	if (ColorAttrib != -1){
		glEnableVertexAttribArray(ColorAttrib);
		glVertexAttribPointer(ColorAttrib, 3, GL_FLOAT, GL_TRUE, stride, BUFFER_OFFSET(sizeof(GLfloat)* 9));
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return true;
}

void GLobject::drawObject()
{
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glDrawElements(GL_TRIANGLES, count_indexes, GL_UNSIGNED_SHORT,NULL);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


GLobject::~GLobject()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAO);
}
