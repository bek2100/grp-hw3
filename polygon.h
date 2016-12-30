#pragma once
#include <vector>
#include "vec4.h"
#include "line.h"
#include <Windows.h>

class polygon
{
public:
	polygon();
	~polygon();
	std::vector<vec4> points;
	std::vector<line> vertexNormalsGiven;
	std::vector<line> vertexNormalsCalculated;
	vec4 Normal_Val(bool given);
	line Normal(bool given);
	std::vector<line> VertexNormal(bool given);
	vec4 Plane;
	bool operator==(const polygon &another_polygon) const;
};

