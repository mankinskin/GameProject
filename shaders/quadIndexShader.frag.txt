#version 450

layout(location = 0)out uint id;

flat in uint quad_id;

void main(){
	id = quad_id;
}
