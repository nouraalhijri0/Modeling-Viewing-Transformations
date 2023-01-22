
#pragma once
#include "Vec3.h"
#include <iostream>
#include <cmath>

// Vec4 for homogeneous coordinate
class Vec4 {

public:
	double x;
	double y;
	double z;
	double w;
	Vec4() :x(0), y(0), z(0), w(0) {}
	Vec4(double _x, double _y, double _z, double _w) :x(_x), y(_y), z(_z), w(_w) {}
	Vec4(Vec3 a, double _w) :x(a.x), y(a.y), z(a.z), w(_w) {}
	static Vec3 toVec3(Vec4 a)
	{
		return Vec3(a.x, a.y, a.z);
	}
};

// Matrix4 for transformation
class Matrix4 {
public:
	double matrix[4][4];

	Matrix4() {
		identity();
	}
	void identity()
	{
		// #TODO: Implement function to create 4x4 identity matrix
        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                if(i==j)
                    matrix[i][j] = 1;
                else
                    matrix[i][j] = 0;
            }
        }
	}
	Matrix4 operator *(Matrix4 a)
	{
		// #TODO: Implement function for matrix multiplication
        Matrix4 multiplication ;
        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                double m = 0;
                for(int k=0;k<4;k++){
                    m = m + matrix[i][k]*a.matrix[k][j];
                }
                multiplication.matrix[i][j] = m;
            }
        }
        return multiplication;
	}
	Vec4 operator *(Vec4 a)
	{
		// #TODO: Implement function for matrix - vector multiplication
        double x, y, z, w;
        x = matrix[0][0]*a.x + matrix[0][1]*a.y + matrix[0][2]*a.z + matrix[0][3]*a.w;
        y = matrix[1][0]*a.x + matrix[1][1]*a.y + matrix[1][2]*a.z + matrix[1][3]*a.w;
        z = matrix[2][0]*a.x + matrix[2][1]*a.y + matrix[2][2]*a.z + matrix[2][3]*a.w;
        w = matrix[3][0]*a.x + matrix[3][1]*a.y + matrix[3][2]*a.z + matrix[3][3]*a.w;
        Vec4 vec(x,y,z,w);
        return vec;
        
	}
	Matrix4 transpose()
	{
		// #TODO: Implement function to calculate transpose of a matrix
        Matrix4 matrixT;
        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                matrixT.matrix[j][i] = matrix[i][j];
            }
        }
        return matrixT;
        
	}
	static Matrix4 Translate(Vec3 t) //Matrix4.translate(vec3)  return translation matrix
	{
		// #TODO: Implement function to calculate translation matrix
        Matrix4 matrixTrans;
        matrixTrans.matrix[0][3] = t.x;
        matrixTrans.matrix[1][3] = t.y;
        matrixTrans.matrix[2][3] = t.z;
        return matrixTrans;
	}
	static Matrix4 TranslateInv(Vec3 t)
	{
		// #TODO: Implement function to calculate inverse of translation matrix
        
	}
	static Matrix4 Scaling(Vec3 t)
	{
		// #TODO: Implement function to calculate scaling matrix
        Matrix4 matrixScale;
        matrixScale.identity();
        matrixScale.matrix[0][0] = t.x;
        matrixScale.matrix[1][1] = t.y;
        matrixScale.matrix[2][2] = t.z;
        return matrixScale;

	}
	static Matrix4 ScalingInv(Vec3 t)
	{
		// #TODO: Implement function to calculate inverse of scaling matrix

	}
	static Matrix4 RotationX(double theta)
	{
		// #TODO: Implement function to calculate rotation matrix - X axis
        Matrix4 matrixRotateX;
        matrixRotateX.identity();
        matrixRotateX.matrix[1][1] = float(cos(theta));
        matrixRotateX.matrix[1][2] = -float(sin(theta));
        matrixRotateX.matrix[2][1] = float(sin(theta));
        matrixRotateX.matrix[2][2] = float(cos(theta));
        return matrixRotateX;

	}
	static Matrix4 RotationXInv(double theta)
	{
		// #TODO: Implement function to calculate inverse of rotation matrix - X axis
	}
	static Matrix4 RotationY(double theta)
	{
		// #TODO: Implement function to calculate rotation matrix - Y axis
        Matrix4 matrixRotateY;
        matrixRotateY.identity();
        matrixRotateY.matrix[0][0] = float(cos(theta));
        matrixRotateY.matrix[2][0] = -float(sin(theta));
        matrixRotateY.matrix[0][2] = float(sin(theta));
        matrixRotateY.matrix[2][2] = float(cos(theta));
        return matrixRotateY;

	}
	static Matrix4 RotationYInv(double theta)
	{
		// #TODO: Implement function to calculate inverse of rotation matrix - Y axis
	}
	static Matrix4 RotationZ(double theta)
	{
		// #TODO: Implement function to calculate rotation matrix - Z axis
        Matrix4 matrixRotateZ;
        matrixRotateZ.identity();
        matrixRotateZ.matrix[0][0] = float(cos(theta));
        matrixRotateZ.matrix[0][1] = -float(sin(theta));
        matrixRotateZ.matrix[1][0] = float(sin(theta));
        matrixRotateZ.matrix[1][1] = float(cos(theta));
        return matrixRotateZ;
	}
    
	static Matrix4 RotationZInv(double theta)
	{
		// #TODO: Implement function to calculate inverse of rotation matrix - Z axis
	}

	static Matrix4 Rotation(Vec3 theta)
	{
		// #TODO: Implement function to calculate rotation matrix for three axes
		// theta.x - rotate angle around X axis
        Matrix4 r1 = Matrix4::RotationX(theta.x);
		// theta.y - rotate angle around Y axis
        Matrix4 r2 = Matrix4::RotationY(theta.y);
		// theta.z - rotate angle around Z axis
        Matrix4 r3 = Matrix4::RotationZ(theta.z);
        return (r1*r2)*r3;
        
	}
	static Matrix4 RotationInv(Vec3 theta)
	{
		// #TODO: Implement function to calculate inverse of rotation matrix for three axes
        // theta.x - rotate angle around X axis
        Matrix4 r1 = Matrix4::RotationX(theta.x);
        // theta.y - rotate angle around Y axis
        Matrix4 r2 = Matrix4::RotationY(theta.y);
        // theta.z - rotate angle around Z axis
        Matrix4 r3 = Matrix4::RotationZ(theta.z);
        return (r3*r2)*r1;
	}
};

std::ostream& operator<< (std::ostream& os, const Matrix4& m)
{
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			os << m.matrix[i][j] << "\t";
		}
		os << "\n";
	}
	return os;
}
