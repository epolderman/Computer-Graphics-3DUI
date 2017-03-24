#include "ShapeGenerator.h"
#define PI 3.14159265359
using glm::vec3;
using glm::vec2;
#define NUM_ARRAY_ELEMENTS(a) sizeof(a) / sizeof(*a)

//randoms
glm::vec3 randomColor()
{
	glm::vec3 random;
	random.x = rand() / (float)RAND_MAX;
	random.y = rand() / (float)RAND_MAX;
	random.z = rand() / (float)RAND_MAX;
	return random;
}

ShapeData ShapeGenerator::makeWater()
{

	ShapeData returnData;

	//static compile time
	Vertex myVerts[] =
	{
		glm::vec3(+1.0f, +0.0f, -1.0f),
		glm::vec3(+1.0f, +0.0f, +1.0f),
		glm::vec3(-1.0f, +0.0f, +1.0f),
		glm::vec3(-1.0f, +0.0f, -1.0f),
	};

	returnData.numVertices = NUM_ARRAY_ELEMENTS(myVerts);
	returnData.vertices = new Vertex[returnData.numVertices];
	memcpy(returnData.vertices, myVerts, sizeof(myVerts));

	GLushort myIndices[] =
	{
		0, 3, 2,
		2, 1, 0
	};

	returnData.numIndices = NUM_ARRAY_ELEMENTS(myIndices);
	returnData.indices = new GLushort[returnData.numIndices];
	memcpy(returnData.indices, myIndices, sizeof(myIndices));

	return returnData;



}

