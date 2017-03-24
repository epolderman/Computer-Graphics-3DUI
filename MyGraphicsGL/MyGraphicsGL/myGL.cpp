#include <gl\glew.h>
#include "myGL.h"
#include <glm\gtc\matrix_transform.hpp>
#include <iostream>
#include <glm\gtx\transform.hpp>
#include "ShapeGenerator.h"
#include <QtGui\QMouseEvent>
#include <QtGui\\qkeyevent>
#include "ModelData.h"
#include <vector>
using std::cout;
using std::endl;
using glm::uint;
using glm::mat4;
using glm::vec3;
using glm::vec4;
using std::vector;

namespace
{
	const uint NUM_VERTICES_PER_TRI = 3;
	const uint NUM_FLOAT_PER_VERTICE = 21;
	const uint VERTEX_BYTE_SIZE = NUM_FLOAT_PER_VERTICE * sizeof(float);
	const uint VERTEX_BYTE_SIZE_TEXTURE = 9 * sizeof(float);
	GLuint cubenumIndices;
	GLuint planenumIndices;
	GLuint spherenumIndices;
	GLuint bigcubebumIndices;
	GLuint fullTransformUniformLocation;
	GLuint planeIndexDataOffset;
	GLuint cubeIndexDataOffset;
	GLuint sphereIndexDataOffset;
	GLuint bigcubeIndexDataOffset;
}

myGL::myGL(ModelData* theModel) :QGLWidget(QGLFormat(QGL::SampleBuffers))
{
	myModel = theModel;
	setMinimumSize(1200, 800);
}

void myGL::initializeGL()
{
	setMouseTracking(false);

	glewInit();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//glEnable(GL_CLIP_DISTANCE0);

	//glEnable(GL_CULL_FACE);

	glEnable(GL_MULTISAMPLE);

	iceShader.ShaderInit("VertexShaderTextureModel.glsl", "FragmentShaderTextureModel.glsl");
	materialDefined.ShaderInit("VertexShaderSnow.glsl", "FragmentShaderSnow.glsl");
	blinnShader.ShaderInit("VertexShaderBlinn.glsl", "FragmentShaderBlinn.glsl");
	quadShader.ShaderInit("VertexShaderQuad.glsl", "FragmentShaderQuad.glsl");
	depthShader.ShaderInit("VertexShaderMappingShader.glsl", "FragmentShaderMappingShader.glsl");
	modelShader.ShaderInit("VertexShaderModel.glsl", "FragmentShaderModel.glsl");
	reflectionShader.ShaderInit("VertexShaderReflection.glsl", "FragmentShaderReflection.glsl");
	materialShader.ShaderInit("VertexShaderMaterial.glsl", "FragmentShaderMaterial.glsl");
	textureShader.ShaderInit("VertexShaderTexture.glsl", "FragmentShaderTexture.glsl");
	skyboxShader.ShaderInit("VertexShaderSky.glsl", "FragmentShaderSky.glsl");
	

	theSky.Init();

	Material pearl;
	pearl.Ambient = vec3(0.25f, 0.20725f, 0.20725f);
	pearl.Diffuse = vec3(0.75f, 0.829f, 0.829f);
	pearl.Specular = vec3(0.296648f, 0.296648f, 0.296648f);
	pearl.Shininess = vec3(0.088f, 0.0f, 0.0f);

	Material gold;
	gold.Ambient = vec3(0.24725, 0.1995, 0.0745);
	gold.Diffuse = vec3(0.75164, 0.60648, 0.22648);
	gold.Specular = vec3(0.628281, 0.555802, 0.366065);
	gold.Shininess = vec3(0.4, 0.0f, 0.0f);
	ShapeData sphere = ShapeGenerator::makeSphereWithMaterial(75, pearl);
	Sphere.InitializeWithShapeData(sphere);

	Material obsidian;
	obsidian.Ambient = vec3(0.05375f, 0.05f, 0.06625f);
	obsidian.Diffuse = vec3(0.18275f, 0.17f, 0.22525f);
	obsidian.Specular = vec3(0.332741f, 0.328634f, 0.346435f);
	obsidian.Shininess = vec3(0.3f, 0.0f, 0.0f);
	ShapeData cube = ShapeGenerator::makeCubeWithMaterial(obsidian);
	Cube.InitializeWithShapeData(cube);

	ShapeData plane = ShapeGenerator::makePlaneWithMaterial(100, pearl);
	Plane.InitializeWithShapeData(plane);

	//ShapeData waterObject = ShapeGenerator::makeWater();
	//Water.InitializeWaterPlane(waterObject);

	crysisModel.initializeModel("nanosuit.obj");
	iceModel.initializeModel("ice3.obj");
	spaceRock.initializeModel("sr.obj");
	iceRock.initializeModel("beb.obj");
	planetModel.initializeModel("erik.obj");

	//for shadow mapping generate a depth buffer with texture
	InitDepthMapBuffer();

	//debugging depth
	InitDebugQuad();
	
	
}

