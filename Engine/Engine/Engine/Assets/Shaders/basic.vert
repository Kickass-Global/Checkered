#version 330

layout (location = 0) in vec3 vPosition;

uniform mat4 View = mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);

void main() {
    gl_Position = View * vec4(vPosition, 1);
}
