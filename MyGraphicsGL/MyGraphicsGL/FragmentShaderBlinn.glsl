#version 430

in vec3 theNormalWorld;
in vec3 thePositionWorld;
in vec3 theAmbientMaterial;
in vec3 theDiffuseMaterial;
in vec3 theSpecularMaterial;
in vec3 theShininessMaterial;
in vec4 PositionLightSpace;

uniform vec3 eyePositionWorld;
uniform vec3 lightPositionWorld;
uniform vec4 ambientLightWorld;

out vec4 theFinalColor;

uniform sampler2D shadowMap;

float ShadowCalculation(vec4 PositionLightSpace)
{
    // perform perspective divide
    vec3 projCoords = PositionLightSpace.xyz / PositionLightSpace.w;

    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 

    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

	vec3 normal = normalize(theNormalWorld);
    vec3 lightDir = normalize(lightPositionWorld - thePositionWorld);

	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.015);

	float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}  

void main()
{	

	vec3 normal = normalize(theNormalWorld);
	
	//ambient
	vec4 newAmbient = vec4(theAmbientMaterial, 1.0);
	vec4 finalAmbient = newAmbient * ambientLightWorld;
	
	
	//diffuse
	vec3 lightVectorWorld = normalize(lightPositionWorld - thePositionWorld);
	float bright = dot(lightVectorWorld, normal);
	vec3 brightMaterial = theDiffuseMaterial * bright;
	vec4 diffusedLight = vec4(brightMaterial, 1.0);

	//specular
	vec3 reflectedlightvectorWorld = reflect(-lightVectorWorld, normal);
	vec3 eyeVectorWorld = normalize(eyePositionWorld - thePositionWorld);

	//blinn
	vec3 halfwayDir = normalize(lightVectorWorld + eyeVectorWorld);
	float specs = 0.0f;
	float shininessValue = theShininessMaterial.x * 128;
	specs = pow(max(dot(normal, halfwayDir), 0.0), shininessValue);
	vec3 newSpec = theSpecularMaterial * specs;
	vec4 specularLight = vec4(newSpec, 1.0);

	//shadows
	float shadow = ShadowCalculation(PositionLightSpace);
	shadow = min(shadow, 0.5);
	theFinalColor = (finalAmbient + (1.0 - shadow) * diffusedLight + specularLight);

	
	//theFinalColor = clamp(diffusedLight,0,1) + 
	//finalAmbient + clamp(specularLight, 0, 1);

	//theFinalColor = lighting;

}
