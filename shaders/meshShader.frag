#version 450

layout(location = 0)out vec4 ambientBuffer;
layout(location = 1)out vec4 diffuseBuffer;
layout(location = 2)out vec4 specularBuffer;
layout(location = 3)out vec3 posBuffer;
layout(location = 4)out vec3 normalBuffer;

layout(binding = 0) uniform sampler2D amb_texture;
layout(binding = 1) uniform sampler2D diff_texture;
layout(binding = 2) uniform sampler2D spec_texture;

struct Material
{
  vec4 ambient_albedo;
  vec4 diffuse_albedo;
  vec4 specular_albedo;
};

layout(std140)uniform MaterialBuffer
{
  vec4 materialBuffer[10];
};

uniform uint materialIndex;

in Vertex
{
  vec3 pos;
  vec3 normal;
  vec2 uv;
} inVertex;

void main()
{
  Material mat = Material(materialBuffer[materialIndex*3], materialBuffer[materialIndex*3 + 1], materialBuffer[materialIndex*3 + 2]);
  ambientBuffer = texture(amb_texture, inVertex.uv);
  diffuseBuffer = texture(diff_texture, inVertex.uv);
  specularBuffer = texture(spec_texture, inVertex.uv);

  //write to material index buffer(texture)
  posBuffer = inVertex.pos;
  normalBuffer = inVertex.normal;
}
