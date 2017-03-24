#version 430
in layout(location=0) vec3 position;


out vec3 TextCoords;

uniform mat4 fullTRPMatrix;


void main()
{
	vec4 pos = fullTRPMatrix * vec4(position, 1.0);
	gl_Position = pos.xyww;
    TextCoords = position;
}