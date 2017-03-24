#version 430
in layout(location=0) vec3 vertexPositioninModel;
in layout(location=1) vec3 surfaceNormal;
in layout(location=2) vec2 textCoords;

out vec3 theNormalWorld;
out vec3 thePositionWorld;
out vec2 texCoords;

uniform mat4 fullTRPMatrix;
uniform mat4 modeltoWorldMatrix;


void main()
{
	thePositionWorld = vec3(modeltoWorldMatrix * vec4(vertexPositioninModel,1.0));
    gl_Position = fullTRPMatrix * vec4(vertexPositioninModel,1.0);
	theNormalWorld = vec3(modeltoWorldMatrix * vec4(surfaceNormal, 0));
	texCoords = textCoords;
}