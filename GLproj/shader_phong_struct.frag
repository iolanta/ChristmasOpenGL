#version 430
#define LIGHT_COUNT 2

out vec4 color;
uniform bool use_texture;

// параметры источника освещения
struct Light
{
	vec4 light_position;
	vec4 light_ambient;
	vec4 light_diffuse;
	vec4 light_specular;
	vec3 light_attenuation;
	vec3 spot_direction;
	float spot_cutoff;      // -1 нет , 1..0 - 0..90грд
	float spot_exp;
};

// параметры материала
uniform sampler2D material_texture;
uniform vec4 material_ambient;
uniform vec4 material_diffuse;
uniform vec4 material_specular;
uniform vec4 material_emission;
uniform float material_shininess;

uniform int lcount;
uniform Light[LIGHT_COUNT] l;

// параметры, полученные из вершинного шейдера
in vec2 vert_texcoord;   // текстурные координаты
in vec3 vert_normal;     // нормаль
in vec3 vert_lightDir[LIGHT_COUNT];   // направления на источники освещения
in vec3 vert_viewDir;    // направление от вершины к наблюдателю
in float vert_distance[LIGHT_COUNT];  // расстояния от вершины до источников освещения
in vec3 vert_color;

void main(){
	vec3 normal = normalize(vert_normal);
	vec3 viewDir = normalize(vert_viewDir);
	color = material_emission;
	
	int cnt = min(lcount, LIGHT_COUNT);
	for(int i = 0; i < cnt; ++i){
		vec3 lightDir = normalize(vert_lightDir[i]);
		vec3 spotDir = normalize(-l[i].spot_direction);
	
		float spotEffect = dot(spotDir, lightDir);  // расчёт угла отклонения от направления прожектора до текущей точки
		float spot;
		
		if(l[i].spot_cutoff == -1)
			spot = 1;
		else
			spot= float(spotEffect > l[i].spot_cutoff); // 0  и  1 - точка под прожектором
		
		if(l[i].spot_exp != 0)
			spotEffect = max(pow(spotEffect, l[i].spot_exp), 0.0);   // затухание к краям зоны влияния
		else
			spotEffect = 1.0;
	
		float attenuation = 1;
		if(length(l[i].light_attenuation) != 0)
			attenuation = 1.0 / (l[i].light_attenuation.x + l[i].light_attenuation.y * vert_distance[i] + l[i].light_attenuation.z * vert_distance[i] * vert_distance[i]);
	
		color += (material_ambient * l[i].light_ambient) * attenuation;
		if(l[i].spot_cutoff != -1)
			attenuation *= spot * spotEffect;
	
		// добавление рассеянного света
		float Ndot = max(dot(normal, lightDir), 0.0);
		color += material_diffuse * l[i].light_diffuse * Ndot * attenuation;
		// добавление отражённого света
		float RdotVpow = pow(max(dot(normal, viewDir), 0.0), material_shininess);
		color += material_specular * l[i].light_specular * RdotVpow * attenuation;
	}
	
	if(use_texture)
		color *= texture(material_texture, vert_texcoord);
	else
		color *= vec4(vert_color, 1.0);
}