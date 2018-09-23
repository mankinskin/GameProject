#version 450

layout(location = 0)out vec4 color;

in vec3 vcolor;


void main(){
	color = vec4(vcolor, 1.0);
}
