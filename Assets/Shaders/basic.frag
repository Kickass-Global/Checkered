#version 330

in vec3 vColour;
in vec3 fTexCoord;
in vec3 fPosition;

out vec3 colour;

vec3 checker(in float u, in float v)
{
  float checkSize = 200;
  float fmodResult = mod(floor(checkSize * u) + floor(checkSize * v), 2.0);
  float fin = max(sign(fmodResult), 0.0);
  return vec3(fin, fin, fin);
}

void main() {
    colour = vColour * checker(fTexCoord.x, fTexCoord.y);
}


