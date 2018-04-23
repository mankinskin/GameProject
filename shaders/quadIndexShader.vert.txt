#version 450

const uint MAX_QUAD_COUNT = 1000;
layout(location = 0)in vec2 corner_pos;

layout(std140)uniform QuadBuffer{
	vec4 quadBuffer[MAX_QUAD_COUNT];
};


flat out uint quad_id;
void main(){

	vec4 quad = quadBuffer[gl_InstanceID];
	quad_id = gl_InstanceID + 1;

	gl_Position = vec4(quad.x + quad.z*corner_pos.x, quad.y - quad.w * (1.0 - corner_pos.y), 0.0, 1.0);
}
