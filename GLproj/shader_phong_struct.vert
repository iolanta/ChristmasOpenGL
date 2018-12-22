#version 430
#define LIGHT_COUNT 2

attribute vec4 coord;
attribute vec3 norm;
attribute vec2 texcoord;
attribute vec3 color;

// параметры преобразований
uniform mat4 transform_model;
uniform mat4 transform_viewProjection;
uniform mat3 transform_normal;
uniform vec3 transform_viewPosition;

// параметры источника освещения
struct Light
{
	vec4 light_position;
	vec4 light_ambient;
	vec4 light_diffuse;
	vec4 light_specular;
	vec3 light_attenuation;
	vec3 spot_direction;
	float spot_cutoff;
	float spot_exp;
};

uniform int lcount;
uniform Light l[LIGHT_COUNT];

// параметры для фрагментного шейдера
out vec2 vert_texcoord;   // текстурные координаты
out vec3 vert_normal;     // нормаль
out vec3 vert_lightDir[LIGHT_COUNT];   // направления на все источники освещения
out vec3 vert_viewDir;    // направление от вершины к наблюдателю
out float vert_distance[LIGHT_COUNT];  // расстояния от вершины до источников освещения
out vec3 vert_color;


void main(){
	vec4 vertex = transform_model * coord;
	gl_Position = transform_viewProjection * vertex;
	
	vert_texcoord = texcoord;
	vert_normal = normalize(transform_normal * norm);
	
	int cnt = min(lcount, LIGHT_COUNT);
	for(int i = 0; i < cnt; ++i){
		vec4 lightDir = l[i].light_position - vertex;
		vert_lightDir[i] = normalize(vec3(lightDir));
		vert_distance[i] = length(lightDir);
	}
	
	vert_viewDir = normalize(transform_viewPosition - vec3(vertex));
	vert_color = color;
}
