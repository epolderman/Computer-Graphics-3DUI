#version 430

in layout(location=0) vec3 vertexPositioninModel;
in layout(location=1) vec2 textCoords;
in layout(location=2) vec3 vertexNormalinModel;

uniform mat4 fullTRPMatrix;
uniform mat4 modeltoWorldMatrix;
uniform vec4 clipPlane;
//uniform mat4 LightSpaceMatrix;

out vec3 vertexNormal;
out vec2 textureCoordinates;
out vec3 vertexPosition;
out vec3 thePositionWorld;
//out vec4 PositionLightSpace;

void main()
{
	thePositionWorld = vec3(modeltoWorldMatrix * vec4(vertexPositioninModel,1.0));
	//gl_ClipDistance[0] = dot(vec4(thePositionWorld,1.0), clipPlane);
	gl_Position = fullTRPMatrix * vec4(vertexPositioninModel, 1.0f);
	textureCoordinates = textCoords;
	vertexNormal = vec3(modeltoWorldMatrix * vec4(vertexNormalinModel, 0));
	//PositionLightSpace = LightSpaceMatrix * vec4(thePositionWorld, 1.0);
}