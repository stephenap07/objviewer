#version 150 core

in vec2 position; 
in vec3 color; 
in vec2 texcoord;
out vec3 Color; 
out vec2 Texcoord; 
out float mixValue;

uniform mat4 model;
uniform mat4 scale;
uniform mat4 view;
uniform mat4 projection;

void main() { 
  Color = color; 
  gl_Position = projection * view * model * scale  * vec4(position, 0.0, 1.0); 
  Texcoord = texcoord; 
}
