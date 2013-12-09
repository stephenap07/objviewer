#version 150 core

in vec3 Color; 
in vec2 Texcoord; 
in vec2 Texcoord2;
out vec4 outColor; 
out vec4 outColor2; 

uniform sampler2D texKitten; 
uniform sampler2D texPuppy; 
uniform float time;

void main() { 
  if (Texcoord.y < 0.5)
    outColor = texture(texKitten, Texcoord);
  else {
    float factor = (sin(Texcoord.y * 60 + time * 2.0) + 1.0) / 30.0;
    vec4 waterColor = vec4(0.5, 0.5, 1.0, 1.0);
    outColor =  waterColor * texture(texKitten, vec2(factor + Texcoord.x, 1.0 - Texcoord.y));
  }
} 
