#include "vec4.h"


vec4::vec4()
{
	x = 0.0;
	y = 0.0;
	z = 0.0;
	p = 0.0;
}

vec4::vec4(double _x, double _y, double _z, double _p)
{
	x = _x;
	y = _y;
	z = _z;
	p = _p;
}


vec4::~vec4()
{
}

vec4& vec4::operator=(vec4 l){
	x = l.x;
	y = l.y;
	z = l.z;
	p = l.p;
	return *this;
}

bool operator==(vec4 l, vec4 r){
	if (l.x == r.x &&
		l.y == r.y &&
		l.z == r.z &&
		l.p == r.p)
		return true;
	else
		return false;
}

vec4 operator+(vec4 lhs, vec4 rhs){
	vec4 res;

	res.x = lhs.x + rhs.x;
	res.y = lhs.y + rhs.y;
	res.z = lhs.z + rhs.z;
	res.p = lhs.p + rhs.p;
	return res;
};

vec4 operator-(vec4 lhs, vec4 rhs){
	vec4 res;

	res.x = lhs.x - rhs.x;
	res.y = lhs.y - rhs.y;
	res.z = lhs.z - rhs.z;
	res.p = lhs.p - rhs.p;
	return res;
};

vec4 operator/(vec4 lhs, double x){
	vec4 res;

	res.x = lhs.x / x;
	res.y = lhs.y / x;
	res.z = lhs.z / x;
	res.p = lhs.p / x;
	return res;
};

vec4 operator*(vec4 lhs, double x){
	vec4 res;

	res.x = lhs.x * x;
	res.y = lhs.y * x;
	res.z = lhs.z * x;
	res.p = lhs.p * x;
	return res;
};

double& vec4::operator[](int indx){
	if (indx == 0)
		return x;
	else if (indx == 1)
		return y;
	else if (indx == 2)
		return z;
	else if (indx == 3)
		return p;
	else
		throw;
};