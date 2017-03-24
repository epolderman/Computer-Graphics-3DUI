#version 430

//empty fragment shader

void main()
{
	gl_FragDepth = gl_FragCoord.z;
}