#version 450
layout(location = 0) in vec3 vertex_pos;
layout(location = 1) in vec3 vertex_normal;
layout(location = 3) in uint transform;
const uint MAX_ENTITIES = 1000;

out vec3 v_pos;
out vec3 v_normal;

uniform GeneralUniformBuffer{
	vec4 generalUniformBuffer[9];
};
uniform NodeMatrixBuffer{
	vec4 matrixBuffer[MAX_ENTITIES*4];
};
void main(){
	mat4 matrix = mat4(generalUniformBuffer[0], generalUniformBuffer[1], generalUniformBuffer[2], generalUniformBuffer[3]);
	matrix = matrix * mat4(generalUniformBuffer[4], generalUniformBuffer[5], generalUniformBuffer[6], generalUniformBuffer[7]);
	mat4 transform_mat = mat4(matrixBuffer[transform*4], matrixBuffer[transform*4 + 1], matrixBuffer[transform*4 + 2], matrixBuffer[transform*4 + 3]);
	
	vec4 t_pos = (matrix * vec4(vertex_pos, 1.0));
	v_pos = t_pos.xyz + vec3(100.0, 100.0, 0.0);
	v_normal = vec4(vec4(vertex_normal, 0.0)).xyz;
	gl_Position = t_pos;
}
