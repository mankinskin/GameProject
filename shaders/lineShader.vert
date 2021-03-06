#version 450
layout(location = 0) in uvec2 vertexColor;

layout(std140)uniform ColorBuffer {
	vec4 colorBuffer[100];
};
layout(std140)uniform PosBuffer {
	vec4 posBuffer[200];
};
layout(std140)uniform GeneralUniformBuffer {
	vec4 generalUniformBuffer[9];
};

out vec4 v_color;

void main(){
	mat4 matrix = mat4(generalUniformBuffer[0], generalUniformBuffer[1], 
                        generalUniformBuffer[2], generalUniformBuffer[3]);
	matrix = matrix * mat4(generalUniformBuffer[4], 
                        generalUniformBuffer[5], generalUniformBuffer[6], generalUniformBuffer[7]);
	
	gl_Position = matrix * posBuffer[vertexColor.x];
	v_color = colorBuffer[vertexColor.y];
}
