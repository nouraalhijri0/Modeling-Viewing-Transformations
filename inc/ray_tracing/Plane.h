#pragma once
#include "Vec3.h"
#include "Color.h"
#include "Ray.h"
#include "GeometricObject.h"
#include<cmath>
#include<iostream>

using namespace std;

class Plane : public GeometricObject {
public:
	double A, B, C, D;
	Vec3 Pn;
    Vec3 center;

	// #TODO: You can declare some additional variables for computation in here

	Plane(double _A, double _B, double _C, double _D, Color _color) : GeometricObject(_color)
	{
		Pn = Vec3(_A, _B, _C).normalize();
		A = Pn.x;
		B = Pn.y;
		C = Pn.z;
		D = _D;
        center = Pn;

	}

	Plane(Vec3 _Pn, double _D, Color _color) : GeometricObject(_color)
	{
		Pn = _Pn.normalize();
		A = Pn.x;
		B = Pn.y;
		C = Pn.z;
		D = _D;
        center = Pn;
	}

	double testIntersection(Ray ray)
	{
		double t = 0;
		ray.direction = ray.direction.normalize();
		double vd = Pn.dotProduct(ray.direction);
		if (vd == 0)
			return t;
		double v0 = -(Pn.dotProduct(ray.origin) + D);
		t = v0 / vd;
		return t;
	}

	Vec3 computeIntersectionPoint(Ray ray, double t)
	{
		Vec3 p = ray.origin + (ray.direction.normalize() * t);
		return p;
	}

	Vec3 computeNormalIntersection(Ray ray, double t)
	{
		Vec3 norm;
		if (Pn.dotProduct(ray.direction.normalize()) < 0)
			norm = Pn;
		else
			norm = Pn.negate();
		return Pn;
	}
    bool clicked_point(int x, int y){
        
    }
    void computeOrigin(){
        
    }
};
