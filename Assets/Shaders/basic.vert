#version 330

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vTexCoord;

layout (location = 4) in mat4 iInstanceMatrix;

uniform mat4 M_View = mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
uniform mat4 M_Perspective = mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);

uniform vec3 kA = vec3(0.2, 0.2, 0.2);
uniform vec3 kD = vec3(0.6, 0.6, 0.6);
uniform vec3 kS = vec3(0.6, 0.6, 0.6);

uniform vec4 light0 = vec4(100.0, 100.0, 100.0, 1.0);

out vec3 fTexcoord;
out vec3 fColour;
out vec3 fNormal;
out vec3 fViewer;
out vec3 fLightDirection;
out vec3 fLightReflection;

void main() {

    vec4 v = iInstanceMatrix * vec4(vPosition,1.0);

    fTexcoord = vTexCoord;
    fNormal = normalize(mat3(iInstanceMatrix) * vNormal);
    fLightDirection = normalize( light0.xyz - v.xyz );
    fLightReflection = reflect(fLightDirection, fNormal);
    fViewer = normalize(vec3(inverse(M_View)[3]));
    fColour = vec3(1,0,0);

    gl_Position = M_Perspective * M_View * iInstanceMatrix * vec4(vPosition,1.0);

}