//no normals
ShapeData ShapeGenerator::makeArrow()
{
	ShapeData returnData;
	Vertex stackVerts[] =
	{
		//top side of arrow head
		vec3(+0.00f, +0.25f, -0.25f), //0
		vec3(+1.00f, +0.00f, +0.00f), //Color
		vec3(+0.50f, +0.25f, -0.25f), //1
		vec3(+1.00f, +0.00f, +0.00f), //Color
		vec3(+0.00f, +0.25f, -1.00f), //2
		vec3(+1.00f, +0.00f, +0.00f), //Color
		vec3(-0.50f, +0.25f, -0.25f), //3
		vec3(+1.00f, +0.00f, +0.00f), //Color
		//bottom side of arrow head	 		 
		vec3(+0.00f, -0.25f, -0.25f), //4
		vec3(+0.00f, +0.00f, +1.00f), //Color
		vec3(+0.50f, -0.25f, -0.25f), //5
		vec3(+0.00f, +0.00f, +1.00f), //Color
		vec3(+0.00f, -0.25f, -1.00f), //6
		vec3(+0.00f, +0.00f, +1.00f), //Color
		vec3(-0.50f, -0.25f, -0.25f), //7
		vec3(+0.00f, +0.00f, +1.00f), //Color
		//right side of arrow tip
		vec3(+0.50f, +0.25f, -0.25f), //8
		vec3(+0.60f, +1.00f, +0.00f), //Color
		vec3(+0.00f, +0.25f, -1.00f), //9
		vec3(+0.60f, +1.00f, +0.00f), //Color
		vec3(+0.00f, -0.25f, -1.00f), //10
		vec3(+0.60f, +1.00f, +0.00f), //Color
		vec3(+0.50f, -0.25f, -0.25f), //11
		vec3(+0.60f, +1.00f, +0.00f), //Color
		//left side of arrow tip
		vec3(+0.00f, +0.25f, -1.00f), //12
		vec3(+0.00f, +1.00f, +0.00f), //Color
		vec3(-0.50f, +0.25f, -0.25f), //13
		vec3(+0.00f, +1.00f, +0.00f), //Color
		vec3(+0.00f, -0.25f, -1.00f), //14
		vec3(+0.00f, +1.00f, +0.00f), //Color
		vec3(-0.50f, -0.25f, -0.25f), //15
		vec3(+0.00f, +1.00f, +0.00f), //Color
		//back side of arrow tip
		vec3(-0.50f, +0.25f, -0.25f), //16
		vec3(+0.50f, +0.50f, +0.50f), //Color
		vec3(+0.50f, +0.25f, -0.25f), //17
		vec3(+0.50f, +0.50f, +0.50f), //Color
		vec3(-0.50f, -0.25f, -0.25f), //18
		vec3(+0.50f, +0.50f, +0.50f), //Color
		vec3(+0.50f, -0.25f, -0.25f), //19
		vec3(+0.50f, +0.50f, +0.50f), //Color
		//top side of arrow back
		vec3(+0.25f, +0.25f, -0.25f), //20
		vec3(+1.00f, +0.00f, +0.00f), //Color
		vec3(+0.25f, +0.25f, +1.00f), //21
		vec3(+1.00f, +0.00f, +0.00f), //Color
		vec3(-0.25f, +0.25f, +1.00f), //22
		vec3(+1.00f, +0.00f, +0.00f), //Color
		vec3(-0.25f, +0.25f, -0.25f), //23
		vec3(+1.00f, +0.00f, +0.00f), //Color
		//bottom side of arrow back
		vec3(+0.25f, -0.25f, -0.25f), //24
		vec3(+0.00f, +0.00f, +1.00f), //Color
		vec3(+0.25f, -0.25f, +1.00f), //25
		vec3(+0.00f, +0.00f, +1.00f), //Color
		vec3(-0.25f, -0.25f, +1.00f), //26
		vec3(+0.00f, +0.00f, +1.00f), //Color
		vec3(-0.25f, -0.25f, -0.25f), //27
		vec3(+0.00f, +0.00f, +1.00f), //Color
		//right side of arrow back
		vec3(+0.25f, +0.25f, -0.25f), //28
		vec3(+0.60f, +1.00f, +0.00f), //Color
		vec3(+0.25f, -0.25f, -0.25f), //29
		vec3(+0.60f, +1.00f, +0.00f), //Color
		vec3(+0.25f, -0.25f, +1.00f), //30
		vec3(+0.60f, +1.00f, +0.00f), //Color
		vec3(+0.25f, +0.25f, +1.00f), //31
		vec3(+0.60f, +1.00f, +0.00f), //Color
		//left side of arrow back
		vec3(-0.25f, +0.25f, -0.25f), //32
		vec3(+0.00f, +1.00f, +0.00f), //Color
		vec3(-0.25f, -0.25f, -0.25f), //33
		vec3(+0.00f, +1.00f, +0.00f), //Color
		vec3(-0.25f, -0.25f, +1.00f), //34
		vec3(+0.00f, +1.00f, +0.00f), //Color
		vec3(-0.25f, +0.25f, +1.00f), //35
		vec3(+0.00f, +1.00f, +0.00f), //Color
		//back side of arrow back
		vec3(-0.25f, +0.25f, +1.00f), //36
		vec3(+0.50f, +0.50f, +0.50f), //Color
		vec3(+0.25f, +0.25f, +1.00f), //37
		vec3(+0.50f, +0.50f, +0.50f), //Color
		vec3(-0.25f, -0.25f, +1.00f), //38
		vec3(+0.50f, +0.50f, +0.50f), //Color
		vec3(+0.25f, -0.25f, +1.00f), //39
		vec3(+0.50f, +0.50f, +0.50f), //Color
	};

	returnData.numVertices = NUM_ARRAY_ELEMENTS(stackVerts);
	returnData.vertices = new Vertex[returnData.numVertices];
	memcpy(returnData.vertices, stackVerts, sizeof(stackVerts));

	GLushort stackIndices[] = {
		0, 1, 2, // top
		0, 2, 3,
		4, 6, 5, //bottom
		4, 7, 6,
		8, 10, 9, //right side of arrow tip
		8, 11, 10,
		12, 15, 13, //left side of arrow tip
		12, 14, 15,
		16, 19, 17, //back side of arrow tip
		16, 18, 19,
		20, 22, 21, //top side of back of arrow
		20, 23, 22,
		24, 25, 26, //bottom side of back of arrow
		24, 26, 27,
		28, 30, 29, //right side of back of arrow
		28, 31, 30,
		32, 33, 34, //left side of back of arrow
		32, 34, 35,
		36, 39, 37, //back side of back of arrow
		36, 38, 39,
	};

	returnData.numIndices = NUM_ARRAY_ELEMENTS(stackIndices);
	returnData.indices = new GLushort[returnData.numIndices];
	memcpy(returnData.indices, stackIndices, sizeof(stackIndices));

	return returnData;


}


