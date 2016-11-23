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
	this->material->drawOpenGL();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(this->center[0], this->center[1], this->center[2]);
	glScalef(this->length[0], this->length[1], this->length[2]);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);
	glDisable(GL_RESCALE_NORMAL);
	glBegin(GL_QUADS);
		// top
		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.5f, 0.5f, 0.5f);
		glVertex3f(0.5f, 0.5f, 0.5f);
		glVertex3f(0.5f, 0.5f, -0.5f);
		glVertex3f(-0.5f, 0.5f, -0.5f);

		// front
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.5f, -0.5f, 0.5f);
		glVertex3f(0.5f, -0.5f, 0.5f);
		glVertex3f(0.5f, 0.5f, 0.5f);
		glVertex3f(-0.5f, 0.5f, 0.5f);

		// right
		glNormal3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.5f, -0.5f, 0.5f);
		glVertex3f(0.5f, -0.5f, -0.5f);
		glVertex3f(0.5f, 0.5f, -0.5f);
		glVertex3f(0.5f, 0.5f, 0.5f);

		// left
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glVertex3f(-0.5f, -0.5f, 0.5f);
		glVertex3f(-0.5f, 0.5f, 0.5f);
		glVertex3f(-0.5f, 0.5f, -0.5f);
		glVertex3f(-0.5f, -0.5f, -0.5f);

		// bottom
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(-0.5f, -0.5f, -0.5f);
		glVertex3f(0.5f, -0.5f, -0.5f);
		glVertex3f(0.5f, -0.5f, 0.5f);
		glVertex3f(-0.5f, -0.5f, 0.5f);

		// back
		glNormal3f(0.0f, 0.0f, -1.0f);
		glVertex3f(0.5f, 0.5f, -0.5f);
		glVertex3f(0.5f, -0.5f, -0.5f);
		glVertex3f(-0.5f, -0.5f, -0.5f);
		glVertex3f(-0.5f, 0.5f, -0.5f);

	glEnd();
	

	glPopMatrix();

	return this->material->index;
}
