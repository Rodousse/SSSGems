#version 400
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;

out float distance;


struct CameraData
{
    mat4 proj;
    mat4 view;
};

uniform CameraData camera;

void main() {
  gl_Position = camera.proj * camera.view * vec4(position + normal*0.003, 1.0);
  distance = length(camera.view * vec4(position, 1.0));
}
