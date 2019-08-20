#version 400
in vec3 vertex_color;
out vec4 frag_colour;
void main() {
  frag_colour = vec4(vertex_color, 1.0);
}
