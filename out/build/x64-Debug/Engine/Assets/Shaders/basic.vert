#version 330

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vTexCoord;

layout (location = 4) in mat4 iInstanceMatrix;

uniform mat4 M_View = mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
uniform mat4 M_Perspective = mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);

uniform vec3 kA = vec3(0.2,0.2,0.2);
uniform vec3 kD = vec3(0.6, 0.9, 0.34);
uniform vec3 kS = vec3(0.6, 0.9, 0.34);

uniform vec3 light0 = vec3(1.0, 1.0, 1.0);

out vec3 debug;
out vec3 vColour;

void main() {

    vec3 n = normalize(vNormal);
    vec3 light0_direction = normalize(vPosition - light0);
    vec3 reflection = reflect(light0_direction, n);
    vec3 viewer = normalize(vec3(inverse(M_View)[3]));

    vColour = kA + vNormal;// * dot(light0_direction, n) + kS * pow(dot(reflection, viewer), 32);

    gl_Position = M_Perspective * iInstanceMatrix * M_View * vec4(vPosition, 1);
}
