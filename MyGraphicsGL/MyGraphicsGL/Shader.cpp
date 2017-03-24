#include "Shader.h"



void Shader::Use()
{
	glUseProgram(programID);
}

void Shader::ShaderInit(const char *vertexShader, const char *fragmentShader)
{
	//creates shader object(we get a handle..vertexShaderID(pointer basically)
	//Shader* myShader = new Shader(GL_VERTEX_SHADER);
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	//c_str() <--- give me the C style string
	const char* adapter[1];
	string temp = readShaderCode(vertexShader);
	adapter[0] = temp.c_str();
	//setter function for my vertex/fragment shader object
	//myshader->setShaderSouce(vertexShaderCode)
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode(fragmentShader);
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	//myShader->compile()
	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	//check if shader compiled or not(check log status)
	if (!shaderErrorCompilerLog(vertexShaderID) || !shaderErrorCompilerLog(fragmentShaderID))
		return;

	//create a program
	//Program* myProgram = new program();
	programID = glCreateProgram();

	//attach the shader
	//myProgram->attachShader(myShader);
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);

	//myProgram->link();
	glLinkProgram(programID);

	//check if program linked
	if (!shaderProgramLinkingLog(programID))
		return;

	//we dont need shader objects anymore because they have been linked in
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	//myProgram->use();
	glUseProgram(programID);
}

bool Shader::shaderErrorCompilerLog(GLuint shaderID)
{

	//getting property on our shader property
	GLint compileStatus;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus != GL_TRUE)
	{
		GLint infoLogLength;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		glGetShaderInfoLog(shaderID, infoLogLength, &bufferSize, buffer);
		cout << buffer << endl;

		delete[] buffer;
		return false;
	}

	return true;

}

bool Shader::shaderProgramLinkingLog(GLuint programID)
{
	//getting property on our program property
	GLint linkStatus;
	glGetProgramiv(programID, GL_LINK_STATUS, &linkStatus);
	if (linkStatus != GL_TRUE)
	{
		GLint infoLogLength;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		glGetProgramInfoLog(programID, infoLogLength, &bufferSize, buffer);
		cout << buffer << endl;
		cout << "Error in Linking" << endl;
		delete[] buffer;
		return false;
	}

	return true;

}

string Shader::readShaderCode(const char* filename)
{
	ifstream myInput(filename);
	if (!myInput.good())
	{
		cout << "File Read Error" << endl;
		exit(1);
	}
	//iterates the through the whole file and returns in one big string
	return string(istreambuf_iterator<char>(myInput),
		istreambuf_iterator<char>());

	myInput.close();
}
