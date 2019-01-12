#version 330

#define NR_POINT_LIGHTS 4
uniform vec3 spotLightsPos[NR_POINT_LIGHTS];
uniform sampler2D texture_1;
 
in vec2 texcoord;
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform vec3 light_direction;
uniform vec3 light_position;
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;
uniform float cut_off_angle;

uniform int t;

layout(location = 0) out vec4 out_color;

float CalcSpotLight(vec3 spotLightPos) {

	vec3 light_direction = vec3(0, 1, 0);
	vec3 N = normalize(world_normal);
	vec3 L = normalize(spotLightPos - world_position);
	vec3 V = normalize(eye_position - world_position);
	vec3 H = normalize(L + V);

	float diffuse_light = material_kd * max(dot(normalize(N), L), 0);
	float specular_light = 0;

	if (diffuse_light > 0) {
		specular_light = pow(max(0, dot(normalize(N), H)), material_shininess) * material_ks;
	}
	
	float light = 0;
	float cut_off = radians(cut_off_angle);
	float spot_light = dot(-L, light_direction);
	float spot_light_limit = cos(cut_off);
	if (spot_light > cos(cut_off)) {
		float linear_att = (spot_light - spot_light_limit) / (1 - spot_light_limit);
		float light_att_factor = pow(linear_att, 2);
		light = light_att_factor * (diffuse_light + specular_light); 
	}

	return light;
}  

void main()
{
	vec4 color1 = texture2D(texture_1, texcoord);
	float light;

	if (t == 1) {
		light = 0.25;	// Ambient Light
		for(int i = 0; i < NR_POINT_LIGHTS; i++)
			light += CalcSpotLight(spotLightsPos[i]);
	} else {
		vec3 N = normalize(world_normal);
		vec3 L = normalize(light_position - world_position);
		vec3 V = normalize(eye_position - world_position);
		vec3 H = normalize(L + V);

		float ambient_light = 0.5;
		float diffuse_light = material_kd * max(dot(normalize(N), L), 0);
		float specular_light = 0;

		if (diffuse_light > 0) {
			specular_light = pow(max(0, dot(normalize(N), H)), material_shininess) * material_ks;
		}
		light = ambient_light + diffuse_light + specular_light;
	}

	vec3 color = color1.xyz * light;
	out_color =  vec4(color, 1);
}