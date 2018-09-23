#version 450
#extension ARB_shader_image_load_store : enable

layout(location = 0) out vec4 color;
uniform layout(location = 0, rgba16f) writeonly image3D voxel_volume;

in vec3 v_pos;
in vec3 v_normal;


void main(){
	const ivec3 volume_size = imageSize(voxel_volume);
	vec4 o_val = vec4(v_normal, 1.0);
	
	ivec3 vol_coord = ivec3(v_pos);
	imageStore(voxel_volume, vol_coord, o_val);
	color = o_val;
}