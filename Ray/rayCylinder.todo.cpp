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

	GLUquadric* s = gluNewQuadric();
	gluQuadricDrawStyle(s, GL_POLYGON);
	gluQuadricNormals(s, GLU_SMOOTH);
	gluQuadricOrientation(s, GLU_OUTSIDE);
	glShadeModel(GL_SMOOTH);
	gluCylinder(s, this->radius, this->radius, this->height, this->openGLComplexity, this->openGLComplexity);
	glShadeModel(GL_FLAT);
	gluDisk(s, 0, this->radius, this->openGLComplexity, this->openGLComplexity);
	glTranslatef(0, 0, this->height);
	gluQuadricOrientation(s, GLU_INSIDE);
	gluDisk(s, 0, this->radius, this->openGLComplexity, this->openGLComplexity);
	glTranslatef(0, 0, -this->height);
	gluDeleteQuadric(s);

	return this->material->index;
}