void myGL::paintGL()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width(), height());

	//Light Transform in our World
	GLfloat near_plane = 1.0f;
	GLfloat far_plane = 70.0f;
	vec3 lightPositionWorld = myModel->LightPosition;
	mat4 lightProjection, lightView;
	mat4 lightSpaceMatrix;
	GLint LightSpaceUniform;
	//left, right(clipping planes), bottom, top(clipping planes), nearVal, farVal
	lightProjection = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, near_plane, far_plane);
	//position of eye point, specifices the position of the reference point, up vector
	lightView = glm::lookAt(lightPositionWorld, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	lightSpaceMatrix = lightProjection * lightView;

	//depth buffer shadows
	rendertoDepthBuffer(lightSpaceMatrix, LightSpaceUniform);

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width(), height());

	//render scene normally
	blinnShader.Use();
	sendUniformData(blinnShader.programID);
	LightSpaceUniform = glGetUniformLocation(blinnShader.programID, "LightSpaceMatrix");
	glUniformMatrix4fv(LightSpaceUniform, 1, GL_FALSE, &lightSpaceMatrix[0][0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	renderShaderingScene(blinnShader.programID);

	//crysis model
	modelShader.Use();
	sendUniformData(modelShader.programID);
	LightSpaceUniform = glGetUniformLocation(modelShader.programID, "LightSpaceMatrix");
	glUniformMatrix4fv(LightSpaceUniform, 1, GL_FALSE, &lightSpaceMatrix[0][0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	renderModel(modelShader.programID);
	glBindTexture(GL_TEXTURE_2D, 0);

	//floating rock
	materialDefined.Use();
	sendUniformData(materialDefined.programID);
	LightSpaceUniform = glGetUniformLocation(materialDefined.programID, "LightSpaceMatrix");
	glUniformMatrix4fv(LightSpaceUniform, 1, GL_FALSE, &lightSpaceMatrix[0][0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	renderRock(materialDefined.programID);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	//reflection cube
	reflectionShader.Use();
	GLint cameraPos = glGetUniformLocation(reflectionShader.programID, "cameraPos");
	vec3 camera = theCamera.getCameraPosition();
	glUniform3f(cameraPos, camera.x, camera.y, camera.z);
	sendUniformData(reflectionShader.programID);
	theSky.bindTexture();
	renderReflectionCube();
	glBindTexture(GL_TEXTURE_2D, 0);

	//explding planet
	iceShader.Use();
	sendUniformData(iceShader.programID);
	renderRockCluster(iceShader.programID);

	//skybox
	renderSkyBox();

	while ((err = glGetError()) != GL_NO_ERROR)
	{
		cout << "Error Code in PaintGL " << err << endl;
	}

}

void myGL::renderShadingSceneDepth(GLuint programID)
{
	
	mat4 fullTransform;
	GLint fullTransformUniformLocation;
	fullTransformUniformLocation = glGetUniformLocation(programID, "fullTRPMatrix");
	//cout << "Full Transform Location Depth: " << fullTransformUniformLocation << endl;

	//crysis model
	mat4 CrysisModelToWorldMatrix = glm::translate(vec3(0.0f, 0.5f, -10.0f));
	fullTransform = CrysisModelToWorldMatrix;
	glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
	crysisModel.Draw(programID);

	////////sphere
	//mat4 sphere2ModelToWorldMatrix = glm::translate(vec3(10.0f, 4.0f, 10.0f)) * glm::scale(2.75f, 2.75f, 2.75f);
	//fullTransform = sphere2ModelToWorldMatrix;
	//glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
	//Sphere.drawShape();

	//////cube1
	//mat4 cubeModelMatrix = glm::translate(vec3(0.0f, 4.5f, 0.0f)) * glm::scale(1.5f, 1.5f, 1.5f);
	//fullTransform = cubeModelMatrix;
	//glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
	//Cube.drawShape();

	//////cube2
	//mat4 cubeModelMatrix2 = glm::translate(vec3(10.0f, 4.0f, -5.0f)) * glm::scale(3.5f, 3.5f, 3.5f);
	//fullTransform = cubeModelMatrix2;
	//glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
	//Cube.drawShape();

	//////cube3
	//mat4 cubeModelMatrix3 = glm::translate(vec3(-10.0f, 4.0f, 2.0f)) * glm::rotate(60.0f, vec3(1.0f, 0.0f, 0.0f)) * glm::scale(2.0f, 2.0f, 2.0f);
	//fullTransform = cubeModelMatrix3;
	//glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
	//Cube.drawShape();

	////reflection cube
	//mat4 cubeModelMatrixReflection = glm::translate(vec3(-3.0f, 1.5f, 6.0f)) * glm::scale(2.0f, 2.0f, 2.0f);
	//fullTransform = cubeModelMatrixReflection;
	//glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
	//Cube.drawShape();

	////plane
	//mat4 planeModelToWorldMatrix;
	//fullTransform = planeModelToWorldMatrix;
	//glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
	//Plane.drawShape();

	//space rock
	mat4 FloatingIslandModelToWorldMatrix = glm::translate(vec3(0.0f, -19.25f, 0.0f)) * glm::scale(30.0f, 30.0f, 30.0f);
	fullTransform = FloatingIslandModelToWorldMatrix;
	glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
	iceModel.Draw(programID);
	
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		cout << "Error Code Material Renderer " << err << endl;
	}
	
}

void myGL::renderShaderingScene(GLuint programID)
{
	mat4 fullTransform;
	mat4 worldtoviewMatrix;
	GLint fullTransformUniformLocation;
	GLint modeltoworldTransformMatrixLocation;
	mat4 ViewtoProjectMatrix = glm::perspective(60.0f, ((float)width()) / height(), 0.1f, 1000.0f);
	fullTransformUniformLocation = glGetUniformLocation(programID, "fullTRPMatrix");
	modeltoworldTransformMatrixLocation = glGetUniformLocation(programID, "modeltoWorldMatrix");
	worldtoviewMatrix = theCamera.getWorldtoViewMatrix();

	//////sphere
	//mat4 sphere2ModelToWorldMatrix = glm::translate(vec3(10.0f, 4.0f, 10.0f)) * glm::scale(2.75f, 2.75f, 2.75f);
	//fullTransform = ViewtoProjectMatrix * worldtoviewMatrix * sphere2ModelToWorldMatrix;
	//glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
	//glUniformMatrix4fv(modeltoworldTransformMatrixLocation, 1, GL_FALSE, &sphere2ModelToWorldMatrix[0][0]);
	//Sphere.drawShape();

	////cube1
	//mat4 cubeModelMatrix = glm::translate(vec3(0.0f, 4.0f, 0.0f)) * glm::scale(1.5f, 1.5f, 1.5f);
	//fullTransform = ViewtoProjectMatrix * worldtoviewMatrix * cubeModelMatrix;
	//glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
	//glUniformMatrix4fv(modeltoworldTransformMatrixLocation, 1, GL_FALSE, &cubeModelMatrix[0][0]);
	//Cube.drawShape();

	////cube2
	//mat4 cubeModelMatrix2 = glm::translate(vec3(10.0f, 4.0f, 5.0f)) * glm::scale(3.5f, 3.5f, 3.5f);
	//fullTransform = ViewtoProjectMatrix * worldtoviewMatrix * cubeModelMatrix2;
	//glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
	//glUniformMatrix4fv(modeltoworldTransformMatrixLocation, 1, GL_FALSE, &cubeModelMatrix2[0][0]);
	//Cube.drawShape();

	////cube3
	//mat4 cubeModelMatrix3 = glm::translate(vec3(-10.0f, 4.0f, 2.0f)) * glm::rotate(60.0f, vec3(1.0f, 0.0f, 0.0f)) * glm::scale(2.0f, 2.0f, 2.0f);
	//fullTransform = ViewtoProjectMatrix * worldtoviewMatrix * cubeModelMatrix3;
	//glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
	//glUniformMatrix4fv(modeltoworldTransformMatrixLocation, 1, GL_FALSE, &cubeModelMatrix3[0][0]);
	//Cube.drawShape();

	////plane
	//mat4 planeModelToWorldMatrix;
	//fullTransform = ViewtoProjectMatrix * worldtoviewMatrix * planeModelToWorldMatrix;
	//glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
	//glUniformMatrix4fv(modeltoworldTransformMatrixLocation, 1, GL_FALSE, &planeModelToWorldMatrix[0][0]);
	//Plane.drawShape();

	while ((err = glGetError()) != GL_NO_ERROR)
	{
		cout << "Error Code Material Renderer Regular " << err << endl;
	}
}

void myGL::renderReflectionCube()
{
	mat4 fullTransform;
	mat4 worldtoviewMatrix;
	GLint fullTransformUniformLocation;
	GLint modeltoworldTransformMatrixLocation;
	mat4 ViewtoProjectMatrix = glm::perspective(60.0f, ((float)width()) / height(), 0.1f, 1000.0f);
	fullTransformUniformLocation = glGetUniformLocation(reflectionShader.programID, "fullTRPMatrix");
	modeltoworldTransformMatrixLocation = glGetUniformLocation(reflectionShader.programID, "modeltoWorldMatrix");
	worldtoviewMatrix = theCamera.getWorldtoViewMatrix();

	//reflection cube
	mat4 cubeModelMatrixReflection = glm::translate(vec3(-3.0f, 20.0f, 6.0f)) * glm::scale(2.0f, 2.0f, 2.0f);
	fullTransform = ViewtoProjectMatrix * worldtoviewMatrix * cubeModelMatrixReflection;
	glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
	glUniformMatrix4fv(modeltoworldTransformMatrixLocation, 1, GL_FALSE, &cubeModelMatrixReflection[0][0]);
	Cube.drawShape();
}

void myGL::renderModel(GLuint programID)
{
	mat4 fullTransform;
	mat4 worldtoviewMatrix;
	GLint fullTransformUniformLocation;
	GLint modeltoworldTransformMatrixLocation;
	mat4 ViewtoProjectMatrix = glm::perspective(60.0f, ((float)width()) / height(), 0.1f, 1000.0f);
	fullTransformUniformLocation = glGetUniformLocation(programID, "fullTRPMatrix");
	modeltoworldTransformMatrixLocation = glGetUniformLocation(programID, "modeltoWorldMatrix");
	worldtoviewMatrix = theCamera.getWorldtoViewMatrix();


	////crysis model
	mat4 CrysisModelToWorldMatrix = glm::translate(vec3(0.0f, 0.5f, -10.0f));
	fullTransform = ViewtoProjectMatrix * worldtoviewMatrix * CrysisModelToWorldMatrix;
	glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
	glUniformMatrix4fv(modeltoworldTransformMatrixLocation, 1, GL_FALSE, &CrysisModelToWorldMatrix[0][0]);
	crysisModel.Draw(programID);


	while ((err = glGetError()) != GL_NO_ERROR)
	{
		cout << "Error Code in renderModel " << err << endl;
	}

}

void myGL::renderRock(GLuint programID)
{
	mat4 fullTransform;
	mat4 worldtoviewMatrix;
	GLint fullTransformUniformLocation;
	GLint modeltoworldTransformMatrixLocation;
	mat4 ViewtoProjectMatrix = glm::perspective(60.0f, ((float)width()) / height(), 0.1f, 1000.0f);
	fullTransformUniformLocation = glGetUniformLocation(programID, "fullTRPMatrix");
	modeltoworldTransformMatrixLocation = glGetUniformLocation(programID, "modeltoWorldMatrix");
	worldtoviewMatrix = theCamera.getWorldtoViewMatrix();

	////floating rock
	mat4 FloatingIslandModelToWorldMatrix = glm::translate(vec3(0.0f, -19.25f, 0.0f)) * glm::scale(30.0f, 30.0f, 30.0f);
	fullTransform = ViewtoProjectMatrix * worldtoviewMatrix * FloatingIslandModelToWorldMatrix;
	glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
	glUniformMatrix4fv(modeltoworldTransformMatrixLocation, 1, GL_FALSE, &FloatingIslandModelToWorldMatrix[0][0]);
	iceModel.Draw(programID);

	while ((err = glGetError()) != GL_NO_ERROR)
	{
		cout << "Error Code in renderRock " << err << endl;
	}

}

void myGL::renderRockCluster(GLuint programID)
{

	mat4 fullTransform;
	mat4 worldtoviewMatrix;
	GLint fullTransformUniformLocation;
	GLint modeltoworldTransformMatrixLocation;
	mat4 ViewtoProjectMatrix = glm::perspective(60.0f, ((float)width()) / height(), 0.1f, 1000.0f);
	fullTransformUniformLocation = glGetUniformLocation(programID, "fullTRPMatrix");
	modeltoworldTransformMatrixLocation = glGetUniformLocation(programID, "modeltoWorldMatrix");
	worldtoviewMatrix = theCamera.getWorldtoViewMatrix();

	////exploding planet
	mat4 planetRock = glm::translate(vec3(0.0f, 100.0f, 700.0f)) * glm::scale(2.0f, 2.0f, 2.0f);
	fullTransform = ViewtoProjectMatrix * worldtoviewMatrix * planetRock;
	glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
	glUniformMatrix4fv(modeltoworldTransformMatrixLocation, 1, GL_FALSE, &planetRock[0][0]);
	planetModel.Draw(programID);

}

void myGL::renderSkyBox()
{
	mat4 fullTransform;
	mat4 worldtoviewMatrix;
	GLint fullTransformUniformLocation;
	mat4 ViewtoProjectMatrix;

	//skybox
	glDepthFunc(GL_LEQUAL);
	skyboxShader.Use();
	worldtoviewMatrix = glm::mat4(glm::mat3(theCamera.getWorldtoViewMatrix()));
	ViewtoProjectMatrix = glm::perspective(60.0f, ((float)width()) / height(), 0.1f, 1000.0f);
	fullTransform = ViewtoProjectMatrix * worldtoviewMatrix;
	fullTransformUniformLocation = glGetUniformLocation(skyboxShader.programID, "fullTRPMatrix");
	glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
	theSky.Draw();
	glDepthFunc(GL_LESS);
	glBindVertexArray(0);

	while ((err = glGetError()) != GL_NO_ERROR)
	{
		cout << "Error Code in SkyBox " << err << endl;
	}

}

void myGL::InitDepthMapBuffer()
{
	//for shadow mapping generate a depth map 
	glGenFramebuffers(1, &depthMapFBO);

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		cout << "ERROR in frame buffer Initialization" << endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void myGL::renderDebugQuad()
{
	quadShader.Use();
	glViewport(0, 0, 300, 300);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void myGL::InitDebugQuad()
{
	///quad
	GLfloat quadVertices[] =
	{

		-1.0f, 1.0f, 0.0f, // Positions 
		0.0f, 1.0f,	 // Texture Coords
		-1.0f, -1.0f, 0.0f,  // Positions 
		0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,  // Positions 
		1.0f, 1.0f,
		1.0f, -1.0f, 0.0f,  // Positions 
		1.0f, 0.0f,
	};
	// Setup plane VAO
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

}

void myGL::mouseMoveEvent(QMouseEvent*e)
{
	setFocus();
	theCamera.theMouseUpdate(glm::vec2(e->x(), e->y()));
	repaint();
}

void myGL::keyPressEvent(QKeyEvent*e)
{
	switch (e->key())
	{
	case Qt::Key::Key_Up:
		theCamera.moveForward();
		break;
	case Qt::Key::Key_Down:
		theCamera.moveBackward();
		break;
	case Qt::Key::Key_Left:
		theCamera.moveLeft();
		break;
	case Qt::Key::Key_Right:
		theCamera.moveRight();
		break;
	case Qt::Key::Key_A:
		theCamera.moveUp();
		break;
	case Qt::Key::Key_S:
		theCamera.moveDown();
		break;
	}
	repaint();
}

void myGL::sendDataOpenGLMaterials()
{
	//materials
	Material obsidian;
	obsidian.Ambient = vec3(0.05375f, 0.05f, 0.06625f);
	obsidian.Diffuse = vec3(0.18275f, 0.17f, 0.22525f);
	obsidian.Specular = vec3(0.332741f, 0.328634f, 0.346435f);
	obsidian.Shininess = vec3(0.3f, 0.0f, 0.0f);

	Material cyanRubber;
	cyanRubber.Ambient = vec3(0.0f, 0.05f, 0.05f);
	cyanRubber.Diffuse = vec3(0.4f, 0.5f, 0.5f);
	cyanRubber.Specular = vec3(0.04f, 0.7f, 0.7f);
	cyanRubber.Shininess = vec3(.078125f, 0.0f, 0.0f);

	Material silver;
	silver.Ambient = vec3(0.19225f, 0.19225f, 0.19225f);
	silver.Diffuse = vec3(0.50754f, 0.50754f, 0.50754f);
	silver.Specular = vec3(0.508273f, 0.508273f, 0.508273f);
	silver.Shininess = vec3(0.4f, 0.0f, 0.0f);

	Material gold;
	gold.Ambient = vec3(0.24725, 0.1995, 0.0745);
	gold.Diffuse = vec3(0.75164, 0.60648, 0.22648);
	gold.Specular = vec3(0.628281, 0.555802, 0.366065);
	gold.Shininess = vec3(0.4, 0.0f, 0.0f);

	Material pearl;
	pearl.Ambient = vec3(0.25f, 0.20725f, 0.20725f);
	pearl.Diffuse = vec3(0.75f, 0.829f, 0.829f);
	pearl.Specular = vec3(0.296648f, 0.296648f, 0.296648f);
	pearl.Shininess = vec3(0.088f, 0.0f, 0.0f);

	Material redpearl;
	redpearl.Ambient = vec3(0.25f, 0.20725f, 0.20725f);
	redpearl.Diffuse = vec3(1.0f, 0.829f, 0.829f);
	redpearl.Specular = vec3(0.296648f, 0.296648f, 0.296648f);
	redpearl.Shininess = vec3(0.088f, 0.0f, 0.0f);

	Material blackRubber;
	blackRubber.Ambient = vec3(0.02f, 0.02f, 0.02f);
	blackRubber.Diffuse = vec3(0.01f, 0.01f, 0.01f);
	blackRubber.Specular = vec3(0.4f, 0.4f, 0.4f);
	blackRubber.Shininess = vec3(.078125f, 0.0f, 0.0f);

	Material ruby;
	ruby.Ambient = vec3(0.1745f, 0.01175f, 0.01175f);
	ruby.Diffuse = vec3(0.61424f, 0.04136f, 0.04136f);
	ruby.Specular = vec3(0.727811f, 0.626959f, 0.626959f);
	ruby.Shininess = vec3(0.6, 0.0f, 0.0f);

	//vertex and index data in one buffer
	ShapeData cube = ShapeGenerator::makeCubeWithMaterial(ruby);
	ShapeData bigCube = ShapeGenerator::makeCubeWithMaterial(obsidian);
	ShapeData plane = ShapeGenerator::makePlaneWithMaterial(40, pearl);
	ShapeData sphere = ShapeGenerator::makeSphereWithMaterial(75, redpearl);

	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, cube.vertexBufferSize() + cube.indexBufferSize() +
		bigCube.vertexBufferSize() + bigCube.indexBufferSize() +
		plane.vertexBufferSize() + plane.indexBufferSize()
		+ sphere.vertexBufferSize() + sphere.indexBufferSize(),
		0, GL_STATIC_DRAW);

	//GLsizeiptr(just an int)
	GLsizeiptr currentOffset = 0;
	//cube
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, cube.vertexBufferSize(), cube.vertices);
	currentOffset += cube.vertexBufferSize();
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, cube.indexBufferSize(), cube.indices);
	//big cube
	currentOffset += cube.indexBufferSize();
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, bigCube.vertexBufferSize(), bigCube.vertices);
	currentOffset += bigCube.vertexBufferSize();
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, bigCube.indexBufferSize(), bigCube.indices);
	//plane
	currentOffset += bigCube.indexBufferSize();
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, plane.vertexBufferSize(), plane.vertices);
	currentOffset += plane.vertexBufferSize();
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, plane.indexBufferSize(), plane.indices);
	//sphere
	currentOffset += plane.indexBufferSize();
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, sphere.vertexBufferSize(), sphere.vertices);
	currentOffset += sphere.vertexBufferSize();
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, sphere.indexBufferSize(), sphere.indices);

	cubenumIndices = cube.numIndices;
	planenumIndices = plane.numIndices;
	spherenumIndices = sphere.numIndices;
	bigcubebumIndices = bigCube.numIndices;

	//vaos	
	//does not create object
	//glGenVertexArrays(1, &cubeVAOID);
	glGenVertexArrays(1, &planeVAOID);
	glGenVertexArrays(1, &cubeVAOID);
	glGenVertexArrays(1, &sphereVAOID);
	glGenVertexArrays(1, &bigcubeVAOID);

	//then the VAO object is created
	//state of the calls will be saved in this object
	//cube
	glBindVertexArray(cubeVAOID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 6));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 9));
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 12));
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 15));
	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 18));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBufferID);

	//big cube
	glBindVertexArray(bigcubeVAOID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	GLuint bigcubeByteOffset = cube.vertexBufferSize() + cube.indexBufferSize();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)bigcubeByteOffset);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(bigcubeByteOffset + sizeof(float) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(bigcubeByteOffset + sizeof(float) * 6));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(bigcubeByteOffset + sizeof(float) * 9));
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(bigcubeByteOffset + sizeof(float) * 12));
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(bigcubeByteOffset + sizeof(float) * 15));
	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(bigcubeByteOffset + sizeof(float) * 18));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBufferID);

	//state saves in this object
	//plane
	glBindVertexArray(planeVAOID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	GLuint planeByteOffset = bigcubeByteOffset + bigCube.vertexBufferSize() + bigCube.indexBufferSize();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)planeByteOffset);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(planeByteOffset + sizeof(float) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(planeByteOffset + sizeof(float) * 6));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(planeByteOffset + sizeof(float) * 9));
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(planeByteOffset + sizeof(float) * 12));
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(planeByteOffset + sizeof(float) * 15));
	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(planeByteOffset + sizeof(float) * 18));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBufferID);

	//sphere
	//state saves in this object
	//plane
	glBindVertexArray(sphereVAOID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	GLuint sphereByteOffset = planeByteOffset + plane.vertexBufferSize() + plane.indexBufferSize();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)sphereByteOffset);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sphereByteOffset + sizeof(float) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sphereByteOffset + sizeof(float) * 6));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sphereByteOffset + sizeof(float) * 9));
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sphereByteOffset + sizeof(float) * 12));
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sphereByteOffset + sizeof(float) * 15));
	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sphereByteOffset + sizeof(float) * 18));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBufferID);


	cubeIndexDataOffset = cube.vertexBufferSize();
	bigcubeIndexDataOffset = bigcubeByteOffset + bigCube.vertexBufferSize();
	planeIndexDataOffset = planeByteOffset + plane.vertexBufferSize();
	sphereIndexDataOffset = sphereByteOffset + sphere.vertexBufferSize();


	plane.CleanUp();
	cube.CleanUp();
	sphere.CleanUp();
	bigCube.CleanUp();
}

