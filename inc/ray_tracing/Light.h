#pragma once
#include "Vec3.h"
#include "Color.h"
#include<iostream>

class Light{
public:
	Vec3 position;
	Color intensity;

	Light() : position(1.0f, 0.0f, 0.0f), intensity(1.0f) {}
	Light(Vec3 _position, Color _color) : position(_position), intensity(_color) {}
};

std::ostream& operator<< (std::ostream& os, const Light& l)
{
	os << "Light position: " << l.position << ", ";
	os << "and intensity: " << l.intensity << "\n";
	return os;
}
