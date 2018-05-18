#version 450

layout( location = 0 ) in vec2 corner;

layout( std140 ) uniform CharBuffer
{
	uvec4 chars[ 200 ];
};

layout( std140 ) uniform PosBuffer
{
	vec4 positions[ 500 ];
};
layout( std140 ) uniform UVBuffer
{
	vec4 uvs[ 255 ];
};

layout( std140 ) uniform SizeBuffer
{
	vec4 sizes[ 255 ];
};

out vec2 uv;

void main()
{
	const uint i = gl_InstanceID;
	const uint c = chars[ i/4 ][ i%4 ];
	vec2 pos = vec2( positions[ i/2 ][ (i%2)*2 ], positions[ i/2 ][ (i%2)*2 + 1] );
	vec2 size = vec2( sizes[ c/2 ][ (c%2)*2 ], sizes[ c/2 ][ (c%2)*2 + 1] );
	vec4 gly = uvs[ c ];

	uv = vec2(
		gly.x + gly.z * corner.x, 
		gly.y + gly.w * ( 1.0 - corner.y ) );

	gl_Position = vec4( pos.x + size.x * corner.x, -size.y + pos.y + (size.y * corner.y), 0.0, 1.0 );
}