void myGL::rendertoDepthBuffer(mat4 &lightSpaceMatrix, GLint& LightSpaceUniform)
{

	LightSpaceUniform = glGetUniformLocation(depthShader.programID, "LightSpaceMatrix");
	depthShader.Use(); ///<---- HAVE TO BIND SHADER BEFORE GETTING UNIFORM LOCATION
	glUniformMatrix4fv(LightSpaceUniform, 1, GL_FALSE, &lightSpaceMatrix[0][0]);
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	renderShadingSceneDepth(depthShader.programID);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	while ((err = glGetError()) != GL_NO_ERROR)
	{
		cout << "Error Code in Depth Buffer Render " << err << endl;
	}

}
//refraction
void myGL::sendUniformData(GLuint shaderID)
{
	//ambientlight
	vec4 ambientLight(0.3f, 0.3f, 0.3f, 1.0f);
	GLint ambientLightUniformLocation =
		glGetUniformLocation(shaderID, "ambientLightWorld");
	glUniform4fv(ambientLightUniformLocation, 1, &ambientLight[0]);

	//eye
	glm::vec3 eyePosition = theCamera.getCameraPosition();
	GLint eyeUniformLocation =
		glGetUniformLocation(shaderID, "eyePositionWorld");
	glUniform3fv(eyeUniformLocation, 1, &eyePosition[0]);

	//light in world
	vec3 lightPositionWorld = myModel->LightPosition;
	GLint LightUniformLocation =
		glGetUniformLocation(shaderID, "lightPositionWorld");
	glUniform3fv(LightUniformLocation, 1, &lightPositionWorld[0]);

	//water clip plane
	vec4 clipPlane(0.0f, -1.0f, 0.0f, 0.1f);
	GLint clipPlaneLocation = glGetUniformLocation(shaderID, "clipPlane");
	glUniform4fv(clipPlaneLocation, 1, &clipPlane[0]);

}
//for reflection
void myGL::sendClipPlane(GLuint shaderID)
{
	//ambientlight
	vec4 ambientLight(0.5f, 0.5f, 0.5f, 1.0f);
	GLint ambientLightUniformLocation =
		glGetUniformLocation(shaderID, "ambientLightWorld");
	glUniform4fv(ambientLightUniformLocation, 1, &ambientLight[0]);

	//eye
	glm::vec3 eyePosition = theCamera.getCameraPosition();
	GLint eyeUniformLocation =
		glGetUniformLocation(shaderID, "eyePositionWorld");
	glUniform3fv(eyeUniformLocation, 1, &eyePosition[0]);

	//light in world
	vec3 lightPositionWorld = myModel->LightPosition;
	GLint LightUniformLocation =
		glGetUniformLocation(shaderID, "lightPositionWorld");
	glUniform3fv(LightUniformLocation, 1, &lightPositionWorld[0]);

	//water clip plane
	vec4 clipPlane(0.0f, 1.0f, 0.0f, -0.1f);
	GLint clipPlaneLocation = glGetUniformLocation(shaderID, "clipPlane");
	glUniform4fv(clipPlaneLocation, 1, &clipPlane[0]);

}

