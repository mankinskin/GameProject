#version 450

layout(location = 0)out vec4 color;

in Vertex{
	vec4 color;
}inVertex;

void main(){
	color = vec4(inVertex.color.xyz, 0.5f);
}