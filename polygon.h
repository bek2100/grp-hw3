#pragma once
#include <vector>
#include "vec4.h"
#include "line.h"
#include <Windows.h>
#include <unordered_map>


class polygon
{
public:
	polygon();
	~polygon();
	std::vector<vec4> points;
	std::unordered_map<vec4, line> vertexNormalsGiven;
	std::unordered_map<vec4, line> vertexNormalsCalculated;
	vec4 Normal_Val(bool given);
	line Normal(bool given);
	std::unordered_map<vec4, line> VertexNormal(bool given);
	vec4 Plane;
	bool operator==(const polygon &another_polygon) const;
};

