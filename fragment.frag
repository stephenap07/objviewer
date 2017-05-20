#version 150 core

in vec3 Color; 
in vec2 Texcoord; 
in vec2 Texcoord2;
out vec4 outColor; 
out vec4 outColor2; 

uniform sampler2D texKitten; 
uniform sampler2D texPuppy; 
uniform float time;

void main ()
{
  if (time > 0.0)
  {
    float factor = (sin(Texcoord.y * 60 + time * 2.0) + 1.0) / 30.0;
    vec4 waterColor = vec4(1.0, 1.0, 1.0, 1.0);
    outColor = mix(texture(texKitten, Texcoord), texture(texPuppy, Texcoord), 0.5);
    outColor = outColor * waterColor * texture(texKitten, vec2(factor + Texcoord.x, 1.0 - Texcoord.y));
  }
  else
  {
    outColor = mix(texture(texKitten, Texcoord), texture(texPuppy, Texcoord), 0.5);
  }
} 
