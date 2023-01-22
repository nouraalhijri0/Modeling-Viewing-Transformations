#pragma once
#include "Vec3.h"
#include "Color.h"
#include "Ray.h"
#include "GeometricObject.h"
#include<cmath>
#include<iostream>

using namespace std;

class Sphere : public GeometricObject {
public:
	Vec3 center;
	double radius;
	double A, B, C, D, E, F, G, H, I, J;
    
	// #TODO: You can declare some additional variables for computation in here

	Sphere(Vec3 _center, double _radius, Color _color) : GeometricObject(_color), center(_center), radius(_radius) 
	{
		A = B = C = D = E = F = G = H = I = J = 0.0f;
        
	}

	double testIntersection(Ray ray)
	{
		double t = -std::numeric_limits<double>::infinity();

		A = 1.0f;
		D = -center.x;
		E = 1.0f;
		G = -center.y;
		H = 1.0f;
		I = -center.z;
		J = center.x * center.x + center.y * center.y + center.z * center.z - radius * radius;

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
		Vec3 norm = ((p - center) * (1.0f / radius)).normalize();
		return norm;
	}
};
