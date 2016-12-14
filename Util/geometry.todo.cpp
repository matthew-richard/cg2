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

/////////////////////
// Animation stuff //
/////////////////////
Matrix3D::Matrix3D(const Point3D& e){
	(*this)=Matrix3D();
	Point3D pt = e; // indexing a point isn't const for some reason
	
	m[0][0] = cos(pt[2]) * cos(pt[0]) - cos(pt[1]) * sin(pt[0]) * sin(pt[2]);
	m[1][0] = cos(pt[2]) * sin(pt[0]) + cos(pt[1]) * cos(pt[0]) * sin(pt[2]);
	m[2][0] = sin(pt[2]) * sin(pt[1]);
	m[0][1] = -sin(pt[2]) * cos(pt[0]) - cos(pt[1]) * sin(pt[0]) * cos(pt[2]);
	m[1][1] = -sin(pt[2]) * sin(pt[0]) + cos(pt[1]) * cos(pt[0]) * cos(pt[2]);
	m[2][1] = cos(pt[2]) * sin(pt[1]);
	m[0][2] = sin(pt[1]) * sin(pt[0]);
	m[1][2] = -sin(pt[1]) * cos(pt[0]);
	m[2][2] = cos(pt[1]);
}

Matrix3D::Matrix3D(const Quaternion& q){
	(*this)=Matrix3D();
}
Matrix3D Matrix3D::closestRotation(void) const {
	return (*this);
}
/* While these Exp and Log implementations are the direct implementations of the Taylor series, the Log
 * function tends to run into convergence issues so we use the other ones:*/
Matrix3D Matrix3D::Exp(const Matrix3D& m,int iter){
	return m;
}
