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
  float normalFactor = 0.2;
  float diffuseFactor = 1.0 - normalFactor;

  Normal = normalize(normal);

  Color = vec3(Normal.x * normalFactor + diffuse.x * diffuseFactor,
               Normal.y * normalFactor + diffuse.y * diffuseFactor,
	             Normal.z * normalFactor + diffuse.z * diffuseFactor);

  Color = normalize(Color);

	Color = vec3(Color.x * 0.5 + 0.5, Color.y * 0.5 + 0.5, Color.z * 0.5 + 0.5);

  Texcoord = texcoord; 

  gl_Position = projection * view * model * vec4(position, 1.0); 
}
