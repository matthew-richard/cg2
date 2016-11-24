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
	Ray3D ray = Ray3D(iInfo.iCoordinate, (this->location - iInfo.iCoordinate).unit());
	return shape->intersect(ray, RayIntersectionInfo(), (this->location - iInfo.iCoordinate).length()) > 0;
}
Point3D RayPointLight::transparency(RayIntersectionInfo& iInfo,RayShape* shape,Point3D cLimit){
	Ray3D ray = Ray3D(iInfo.iCoordinate, (this->location - iInfo.iCoordinate).unit());
	RayIntersectionInfo tempInfo;
	if (shape->intersect(ray, tempInfo) > 0)
		return tempInfo.material->transparent;
	return Point3D(1, 1, 1);
}


//////////////////
// OpenGL stuff //
//////////////////
void RayPointLight::drawOpenGL(int index){
	GLfloat c[] = { this->color[0], this->color[1], this->color[2], 1 };
	GLfloat p[] = { this->location[0], this->location[1], this->location[2], 1 };
	GLfloat zero[] = { 0, 0, 0, 1 };
	GLfloat ones[] = { 1, 1, 1, 1 };

	// Light is not infinitely far away (only affects specular)
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

	// Ambient light is contributed by materials, not individual lights
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, zero);

	glLightfv(GL_LIGHT0 + index, GL_AMBIENT, ones);
	glLightfv(GL_LIGHT0 + index, GL_SPECULAR, c);
	glLightfv(GL_LIGHT0 + index, GL_DIFFUSE, c);

	glLightfv(GL_LIGHT0 + index, GL_POSITION, p);

	glLightf(GL_LIGHT0 + index, GL_SPOT_EXPONENT, 0);
	glLighti(GL_LIGHT0 + index, GL_SPOT_CUTOFF, 180);

	glLightf(GL_LIGHT0 + index, GL_CONSTANT_ATTENUATION, this->constAtten);
	glLightf(GL_LIGHT0 + index, GL_LINEAR_ATTENUATION, this->linearAtten);
	glLightf(GL_LIGHT0 + index, GL_QUADRATIC_ATTENUATION, this->quadAtten);

	glEnable(GL_LIGHT0 + index);
}