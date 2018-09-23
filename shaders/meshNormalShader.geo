#version 450

layout(points) in;
layout(line_strip, max_vertices = 2) out;


uniform GeneralUniformBuffer{
	vec4 generalUniformBuffer[9];
};

in vec4 v2g_normal[1];

out vec3 vcolor;

void main(){
	mat4 matrix = mat4(generalUniformBuffer[0], generalUniformBuffer[1], generalUniformBuffer[2], generalUniformBuffer[3]);
	matrix = matrix * mat4(generalUniformBuffer[4], generalUniformBuffer[5], generalUniformBuffer[6], generalUniformBuffer[7]);
	
	gl_Position = matrix * gl_in[0].gl_Position;
	vcolor = abs(vec3(v2g_normal[0]));
	EmitVertex();
	
	
	gl_Position = matrix * (v2g_normal[0] + gl_in[0].gl_Position);
	vcolor = abs(vec3(v2g_normal[0]));
	EmitVertex();
	EndPrimitive();
}

