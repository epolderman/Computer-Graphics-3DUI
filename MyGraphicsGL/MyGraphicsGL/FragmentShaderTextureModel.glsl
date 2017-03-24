#version 430

in vec2 texCoords;
in vec3 theNormalWorld;
in vec3 thePositionWorld;


uniform sampler2D texture_diffuse1;

uniform vec3 eyePositionWorld;
uniform vec3 lightPositionWorld;
uniform vec4 ambientLightWorld;

out vec4 theFinalColor;

void main()
{	

	vec3 normal = normalize(theNormalWorld);
	vec3 color = texture(texture_diffuse1, texCoords).rgb;

	//diffuse + ambient
	vec3 lightVectorWorld = normalize(lightPositionWorld - thePositionWorld);
	float bright = dot(lightVectorWorld, normal);
	vec3 diffusedLight = vec3(bright,bright,bright);
	vec3 ambientLight = vec3(ambientLightWorld);
	
	//specular
	vec3 reflectedlightvectorWorld = reflect(-lightVectorWorld, normal);
	vec3 eyeVectorWorld = normalize(eyePositionWorld - thePositionWorld);
	float specs = clamp(dot(reflectedlightvectorWorld, eyeVectorWorld),0,1);
	vec3 specularLight = vec3(specs, specs, specs);

	theFinalColor = (vec4(ambientLight,1.0) + vec4(diffusedLight, 1.0) + vec4(specularLight, 1.0)) * vec4(color,1.0);

}
