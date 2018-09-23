#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in uint transform;
const uint MAX_ENTITIES = 1000;

layout(std140)uniform GeneralUniformBuffer
{
  vec4 generalUniformBuffer[52];
};

layout(std140)uniform NodeMatrixBuffer
{
  vec4 matrixBuffer[MAX_ENTITIES*4];
};

out Vertex
{
  vec3 pos;
  vec3 normal;
  vec2 uv;
} outVertex;

void main()
{
  mat4 matrix = mat4(generalUniformBuffer[0], generalUniformBuffer[1], generalUniformBuffer[2], generalUniformBuffer[3]);
  matrix = matrix * mat4(generalUniformBuffer[4], generalUniformBuffer[5], generalUniformBuffer[6], generalUniformBuffer[7]);
  mat4 transform_mat = mat4(matrixBuffer[transform*4], matrixBuffer[transform*4 + 1],
  matrixBuffer[transform*4 + 2], matrixBuffer[transform*4 + 3]);

  outVertex.uv = uv;
  outVertex.normal = vec4(transform_mat * vec4(normal, 0.0)).xyz;
  vec4 tpos = transform_mat * vec4(pos, 1.0);
  outVertex.pos = tpos.xyz;
  gl_Position = matrix * tpos;

}
