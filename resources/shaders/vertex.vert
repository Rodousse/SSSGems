#version 400
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;

out vec3 vertex_normal;
out vec3 camDir;


struct UniformData
{
    mat4 proj;
    mat4 view;
};

uniform UniformData VP;

void main() {
  gl_Position = VP.proj * VP.view *vec4(position, 1.0);
  camDir = vec3(VP.view *vec4(position, 1.0));
  vertex_normal = normalize(mat3(VP.view)*normal);
}