void myGL::sendClipPlane2(GLuint shaderID)
{
	//ambientlight
	vec4 ambientLight(0.5f, 0.5f, 0.5f, 1.0f);
	GLint ambientLightUniformLocation =
		glGetUniformLocation(shaderID, "ambientLightWorld");
	glUniform4fv(ambientLightUniformLocation, 1, &ambientLight[0]);

	//eye
	glm::vec3 eyePosition = theCamera.getCameraPosition();
	GLint eyeUniformLocation =
		glGetUniformLocation(shaderID, "eyePositionWorld");
	glUniform3fv(eyeUniformLocation, 1, &eyePosition[0]);

	//light in world
	vec3 lightPositionWorld = myModel->LightPosition;
	GLint LightUniformLocation =
		glGetUniformLocation(shaderID, "lightPositionWorld");
	glUniform3fv(LightUniformLocation, 1, &lightPositionWorld[0]);

	//water clip plane
	vec4 clipPlane(0.0f, -1.0f, 0.0f, 10000000.00f);
	GLint clipPlaneLocation = glGetUniformLocation(shaderID, "clipPlane");
	glUniform4fv(clipPlaneLocation, 1, &clipPlane[0]);

}

myGL::~myGL()
{
	glDeleteBuffers(1, &vertexBufferID);
	glUseProgram(0);
	glDeleteProgram(programID);
}


