#version 150 core

in vec3 position; 
in vec3 normal;
in vec3 diffuse; 
in vec2 texcoord;

out vec3 Color; 
out vec3 Normal;
out vec2 Texcoord; 
out float mixValue;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  float normalFactor = 0.8;
  float diffuseFactor = 1.0 - normalFactor;

  Color = vec3(normal.x * normalFactor + diffuse.x * diffuseFactor,
               normal.y * normalFactor + diffuse.y * diffuseFactor,
	             normal.z * normalFactor + diffuse.z * diffuseFactor);
  Texcoord = texcoord; 
  Normal = normal;

  gl_Position = projection * view * model * vec4(position, 1.0); 
}