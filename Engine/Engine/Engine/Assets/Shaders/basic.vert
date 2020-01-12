#version 330

layout (location = 0) in vec3 vPosition;

uniform mat4 M_View = mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
uniform mat4 M_Perspective = mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);

void main() {
    gl_Position = M_Perspective * M_View * vec4(vPosition, 1);
}
