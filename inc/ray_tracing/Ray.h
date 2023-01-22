#pragma once
#include<iostream>

class Ray {
public:
	Vec3 origin;
	Vec3 direction;

	Ray(){}
	Ray(Vec3 _origin,Vec3 _direction) : origin(_origin), direction(_direction){}
};

std::ostream& operator<< (std::ostream& os, const Ray& r)
{
	os << "Ray origin: " << r.origin << "; ";
	os << "Ray direction: " << r.direction << "\n";
	return os;
}

