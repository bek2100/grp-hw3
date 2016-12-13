#pragma once
#include "vec4.h"
#include <vector>
class mat4
{
public:
	mat4();
	~mat4();
	
	std::vector<vec4> rows;

	vec4& operator[](int indx);
};

mat4 transpose(mat4 &rhs);

vec4 operator*(mat4 lhs, vec4 rhs);

vec4 operator*(vec4 lhs, mat4 rhs);

mat4 operator*(mat4 lhs, mat4 rhs);

mat4 operator+(mat4 lhs, mat4 rhs);
