#pragma once
class vec4
{
public:
	vec4();
	vec4(double _x, double _y, double _z, double _p);
	~vec4();
	unsigned int hash();
	double x;
	double y;
	double z;
	double p;
	double& operator[](int indx);
	vec4& operator=(vec4 l);
};

bool operator==(vec4 l, vec4 r);
vec4 operator+(vec4 lhs, vec4 rhs);
vec4 operator-(vec4 lhs, vec4 rhs);
vec4 operator/(vec4 lhs, double x);
vec4 operator*(vec4 lhs, double x);