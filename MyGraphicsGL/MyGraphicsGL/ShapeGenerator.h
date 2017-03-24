#ifndef SHAPE_GEN_H
#define SHAPE_GEN_H
#include "ShapeData.h"
using glm::uint;
using glm::vec3;
using glm::vec2;


class ShapeGenerator
{
private:
	static ShapeData makePlaneIndices(uint dimensions);
	static ShapeData makePlaneVerts(uint dimensions, const struct Material& m);
	static ShapeData makePlaneVertsSphere(uint dimensions, const struct Material& m);

public:
	static ShapeData makeWater();
	static ShapeData makeArrow();

	static ShapeData makeCubeWithMaterial(const struct Material& theMaterial);
	static ShapeData makePlaneWithMaterial(uint dimensions, const struct Material& m);
	static ShapeData makeSphereWithMaterial(uint tesselation,const struct Material& theMaterial);

};
#endif
