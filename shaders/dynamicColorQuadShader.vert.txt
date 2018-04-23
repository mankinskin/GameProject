#version 450
const unsigned int MAX_QUAD_COUNT = 10000;

layout(location = 0)in vec2 corner_pos;
layout(location = 1)in vec4 coloring;


layout(std140)uniform QuadBuffer{
	vec4 quadBuffer[MAX_QUAD_COUNT];
};

out vec4 vcolor;
void main(){
	vec4 quad = quadBuffer[gl_InstanceID];
	vcolor = coloring;
	gl_Position = vec4(quad.x + quad.z*corner_pos.x, quad.y - quad.w * (1.0 - corner_pos.y), 0.0, 1.0);
}