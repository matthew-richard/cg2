#include <stdlib.h>
#include <math.h>

#include <SVD/SVDFit.h>
#include <SVD/MatrixMNTC.h>

#include "geometry.h"


///////////////////////
// Ray-tracing stuff //
///////////////////////
double BoundingBox3D::intersect(const Ray3D& ray) const {
	return -1;
}

static void swapDoubles(double& a, double& b) {
	double temp = a;
	a = b;
	b = temp;
}

/////////////////////
// Animation stuff //
/////////////////////
Matrix3D::Matrix3D(const Point3D& e){
	(*this)=Matrix3D();
	Point3D pt = e; // indexing a point isn't const for some reason

	m[0][0] = cos(pt[1]) * cos(pt[2]);
	m[0][1] = cos(pt[0]) * sin(pt[2]) + cos(pt[2]) * sin(pt[0]) * sin(pt[1]);
	m[0][2] = sin(pt[0]) * sin(pt[2]) - cos(pt[0]) * cos(pt[2]) * sin(pt[1]);
	m[1][0] = -cos(pt[1]) * sin(pt[2]);
	m[1][1] = cos(pt[0]) * cos(pt[2]) - sin(pt[0]) * sin(pt[1]) * sin(pt[2]);
	m[1][2] = cos(pt[2]) * sin(pt[0]) + cos(pt[0]) * sin(pt[1]) * sin(pt[2]);
	m[2][0] = sin(pt[1]);
	m[2][1] = -cos(pt[1]) * sin(pt[0]);
	m[2][2] = cos(pt[0]) * cos(pt[1]);

	/*
	m[0][0] = cos(pt[2]) * cos(pt[0]) - cos(pt[1]) * sin(pt[0]) * sin(pt[2]);
	m[1][0] = cos(pt[2]) * sin(pt[0]) + cos(pt[1]) * cos(pt[0]) * sin(pt[2]);
	m[2][0] = sin(pt[2]) * sin(pt[1]);
	m[0][1] = -sin(pt[2]) * cos(pt[0]) - cos(pt[1]) * sin(pt[0]) * cos(pt[2]);
	m[1][1] = -sin(pt[2]) * sin(pt[0]) + cos(pt[1]) * cos(pt[0]) * cos(pt[2]);
	m[2][1] = cos(pt[2]) * sin(pt[1]);
	m[0][2] = sin(pt[1]) * sin(pt[0]);
	m[1][2] = -sin(pt[1]) * cos(pt[0]);
	m[2][2] = cos(pt[1]);
	*/
}

Matrix3D::Matrix3D(const Quaternion& q){
	(*this)=Matrix3D();
	Quaternion r = q; // get rid of const
	m[0][0] = 1 - 2 * r.imag[1] * r.imag[1] - 2 * r.imag[2] * r.imag[2];
	m[0][1] = 2 * r.imag[0] * r.imag[1] + 2 * r.real * r.imag[2];
	m[0][2] = 2 * r.imag[0] * r.imag[2] - 2 * r.real * r.imag[1];
	m[1][0] = 2 * r.imag[0] * r.imag[1] - 2 * r.real * r.imag[2];
	m[1][1] = 1 - 2 * r.imag[0] * r.imag[0] - 2 * r.imag[2] * r.imag[2];
	m[1][2] = 2 * r.imag[1] * r.imag[2] + 2 * r.real * r.imag[0];
	m[2][0] = 2 * r.imag[0] * r.imag[2] + 2 * r.real * r.imag[1];
	m[2][1] = 2 * r.imag[1] * r.imag[2] - 2 * r.real * r.imag[0];
	m[2][2] = 1 - 2 * r.imag[0] * r.imag[0] - 2 * r.imag[1] * r.imag[1];
}
Matrix3D Matrix3D::closestRotation(void) const {
	Matrix3D r1, r2, diag;
	this->SVD(r1, diag, r2);
	for (int i = 0; i < 3; i++) {
		if (diag(i,i))
			diag(i,i) = diag(i,i) > 0 ? 1 : -1;
	}
	return r1 * diag * r2;
}

/* Helper functions */
static Matrix3D pow(const Matrix3D& m, int exponent);
static unsigned long factorial(unsigned long n);

/* While these Exp and Log implementations are the direct implementations of the Taylor series, the Log
 * function tends to run into convergence issues so we use the other ones:*/
Matrix3D Matrix3D::Exp(const Matrix3D& m,int iter){
	Matrix3D mat = Matrix3D::IdentityMatrix();

	for (int i = 1; i <= iter; i++) {
		mat += pow(m, i) / factorial(i);
	}
	
	return mat;
}

static Matrix3D pow(const Matrix3D& m, int exponent) {
	Matrix3D ret = Matrix3D::IdentityMatrix();
	for (; exponent > 0; exponent--) ret *= m;
	return ret;
}

static unsigned long factorial(unsigned long n) {
	unsigned long ret = 1;
	for (; n > 0; n--) ret *= n;
	return ret;
}