//objects with materials

ShapeData ShapeGenerator::makeCubeWithMaterial(const struct Material &m)
{
	ShapeData returnData;
	Vertex stackVerts[] =
	{
		vec3(-1.0f, +1.0f, +1.0f),  // 0
		vec3(+1.0f, +0.0f, +0.0f),	// Color
		vec3(+0.0f, +1.0f, +0.0f),  // Normal
		m.Ambient,
		m.Diffuse,
		m.Specular,
		m.Shininess,

		vec3(+1.0f, +1.0f, +1.0f),  // 1
		vec3(+0.0f, +1.0f, +0.0f),	// Color
		vec3(+0.0f, +1.0f, +0.0f),  // Normal
		m.Ambient,
		m.Diffuse,
		m.Specular,
		m.Shininess,

		vec3(+1.0f, +1.0f, -1.0f),  // 2
		vec3(+0.0f, +0.0f, +1.0f),  // Color
		vec3(+0.0f, +1.0f, +0.0f),  // Normal
		m.Ambient,
		m.Diffuse,
		m.Specular,
		m.Shininess,

		vec3(-1.0f, +1.0f, -1.0f),  // 3
		vec3(+1.0f, +1.0f, +1.0f),  // Color
		vec3(+0.0f, +1.0f, +0.0f),  // Normal
		m.Ambient,
		m.Diffuse,
		m.Specular,
		m.Shininess,


		vec3(-1.0f, +1.0f, -1.0f),  // 4
		vec3(+1.0f, +0.0f, +1.0f),  // Color
		vec3(+0.0f, +0.0f, -1.0f),  // Normal
		m.Ambient,
		m.Diffuse,
		m.Specular,
		m.Shininess,


		vec3(+1.0f, +1.0f, -1.0f),  // 5
		vec3(+0.0f, +0.5f, +0.2f),  // Color
		vec3(+0.0f, +0.0f, -1.0f),  // Normal
		m.Ambient,
		m.Diffuse,
		m.Specular,
		m.Shininess,


		vec3(+1.0f, -1.0f, -1.0f),  // 6
		vec3(+0.8f, +0.6f, +0.4f),  // Color
		vec3(+0.0f, +0.0f, -1.0f),  // Normal
		m.Ambient,
		m.Diffuse,
		m.Specular,
		m.Shininess,


		vec3(-1.0f, -1.0f, -1.0f),  // 7
		vec3(+0.3f, +1.0f, +0.5f),  // Color
		vec3(+0.0f, +0.0f, -1.0f),  // Normal
		m.Ambient,
		m.Diffuse,
		m.Specular,
		m.Shininess,


		vec3(+1.0f, +1.0f, -1.0f),  // 8
		vec3(+0.2f, +0.5f, +0.2f),  // Color
		vec3(+1.0f, +0.0f, +0.0f),  // Normal
		m.Ambient,
		m.Diffuse,
		m.Specular,
		m.Shininess,


		vec3(+1.0f, +1.0f, +1.0f),  // 9
		vec3(+0.9f, +0.3f, +0.7f),  // Color
		vec3(+1.0f, +0.0f, +0.0f),  // Normal
		m.Ambient,
		m.Diffuse,
		m.Specular,
		m.Shininess,


		vec3(+1.0f, -1.0f, +1.0f),  // 10
		vec3(+0.3f, +0.7f, +0.5f),  // Color
		vec3(+1.0f, +0.0f, +0.0f),  // Normal
		m.Ambient,
		m.Diffuse,
		m.Specular,
		m.Shininess,


		vec3(+1.0f, -1.0f, -1.0f),  // 11
		vec3(+0.5f, +0.7f, +0.5f),  // Color
		vec3(+1.0f, +0.0f, +0.0f),  // Normal
		m.Ambient,
		m.Diffuse,
		m.Specular,
		m.Shininess,


		vec3(-1.0f, +1.0f, +1.0f),  // 12
		vec3(+0.7f, +0.8f, +0.2f),  // Color
		vec3(-1.0f, +0.0f, +0.0f),  // Normal
		m.Ambient,
		m.Diffuse,
		m.Specular,
		m.Shininess,


		vec3(-1.0f, +1.0f, -1.0f),  // 13
		vec3(+0.5f, +0.7f, +0.3f),  // Color
		vec3(-1.0f, +0.0f, +0.0f),  // Normal
		m.Ambient,
		m.Diffuse,
		m.Specular,
		m.Shininess,


		vec3(-1.0f, -1.0f, -1.0f),  // 14
		vec3(+0.4f, +0.7f, +0.7f),  // Color
		vec3(-1.0f, +0.0f, +0.0f),  // Normal
		m.Ambient,
		m.Diffuse,
		m.Specular,
		m.Shininess,


		vec3(-1.0f, -1.0f, +1.0f),  // 15
		vec3(+0.2f, +0.5f, +1.0f),  // Color
		vec3(-1.0f, +0.0f, +0.0f),  // Normal
		m.Ambient,
		m.Diffuse,
		m.Specular,
		m.Shininess,


		vec3(+1.0f, +1.0f, +1.0f),  // 16
		vec3(+0.6f, +1.0f, +0.7f),  // Color
		vec3(+0.0f, +0.0f, +1.0f),  // Normal
		m.Ambient,
		m.Diffuse,
		m.Specular,
		m.Shininess,

		vec3(-1.0f, +1.0f, +1.0f),  // 17
		vec3(+0.6f, +0.4f, +0.8f),  // Color
		vec3(+0.0f, +0.0f, +1.0f),  // Normal
		m.Ambient,
		m.Diffuse,
		m.Specular,
		m.Shininess,


		vec3(-1.0f, -1.0f, +1.0f),  // 18
		vec3(+0.2f, +0.8f, +0.7f),  // Color
		vec3(+0.0f, +0.0f, +1.0f),  // Normal
		m.Ambient,
		m.Diffuse,
		m.Specular,
		m.Shininess,


		vec3(+1.0f, -1.0f, +1.0f),  // 19
		vec3(+0.2f, +0.7f, +1.0f),  // Color
		vec3(+0.0f, +0.0f, +1.0f),  // Normal
		m.Ambient,
		m.Diffuse,
		m.Specular,
		m.Shininess,

		vec3(+1.0f, -1.0f, -1.0f),  // 20
		vec3(+0.8f, +0.3f, +0.7f),  // Color
		vec3(+0.0f, -1.0f, +0.0f),  // Normal
		m.Ambient,
		m.Diffuse,
		m.Specular,
		m.Shininess,

		vec3(-1.0f, -1.0f, -1.0f),  // 21
		vec3(+0.8f, +0.9f, +0.5f),  // Color
		vec3(+0.0f, -1.0f, +0.0f),  // Normal
		m.Ambient,
		m.Diffuse,
		m.Specular,
		m.Shininess,

		vec3(-1.0f, -1.0f, +1.0f),  // 22
		vec3(+0.5f, +0.8f, +0.5f),  // Color
		vec3(+0.0f, -1.0f, +0.0f),  // Normal
		m.Ambient,
		m.Diffuse,
		m.Specular,
		m.Shininess,

		vec3(+1.0f, -1.0f, +1.0f),  // 23
		vec3(+0.9f, +1.0f, +0.2f),  // Color
		vec3(+0.0f, -1.0f, +0.0f),  // Normal
		m.Ambient,
		m.Diffuse,
		m.Specular,
		m.Shininess,

	};

	returnData.numVertices = NUM_ARRAY_ELEMENTS(stackVerts);
	returnData.vertices = new Vertex[returnData.numVertices];
	memcpy(returnData.vertices, stackVerts, sizeof(stackVerts));

	GLushort stackIndices[] = {
		0, 1, 2, 0, 2, 3, // Top
		4, 5, 6, 4, 6, 7, // Front
		8, 9, 10, 8, 10, 11, // Right
		12, 13, 14, 12, 14, 15, // Left
		16, 17, 18, 16, 18, 19, // Back
		20, 22, 21, 20, 23, 22, // Bottom
	};

	returnData.numIndices = NUM_ARRAY_ELEMENTS(stackIndices);
	returnData.indices = new GLushort[returnData.numIndices];
	memcpy(returnData.indices, stackIndices, sizeof(stackIndices));

	return returnData;


}

