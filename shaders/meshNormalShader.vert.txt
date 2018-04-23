#version 450
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 3) in unsigned int transform;
const unsigned int MAX_ENTITIES = 1000;



uniform NodeMatrixBuffer{
	vec4 matrixBuffer[MAX_ENTITIES*4];
};

out vec4 v2g_normal;

void main(){
	
	mat4 transform_mat = mat4(matrixBuffer[transform*4], matrixBuffer[transform*4 + 1], matrixBuffer[transform*4 + 2], matrixBuffer[transform*4 + 3]);

	v2g_normal = transform_mat * vec4(normal, 0.0f);
	gl_Position = transform_mat * vec4(pos, 1.0);
}
