#ifndef SKYBOX_H
#define SKYBOX_H
#include <GL/glew.h>
#include <string>
#include <vector>
using std::vector;


class SkyBox
{
public:
	SkyBox();
	void Init();
	void Draw();
	void bindTexture();
	~SkyBox();
private:
	GLuint skyboxVAO;
	GLuint loadCubemap(vector<const GLchar*> faces);
	GLuint cubemapTexture;
	GLuint skyboxVBO;
	vector<const GLchar*> faces;
	GLuint LoadTexture(const std::string& filename);
};
#endif
