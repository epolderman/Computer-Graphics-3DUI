#ifndef TEXTURE_H
#define TEXTURE_H
#include <GL/glew.h>
#include <string>



class Texture
{
public:
	Texture(const std::string& filename);
	void Bind();
	void InitializeTexture(const std::string& filename);
	Texture(){}
	GLuint the_texture;
	virtual ~Texture();
private:
	Texture(const Texture& texture){}
	void operator=(const Texture& texture){}
};



#endif
