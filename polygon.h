#pragma once
#include <vector>
#include "vec4.h"
#include "line.h"

class polygon
{
public:
	polygon();
	~polygon();
	std::vector<vec4> points;
	vec4 Normal_Val(bool given);
	line Normal(bool given);
	vec4 Plane;	
	bool operator==(const polygon &another_polygon) const;
};

