#pragma once
#include "Color.h"
#include <iostream>
#include <cmath>

class Vec3 {
public:
	double x;
	double y;
	double z;
	Vec3() : x(0), y(0), z(0) {}
	Vec3(double v) : x(v), y(v), z(v) {}
	Vec3(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
	Vec3(const Vec3& v) : x(v.x), y(v.y), z(v.z) {}

	// from linear Algebra 
	double vecLength() {
		return sqrt((x * x) + (y * y) + (z * z));
	}
	double squareLength() {
		return (x * x) + (y * y) + (z * z);
	}
	double dotProduct(Color b) {
		return ((x * b.red) + (y * b.green) + (z * b.blue));
	}
	// a . b
	double dotProduct(Vec3 b) {
		return ((x * b.x) + (y * b.y) + (z * b.z));
	}
	// a x b
	Vec3 crossProduct(Vec3 b) {
		return Vec3((y * b.z) - (z * b.y), (z * b.x) - (x * b.z), (x * b.y) - (y * b.x));
	}
	Vec3 operator*(double a) {
		return Vec3(x * a, y * a, z * a);
	}
	Vec3 operator/(double a) {
		return Vec3(x / a, y / a, z / a);
	}
	Vec3 operator+(Vec3 b) {
		return Vec3(x + b.x, y + b.y, z + b.z);
	}
	Vec3 operator+(double a) {
		return Vec3(x + a, y + a, z + a);
	}
	Vec3 operator-(double a) {
		return Vec3(x - a, y - a, z - a);
	}
	Vec3 operator-(Vec3 b) {
		return Vec3(x - b.x, y - b.y, z - b.z);
	}
	Vec3 normalize() {
		return (*this) * (1.0 / vecLength());
	}
	Vec3 negate() {
		return Vec3(-x, -y, -z);
	}
};

std::ostream& operator<< (std::ostream& os, const Vec3& v)
{
	os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	return os;
}

