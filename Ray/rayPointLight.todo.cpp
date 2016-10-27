#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "rayPointLight.h"
#include "rayScene.h"

////////////////////////
//  Ray-tracing stuff //
////////////////////////
Point3D RayPointLight::getDiffuse(Point3D cameraPosition,RayIntersectionInfo& iInfo){
	double strength = ((this->location - iInfo.iCoordinate).unit()).dot(iInfo.normal);
	double dist = (this->location - iInfo.iCoordinate).length();
	double intensity = 1 / (this->constAtten + this->linearAtten * dist + this->quadAtten * dist * dist);
	return iInfo.material->diffuse * this->color * intensity * (strength >= 0 ? strength : 0);
}
Point3D RayPointLight::getSpecular(Point3D cameraPosition,RayIntersectionInfo& iInfo){
	Point3D toLight = (this->location - iInfo.iCoordinate).unit();
	Point3D toCamera = (cameraPosition - iInfo.iCoordinate).unit();
	double projection = iInfo.normal.dot(toCamera);
	Point3D reflection = iInfo.normal * projection + -(toCamera - iInfo.normal * projection);

	double cos_angle = toLight.dot(reflection.unit());
	if (cos_angle < 0)
		return Point3D();

	double strength = pow(cos_angle, iInfo.material->specularFallOff);

	double dist = (this->location - iInfo.iCoordinate).length();
	double intensity = 1 / (this->constAtten + this->linearAtten * dist + this->quadAtten * dist * dist);

	return iInfo.material->specular * this->color * intensity * strength;
}
int RayPointLight::isInShadow(RayIntersectionInfo& iInfo,RayShape* shape){
	return 0;
}
Point3D RayPointLight::transparency(RayIntersectionInfo& iInfo,RayShape* shape,Point3D cLimit){
	return Point3D(1,1,1);
}


//////////////////
// OpenGL stuff //
//////////////////
void RayPointLight::drawOpenGL(int index){
}