#include "polygon.h"
#include <functional>

polygon::polygon()
{
	m_inv = 1;
}


polygon::~polygon()
{
}

vec4 polygon::Normal_Val(bool given){
	if (given)
		return Plane * m_inv;
	vec4 a = points[1] - points[0];
	vec4 b = points[2] - points[1];
	vec4 plane;
	plane[0] = a[1] * b[2] - a[2] * b[1];
	plane[1] = a[2] * b[0] - a[0] * b[2];
	plane[2] = a[0] * b[1] - a[1] * b[0];
	plane[3] = 0;
	return (plane * m_inv)/ (sqrt(pow(plane[0], 2) + pow(plane[1], 2) + pow(plane[2], 2)));
}

void polygon::inverse(){
	m_inv = m_inv * - 1;
}
line polygon::Normal(bool given){ 
	vec4 start;
	vec4 end;
	for (unsigned int i = 0; i < points.size(); i++)
		start = start + points[i];
	start = start / points.size();

	end = start + Normal_Val(given);

	return line(start, end);
}

std::unordered_map<vec4, line> polygon::VertexNormal(bool given){
	if (given) return vertexNormalsGiven;
	return vertexNormalsCalculated;
}

bool polygon::operator==(const polygon &another_polygon) const{
	bool eq = true;
	for (unsigned int p = 0; p < points.size(); p++){
		eq = (eq && (points[p] == another_polygon.points[p]));
	}

	return eq;
}

