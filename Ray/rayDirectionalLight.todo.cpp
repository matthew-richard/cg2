#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "rayDirectionalLight.h"
#include "rayScene.h"

////////////////////////
//  Ray-tracing stuff //
////////////////////////
Point3D RayDirectionalLight::getDiffuse(Point3D cameraPosition,RayIntersectionInfo& iInfo){
	double strength = (-this->direction.unit()).dot(iInfo.normal);
	return iInfo.material->diffuse * this->color * (strength >= 0 ? strength : 0);
}
Point3D RayDirectionalLight::getSpecular(Point3D cameraPosition,RayIntersectionInfo& iInfo){
	Point3D toLight = -this->direction.unit();
	Point3D toCamera = (cameraPosition - iInfo.iCoordinate).unit();
	double projection = iInfo.normal.dot(toCamera);
	Point3D reflection = iInfo.normal * projection + -(toCamera - iInfo.normal * projection);

	double cos_angle = toLight.dot(reflection.unit());
	if (cos_angle < 0)
		return Point3D();

	double strength = pow(cos_angle, iInfo.material->specularFallOff);
	return iInfo.material->specular * this->color * strength;
}
int RayDirectionalLight::isInShadow(RayIntersectionInfo& iInfo,RayShape* shape){
	return 0;
}
Point3D RayDirectionalLight::transparency(RayIntersectionInfo& iInfo,RayShape* shape,Point3D cLimit){
	return Point3D(1,1,1);
}

//////////////////
// OpenGL stuff //
//////////////////
void RayDirectionalLight::drawOpenGL(int index){
}