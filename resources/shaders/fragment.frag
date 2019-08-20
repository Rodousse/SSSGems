#version 400
in vec3 vertex_normal;
in vec3 camDir;

out vec4 frag_colour;
void main() {
  vec3 fnormal = normalize(vertex_normal);
  frag_colour = vec4(vec3(dot(fnormal,-normalize(camDir))), 1.0);
}
