#version 330

uniform vec3 kA = vec3(0.2, 0.2, 0.2);
uniform vec3 kD = vec3(0.6, 0.6, 0.6);
uniform vec3 kS = vec3(0.6, 0.6, 0.6);

in vec3 fColour;
in vec3 fNormal;
in vec3 fLightDirection;
in vec3 fViewer;
in vec3 fLightReflection;

out vec3 colour;


void main() {
    colour = kA + fColour * max(dot(fLightDirection, fNormal), 0.0);// + kS * pow(dot(fLightReflection, fViewer), 4);
}


