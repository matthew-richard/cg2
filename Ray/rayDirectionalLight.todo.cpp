#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "rayDirectionalLight.h"
#include "rayScene.h"

////////////////////////
//  Ray-tracing stuff //
////////////////////////
Point3D RayDirectionalLight::getDiffuse(Point3D cameraPosition,RayIntersectionInfo& iInfo){
	double strength = (-this->direction.unit()).dot(iInfo.normal.unit());
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
	Ray3D ray = Ray3D(iInfo.iCoordinate, -this->direction.unit());
	return shape->intersect(ray, RayIntersectionInfo()) > 0;
}
Point3D RayDirectionalLight::transparency(RayIntersectionInfo& iInfo,RayShape* shape,Point3D cLimit){
	Ray3D ray = Ray3D(iInfo.iCoordinate, -this->direction.unit());
	RayIntersectionInfo tempInfo;
	if (shape->intersect(ray, tempInfo) > 0)
		return tempInfo.material->transparent;
	return Point3D(1,1,1);
}

//////////////////
// OpenGL stuff //
//////////////////
void RayDirectionalLight::drawOpenGL(int index){
	GLfloat c[] = { this->color[0], this->color[1], this->color[2], 1 };
	GLfloat d[] = { this->direction[0], this->direction[1], -this->direction[2] };
	GLfloat zero[] = { 0, 0, 0, 1 };

	// 0 at the end indicates that this is a directional light.
	GLfloat p[] = { -d[0], -d[1], -d[2], 0 };

	// Light is infinitely far away (only affects specular lighting)
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);

	// Ambient light is contributed by materials, not individual lights
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, zero);
	
	glLightfv(GL_LIGHT0+index, GL_AMBIENT, zero);
	glLightfv(GL_LIGHT0+index, GL_SPECULAR, c);
	glLightfv(GL_LIGHT0+index, GL_DIFFUSE, c);

	glLightfv(GL_LIGHT0+index, GL_POSITION, p);

	glLighti(GL_LIGHT0 + index, GL_SPOT_EXPONENT, 0);
	glLighti(GL_LIGHT0 + index, GL_SPOT_CUTOFF, 180);

	glEnable(GL_LIGHT0+index);
}