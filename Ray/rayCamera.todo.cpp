#include <GL/glew.h>
#include <GL/glut.h>
#include <math.h>
#include "rayCamera.h"


static Point3D rotateAboutAxis(const Point3D & p, const Point3D & axis, float angle);

//////////////////
// OpenGL stuff //
//////////////////
void RayCamera::drawOpenGL(void){
	Point3D refPoint = this->position + this->direction.unit();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(this->position[0], this->position[1], this->position[2],
		refPoint[0], refPoint[1], refPoint[2],
		this->up[0],this->up[1], this->up[2]);
}
void RayCamera::rotateUp(Point3D center,float angle){
	Point3D pos = this->position - center;
	Point3D rotated = rotateAboutAxis(pos, this->up, -angle);

	this->position = rotated + center;
	this->direction = rotateAboutAxis(this->direction, this->up, -angle);
}
void RayCamera::rotateRight(Point3D center,float angle){
	Point3D pos = this->position - center;
	Point3D right = this->up.crossProduct(this->direction);
	Point3D rotated = rotateAboutAxis(pos, right, angle);

	this->position = rotated + center;
	this->direction = rotateAboutAxis(this->direction, right, angle);

	this->up = this->direction.crossProduct(right);
}
void RayCamera::moveForward(float dist){
	this->position += this->direction.unit() * dist;
}
void RayCamera::moveRight(float dist){
	this->position += -this->up.crossProduct(this->direction).unit() * dist;
}
void RayCamera::moveUp(float dist){
	this->position += this->up.unit() * dist;
}

static Point3D rotateAboutAxis(const Point3D & p, const Point3D & axis, float angle) {
	Point3D a = axis.unit();
	float c = cos(angle);
	float s = sin(angle);

	Matrix3D R;

	// First row
	R(0, 0) = c + a[0] * a[0] * (1 - c);
	R(1, 0) = a[0] * a[1] * (1 - c) - a[2] * s;
	R(2, 0) = a[0] * a[2] * (1 - c) + a[1] * s;
	
	// Second row
	R(0, 1) = a[1] * a[0] * (1 - c) + a[2] * s;
	R(1, 1) = c + a[1] * a[1] * (1 - c);
	R(2, 1) = a[1] * a[2] * (1 - c) - a[0] * s;

	// Third row
	R(0, 2) = a[2] * a[0] * (1 - c) - a[1] * s;
	R(1, 2) = a[2] * a[1] * (1 - c) + a[0] * s;
	R(2, 2) = c + a[2] * a[2] * (1 - c);

	return R * p;
}