//water tutorial
//glEnable(GL_CLIP_DISTANCE0);


/////////////REFRACTION//////////////////
//
//WaterBuffer.bindRefractionFrameBuffer();
////monkey
//textureShader.Use();
//worldtoviewMatrix = theCamera.getWorldtoViewMatrix();
//mat4 monkeyModelToWorldMatrix =
//	glm::translate(vec3(-100.0f, 0.0f, 0.0f)) *
//	glm::rotate(180.0f, vec3(0.0f, 1.0f, 0.0f)) *
//	glm::scale(7.0f, 7.0f, 7.0f);
//worldtoProjectionMatrix = ViewtoProjectMatrix * worldtoviewMatrix;
//fullTransform = worldtoProjectionMatrix * monkeyModelToWorldMatrix;
//fullTransformUniformLocation = glGetUniformLocation(textureShader.programID, "fullTRPMatrix");
//modeltoworldTransformMatrixLocation = glGetUniformLocation(textureShader.programID, "modeltoWorldMatrix");
//glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
//glUniformMatrix4fv(modeltoworldTransformMatrixLocation, 1, GL_FALSE, &monkeyModelToWorldMatrix[0][0]);
//sendUniformData(textureShader.programID);
//theText.Bind();
//MonkeyObject.draw();


//////////////////////REFLECTION
//
//WaterBuffer.bindReflectionFrameBuffer();
//textureShader.Use();
//float distance = 2 * theCamera.position.y + 0.1f;
//theCamera.position.y -= distance;
//worldtoviewMatrix = theCamera.getWorldtoViewMatrix();
//monkeyModelToWorldMatrix =
//	glm::translate(vec3(-100.0f, 0.0f, 0.0f)) *
//	glm::rotate(180.0f, vec3(0.0f, 1.0f, 0.0f)) *
//	glm::scale(7.0f, 7.0f, 7.0f);
//worldtoProjectionMatrix = ViewtoProjectMatrix * worldtoviewMatrix;
//fullTransform = worldtoProjectionMatrix * monkeyModelToWorldMatrix;
//fullTransformUniformLocation = glGetUniformLocation(textureShader.programID, "fullTRPMatrix");
//modeltoworldTransformMatrixLocation = glGetUniformLocation(textureShader.programID, "modeltoWorldMatrix");
//glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
//glUniformMatrix4fv(modeltoworldTransformMatrixLocation, 1, GL_FALSE, &monkeyModelToWorldMatrix[0][0]);
//sendClipPlane(textureShader.programID);
//theText.Bind();
//MonkeyObject.draw();
//theCamera.position.y += distance;

////REGULAR RENDER
//WaterBuffer.unbindCurrentFrameBuffer();
//glViewport(0, 0, width(), height());
//glDisable(GL_CLIP_DISTANCE0);

//textureShader.Use();
//worldtoviewMatrix = theCamera.getWorldtoViewMatrix();
//monkeyModelToWorldMatrix =
//	glm::translate(vec3(0.0f, 0.0f, 0.0f)) *
//	glm::rotate(180.0f, vec3(0.0f, 1.0f, 0.0f));
//worldtoProjectionMatrix = ViewtoProjectMatrix * worldtoviewMatrix;
//fullTransform = worldtoProjectionMatrix * monkeyModelToWorldMatrix;
//fullTransformUniformLocation = glGetUniformLocation(textureShader.programID, "fullTRPMatrix");
//modeltoworldTransformMatrixLocation = glGetUniformLocation(textureShader.programID, "modeltoWorldMatrix");
//glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
//glUniformMatrix4fv(modeltoworldTransformMatrixLocation, 1, GL_FALSE, &monkeyModelToWorldMatrix[0][0]);
//sendClipPlane2(textureShader.programID);
//theText.Bind();
//MonkeyObject.draw();
//glBindTexture(GL_TEXTURE_2D, NULL);
//glDisable(GL_TEXTURE_2D);
//glEnable(GL_TEXTURE_2D);

////water plane
//waterShader.Use();
//mat4 plane2ModelToWorldMatrix;
//fullTransform = worldtoProjectionMatrix * plane2ModelToWorldMatrix;
//fullTransformUniformLocation = glGetUniformLocation(waterShader.programID, "fullTRPMatrix");
//glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
//glUniformMatrix4fv(modeltoworldTransformMatrixLocation, 1, GL_FALSE, &plane2ModelToWorldMatrix[0][0]);
//sendClipPlane2(waterShader.programID);

//GLuint textureLocation;
//textureLocation = glGetUniformLocation(waterShader.programID, "reflectionTexture");
//cout << "Texture Reflection " << textureLocation << endl;
//glUniform1i(textureLocation, 1);

//GLuint refractionTexture;
//refractionTexture = glGetUniformLocation(waterShader.programID, "refractionTexture");
//cout << "Texture Reflraction " << refractionTexture << endl;
//glUniform1i(refractionTexture, 2);

//glActiveTexture(GL_TEXTURE1);
//glBindTexture(GL_TEXTURE_2D, WaterBuffer.reflectionTexture);
//glActiveTexture(GL_TEXTURE2);
//glBindTexture(GL_TEXTURE_2D,WaterBuffer.refractionTexture);
//Water.drawShape();


////light in world
//GLfloat near_plane = 1.0f;
//GLfloat far_plane = 50.0f;
//vec3 lightPositionWorld = myModel->LightPosition;
//mat4 lightProjection, lightView;
//mat4 lightSpaceMatrix;
//GLint LightSpaceUniform;
////1.render scene to depth buffer
////left, right(clipping planes), bottom, top(clipping planes), nearVal, farVal
//lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
////position of eye point, specifices the position of the reference point, up vector
//lightView = glm::lookAt(lightPositionWorld, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
//lightSpaceMatrix = lightProjection * lightView;
//LightSpaceUniform = glGetUniformLocation(depthShader.programID, "LightSpaceMatrix");
//depthShader.Use(); ///<---- HAVE TO BIND SHADER BEFORE GETTING UNIFORM LOCATION
//glUniformMatrix4fv(LightSpaceUniform, 1, GL_FALSE, &lightSpaceMatrix[0][0]);
//glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
//glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//glClear(GL_DEPTH_BUFFER_BIT);
//renderShadingSceneDepth(depthShader.programID);
//glBindFramebuffer(GL_FRAMEBUFFER, 0);








































//interface of openGL is written in C
//iv integer vector(an array of ints in openGl)

//Notes on how our RAM vertices work
//shapedata = shapegenerator(we are genererating array of vertices, indices(RAM) on our
//client side RAM
//send that data(buffer data calls) to put that data on the GPU hardware(RAM in graphics
//card.(then later we render it)(it is saved on the GPU)
//we then clean up the RAM and deletes it on our client side RAM

//instancing our data(1 geometry with different matrix transformations)(1 enemy -> different
//locations :)

//notes about vec4(graphics hardware tears through vector 4's
//if you sent vec3 down to the GPU it fills in the last value to 1.0
//which turns on Translation :)

//clamp(if angle > 90 degrees) we clamp it to zero(because it will be negative)
//if < 0 = 0 
//if > 1 = 1 
//if .5 leave it alone

