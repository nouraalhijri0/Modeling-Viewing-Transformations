#pragma once
#include "Ray.h"
#include "Camera.h"
#include<iostream>
using namespace std;
class PerspectiveCamera : public Camera {
public:
	Vec3 w, u, v;
	
	using Camera::Camera;
    
	void setCameraFrame() {
		w = direction.negate().normalize();
		u = up.crossProduct(w).normalize();
		v = w.crossProduct(u).normalize();
	}

	void getRay(Ray& outRay, double ui, double vj) {
		//In a perspective view, pixels has the same origin but different directions
		outRay.direction = ((w.negate() * distance) + (u * ui) + (v * vj)).normalize();
		outRay.origin = position;
	}
};

std::ostream& operator<< (std::ostream& os, const PerspectiveCamera& camera)
{
	os << "Perspective basis vectors: u = " << camera.u << "; v = " << camera.v << "; w = " << camera.w << "\n";
	return os;
}
