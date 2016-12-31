#pragma once
#include <unordered_map>

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

struct vec4Hasher
{
	std::size_t operator()(const vec4& lhs) const
	{
		using std::size_t;
		using std::hash;
		return (hash<double>()(lhs.x) + hash<double>()(lhs.y) + hash<double>()(lhs.z));
	}
};

namespace std
{
	template < >
	struct hash<vec4>
	{
		size_t operator()(const vec4& p) const
		{
			// Compute individual hash values for two data members and combine them using XOR and bit shifting
			return (hash<double>()(p.x) + hash<double>()(p.y) + hash<double>()(p.z));
		}
	};
}

