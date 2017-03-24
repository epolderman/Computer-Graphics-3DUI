#version 430

in vec3 theNormalWorld;
in vec3 thePositionWorld;

out vec4 theColor;

uniform samplerCube skybox;
uniform vec3 cameraPos;

void main()
{	
	
	vec3 I = normalize(thePositionWorld - cameraPos);
	vec3 R = reflect(I, normalize(theNormalWorld));
	theColor = texture(skybox, R);
}