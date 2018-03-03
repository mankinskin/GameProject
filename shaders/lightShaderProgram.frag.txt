#version 450
#extension ARB_shader_image_load_store : enable

//layout(binding = 0)uniform sampler2D ambient_texture;
//layout(binding = 1)uniform sampler2D diffuse_texture;
//layout(binding = 2)uniform sampler2D specular_texture;
//layout(binding = 3)uniform sampler2D pos_texture;
//layout(binding = 4)uniform sampler2D normal_texture;

layout(location = 0)out vec4 color;

layout(std140)uniform GeneralUniformBuffer{
    vec4 generalUniformBuffer[9];
};

//layout(std140)uniform LightDataBuffer{
//    vec4 lightDataBuffer[300];
//};
uniform layout(location = 0, rgba16f) readonly image3D voxel_volume;

in vec3 uv;
//in flat uvec2 i_rng;

in layout(origin_upper_left, pixel_center_integer) vec4 gl_FragCoord; 

void main(){
	const float gamma = 2.2;
	const float exposure = 1.0f;
	const vec3 amb = vec3(0.2, 0.2, 0.2);
	const ivec3 volume_size = imageSize(voxel_volume);
	
	//vec2 screen_coord = gl_FragCoord.xy / vec2(1600.0, 850.0);
	//read camera data
	vec3 cam_pos = generalUniformBuffer[8].xyz;
	////read light data
	//vec4 light_pos = lightDataBuffer[i_rng.x];
	//vec4 light_color = lightDataBuffer[i_rng.x + 1];
	
	//vec4 o_col = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	//for(unsigned int i = 0; i < 200; ++i){
	//	
	//	ivec3 coord = ivec3(uv * volume_size.xy, i);
	//	o_col = (o_col + imageLoad(voxel_volume, coord))/2.0f;
	//}
	
	
	ivec3 vol_coord = ivec3(uv * volume_size);
	color = imageLoad(voxel_volume, vol_coord);// + vec4(1.0, 0.6, 0.7, 0.1);
	//light_color.xyz = vec3(1.0) - exp(-light_color * exposure);
	//light_color.xyz = pow(light_color, vec3(1.0/gamma));
	
	////read geometry data (normal and position)
	//vec3 pos = texture(pos_texture, uv).xyz;
	//vec3 normal = texture(normal_texture, uv).xyz;
	//
	////read material data (of the surface)
	//vec4 g_ambient_albedo =  texture(ambient_texture, uv);
	//vec4 g_diffuse_albedo = texture(diffuse_texture, uv);
	//vec4 g_specular_albedo = texture(specular_texture, uv);
    //
	
	//
	////compute light vectors
	//vec3 cam_vec = cam_pos - pos;
	//vec3 light_vec = light_pos.xyz - pos * light_pos.w;
	//vec3 light_dir = normalize(light_vec);
	//float light_dist = length(light_vec)*light_pos.w;
	//vec3 reflection_ray = -reflect(light_dir, normal);
	//
	////compute diffuse/specular amount
	//float diffuse_amt = clamp(dot(light_dir, normal), 0.0, 1.0);
	//float specular_amt = clamp(dot(normalize(cam_vec), reflection_ray), 0.0, 1.0);
	//
	////compute attenuation
	//float att_linear = 1.0/(light_color.w)*0.1;
	//float att_quadric = 1.0/(light_color.w*light_color.w)*0.1;
	//float att = max(1.0/(1.0 + att_linear * light_dist + att_quadric * light_dist * light_dist), 0.0);
	//
	////compute output colors
	//float diff_intensity = diffuse_amt * att;//the amt of diffuse light at this fragment
	//float spec_intensity = pow(specular_amt, 100) * att;
	//
	////vec3 ambient_color = g_diffuse_albedo.xyz * amb;
	//vec3 diffuse_color = g_diffuse_albedo.xyz * light_color.xyz;
	////vec3 specular_color = g_specular_albedo.xyz * light_color.xyz;
	//
	//vec3 color_sum = diffuse_color;
	//float light_intensity = diff_intensity;
	//float light_amt = light_intensity * g_diffuse_albedo.w;
	

}
