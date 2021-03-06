#version 450

layout( location = 0 )out vec4 ocolor;


in vec2 uv;

layout( binding = 0 )uniform sampler2D atlas;

void main()
{
	float alpha = texture( atlas, uv ).r;
    vec3 color = vec3( 0.99, 0.99, 0.99 );
	ocolor = vec4( color, alpha );
}
