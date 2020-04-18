#version 430

uniform vec3 kA = vec3(0.5, 0.5, 0.5);
uniform vec3 kD = vec3(0.8, 0.8, 0.8);
uniform vec3 kS = vec3(0.6, 0.6, 0.6);

layout (location = 0) uniform sampler2D tShadow;
layout (location = 1) uniform sampler2D tDiffuse;

in vec3 fPosition;
in vec3 fTexcoord;
in vec3 fColour;
in vec3 fNormal;
in vec3 fLightDirection;
in vec3 fViewer;
in vec3 fLightReflection;
in vec3 fLightSpacePosition;

out vec3 colour;

float ShadowCalculation(vec4 fragPosLightSpace)
{

    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(tShadow, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
	float bias =  max(0.005 * (1.0 - dot(fNormal, fLightDirection)), 0.005);

	float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(tShadow, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(tShadow, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? min(1.0, 1.0 - abs(currentDepth - bias - pcfDepth)) : 0.0;
        }
    }
    shadow /= 9.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;
	return shadow;
}

void main() {
    vec3 diffuse = texture(tDiffuse, fTexcoord.st).xyz;

    const float gamma = 2.0;
  
    // reinhard tone mapping
    vec3 mapped = diffuse / (diffuse + vec3(1.0));
    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));

    float shadow = ShadowCalculation(vec4(fLightSpacePosition,1));

    colour = kA * (1.0 - shadow) * mapped + mapped * max(dot(fLightDirection, fNormal), 0.0);
    //+ diffuse * pow(dot(fLightReflection, fViewer), 4);
}


