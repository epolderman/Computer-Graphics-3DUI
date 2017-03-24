#include "Mesh.h"



void Mesh::InitializeWithShapeData(const ShapeData& shape)
{
	glGenBuffers(1, &mesh_vertexArrayBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mesh_vertexArrayBuffer);
	glBufferData(GL_ARRAY_BUFFER, shape.vertexBufferSize() + shape.indexBufferSize(), 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, shape.vertexBufferSize(), shape.vertices);
	glBufferSubData(GL_ARRAY_BUFFER, shape.vertexBufferSize(), shape.indexBufferSize(), shape.indices);
	mesh_numIndices = shape.numIndices;
	glGenVertexArrays(1, &mesh_vertexArrayObject);
	glBindVertexArray(mesh_vertexArrayObject);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	glBindBuffer(GL_ARRAY_BUFFER, mesh_vertexArrayBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 21 * sizeof(float), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 21 * sizeof(float), (void*)(sizeof(float) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 21 * sizeof(float), (void*)(sizeof(float) * 6));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 21 * sizeof(float), (void*)(sizeof(float) * 9));
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 21 * sizeof(float), (void*)(sizeof(float) * 12));
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 21 * sizeof(float), (void*)(sizeof(float) * 15));
	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, 21 * sizeof(float), (void*)(sizeof(float) * 18));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_vertexArrayBuffer);
	meshShapeDataOffset = shape.vertexBufferSize();
}

void Mesh::InitializeWaterPlane(const ShapeData& shape)
{

	glGenBuffers(1, &mesh_vertexArrayBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mesh_vertexArrayBuffer);
	glBufferData(GL_ARRAY_BUFFER, shape.vertexBufferSize() + shape.indexBufferSize(), 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, shape.vertexBufferSize(), shape.vertices);
	glBufferSubData(GL_ARRAY_BUFFER, shape.vertexBufferSize(), shape.indexBufferSize(), shape.indices);
	mesh_numIndices = shape.numIndices;
	glGenVertexArrays(1, &mesh_vertexArrayObject);
	glBindVertexArray(mesh_vertexArrayObject);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, mesh_vertexArrayBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_vertexArrayBuffer);
	meshShapeDataOffset = shape.vertexBufferSize();

}

void Mesh::InitializeObjModel(const std::string& filename)
{
	initializeMeshwithModel(OBJModel(filename).ToIndexedModel());
}

void Mesh::initializeMeshwithModel(const IndexedModel &model)
{
	mesh_numIndices = model.indices.size();

	glGenBuffers(NUM_BUFFERS, mesh_vertexArrayBuffers);

	glBindBuffer(GL_ARRAY_BUFFER, mesh_vertexArrayBuffers[POSITION_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(model.positions[0]) * model.positions.size(), &model.positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, mesh_vertexArrayBuffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(model.texCoords[0]) * model.texCoords.size(), &model.texCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, mesh_vertexArrayBuffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(model.normals[0]) * model.normals.size(), &model.normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_vertexArrayBuffers[INDEX_VB]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(model.indices[0]) * model.indices.size(), &model.indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

//without offset(for obj models)
void Mesh::draw()
{
	glBindVertexArray(mesh_vertexArrayObject);
	glDrawElements(GL_TRIANGLES, mesh_numIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

//with offset(for shapeData)
void Mesh::drawShape()
{
	glBindVertexArray(mesh_vertexArrayObject);
	glDrawElements(GL_TRIANGLES, mesh_numIndices, GL_UNSIGNED_SHORT, (void*)meshShapeDataOffset);
	glBindVertexArray(0);
}

Mesh::~Mesh()
{
	glDeleteBuffers(NUM_BUFFERS, mesh_vertexArrayBuffers);
	glDeleteVertexArrays(1, &mesh_vertexArrayObject);
}
