#version 330 core
layout (location = 0) in vec3 aPos;

layout (location = 4) in mat4 iInstanceMatrix;

uniform mat4 M_View = mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
uniform mat4 M_Perspective = mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);

void main()
{
    gl_Position = M_Perspective * M_View * iInstanceMatrix * vec4(aPos, 1.0);
}  