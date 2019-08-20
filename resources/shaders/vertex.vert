#version 400
layout(location=0) in vec3 vp;
layout(location=1) in vec3 vc;

out vec3 vertex_color;

uniform mat4 MVP;

void main() {
  gl_Position = MVP*vec4(vp, 1.0);
  vertex_color = vc;
}
