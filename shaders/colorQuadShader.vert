#version 450
const uint MAX_QUAD_COUNT = 1000;

layout(location = 0)in vec2 corner_pos;


layout(std140)uniform QuadBuffer{
	vec4 quadBuffer[MAX_QUAD_COUNT];
};

layout(std140)uniform QuadColorBuffer{
	uvec4 coloringBuffer[MAX_QUAD_COUNT/4];
};

layout(std140)uniform ColorBuffer{
	vec4 colorBuffer[100];
};

out vec4 vertex_color;

void main(){
	vec4 quad = quadBuffer[gl_InstanceID];
	uint coloring = coloringBuffer[gl_InstanceID/4][gl_InstanceID%4];
	vertex_color = colorBuffer[coloring];
	
	gl_Position = vec4(quad.x + quad.z*corner_pos.x, quad.y - quad.w * (1.0 - corner_pos.y), 0.0, 1.0);
}
