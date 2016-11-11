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

	for (int i = 0; i < this->shapeNum(); i++) {
		if (shapes[i]->drawOpenGL(materialIndex) < 0)
			return -1;
	}
	return 0;
}

/////////////////////
// Animation Stuff //
/////////////////////
Matrix4D ParametrizedEulerAnglesAndTranslation::getMatrix(void){
	return Matrix4D::IdentityMatrix();
}
Matrix4D ParametrizedClosestRotationAndTranslation::getMatrix(void){
	return Matrix4D::IdentityMatrix();
}
Matrix4D ParametrizedRotationLogarithmAndTranslation::getMatrix(void){
	return Matrix4D::IdentityMatrix();
}
Matrix4D ParametrizedQuaternionAndTranslation::getMatrix(void){
	return Matrix4D::IdentityMatrix();
}
