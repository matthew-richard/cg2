#include <GL/glew.h>
#include <GL/glut.h>
#include <math.h>
#include "rayCamera.h"



//////////////////
// OpenGL stuff //
//////////////////
void RayCamera::drawOpenGL(void){
	Point3D refPoint = this->position + this->direction.unit();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(this->position[0], this->position[1], this->position[2],
		refPoint[0], refPoint[1], refPoint[2], 0, 1, 0);
}
void RayCamera::rotateUp(Point3D center,float angle){
}
void RayCamera::rotateRight(Point3D center,float angle){
}
void RayCamera::moveForward(float dist){
}
void RayCamera::moveRight(float dist){
}
void RayCamera::moveUp(float dist){
}
