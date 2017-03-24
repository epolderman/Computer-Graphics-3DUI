#version 430
in layout(location=0) vec3 position;

uniform mat4 LightSpaceMatrix;
uniform mat4 fullTRPMatrix;

void main()
{
	gl_Position = LightSpaceMatrix * fullTRPMatrix * vec4(position, 1.0f);
}