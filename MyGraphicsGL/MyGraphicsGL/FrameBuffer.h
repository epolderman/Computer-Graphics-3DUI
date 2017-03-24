#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H
#include <GL/glew.h>

class FrameBuffer
{

public:
	FrameBuffer()
	{
	}
	~FrameBuffer();
	void initializeReflectionBuffer();
	void unbindCurrentFrameBuffer();
	void bindReflectionFrameBuffer();
	void bindRefractionFrameBuffer();
	void initializeRefractionBuffer();
	void WaterFrameBuffer();
	GLuint reflectionTexture;
	GLuint refractionTexture;
private:
	//myGL display;
	const static unsigned int REFLECTION_WIDTH = 1280;
	const static unsigned int REFLECTION_HEIGHT = 720;
	GLuint reflectionFrameBuffer;
	GLuint reflectionDepthBuffer;
	GLuint createFrameBuffer();
	GLuint createTextureAttachement(unsigned int width, unsigned int height);
	GLuint createDepthBufferAttachement(unsigned int width, unsigned int height);
	void bindFrameBuffer(unsigned int FrameBuffer, unsigned int width, unsigned int height);


	const static unsigned int REFRACTION_WIDTH = 1280;
	const static unsigned int REFRACTION_HEIGHT = 720;
	GLuint refractionFrameBuffer;
	GLuint refractionDepthTexture;
	GLuint createDepthTextureAttachement(unsigned int width, unsigned int height);
	
};

#endif