#version 430
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec2 TexCoords;
out vec3 theNormalWorld;
out vec3 thePositionWorld;
out vec4 PositionLightSpace;

uniform mat4 fullTRPMatrix;
uniform mat4 modeltoWorldMatrix;
uniform mat4 LightSpaceMatrix;


void main()
{
    gl_Position = fullTRPMatrix * vec4(position, 1.0f);
	thePositionWorld = vec3(modeltoWorldMatrix * vec4(position,1.0));
	theNormalWorld = vec3(modeltoWorldMatrix * vec4(normal, 0));
    TexCoords = texCoords;
	PositionLightSpace = LightSpaceMatrix * vec4(thePositionWorld, 1.0);
}