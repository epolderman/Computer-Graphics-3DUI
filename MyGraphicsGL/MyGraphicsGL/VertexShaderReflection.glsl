#version 430
in layout(location=0) vec3 position;
in layout(location=2) vec3 normal;


out vec3 theNormalWorld;
out vec3 thePositionWorld;

uniform mat4 fullTRPMatrix;
uniform mat4 modeltoWorldMatrix;

void main()
{

	gl_Position = fullTRPMatrix * vec4(position, 1.0);  
	theNormalWorld = mat3(transpose(inverse(modeltoWorldMatrix))) * normal; 
	thePositionWorld = vec3(modeltoWorldMatrix * vec4(position,1.0f));

}