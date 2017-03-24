#ifndef MY_GL_H
#define MY_GL_H
#include "Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include <QtOpenGL\qglwidget>
#include <string>
#include "Camera.h"
#include "SkyBox.h"
#include "FrameBuffer.h"

using glm::mat4;
using std::string;
class ModelData;
class myGL : public QGLWidget
{
private:
	GLuint vertexBufferID;
	GLuint programID;
	GLuint cubeVAOID;
	GLuint planeVAOID;
	GLuint sphereVAOID;
	GLuint bigcubeVAOID;
	void sendDataOpenGLMaterials();
	GLenum err;
	const GLuint SHADOW_WIDTH = 1024;
	const GLuint SHADOW_HEIGHT = 1024;
	GLuint depthMap;
	GLuint depthMapFBO;

	Shader blinnShader;
	Shader depthShader;
	Shader quadShader;
	Shader modelShader;
	Shader reflectionShader;
	Shader textureShader;
	Shader materialShader;
	Shader waterShader;

	void sendUniformData(GLuint shaderID);
	Camera theCamera;
	ModelData* myModel;
	Mesh Water;
	FrameBuffer WaterBuffer;
	void sendClipPlane(GLuint shaderID);
	void sendClipPlane2(GLuint shaderID);

	
	//blown up planet
	Model planetModel;


	//space rock rendering
	Model iceModel;
	Shader iceShader;

	//working
	Model spaceRock;
	Model iceRock;
	void renderRock(GLuint programID);
	Shader materialDefined;
	void renderRockCluster(GLuint programID);

	//primitive rendering
	Mesh Sphere;
	Mesh Cube;
	Mesh Plane;
	void renderShadingSceneDepth(GLuint programID);
	void renderShaderingScene(GLuint programID);
	void renderReflectionCube();

	//cryis complex mesh
	Model crysisModel;
	void renderModel(GLuint programID);

	//buffer for shadows
	void InitDepthMapBuffer();
	void rendertoDepthBuffer(mat4 &lightSpaceMatrix, GLint& LightSpaceUniform);

	//skybox
	SkyBox theSky;
	Shader skyboxShader;
	void renderSkyBox();

	//debug to texture
	GLuint quadVAO;
	GLuint quadVBO;
	void InitDebugQuad();
	void renderDebugQuad();
protected:
	void paintGL();
	void initializeGL();
	void mouseMoveEvent(QMouseEvent*);
	void keyPressEvent(QKeyEvent*);
public:
	~myGL();
	myGL(ModelData *theModel);
	float getWidth(){return this->width();}
	float getHeight(){return this->height();}
};

#endif