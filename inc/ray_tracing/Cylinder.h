#pragma once
#include "Vec3.h"
#include "Color.h"
#include "Ray.h"
#include "GeometricObject.h"
#include "Plane.h"
#include<cmath>
#include<iostream>

using namespace std;
	
class Cylinder : public GeometricObject {
public:
    double radius;
    double height;
    Vec3 center;
    
    // #TODO: You can declare some additional variables for computation in here

    Cylinder(Vec3 _center, double _radius, double _height, Color _color) : GeometricObject(_color), center(_center), radius(_radius), height(_height)
    {
        //origin = center;
    }

    double testIntersection(Ray ray)
    {
        // #TODO: Implement function to check ray intersects with cylinder or not, return t
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

