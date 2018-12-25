#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/glut.h>
#include <SOIL2.h>
#include <cmath>
#include <utility>
#include <iostream>
#include <vector>
#include <list>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/transform.hpp> 
#include<glm/trigonometric.hpp>

#include "GLShader.h"
#include "GLobject.h"

int w = 0, h = 0;
std::vector<GLobject*> scene;

GLShader * shaderwrap;

std::vector<int> VertShaders;
std::vector<int> FragShaders;
glm::mat4 Matrix_projection;

float rotateX = 0;
float rotateY = 0;

int VertShaderPhong, FragShaderPhong;

glm::vec3 eye {100,0,0};
float dist = eye[0];
glm::vec4 light_position, light_ambient, light_diffuse, light_specular;
glm::vec3 light_attenuation;

int oldTimeSinceStart = 0;
bool animate = false;

// параметры источника освещения
struct Light
{
public: 
	glm::vec4 light_position;
	glm::vec4 light_ambient;
	glm::vec4 light_diffuse;
	glm::vec4 light_specular;
	glm::vec3 light_attenuation;
	glm::vec3 spot_direction;
	float spot_cutoff;
	float spot_exp;
};

std::vector<Light> lights;

std::vector<std::string> pathsVert = {
"shader_phong_struct.vert"
};

std::vector<std::string> pathsFrag = {
"shader_phong_struct.frag"
};

void LoadShaders() {
	VertShaderPhong = shaderwrap->load_shader(pathsVert[0], GL_VERTEX_SHADER);
	FragShaderPhong = shaderwrap->load_shader(pathsFrag[0], GL_FRAGMENT_SHADER);
}

