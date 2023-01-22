#pragma once
#include "Vec3.h"
#include "Color.h"
#include "Ray.h"
#include "GeometricObject.h"
#include<cmath>
#include<iostream>

using namespace std;

class Ellipsoid : public GeometricObject {
public:
	double aRadius; // radius along x axis
	double bRadius; // radius along y axis
	double cRadius; // radius along z axis
	Vec3 center;
	double A, B, C, D, E, F, G, H, I, J;

	// #TODO: You can declare some additional variables for computation in here

	Ellipsoid(Vec3 _center, double _aRadius, double _bRadius, double _cRadius, Color _color) : GeometricObject(_color), center(_center), aRadius(_aRadius), bRadius(_bRadius), cRadius(_cRadius) 
	{
		A = B = C = D = E = F = G = H = I = J = 0.0f;
        
	}

	double testIntersection(Ray ray)
	{
		double t = -std::numeric_limits<double>::infinity();
		double aRadiusSquare = aRadius * aRadius;
		double bRadiusSquare = bRadius * bRadius;
		double cRadiusSquare = cRadius * cRadius;

		A = 1.0f / aRadiusSquare;
		D = -center.x / aRadiusSquare;
		E = 1.0f / bRadiusSquare;
		G = -center.y / bRadiusSquare;
		H = 1.0f / cRadiusSquare;
		I = -center.z / cRadiusSquare;
		J = (center.x * center.x) / aRadiusSquare + (center.y * center.y) / bRadiusSquare
			+ (center.z * center.z) / cRadiusSquare - 1;

		ray.direction = ray.direction.normalize();

		double Aq = A * (ray.direction.x * ray.direction.x) + E * (ray.direction.y * ray.direction.y) + H * (ray.direction.z * ray.direction.z);
		double Bq = 2.0f * ((A * ray.origin.x * ray.direction.x) + (D * ray.direction.x)
			+ (E * ray.origin.y * ray.direction.y) + (G * ray.direction.y)
			+ (H * ray.origin.z * ray.direction.z) + (I * ray.direction.z));
		double Cq = (A * (ray.origin.x * ray.origin.x)) + (2.0f * D * ray.origin.x)
			+ (E * (ray.origin.y * ray.origin.y)) + (2.0 * G * ray.origin.y)
			+ (H * (ray.origin.z * ray.origin.z)) + (2.0f * I * ray.origin.z) + J;


		double discriminant = (Bq * Bq) - (4.0f * Aq * Cq);

		if (Aq == 0)
			return -1 * Cq / Bq;
		if (discriminant >= 0)
		{
			double t0 = (-Bq - sqrt(discriminant)) / (2.0f * Aq);
			double t1 = (-Bq + sqrt(discriminant)) / (2.0f * Aq);
			t = t0 < t1 ? t0 : t1;
		}
		return t;
	}

	Vec3 computeIntersectionPoint(Ray ray, double t)
	{
		Vec3 p = ray.origin + (ray.direction.normalize() * t);
		return p;
	}

	Vec3 computeNormalIntersection(Ray ray, double t)
	{
		Vec3 p = computeIntersectionPoint(ray, t);
		Vec3 norm;
		norm.x = 2.0f * (A * p.x + D);
		norm.y = 2.0f * (E * p.y + G);
		norm.z = 2.0f * (H * p.z + I);
		return norm.normalize();
	}
    bool clicked_point(int x, int y){
        
    }

};
