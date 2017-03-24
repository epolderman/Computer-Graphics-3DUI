#ifndef MESH_H
#define MESH_H
#include <GL/glew.h>
#include <string>
#include <vector>
#include "obj_loader.h"
#include "Material.h"
#include "ShapeGenerator.h"


enum MeshBufferPositions
{
	POSITION_VB, //0
	TEXCOORD_VB, //1
	NORMAL_VB, // 2
	INDEX_VB // 3
};

class Mesh
{
public:
	Mesh(){}
	void InitializeWithShapeData(const ShapeData& shape);

	void InitializeObjModel(const std::string& filename);
	void initializeMeshwithModel(const IndexedModel& theModel);
	void InitializeWaterPlane(const ShapeData& shape);

	void draw();
	void drawShape();
	~Mesh();
private:
	GLuint meshShapeDataOffset;
	unsigned int mesh_numIndices;
	static const unsigned int NUM_BUFFERS = 4;
	GLuint mesh_vertexArrayObject;
	GLuint mesh_vertexArrayBuffer;
	GLuint mesh_vertexArrayBuffers[NUM_BUFFERS];
	void operator=(const Mesh& mesh) {}
	Mesh(const Mesh& mesh) {}
};

#endif