#version 430

in vec3 TextCoords;

out vec4 theColor;

uniform samplerCube skybox;


void main()
{	
	theColor = texture(skybox, TextCoords);
}