//lookat(eye, lookat, up vector)
//eye where the camera is at, lookat(what point am i looking at?, the up vector(i can be laying on the ground)
//i can be looking from up high, etc..)

//NOTES*DEPTH BUFFER
//fragment is not a pixel until it passes (depth test)certain tests(depth buffer vs RGB value in corresponding 2D arrays)
//color buffer is a 2D array of RGB values which corresponds to the depth buffer of -1 to +1 (+1) is set standard
//to determine if a fragment becomes a pixel to be drawn it takes the depth test of the z value
//if the z value of the 2d color array at RGB which corresponds to the depth buffer is less than the
//pixel that is being colored right now, then it replaces it because it is closer to the screen in depth.

//videos 23-24 COLOR BUFFER ARRAY
//They are two different buffers regarding to color. The front and back buffers. We must clear
//the color buffer which can be an expensive operation but if we or it when we clear the depth buffer
//then it becomes less expensive glClear(color buffer | depth buffer);

//videos 25 MATRICES
//Model to World Transofrmation matrix (applied to each object) (rotation, scale, translate)
//World to view transformation matrix (applied to all objects after the Model to World transormation)
//moves the world around to the camera
//view to projection matrix (squishes into 2d scene but not perfect there are still some depth)

//video 26 
//Perspective Projection matrix
//glm::mat4 projectionMatrix = glm::perspective(field of view, aspect ratio, frustrum near, frustrum far);
//matrix 4x4 = (?,?,units from camera, far from camera) -> we smash all vertices intelligently
//doesnt matter if camera is pointing in negative direction or not

//video 27 28
//OpenGl is right handed coordinate system(we look down the -z)
//stays right handed until we do the perspective projection which flips to left hand cooridnate system
//reason for that is depth tests(openGL is better)
//Directx is the left handed coordinate eystem(we look down the +z)
//

//video 29
//once we smash we are in projection space (camera no long exists)
//hardware then takes the smashed x y coordinates and figurous out what pixels to fill
//the z depth test thing

//video 30 31
//setting up GLM math library and defined the vertex header file

//video 32 - 39(uniform variables are constant throughout and can be sent to the vertex shader
//glm::vec3 dominatingColor(0.0f, 1.0f, 0.5f);
////id or handle so it can link the dominatingColor on the stack to the GPU
//GLint dominatingColorUniformLocation = glGetUniformLocation(programID, "dominatingColor");
//glUniform3fv(dominatingColorUniformLocation,1, &dominatingColor[0]);

//Optimization(in paintGL)
/*Projection matrix does not take a matrix in the function for optimization.When
we past the projection matrix through the translate function it applies the
translation matrix first before the projection then we do the same for rotation
matrix as we are applying the rotation matrix first(you dont see the optimization)
before applying the TranslationProjection matrix.*/

//Matrix Multiplcation does matter(in paintGL)
//projection * translation * rotation
//mat4 fullTRPMatrix = projectMatrix* translationMatrix * rotationMatrix * (vertices which
//is really a vector);

//on video 40(field of view arg, aspect ratioa arg, near plane arg, far plane arg)
//mat4 projectMatrix = glm::perspective(60.0f, ((float)width()) / height(), 0.1f, 10.0f);

//field of view breakdown argument
//objects closer to the camera gets bigger(take up more surface area)
//further away objects get smaller. 
//FOV lines looks like a triangle(coming from the camera)
//but the perspective projection lines go parallel looking more 
//like a square rather than a trianlge field of view coming from the camera. 

//aspect ratio argument
//ratio screen width(2) / height(1) = 2
//Hey our screen is this much wider than it is tall, compensate for that
//COMPENSATE(Taller Screens(smaller the aspect ratio should be(bigger the height)
//wider screens(bigger aspect ration should be(bigger width)

//near plane and far plane argument
//openGl knows nothing about 3d space 
//everything in openGL is right handed(look down -z) until you go to 
//projected space(depth test goes to left hand space(-1z to +z)
//screen space coordinates are used to position elements in our world
//when we apply the perspective matrix(only the depth test decide where the
//objects are depth wise)
//objects in the near and far plane must be inside to be rendered 
// they are sent to the depth test before being drawn
//if an object fills the entire near plane and far plane then
//they will fill the whole 2d SCENE(what we see)(seeing inside a characters head GTA)

//z fighting
//*try to keep planes close together with your objects or you will have z-fight
//objects closer to the near place get more detail than objects further out
//objects further out get less depth(so z fighting is more common)
//objects closer get more depth range


//OPTMIZED ONE DRAW CALL
//optimized (perspective * translation * rotation) * each vertex
//mat4 projectMatrix = glm::perspective(60.0f, ((float)width()) / height(), 0.1f, 10.0f);
//mat4 translationMatrix = glm::translate(projectMatrix, vec3(0.0f, 0.0f, -3.0f));
//mat4 fullTRPMatrix = glm::rotate(translationMatrix, 54.0f, vec3(1.0f, 0.0f, 0.0f));
//GLint fullTRPuniform = glGetUniformLocation(programID, "fullTRPMatrix");
//
//function(GLint count(uniform location , count is 1 matrix, 
//boolean transpose(false), pointer to 16 floats(give me the first vector address))
//vector(is a fancy term for array in openGL, a glm::vec3 is a vector array of three floats [r|g|b]
//GLfloat* pointerTRP = reinterpret_cast<float*>(&fullTRPMatrix[0][0]);
//glUniformMatrix4fv(fullTRPuniform, 1, GL_FALSE, pointerTRP);
//
//mode, count, type(short, float), *indices(offset position)
//glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);


///WITHOUT INSTANCING DATA
//void myGL::paintGL()
//{
//	//must clear after every paint to the furthest most value -1z way in +1z way out
//	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//
//	glViewport(0, 0, width(), height());
//	//mode(lines points, trianles) first(start where?), count(how many vertices are we rendering?)
//	//glDrawArrays(GL_TRIANGLES, 0, 6);
//
//	//projection matrix is the same for all instances
//	mat4 projectMatrix = glm::perspective(60.0f, ((float)width()) / height(), 0.1f, 10.0f);
//	GLint fullTRPuniform = glGetUniformLocation(programID, "fullTRPMatrix");
//
//	//cube 1
//	//optimized (perspective * translation * rotation) * each vertex
//	mat4 translationMatrix = glm::translate(projectMatrix, vec3(-1.0f, 0.0f, -3.0f));
//	mat4 fullTRPMatrix = glm::rotate(translationMatrix, 36.0f, vec3(1.0f, 0.0f, 0.0f));
//
//	//function(GLint count(uniform location , count is 1 matrix, 
//	//boolean transpose(false), pointer to 16 floats(give me the first vector address))
//	//vector(is a fancy term for array in openGL, a glm::vec3 is a vector array of three floats [r|g|b]
//	GLfloat* pointerTRP = reinterpret_cast<float*>(&fullTRPMatrix[0][0]);
//	glUniformMatrix4fv(fullTRPuniform, 1, GL_FALSE, pointerTRP);
//
//	//mode, count, type(short, float), *indices(offset position)
//	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);
//
//
//	//cube 2(same geometry with different matrix transforms applied)
//	translationMatrix = glm::translate(projectMatrix, vec3(1.0f, 0.0f, -3.75f));
//	fullTRPMatrix = glm::rotate(translationMatrix, 126.0f, vec3(0.0f, 1.0f, 0.0f));
//	glUniformMatrix4fv(fullTRPuniform, 1, GL_FALSE, pointerTRP);
//	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);
//
//}

