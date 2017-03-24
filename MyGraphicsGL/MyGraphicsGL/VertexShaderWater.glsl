#version 430

in layout(location=0) vec3 vertexPositioninModel;

uniform mat4 fullTRPMatrix;

out vec2 textureCoords;

void main()
{
	gl_Position = fullTRPMatrix * vec4(vertexPositioninModel,1.0);
	//textureCoords = vec2(vertexPositioninModel.x/2.0 + 0.5, vertexPositioninModel.z/2.0 + 0.5);
	
}