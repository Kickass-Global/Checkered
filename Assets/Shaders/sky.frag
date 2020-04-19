#version 430

layout (location = 1) uniform sampler2D tDiffuse;

in vec3 fTexcoord;
out vec3 colour;

void main() {
    vec3 diffuse = texture(tDiffuse, fTexcoord.st).xyz;
    const float gamma = 2.0;
  
    // reinhard tone mapping
    vec3 mapped = diffuse / (diffuse + vec3(1.0));
    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
    colour = mapped;
}