ShapeData ShapeGenerator::makePlaneVerts(uint dimensions, const struct Material& m)
{
	ShapeData ret;
	ret.numVertices = dimensions * dimensions;
	int half = dimensions / 2;
	ret.vertices = new Vertex[ret.numVertices];
	for (int i = 0; i < dimensions; i++)
	{
		for (int j = 0; j < dimensions; j++)
		{
			Vertex& thisVert = ret.vertices[i * dimensions + j];
			thisVert.position.x = j - half;
			thisVert.position.z = i - half;
			thisVert.position.y = 0;
			thisVert.surfaceNormal = glm::vec3(0.0f, 1.0f, 0.0f);
			thisVert.color = randomColor();
			thisVert.ambientMaterial = m.Ambient;
			thisVert.diffuseMaterial = m.Diffuse;
			thisVert.specularMaterial = m.Specular;
			thisVert.shininessMaterial = m.Shininess;
		}
	}
	return ret;
}

ShapeData ShapeGenerator::makePlaneIndices(uint dimensions)
{
	ShapeData ret;
	ret.numIndices = (dimensions - 1) * (dimensions - 1) * 2 * 3; // 2 triangles per square, 3 indices per triangle
	ret.indices = new unsigned short[ret.numIndices];
	int runner = 0;
	for (int row = 0; row < dimensions - 1; row++)
	{
		for (int col = 0; col < dimensions - 1; col++)
		{
			ret.indices[runner++] = dimensions * row + col;
			ret.indices[runner++] = dimensions * row + col + dimensions;
			ret.indices[runner++] = dimensions * row + col + dimensions + 1;

			ret.indices[runner++] = dimensions * row + col;
			ret.indices[runner++] = dimensions * row + col + dimensions + 1;
			ret.indices[runner++] = dimensions * row + col + 1;
		}
	}
	assert(runner = ret.numIndices);
	return ret;
}

