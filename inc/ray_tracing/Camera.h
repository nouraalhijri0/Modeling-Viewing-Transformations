#pragma once
#include "Vec3.h"
#include "Ray.h"
/* Base class for two types of camera*/
#pragma once
#include<iostream>
using namespace std;

class Camera{
public:
	Vec3 position;
	Vec3 direction;
	Vec3 up;
	Vec3 lookAt;
    double distance = 1.0f;
    
	// #TODO: You can declare some additional variables for computation in here

	Camera(Vec3 _position, Vec3 _direction, Vec3 _up): position(_position), direction(_direction), up(_up) 
	{
        computeLookAt();
	}
    Vec3 computeLookAt(){
        lookAt = position + direction;
        return lookAt;
    }
    
	Vec3 computeDirectionFromLookAt() {
		return position - lookAt;
	}
	Vec3 getPostion() {
		return position;
	}
	virtual void setCameraFrame() = 0;	
	virtual void getRay(Ray &outRay, double ui, double vj) = 0;
};


