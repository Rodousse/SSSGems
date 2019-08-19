#version 400
layout(location=0) in vec3 vp;

uniform mat4 MVP;

void main() {
  gl_Position = MVP*vec4(vp, 1.0);
}
