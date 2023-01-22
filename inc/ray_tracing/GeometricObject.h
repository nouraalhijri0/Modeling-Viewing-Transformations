/* Base class for geometric objects*/
#pragma once
#include "Vec3.h"
#include "Color.h"
#include "Ray.h"
	
class GeometricObject{
public:
	Color color;
    Vec3 center;
	GeometricObject(Color _color) : color(_color){}
	virtual double testIntersection(Ray ray) = 0;
	virtual Vec3 computeIntersectionPoint(Ray ray, double t) = 0;
	virtual Vec3 computeNormalIntersection(Ray ray, double t) = 0;
};