//INSTANCING MULTIPLE OBJECTS
//void myGL::sendDataOpenGL()
//{
//	ShapeData myVerts = ShapeGenerator::makeCube();
//
//	//Vertexes buffer
//	//create buffer object
//	//need buffer object to copy myVerts stack data to the graphics card
//	glGenBuffers(1, &vertexBufferID);
//
//	//bind
//	//array buffer is a binding point from our buffer object we generated in glgenBufers
//	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
//
//	//send data to graphics card
//	//creates array of bytes to store the data and send to the GPU
//	glBufferData(GL_ARRAY_BUFFER, myVerts.vertexBufferSize(), myVerts.vertices, GL_STATIC_DRAW);
//
//	//enable the data and the graphics pipeline
//	//first attribute of the index
//	glEnableVertexAttribArray(0);
//
//	//describe the data (where does data start, how many floats is it made of?, type, normalized?, 
//	//stride(beginning of an attrib - beggining of next, void * pointer(data type)(old school function)(what byte offset?))
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, 0);
//
//	//color attrib *remember the stride is from where we go from one color attribute to another color attribute*
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (char*)(sizeof(float) * 3));
//
//	//indexes buffer
//	glGenBuffers(1, &indexBufferID);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, myVerts.indexBufferSize(), myVerts.indices, GL_STATIC_DRAW);
//	numIndices = myVerts.numIndices;
//	myVerts.CleanUp();
//
//	//transformation buffer(applying transformation matrices to different instances of an object)
//	//this is not sent down per vertex (its sent down to the GPU per INSTANCE)
//	glGenBuffers(1, &transformMatrixID);
//	glBindBuffer(GL_ARRAY_BUFFER, transformMatrixID);
//
//	//data will be sent down frequently with 0 ->
//	glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * 2, 0, GL_STATIC_DRAW);
//	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 0));
//	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 4));
//	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 8));
//	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 12));
//	glEnableVertexAttribArray(2);
//	glEnableVertexAttribArray(3);
//	glEnableVertexAttribArray(4);
//	glEnableVertexAttribArray(5);
//	glVertexAttribDivisor(2, 1);
//	glVertexAttribDivisor(3, 1);
//	glVertexAttribDivisor(4, 1);
//	glVertexAttribDivisor(5, 1);
//
//}
//VERTEX SHADER
//#version 430
//
//in layout(location = 0) vec3 position;
//in layout(location = 1) vec3 vertexColor;
//in layout(location = 2) mat4 fullTRPMatrix;
//
//out vec3 theColor;
//
//void main()
//{
//	vec4 v = vec4(position, 1.0);
//	gl_Position = fullTRPMatrix * v;
//	theColor = vertexColor;
//}
//void myGL::paintGL()
//{
//
//	mat4 projectMatrix = glm::perspective(60.0f, ((float)width()) / height(), 0.1f, 10.0f);
//	mat4 fullTransforms[] =
//	{
//		projectMatrix * theCamera.getWorldtoViewMatrix() *
//		glm::translate(vec3(-1.0f, 0.0f, -3.0f)) *
//		glm::rotate(36.0f, vec3(1.0f, 0.0f, 0.0f)),
//
//		projectMatrix * theCamera.getWorldtoViewMatrix() *
//		glm::translate(vec3(1.0f, 0.0f, -3.75f)) *
//		glm::rotate(126.0f, vec3(0.0f, 1.0f, 0.0f)),
//	};
//	glBufferData(GL_ARRAY_BUFFER, sizeof(fullTransforms), fullTransforms, GL_DYNAMIC_DRAW);
//
//
//	must clear after every paint to the furthest most value -1z way in +1z way out
//	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//
//	glViewport(0, 0, width(), height());
//	mode(lines points, trianles) first(start where?), count(how many vertices are we rendering?)
//	glDrawArrays(GL_TRIANGLES, 0, 6);
//
//	mode, count, type(short, float), *indices(offset position)
//	glDrawElementsInstanced(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0, 2);
//
//
//}


//WITHOUT VAOS
//void myGL::sendDataOpenGL()
//{
//	ShapeData myVerts = ShapeGenerator::makeCube();
//
//	//Vertexes buffer
//	//create buffer object
//	//need buffer object to copy myVerts stack data to the graphics card
//	glGenBuffers(1, &cubeVertexBufferID);
//
//	//bind
//	//array buffer is a binding point from our buffer object we generated in glgenBufers
//	glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBufferID);
//
//	//send data to graphics card
//	//creates array of bytes to store the data and send to the GPU
//	glBufferData(GL_ARRAY_BUFFER, myVerts.vertexBufferSize(), myVerts.vertices, GL_STATIC_DRAW);
//
//	//enable the data and the graphics pipeline
//	//first attribute of the index
//	glEnableVertexAttribArray(0);
//
//	//describe the data (where does data start, how many floats is it made of?, type, normalized?, 
//	//stride(beginning of an attrib - beggining of next, void * pointer(data type)(old school function)(what byte offset?))
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, 0);
//
//	//color attrib *remember the stride is from where we go from one color attribute to another color attribute*
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (char*)(sizeof(float) * 3));
//
//	//indexes buffer
//	glGenBuffers(1, &cubeIndexBufferID);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIndexBufferID);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, myVerts.indexBufferSize(), myVerts.indices, GL_STATIC_DRAW);
//	cubenumIndices = myVerts.numIndices;
//	myVerts.CleanUp();
//
//	//////////////////////////////////////////////////
//
//	ShapeData myVerts = ShapeGenerator::makeplane();
//
//	//Vertexes buffer
//	//create buffer object
//	//need buffer object to copy myVerts stack data to the graphics card
//	glGenBuffers(1, &planeVertexBufferID);
//
//	//bind
//	//array buffer is a binding point from our buffer object we generated in glgenBufers
//	glBindBuffer(GL_ARRAY_BUFFER, planeVertexBufferID);
//
//	//send data to graphics card
//	//creates array of bytes to store the data and send to the GPU
//	glBufferData(GL_ARRAY_BUFFER, myVerts.vertexBufferSize(), myVerts.vertices, GL_STATIC_DRAW);
//
//	//enable the data and the graphics pipeline
//	//first attribute of the index
//	glEnableVertexAttribArray(0);
//
//	//describe the data (where does data start, how many floats is it made of?, type, normalized?, 
//	//stride(beginning of an attrib - beggining of next, void * pointer(data type)(old school function)(what byte offset?))
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, 0);
//
//	//color attrib *remember the stride is from where we go from one color attribute to another color attribute*
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (char*)(sizeof(float) * 3));
//
//	//indexes buffer
//	glGenBuffers(1, &planeIndexBufferID);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeIndexBufferID);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, myVerts.indexBufferSize(), myVerts.indices, GL_STATIC_DRAW);
//	planenumIndices = myVerts.numIndices;
//	myVerts.CleanUp();
//
//}




///TWOBUFFERS
//void myGL::sendDataOpenGL()
//{
//	//vertex data in one buffer!
//	ShapeData cube = ShapeGenerator::makeCube();
//	ShapeData plane = ShapeGenerator::makeplane();
//	glGenBuffers(1, &vertexBufferID);
//	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
//	glBufferData(GL_ARRAY_BUFFER, cube.vertexBufferSize() + plane.vertexBufferSize(), 0, GL_STATIC_DRAW);
//	//plane after the cube
//	glBufferSubData(GL_ARRAY_BUFFER, 0, cube.vertexBufferSize(), cube.vertices);
//	glBufferSubData(GL_ARRAY_BUFFER, cube.vertexBufferSize(), plane.vertexBufferSize(), plane.vertices);
//
//
//	//indexes data in one buffer
//	glGenBuffers(1, &indexBufferID);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube.indexBufferSize() + plane.indexBufferSize(), 0, GL_STATIC_DRAW);
//	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, cube.indexBufferSize(), cube.indices);
//	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, cube.indexBufferSize(), plane.indexBufferSize(), plane.indices);
//
//	cubenumIndices = cube.numIndices;
//	planenumIndices = plane.numIndices;
//
//	//vaos	
//
//	//does not create object
//	glGenVertexArrays(1, &cubeVAOID);
//	glGenVertexArrays(1, &planeVAOID);
//
//	//then the VAO object is created
//	//state of the calls will be saved in this object
//	glBindVertexArray(cubeVAOID);
//	glEnableVertexAttribArray(0);
//	glEnableVertexAttribArray(1);
//	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, 0);
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 3));
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
//
//	//state saves in this object
//	glBindVertexArray(planeVAOID);
//	glEnableVertexAttribArray(0);
//	glEnableVertexAttribArray(1);
//	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)cube.vertexBufferSize());
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(cube.vertexBufferSize() + sizeof(float) * 3));
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
//
//	planeIndexDataOffset = cube.indexBufferSize();
//
//	plane.CleanUp();
//	cube.CleanUp();
//}

