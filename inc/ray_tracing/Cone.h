#pragma once
#include "Vec3.h"
#include "Color.h"
#include "Ray.h"
#include "GeometricObject.h"
#include "Plane.h"
#include<cmath>
#include<iostream>

using namespace std;

class Cone : public GeometricObject {
public:
	double radius;
	double height;
	Vec3 apex; // tip or vertex or apex of cone
    Vec3 center;

	// #TODO: You can declare some additional variables for computation in here

	Cone(Vec3 _apex, double _radius, double _height, Color _color) : GeometricObject(_color), apex(_apex), radius(_radius), height(_height)
	{
        //origin = center;
	}

	double testIntersection(Ray ray)
	{
		// #TODO: Implement function to check ray intersects with cone or not, return t
	}

	Vec3 computeIntersectionPoint(Ray ray, double t)
	{
		// #TODO: Implement function to find intesection point, return intersection point
	}

	Vec3 computeNormalIntersection(Ray ray, double t)
	{
		// #TODO: Implement function to find normal vector at intesection point, return normal vector
	}
    bool clicked_point(int x, int y){
        
    }
    void computeOrigin(){
        
    }
};
