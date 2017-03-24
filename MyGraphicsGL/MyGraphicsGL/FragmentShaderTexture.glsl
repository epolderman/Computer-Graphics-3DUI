#version 430

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 textureCoordinates;
in vec3 thePositionWorld;


uniform sampler2D ourTexture;

uniform vec3 eyePositionWorld;
uniform vec3 lightPositionWorld;
uniform vec4 ambientLightWorld;

out vec4 theFinalColor;


void main()
{	
	vec3 colorTexture = texture(ourTexture, textureCoordinates).rgb;

	//diffuse
	vec3 lightVectorWorld = normalize(lightPositionWorld - thePositionWorld);
	float bright = dot(lightVectorWorld, normalize(vertexNormal));
	vec4 diffusedLight = vec4(bright,bright,bright, 1.0);

	//specular
	vec3 reflectedlightvectorWorld = reflect(-lightVectorWorld, normalize(vertexNormal));
	vec3 eyeVectorWorld = normalize(eyePositionWorld - thePositionWorld);
	float specs = clamp(dot(reflectedlightvectorWorld, eyeVectorWorld),0,1);
	specs = pow(specs, 20);
	vec4 specularLight = vec4(specs,specs,specs, 1.0);

	//diffuse + specular + ambient
	vec4 finalLight = (ambientLightWorld + diffusedLight + specularLight) * texture2D(ourTexture, textureCoordinates);

	//shadows
	//float shadow = ShadowCalculation(PositionLightSpace);
	//shadow = min(shadow, 0.5);
	//theFinalColor = (ambientLightWorld + (1.0 - shadow) * (diffusedLight + specularLight)) * vec4(colorTexture,1.0);

	//theFinalColor = finalLight;
	theFinalColor = texture2D(ourTexture, textureCoordinates);
}