//OLD PAINTGL
//void myGL::paintGL()
//{
//	//must clear after every paint to the furthest most value -1z way in +1z way out
//	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//
//	glViewport(0, 0, width(), height());
//	//mode(lines points, trianles) first(start where?), count(how many vertices are we rendering?)
//	//glDrawArrays(GL_TRIANGLES, 0, 6);
//
//	sendUniformData();
//
//	//perpective projection
//	mat4 fullTransform;
//	mat4 ViewtoProjectMatrix = glm::perspective(60.0f, ((float)width()) / height(), 0.1f, 90.0f);
//	mat4 worldtoviewMatrix = theCamera.getWorldtoViewMatrix();
//	mat4 worldtoProjectionMatrix = ViewtoProjectMatrix * worldtoviewMatrix;
//	GLint modeltoworldTransformMatrixLocation = glGetUniformLocation(programID, "modeltoWorldMatrix");
//	GLint fullTransformUniformLocation = glGetUniformLocation(programID, "fullTRPMatrix");
//
//	//cube 1
//	glBindVertexArray(cubeVAOID);
//	mat4 cube1ModelToWorldMatrix =
//		glm::translate(vec3(-7.0f, 3.9f, 5.0f)) *
//		glm::rotate(50.0f, vec3(0.0f, 1.0f, 0.0f)) *
//		glm::scale(4.0f, 4.0f, 4.0f);
//	fullTransform = worldtoProjectionMatrix * cube1ModelToWorldMatrix;
//	glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
//	//cube1modeltoworldmatrix[] <--- returns vec4 array [] <--index operator first float in vec4
//	glUniformMatrix4fv(modeltoworldTransformMatrixLocation, 1, GL_FALSE, &cube1ModelToWorldMatrix[0][0]);
//	//cout << "Cube 1 Matrix Uniform " << modeltoworldTransformMatrixLocation << endl;
//	glDrawElements(GL_TRIANGLES, cubenumIndices, GL_UNSIGNED_SHORT, (void*)cubeIndexDataOffset);
//
//	//cube 2
//	glBindVertexArray(bigcubeVAOID);
//	mat4 cube2ModelToWorldMatrix =
//		glm::translate(vec3(3.0f, 5.5f, -2.0f)) *
//		glm::rotate(36.0f, vec3(1.0f, 0.0f, 0.0f)) *
//		glm::scale(4.0f, 4.0f, 4.0f);
//	fullTransform = worldtoProjectionMatrix * cube2ModelToWorldMatrix;
//	glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
//	glUniformMatrix4fv(modeltoworldTransformMatrixLocation, 1, GL_FALSE, &cube2ModelToWorldMatrix[0][0]);
//	//cout << "Cube 2 Matrix Uniform " << modeltoworldTransformMatrixLocation << endl;
//	glDrawElements(GL_TRIANGLES, bigcubebumIndices, GL_UNSIGNED_SHORT, (void*)bigcubeIndexDataOffset);
//
//	//plane(bottom)
//	glBindVertexArray(planeVAOID);
//	mat4 planeModelToWorldMatrix;
//	fullTransform = worldtoProjectionMatrix * planeModelToWorldMatrix;
//	glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
//	glUniformMatrix4fv(modeltoworldTransformMatrixLocation, 1, GL_FALSE, &planeModelToWorldMatrix[0][0]);
//	//cout << "Plane Matrix Uniform " << fullTransformUniformLocation << endl;
//	glDrawElements(GL_TRIANGLES, planenumIndices, GL_UNSIGNED_SHORT, (void*)planeIndexDataOffset);
//
//	//plane 4(top)
//	mat4 planetopModelToWorldMatrix =
//		glm::translate(vec3(0.0f, 39.0f, -1.0f)) *
//		glm::rotate(180.0f, vec3(1.0f, 0.0f, 0.0f));
//	fullTransform = worldtoProjectionMatrix * planetopModelToWorldMatrix;
//	glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
//	glUniformMatrix4fv(modeltoworldTransformMatrixLocation, 1, GL_FALSE, &planetopModelToWorldMatrix[0][0]);
//	//cout << "Plane 3 Matrix Uniform " << fullTransformUniformLocation << endl;
//	glDrawElements(GL_TRIANGLES, planenumIndices, GL_UNSIGNED_SHORT, (void*)planeIndexDataOffset);
//
//	//plane2(far)
//	mat4 plane2ModelToWorldMatrix = 
//		glm::translate(vec3(0.0f, 19.0f, -20.0f)) *
//		glm::rotate(90.0f, vec3(1.0f, 0.0f, 0.0f));
//	fullTransform = worldtoProjectionMatrix * plane2ModelToWorldMatrix;
//	glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
//	glUniformMatrix4fv(modeltoworldTransformMatrixLocation, 1, GL_FALSE, &plane2ModelToWorldMatrix[0][0]);
//	//cout << "Plane 2 Matrix Uniform " << fullTransformUniformLocation << endl;
//	glDrawElements(GL_TRIANGLES, planenumIndices, GL_UNSIGNED_SHORT, (void*)planeIndexDataOffset);
//
//	//plane 3(far right)
//	mat4 plane3ModelToWorldMatrix =
//		glm::translate(vec3(19.0f, 20.0f, 0.0f)) *
//		glm::rotate(90.0f, vec3(0.0f, 0.0f, 1.0f));
//	fullTransform = worldtoProjectionMatrix * plane3ModelToWorldMatrix;
//	glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
//	glUniformMatrix4fv(modeltoworldTransformMatrixLocation, 1, GL_FALSE, &plane3ModelToWorldMatrix[0][0]);
//	//cout << "Plane 3 Matrix Uniform " << fullTransformUniformLocation << endl;
//	glDrawElements(GL_TRIANGLES, planenumIndices, GL_UNSIGNED_SHORT, (void*)planeIndexDataOffset);
//
//	//plane 4(far left)
//	mat4 plane4ModelToWorldMatrix =
//		glm::translate(vec3(-20.0f, 19.0f, 0.0f)) *
//		glm::rotate(270.0f, vec3(0.0f, 0.0f, 1.0f));
//	fullTransform = worldtoProjectionMatrix * plane4ModelToWorldMatrix;
//	glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
//	glUniformMatrix4fv(modeltoworldTransformMatrixLocation, 1, GL_FALSE, &plane4ModelToWorldMatrix[0][0]);
//	//cout << "Plane 3 Matrix Uniform " << fullTransformUniformLocation << endl;
//	glDrawElements(GL_TRIANGLES, planenumIndices, GL_UNSIGNED_SHORT, (void*)planeIndexDataOffset);
//
//	//sphere
//	glBindVertexArray(sphereVAOID);
//	mat4 sphere2ModelToWorldMatrix =
//		glm::translate(vec3(-10.0f, 5.0f, -12.5f)) *
//		glm::scale(5.0f,5.0f,5.0f);
//	fullTransform = worldtoProjectionMatrix * sphere2ModelToWorldMatrix;
//	glUniformMatrix4fv(fullTransformUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
//	glUniformMatrix4fv(modeltoworldTransformMatrixLocation, 1, GL_FALSE, &sphere2ModelToWorldMatrix[0][0]);
//	//cout << "Sphere2 Matrix Uniform " << modeltoworldTransformMatrixLocation << endl;
//	glDrawElements(GL_TRIANGLES, spherenumIndices, GL_UNSIGNED_SHORT, (void*)sphereIndexDataOffset);
//
//
//	GLenum err;
//	while ((err = glGetError()) != GL_NO_ERROR)
//	{
//		cout << "Error Code " << err << endl;
//	}
//}