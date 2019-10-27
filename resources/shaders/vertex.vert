#version 400
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;

out vec3 vertexNormal;
out vec3 lightDir;
out vec3 vertexCameraNormal;
out vec3 camDir;
out vec4 vertexLightCoord;
out float vertexLightDist;


struct CameraData
{
    mat4 proj;
    mat4 view;
};

struct DirectionalLightData
{
    mat4 biasProjView;
    mat4 projView;
    mat4 view;
    vec3 dir;
};

uniform CameraData camera;
uniform DirectionalLightData light;

void main() {
  gl_Position = camera.proj * camera.view * vec4(position, 1.0);
  camDir = -vec3(camera.view *vec4(position, 1.0));
  vertexNormal = normal;
  vertexCameraNormal = normalize(mat3(camera.view)*normal);
  vertexLightCoord = light.biasProjView * vec4(position, 1.0);
  vertexLightDist = length(light.view * vec4(position, 1.0));
  lightDir = -light.dir;
}
