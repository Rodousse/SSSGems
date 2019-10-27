#version 400

in float distance;

out vec4 frag_colour;

void main() {
  frag_colour = vec4(distance);
}
