#version 450

layout(location = 0)out vec4 color;

layout(binding = 0) uniform sampler2D amb_texture;
layout(binding = 1) uniform sampler2D diff_texture;
layout(binding = 2) uniform sampler2D spec_texture;

uniform unsigned int materialIndex;
struct Material{
        vec4 ambient_albedo;
        vec4 diffuse_albedo;
        vec4 specular_albedo;
};

layout(std140)uniform MaterialBuffer{
        vec4 materialBuffer[10];
};

layout(std140)uniform LightDataBuffer{
        vec4 lightDataBuffer[300];
};

in Vertex{
	vec3 pos;
	vec3 normal;
	vec2 uv;
}inVertex;

in vec3 cam_pos;

void main(){

	Material mat = Material(materialBuffer[materialIndex*3], materialBuffer[materialIndex*3 + 1], materialBuffer[materialIndex*3 + 2]);
	
	vec4 ambient_albedo = texture(amb_texture, inVertex.uv);
	vec4 diffuse_albedo = texture(diff_texture, inVertex.uv);
	vec4 specular_albedo = texture(spec_texture, inVertex.uv);
	
	//Lighting calculations-----
	//constants
	const float gamma = 2.2;
	const float exposure = 1.0f;
	
	vec3 pos = inVertex.pos;
	vec3 normal = inVertex.normal;
	
	//for each light
	//read light data
	vec4 light_pos = lightDataBuffer[0];
	vec4 light_color = lightDataBuffer[1];
	
	//read camera data
	
	
	//light_color.xyz = vec3(1.0) - exp(-light_color * exposure);
	//light_color.xyz = pow(light_color, vec3(1.0/gamma));
	
	//compute light vectors
	vec3 cam_vec = cam_pos - pos;
	vec3 light_vec = light_pos.xyz - pos * light_pos.w;
	vec3 light_dir = normalize(light_vec);
	float light_dist = length(light_vec)*light_pos.w;
	vec3 reflection_ray = -reflect(light_dir, normal);
	
	//compute diffuse/specular amount
	float diffuse_amt = clamp(dot(light_dir, normal), 0.0, 1.0);
	float specular_amt = clamp(  dot(normalize(cam_vec), reflection_ray), 0.0, 1.0);
	
	//compute attenuation
	float att_linear = 1.0/(light_color.w)*0.1;
	float att_quadric = 1.0/(light_color.w*light_color.w)*0.1;
	float att = max(1.0/(1.0 + att_linear * light_dist + att_quadric * light_dist * light_dist), 0.0);
	
	//compute output colors
	vec3 ambient_light = ambient_albedo.xyz* mat.ambient_albedo.xyz * light_color.xyz ;
	vec3 diffuse_light = diffuse_albedo.xyz * mat.diffuse_albedo.xyz * light_color.xyz * diffuse_amt * att;
	vec3 specular_light = specular_albedo.xyz * light_color.xyz * mat.specular_albedo.xyz* pow(specular_amt, specular_albedo.w * mat.specular_albedo.w) * att ;
	
	color = vec4(specular_light + diffuse_light + ambient_light, diffuse_albedo.w*ambient_albedo.w);
}
