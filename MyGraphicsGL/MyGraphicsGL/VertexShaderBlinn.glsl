#version 430
in layout(location=0) vec4 vertexPositioninModel;
in layout(location=1) vec3 vertexColorinModel;
in layout(location=2) vec3 surfaceNormalinModel;
in layout(location=3) vec3 ambientMaterial;
in layout(location=4) vec3 diffuseMaterial;
in layout(location=5) vec3 specularMaterial;
in layout(location=6) vec3 shininessMaterial;


uniform mat4 fullTRPMatrix;
uniform mat4 modeltoWorldMatrix;
uniform mat4 LightSpaceMatrix;


out vec3 theNormalWorld;
out vec3 thePositionWorld;
out vec3 theAmbientMaterial;
out vec3 theDiffuseMaterial;
out vec3 theSpecularMaterial;
out vec3 theShininessMaterial;
out vec4 PositionLightSpace;


void main()
{
	thePositionWorld = vec3(modeltoWorldMatrix * vertexPositioninModel);
    gl_Position = fullTRPMatrix * vertexPositioninModel;
	theNormalWorld = vec3(modeltoWorldMatrix * vec4(surfaceNormalinModel, 0));
	theAmbientMaterial = ambientMaterial;
	theDiffuseMaterial = diffuseMaterial;
	theSpecularMaterial = specularMaterial;
	theShininessMaterial = shininessMaterial;
	PositionLightSpace = LightSpaceMatrix * vec4(thePositionWorld, 1.0);
}