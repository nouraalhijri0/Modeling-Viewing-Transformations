#pragma once
#include "Ray.h"
#include "Camera.h"
#include<iostream>

using namespace std;

class OrthographicCamera : public Camera {
public:
	Vec3 u, v, w;
	using Camera::Camera;

	void setCameraFrame() {
		w = direction.negate().normalize();
		u = up.crossProduct(w).normalize();
		v = w.crossProduct(u).normalize();
	}

	void getRay(Ray& outRay, double ui, double vj) {
		//In an  view, pixels has the same direction but different origins
		outRay.direction = (w.negate()*distance).normalize();// should normalized
		outRay.origin = position + (u * ui) + (v * vj);
	}

};

std::ostream& operator<< (std::ostream& os, const OrthographicCamera& camera)
{
	os << "Orthographic basis vectors: u = " << camera.u << "; v = " << camera.v << "; w = " << camera.w << "\n";
	return os;
}
