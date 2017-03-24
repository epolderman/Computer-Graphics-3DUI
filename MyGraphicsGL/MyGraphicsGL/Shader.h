#ifndef SHADER_H
#define SHADER_H
#include <string>
#include <GL/glew.h>
#include <iostream>
#include <fstream>
using std::ifstream;
using std::istreambuf_iterator;
using std::cout;
using std::endl;
using std::string;

class Shader
{
public:
	Shader(){}
	GLuint programID;
	void ShaderInit(const char *vertexShader, 
					const char *fragmentShader);
	void Use();
	Shader(const Shader& shader) {}
private:
	bool shaderErrorCompilerLog(GLuint shaderID);
	bool shaderProgramLinkingLog(GLuint programID);
	string readShaderCode(const char* filename);
	void operator=(const Shader& shader) {}
	
};

#endif