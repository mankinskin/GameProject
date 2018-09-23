#version 450
layout(location = 0) in vec2 corner_pos;
layout(location = 1) in uvec2 index_range;

layout(std140)uniform GeneralUniformBuffer{
    vec4 generalUniformBuffer[9];
};


out vec3 uv;

void main(){
	mat4 p_matrix = mat4(generalUniformBuffer[0], generalUniformBuffer[1], generalUniformBuffer[2], generalUniformBuffer[3]);
	mat4 v_matrix = mat4(generalUniformBuffer[4], generalUniformBuffer[5], generalUniformBuffer[6], generalUniformBuffer[7]);
	mat4 matrix = p_matrix * v_matrix;
	vec3 cam_pos = generalUniformBuffer[8].xyz;
	
	uv = vec3(corner_pos.x, corner_pos.y, 1.0 - gl_InstanceID/100.0);//relative coord of each corner vertex
	gl_Position = (vec4(-1.0, -1.0, 0.0, 0.0) + vec4(uv.x*2.0, uv.y*2.0, -uv.z, 1.0));//screen position of the 3d viewport
	//gl_Position = matrix * vec4(uv*10.0f, 1.0);//world position of the representation volume vertices
}
