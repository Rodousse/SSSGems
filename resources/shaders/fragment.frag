#version 400

in vec3 vertexNormal;
in vec3 lightDir;
in vec3 vertexCameraNormal;
in vec3 camDir;
in vec4 vertexLightCoord;
in float vertexLightDist;

out vec4 frag_colour;

uniform sampler2D texDepth;
uniform sampler2D texShadow;


void main() {
  float bias = 0.0005 ;
  float visibility = 1.0f;
  visibility = mix(0.0,1.0 , ceil(texture(texShadow, vertexLightCoord.xy ).z - vertexLightCoord.z + bias));

  float hitShadowDist = texture(texDepth, vertexLightCoord.xy).x;
  float scatDistance = vertexLightDist - hitShadowDist;
  float scatPower = exp(-scatDistance * 10);


  vec4 albedo = vec4(0.7);// vec4(fragTexCoord, 1.0 ,1.0);

  vec3 N = normalize(vertexNormal);
  vec3 L = normalize(lightDir);
  vec3 V = normalize(camDir);
  vec3 R = reflect(L,N);
  float alpha = 100.0;

  vec3 diff = max(dot(N,L), 0.0) * albedo.xyz;
  frag_colour = vec4(scatPower);// * vec4(diff + (albedo * 0.05).xyz, 1.0);//vec4((fragNormal + vec3(1.0))/2.0, 1.0);//
}
