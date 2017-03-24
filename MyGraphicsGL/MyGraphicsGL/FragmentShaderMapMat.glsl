#version 430

in vec3 theColorWorld;
in vec3 theNormalWorld;
in vec3 thePositionWorld;
in vec3 theAmbientMaterial;
in vec3 theDiffuseMaterial;
in vec3 theSpecularMaterial;
in vec3 theShininessMaterial;

uniform vec3 eyePositionWorld;
uniform vec3 lightPositionWorld;
uniform vec4 ambientLightWorld;

out vec4 theFinalColor;


void main()
{	
	//ambient
	vec4 newAmbient = vec4(theAmbientMaterial, 1.0);
	vec4 finalAmbient = newAmbient * ambientLightWorld;

	//diffuse
	vec3 lightVectorWorld = normalize(lightPositionWorld - thePositionWorld);
	float bright = dot(lightVectorWorld, normalize(theNormalWorld));
	vec3 brightMaterial = theDiffuseMaterial * bright;
	vec4 diffusedLight = vec4(brightMaterial, 1.0);

	//specular
	vec3 reflectedlightvectorWorld = reflect(-lightVectorWorld, normalize(theNormalWorld));
	vec3 eyeVectorWorld = normalize(eyePositionWorld - thePositionWorld);
	float specs = clamp(dot(reflectedlightvectorWorld, eyeVectorWorld),0,1);
	float shininessValue = theShininessMaterial.x * 128;
	specs = pow(specs, shininessValue);
	vec3 newSpec = theSpecularMaterial * specs;
	vec4 specularLight = vec4(newSpec, 1.0);
	theFinalColor = clamp(diffusedLight,0,1) + 
	finalAmbient + clamp(specularLight, 0, 1);

}