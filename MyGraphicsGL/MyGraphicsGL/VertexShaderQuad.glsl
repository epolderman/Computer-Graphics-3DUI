#version 430 


in layout(location=0) vec3 position;
in layout(location=1) vec2 texCoords;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(position,1.0f); 
    TexCoords = texCoords;
}  