#version 430

in layout(location=0) vec3 vertexPositioninModel;
in layout(location=1) vec3 surfaceNormalinModel;


uniform mat4 fullTRPMatrix;
uniform mat4 modeltoWorldMatrix;
uniform mat4 LightSpaceMatrix;

out vec3 theColorWorld;
out vec3 theNormalWorld;
out vec3 thePositionWorld;
out vec4 PositionLightSpace;

void main()
{
	thePositionWorld = vec3(modeltoWorldMatrix * vec4(vertexPositioninModel,1.0));
	gl_Position = fullTRPMatrix * vec4(vertexPositioninModel,1.0);
	theNormalWorld = vec3(modeltoWorldMatrix * vec4(surfaceNormalinModel, 0));
	PositionLightSpace = LightSpaceMatrix * vec4(thePositionWorld, 1.0);
}