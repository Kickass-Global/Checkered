#version 330

layout (location = 0) in vec3 vPosition;
layout (location = 2) in vec3 vTexCoord;

layout (location = 4) in mat4 iInstanceMatrix;

uniform mat4 M_View = mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);

out vec3 fTexCoord;

void main() {
    fTexCoord = vTexCoord;
    gl_Position = iInstanceMatrix * M_View * vec4(vPosition, 1);
}