ShapeData ShapeGenerator::makePlaneWithMaterial(uint dimensions, const struct Material& m)
{
	ShapeData Data = makePlaneVerts(dimensions, m);
	ShapeData Data2 = makePlaneIndices(dimensions);

	Data.numIndices = Data2.numIndices;
	Data.indices = Data2.indices;
	return Data;
}

ShapeData ShapeGenerator::makeSphereWithMaterial(uint tesselation, const struct Material &m)
{
	ShapeData ret = makePlaneVertsSphere(tesselation, m);
	ShapeData ret2 = makePlaneIndices(tesselation);
	ret.indices = ret2.indices;
	ret.numIndices = ret2.numIndices;

	uint dimensions = tesselation;
	const float RADIUS = 1.0f;
	const double CIRCLE = PI * 2;
	const double SLICE_ANGLE = CIRCLE / (dimensions - 1);
	for (size_t col = 0; col < dimensions; col++)
	{
		double phi = -SLICE_ANGLE * col;
		for (size_t row = 0; row < dimensions; row++)
		{
			double theta = -(SLICE_ANGLE / 2.0) * row;
			size_t vertIndex = col * dimensions + row;
			Vertex& v = ret.vertices[vertIndex];
			v.position.x = RADIUS * cos(phi) * sin(theta);
			v.position.y = RADIUS * sin(phi) * sin(theta);
			v.position.z = RADIUS * cos(theta);
			v.surfaceNormal = glm::normalize(v.position);
		}
	}
	return ret;
}

ShapeData ShapeGenerator::makePlaneVertsSphere(uint dimensions, const struct Material& m)
{
	ShapeData ret;
	ret.numVertices = dimensions * dimensions;
	int half = dimensions / 2;
	ret.vertices = new Vertex[ret.numVertices];
	for (int i = 0; i < dimensions; i++)
	{
		for (int j = 0; j < dimensions; j++)
		{
			Vertex& thisVert = ret.vertices[i * dimensions + j];
			thisVert.position.x = j - half;
			thisVert.position.z = i - half;
			thisVert.position.y = 0;
			thisVert.surfaceNormal = glm::vec3(0.0f, 1.0f, 0.0f);
			thisVert.color = randomColor();
			thisVert.ambientMaterial = m.Ambient;
			thisVert.diffuseMaterial = m.Diffuse;
			thisVert.specularMaterial = m.Specular;
			thisVert.shininessMaterial = m.Shininess;
		}
	}
	return ret;
}

