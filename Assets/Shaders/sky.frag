#version 430

layout (location = 1) uniform sampler2D tDiffuse;

in vec3 fTexcoord;
out vec3 colour;

void main() {
    vec3 diffuse = texture(tDiffuse, fTexcoord.st).xyz;
    colour = diffuse;
}