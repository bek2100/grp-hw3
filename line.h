#pragma once
#include "vec4.h"

class line
{
public:
	line();
	line(vec4 a, vec4 b);
	~line();
	vec4 p_a;
	vec4 p_b;
	bool operator==(const line &r) const;
	bool on_screen;
	int draw_count;
};

struct lineHasher
{
	std::size_t operator()(const line& p) const
	{
		using std::size_t;
		using std::hash;
		return (hash<double>()(p.p_a.x + p.p_a.y + p.p_a.z) + hash<double>()(p.p_b.x + p.p_b.y + p.p_b.z));
	}
};

namespace std
{
	template < >
	struct hash<line>
	{
		size_t operator()(const line& p) const
		{
			// Compute individual hash values for two data members and combine them using XOR and bit shifting
			return (hash<double>()(p.p_a.x + p.p_a.y + p.p_a.z) + hash<double>()(p.p_b.x + p.p_b.y + p.p_b.z));
		}
	};
}

