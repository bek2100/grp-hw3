#include "mat4.h"


mat4::mat4()
{
	vec4 empty_row;
	rows.push_back(empty_row);
	rows.push_back(empty_row);
	rows.push_back(empty_row);
	rows.push_back(empty_row);
}


mat4::~mat4()
{
}

vec4& mat4::operator[](int indx){
	return rows[indx];
};

vec4 operator*(mat4 lhs, vec4 rhs){
	vec4 res;
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			res[i] += lhs[i][j] * rhs[j];
		}
	}
	return res;
};

mat4 transpose(mat4 &rhs){
	mat4 res;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++){
			res[i][j] = rhs[j][i];
		}

	return res;
};

mat4 operator*(mat4 lhs, mat4 rhs){
	mat4 res;
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			for (int k = 0; k < 4; k++){
				res[i][j] += lhs[i][k] * rhs[k][j];
			}
		}
	}
	return res;
};

vec4 operator*(vec4 lhs, mat4 rhs){
	vec4 res;
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			res[i] += lhs[j] * rhs[j][i];
		}
	}
	return res;
};

mat4 operator+(mat4 lhs, mat4 rhs){
	mat4 res;

	for (int i = 0; i < 4; i++){
		res[i] = lhs[i] + rhs[i];
	}

	return res;
};