#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "rayGroup.h"
#include "raySphere.h"

////////////////////////
//  Ray-tracing stuff //
////////////////////////
double RayGroup::intersect(Ray3D ray,RayIntersectionInfo& iInfo,double mx){
	double result = -1;
	RayIntersectionInfo tempInfo;

	Ray3D ray_local = this->getInverseMatrix() * ray;

	double dist_ratio = ray.direction.length() / ray_local.direction.length();
	double mx_local = mx < 0 ? -1 : mx / dist_ratio;

	ray_local.direction = ray_local.direction.unit();

	for (int i = 0; i < sNum; i++) {
		// Convert local distance to world distance
		double rayDist = shapes[i]->intersect(ray_local, tempInfo, mx_local) * dist_ratio;

		if (rayDist > 0 && (rayDist < mx || mx <= 0) && (rayDist < result || result == -1)) {
			result = rayDist;

			iInfo.iCoordinate = this->getMatrix() * tempInfo.iCoordinate;
			iInfo.material = tempInfo.material;
			iInfo.normal = this->getNormalMatrix().multDirection(tempInfo.normal).unit();
			iInfo.texCoordinate = tempInfo.texCoordinate;
		}
	}

	return result;
}

BoundingBox3D RayGroup::setBoundingBox(void){
	return bBox;
}

int StaticRayGroup::set(void){
	this->inverseTransform = this->localTransform.invert();
	this->normalTransform = this->localTransform.transpose().invert();
	return 1;
}
//////////////////
// OpenGL stuff //
//////////////////

int RayGroup::drawOpenGL(int materialIndex){
	// column-major order
	Matrix4D& mat = this->getMatrix();
	GLdouble m[16] = {mat(0,0), mat(0,1), mat(0,2), mat(0,3),
					  mat(1,0), mat(1,1), mat(1,2), mat(1,3),
				      mat(2,0), mat(2,1), mat(2,2), mat(2,3),
					  mat(3,0), mat(3,1), mat(3,2), mat(3,3)};

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixd(m);

	int ret = 0;
	for (int i = 0; i < this->shapeNum(); i++) {
		materialIndex = shapes[i]->drawOpenGL(materialIndex);
		if (materialIndex < 0)
			ret = -1;
	}

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	return ret;
}

/////////////////////
// Animation Stuff //
/////////////////////
Matrix4D ParametrizedEulerAnglesAndTranslation::getMatrix(void){
	Matrix4D mat = Matrix4D(Matrix3D(this->value->eulerAngles));
	mat(3,0) = this->value->translate[0];
	mat(3,1) = this->value->translate[1];
	mat(3,2) = this->value->translate[2];

	return mat;
}
Matrix4D ParametrizedClosestRotationAndTranslation::getMatrix(void){
	Matrix4D mat = Matrix4D(this->value->rotation.closestRotation());
	mat(3,0) = this->value->translate[0];
	mat(3,1) = this->value->translate[1];
	mat(3,2) = this->value->translate[2];
	
	return mat;
}
Matrix4D ParametrizedRotationLogarithmAndTranslation::getMatrix(void){
	Matrix4D mat = Matrix4D(Matrix3D::Exp(this->value->skewSymmetric));
	mat(3,0) = this->value->translate[0];
	mat(3,1) = this->value->translate[1];
	mat(3,2) = this->value->translate[2];

	return mat;
}
Matrix4D ParametrizedQuaternionAndTranslation::getMatrix(void){
	Matrix4D mat = Matrix4D(Matrix3D(this->value->quaternion));
	mat(3,0) = this->value->translate[0];
	mat(3,1) = this->value->translate[1];
	mat(3,2) = this->value->translate[2];

	return mat;
}
