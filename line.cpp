#include "line.h"
#include <functional>

line::line()
{
}

line::line(vec4 a, vec4 b)
{
	p_a = a;
	p_b = b;
	on_screen = false;
	draw_count = 0;
}

line::~line()
{
}

bool line::operator==(const line &r) const{
	if (p_a == r.p_a && p_b == r.p_b || 
		p_a == r.p_b && p_b == r.p_a)
		return true;
	else
		return false;
}