void Init(void)
{
	glClearColor(0, 0, 0, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Set_cam() {
	Matrix_projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.01f, 200.0f);
	glm::vec3 center = { 0,0,0 };
	glm::vec3 up = { 0,0,1 };

	Matrix_projection *= glm::lookAt(eye, center, up);   //создает матрицу вида, которая смотрит на заданную цель
}

void Reshape(int x, int y)
{
	if (y == 0 || x == 0) return;

	w = x;
	h = y;
	glViewport(0, 0, w, h);

	Set_cam();
}


void set_light() {
	Light l1;
	l1.light_position = { 65,0,38,1 };
	l1.light_ambient = { 0.2,0.2,0.2,1 };
	l1.light_diffuse = { 0.8,0,0,1 };
	l1.light_specular = { 0.8,0,0,1 };
	l1.light_attenuation = {1,0,0.0001};
	l1.spot_direction = { 0, 0, -1 };
	l1.spot_cutoff = std::cos(glm::radians(40.0f));
	l1.spot_exp = 250;
	lights.push_back(l1);

	Light l2;
	l2.light_position = { 0,0,100,1 };
	l2.light_ambient = { 0.2,0.2,0.2,1 };
	l2.light_diffuse = { 0.4,0.4,0.4,1 };
	l2.light_specular = { 0.3,0.3,0.3,1 };
	l2.light_attenuation = { 0,0,0 };
	l2.spot_cutoff = -1;
	l2.spot_exp = 0;
	lights.push_back(l2);
}

void Update(void) {
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(shaderwrap->ShaderProgram);  //Используем шейдерную программу
	
	shaderwrap->setUniformmat4("transform_viewProjection", false, Matrix_projection);
	shaderwrap->setUniformfv3("transform_viewPosition", eye);
	shaderwrap->setUniform1s("material_texture", 0);

	shaderwrap->setUniform1i("lcount", lights.size());

	for (int i = 0; i < lights.size(); ++i) {
		std::string prefix = "l[" + std::to_string(i) + "].";
		shaderwrap->setUniformfv4(prefix+"light_position", lights[i].light_position);
		shaderwrap->setUniformfv4(prefix+"light_ambient", lights[i].light_ambient);
		shaderwrap->setUniformfv4(prefix + "light_diffuse", lights[i].light_diffuse);
		shaderwrap->setUniformfv4(prefix + "light_specular", lights[i].light_specular);
		shaderwrap->setUniformfv3(prefix + "light_attenuation", lights[i].light_attenuation);
		shaderwrap->setUniformfv3(prefix + "spot_direction", lights[i].spot_direction);
		shaderwrap->setUniform1f(prefix + "spot_cutoff", lights[i].spot_cutoff);
		shaderwrap->setUniform1f(prefix + "spot_exp", lights[i].spot_exp);
	}


	for (int i = 0; i < scene.size(); ++i) {
		shaderwrap->setUniformfv4("material_ambient", scene[i]->material_ambient);
		shaderwrap->setUniformfv4("material_diffuse", scene[i]->material_diffuse);
		shaderwrap->setUniformfv4("material_specular", scene[i]->material_specular);
		shaderwrap->setUniformfv4("material_emission", scene[i]->material_emission);
		shaderwrap->setUniform1f("material_shininess", scene[i]->material_shininess);
		shaderwrap->setUniformmat4("transform_model", false, scene[i]->object_transformation);
		glm::mat3 transform_normal = glm::inverseTranspose(glm::mat3(scene[i]->object_transformation));
		shaderwrap->setUniformmat3("transform_normal", false, transform_normal);
		shaderwrap->setUniform1b("use_texture", scene[i]->use_texture);
		
		scene[i]->drawObject();
	}

	glUseProgram(0);

	glFlush();
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)	
{
	switch (key)
	{
	case 'w':
		rotateY+= 0.1;
		break;
	case 's':
		rotateY -= 0.1;
		break;
	case 'a':
		rotateX += 0.1;
		break;
	case 'd':
		rotateX -= 0.1;
		break;
	case 'r':
		dist -= 1;
		break;
	case 'f':
		dist += 1;
		break;
	default:
		break;
	}

	glm::vec4 e = { dist, 0, 0, 1 };
	e = e * glm::rotate(rotateX, glm::vec3{ 0, 0, 1 });
	glm::vec3 d = glm::cross(glm::vec3{ 0, 0, 1 }, glm::vec3(e));
	e = e * glm::rotate(rotateY, d);

	eye = glm::vec3(e);
	Set_cam();
	glutPostRedisplay();
}

void rotate_light(float angle) {
	lights[0].spot_direction = glm::mat3(glm::rotate(angle, glm::vec3{ 1, 0, 0 })) * lights[0].spot_direction;
}

void specialKey(int key, int x, int y) {
	float angle = 0;
	switch (key)
	{
	case GLUT_KEY_RIGHT:
		angle += 0.01f;
		break;
	case GLUT_KEY_LEFT:
		angle -= 0.01f;
		break;
	case GLUT_KEY_F1:
		animate = !animate;
	default:
		break;
	}

	rotate_light(angle);
	glutPostRedisplay();
}

void animate_tree() {
	int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	int deltaTime = timeSinceStart - oldTimeSinceStart;
	oldTimeSinceStart = timeSinceStart;
	if (!animate)
		return;

	glm::mat4 anim = glm::translate(glm::vec3{ 55, 16, 0 });
	anim *= glm::rotate(glm::radians((float)deltaTime / 100), glm::vec3{ 0, 0, 1 });
	anim *= glm::translate(glm::vec3{ -55, -16, 0 });
	scene[1]->object_transformation = anim * scene[1]->object_transformation;
	glutPostRedisplay();
}

void load_scene() {
	scene.push_back(new GLobject("obj/Cottage.obj", "textures/Cottage1.jpg"));									// house - 0
	scene.push_back(new GLobject("obj/toon_pine.obj", "textures/toon_pine.png"));								// christmas tree - 1
	scene.push_back(new GLobject("obj/Circle_box.obj", "", glm::vec3{ 0.5, 0, 0.5 }, glm::vec3{0, 0, -1}));     // circle gift - 2
	scene.push_back(new GLobject("obj/Bow.obj", "textures/bow1.jpg"));											 // circle gift bow - 3
	scene.push_back(new GLobject("obj/Heart_Shaped_Box.obj", "", glm::vec3{ 1, 0, 0 }));						// heart box - 4
	scene.push_back(new GLobject("obj/cube_box.obj", "textures/box3.jpg"));										// cube box - 5
	scene.push_back(new GLobject("obj/Bow.obj", "textures/bow2.jpg"));											// cube box bow - 6
	scene.push_back(new GLobject("obj/Gingerbread_male.obj", "textures/Gingerbread_male.jpg"));					 // gingerbread male - 7
	scene.push_back(new GLobject("obj/Gingerbread_female.obj", "textures/Gingerbread_female.jpg"));				 // gingerbread female - 8
	scene.push_back(GLobject::draw_ground(-80, 80, -60, 60, 10, 10));											// ground - 9
	scene.push_back(new GLobject("obj/cat.obj", "textures/cat_diff.tga"));										// cat

	scene[0]->object_transformation *= glm::rotate(glm::radians(90.0f), glm::vec3{ 1, 0, 0 });
	scene[0]->object_transformation *= glm::rotate(glm::radians(90.0f), glm::vec3{ 0, -1, 0 });

	scene[1]->material_emission = glm::vec4{ 0.7, 0.7, 0.7, 1 };
	scene[1]->object_transformation *= glm::translate(glm::vec3{ 55, 16, 0 });
	scene[1]->object_transformation *= glm::rotate(glm::radians(90.0f), glm::vec3{ 1, 0, 0 });
	scene[1]->object_transformation *= glm::scale(glm::vec3{ 0.12f, 0.12f, 0.12f });

	scene[2]->material_ambient = { 0.2, 0.2, 0.2, 1 };
	scene[2]->material_diffuse = { 0.6, 0.6, 0.6, 1 };
	//scene[2]->material_specular = { 0.6, 0.6, 0.6, 1 };
	scene[2]->object_transformation *= glm::translate(glm::vec3{ 54, 5, 3 });
	scene[2]->object_transformation *= glm::rotate(glm::radians(180.0f), glm::vec3{ 1, 0, 0 });
	scene[2]->object_transformation *= glm::scale(glm::vec3{ 0.7f, 0.7f, 0.7f });

	scene[3]->material_ambient = { 0.2, 0.2, 0.2, 1 };
	scene[3]->material_diffuse = { 0.6, 0.6, 0.6, 1 };
	scene[3]->material_specular = { 0.6, 0.6, 0.6, 1 };
	scene[3]->object_transformation *= glm::translate(glm::vec3{ 54, 5, 3 });

	scene[4]->material_ambient = { 0.2, 0.2, 0.2, 1 };
	scene[4]->material_diffuse = { 0.5, 0.5, 0.5, 1 };
	//scene[4]->material_specular = { 0.5, 0.5, 0.5, 1 };
	scene[4]->object_transformation *= glm::translate(glm::vec3{ 62, 10, 1 });
	scene[4]->object_transformation *= glm::rotate(glm::radians(60.0f), glm::vec3{ 0, 0, 1 });
	scene[4]->object_transformation *= glm::scale(glm::vec3{ 0.6f, 0.6f, 0.6f });

	scene[5]->material_ambient = { 0.2, 0.2, 0.2, 1 };
	scene[5]->material_diffuse = { 0.6, 0.6, 0.6, 1 };
	//scene[5]->material_specular = { 0.6, 0.6, 0.6, 1 };
	scene[5]->object_transformation *= glm::translate(glm::vec3{ 61,4, 2.5 });
	scene[5]->object_transformation *= glm::rotate(glm::radians(120.0f), glm::vec3{ 0, 0, 1 });
	scene[5]->object_transformation *= glm::scale(glm::vec3{ 2.0f, 2.0f, 2.0f });

	scene[6]->material_ambient = { 0.2, 0.2, 0.2, 1 };
	scene[6]->material_diffuse = { 0.6, 0.6, 0.6, 1 };
	scene[6]->material_specular = { 0.6, 0.6, 0.6, 1 };
	scene[6]->object_transformation *= glm::translate(glm::vec3{ 61, 4, 4.5 });
	scene[6]->object_transformation *= glm::rotate(glm::radians(30.0f), glm::vec3{ 0, 0, 1 });
	scene[6]->object_transformation *= glm::scale(glm::vec3{ 0.8f, 0.8f, 0.8f });

	scene[8]->object_transformation *= glm::translate(glm::vec3{ 68, 20, 2 });
	scene[8]->object_transformation *= glm::rotate(glm::radians(90.0f), glm::vec3{ 0, 0, 1 });

	scene[7]->object_transformation *= glm::translate(glm::vec3{ 68, -20, 2 });
	scene[7]->object_transformation *= glm::rotate(glm::radians(90.0f), glm::vec3{ 0, 0, 1 });
	
	scene[9]->material_ambient = { 0.2, 0.2, 0.2, 1 };
	scene[9]->material_diffuse = { 1, 1, 1, 1 };
	scene[9]->material_specular = { 0.2, 0.2, 0.2, 1 };
	scene[9]->object_transformation *= glm::translate(glm::vec3{ 10, 0, -3 });

	scene[10]->object_transformation *= glm::translate(glm::vec3{ 60, -6, 0.5 });
	scene[10]->object_transformation *= glm::rotate(glm::radians(90.0f), glm::vec3{ 1, 0, 0 });
	scene[10]->object_transformation *= glm::rotate(glm::radians(180.0f), glm::vec3{ 0, 1, 0 });
	scene[10]->object_transformation *= glm::scale(glm::vec3{ 7, 7, 7 });
}


int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);

	glutCreateWindow("Merry Christmas!");
	glEnable(GL_DEPTH_TEST);
	
	glutDisplayFunc(Update);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKey);
	glutIdleFunc(animate_tree);
	GLenum err = glewInit();
	if (GLEW_OK != err)
		std::cout << glewGetErrorString(err) << std::endl;

	Init();

	shaderwrap = new GLShader();
	LoadShaders();

	load_scene();

	shaderwrap->linkProgram(VertShaderPhong, FragShaderPhong);

	for (int i = 0; i < scene.size(); ++i)
		scene[i]->BindAttributesToShader(*shaderwrap);

	shaderwrap->checkOpenGLerror();
	set_light();
	glutMainLoop();
	return 0;         
}