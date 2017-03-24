#version 430


in vec2 textureCoords;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;

out vec4 theFinalColor;


void main()
{	
	//vec4 reflection = texture(reflectionTexture, textureCoords);
	//vec4 refraction = texture(refractionTexture, textureCoords);
	//theFinalColor = mix(reflection, refraction, 0.5f);
	theFinalColor = vec4(0.0,0.0,1.0,1.0);
}