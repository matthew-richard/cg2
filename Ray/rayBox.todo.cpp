#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "rayScene.h"
#include "rayBox.h"

////////////////////////
//  Ray-tracing stuff //
////////////////////////
double RayBox::intersect(Ray3D ray,RayIntersectionInfo& iInfo,double mx){
	return -1;
}
BoundingBox3D RayBox::setBoundingBox(void){
	return bBox;
}

//////////////////
// OpenGL stuff //
//////////////////
int RayBox::drawOpenGL(int materialIndex){
	if (this->material->index != materialIndex)
		this->material->drawOpenGL();

	glMatrixMode(GL_MODELVIEW);
	glTranslatef(this->center[0], this->center[1], this->center[2]);
	glScalef(this->length[0], this->length[1], this->length[2]);

	glShadeModel(GL_FLAT);
	glBegin(GL_TRIANGLE_STRIP);
		// major strip
		glVertex3f(0.5, 0.5, 0.5);
		glVertex3f(-0.5, 0.5, 0.5);
		glVertex3f(0.5, -0.5, 0.5);
		glVertex3f(-0.5, -0.5, 0.5);
		glVertex3f(0.5, -0.5, -0.5);
		glVertex3f(-0.5, -0.5, -0.5);
		glVertex3f(0.5, 0.5, -0.5);
		glVertex3f(-0.5, 0.5, -0.5);
		glVertex3f(0.5, 0.5, 0.5);
		glVertex3f(-0.5, 0.5, 0.5);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
		// left side
		glVertex3f(-0.5, 0.5, 0.5);
		glVertex3f(-0.5, 0.5, -0.5);
		glVertex3f(-0.5, -0.5, 0.5);
		glVertex3f(-0.5, -0.5, -0.5);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
		// right side
		glVertex3f(0.5, 0.5, -0.5);
		glVertex3f(0.5, 0.5, 0.5);
		glVertex3f(0.5, -0.5, -0.5);
		glVertex3f(0.5, -0.5, 0.5);
	glEnd();

	glScalef(1 / this->length[0], 1 / this->length[1], 1 / this->length[2]);
	glTranslatef(-this->center[0], -this->center[1], -this->center[2]);

	return this->material->index;
}
