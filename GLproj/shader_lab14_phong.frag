#version 430

out vec4 color;
uniform bool use_texture;

// параметры источника освещения

uniform vec4 light_position;
uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;
uniform vec3 light_attenuation;

// параметры материала
uniform sampler2D material_texture;
uniform vec4 material_ambient;
uniform vec4 material_diffuse;
uniform vec4 material_specular;
uniform vec4 material_emission;
uniform float material_shininess;

// параметры, полученные из вершинного шейдера
in vec2 vert_texcoord;   // текстурные координаты
in vec3 vert_normal;     // нормаль
in vec3 vert_lightDir;   // направление на источник освещения
in vec3 vert_viewDir;    // направление от вершины к наблюдателю
in float vert_distance;  // расстояние от вершины до источника освещения
in vec3 vert_color;

void main(){
	vec3 normal = normalize(vert_normal);
	vec3 lightDir = normalize(vert_lightDir);
	vec3 viewDir = normalize(vert_viewDir);
	
	float attenuation = 1;
	if(length(light_attenuation) != 0)
		 attenuation = 1.0 / (light_attenuation[0] + light_attenuation[1] * vert_distance + light_attenuation[2] * vert_distance * vert_distance);
	
	color = material_emission;
	color += material_ambient * light_ambient * attenuation;
	float Ndot = max(dot(normal, lightDir), 0.0);
	color += material_diffuse * light_diffuse * Ndot * attenuation;
	
	// добавление отражённого света
	float RdotVpow = max(pow(dot(reflect(-lightDir, normal), viewDir), material_shininess), 0.0);
	color += material_specular * light_specular * RdotVpow * attenuation;
	
	if(use_texture)
		color *= texture(material_texture, vert_texcoord);
	else
		color *= vec4(vert_color, 1.0);
}