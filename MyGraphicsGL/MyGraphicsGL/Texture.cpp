#include "Texture.h"
#include <iostream>
#include <SOIL.h>

Texture::Texture(const std::string& filename)
{

	int width, height, numComps;

	unsigned char* image = SOIL_load_image((filename).c_str(), &width, &height, &numComps, SOIL_LOAD_RGB);

	if (image == NULL)
		std::cerr << "Unable to load texture: " << filename << std::endl;

	glGenTextures(1, &the_texture);
	glBindTexture(GL_TEXTURE_2D, the_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	if (0 == the_texture)
	{
		printf("SOIL loading error: '%s'\n", SOIL_last_result());
	}

	SOIL_free_image_data(image);

}

void Texture::InitializeTexture(const std::string& filename)
{
	int width, height, numComps;

	unsigned char* image = SOIL_load_image((filename).c_str(), &width, &height, &numComps, SOIL_LOAD_RGB);

	if (image == NULL)
		std::cerr << "Unable to load texture: " << filename << std::endl;

	glGenTextures(1, &the_texture);
	glBindTexture(GL_TEXTURE_2D, the_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	if (0 == the_texture)
	{
		printf("SOIL loading error: '%s'\n", SOIL_last_result());
	}

	SOIL_free_image_data(image);
}

Texture::~Texture()
{
	glDeleteTextures(1, &the_texture);
}

void Texture::Bind()
{
	glBindTexture(GL_TEXTURE_2D, the_texture);
}
