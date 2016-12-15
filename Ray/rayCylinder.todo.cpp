#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "rayScene.h"
#include "rayCylinder.h"


////////////////////////
//  Ray-tracing stuff //
////////////////////////
double RayCylinder::intersect(Ray3D ray,RayIntersectionInfo& iInfo,double mx){
	return -1;
}

BoundingBox3D RayCylinder::setBoundingBox(void){
	return bBox;
}

//////////////////
// OpenGL stuff //
//////////////////
int RayCylinder::drawOpenGL(int materialIndex){
	this->material->drawOpenGL();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(this->center[0], this->center[1], this->center[2]);
	glRotatef(90, 1, 0, 0);
	glTranslatef(0, 0, -this->height/2);
	
	GLUquadric* s = gluNewQuadric();
	gluQuadricDrawStyle(s, GL_POLYGON);
	gluQuadricNormals(s, GLU_SMOOTH);
	gluQuadricOrientation(s, GLU_OUTSIDE);
	glShadeModel(GL_SMOOTH);
	gluCylinder(s, this->radius, this->radius, this->height, this->openGLComplexity, this->openGLComplexity);
	gluQuadricOrientation(s, GLU_INSIDE);
	glShadeModel(GL_FLAT);
	gluDisk(s, 0, this->radius, this->openGLComplexity, this->openGLComplexity);
	glTranslatef(0, 0, this->height);
	gluQuadricOrientation(s, GLU_OUTSIDE);
	gluDisk(s, 0, this->radius, this->openGLComplexity, this->openGLComplexity);

	gluDeleteQuadric(s);
	glPopMatrix();

	return this->material->index;
}
