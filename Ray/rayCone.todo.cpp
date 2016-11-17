#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "rayScene.h"
#include "rayCone.h"


////////////////////////
//  Ray-tracing stuff //
////////////////////////
double RayCone::intersect(Ray3D ray,RayIntersectionInfo& iInfo,double mx){
	return -1;
}

BoundingBox3D RayCone::setBoundingBox(void){
	return bBox;
}

//////////////////
// OpenGL stuff //
//////////////////
int RayCone::drawOpenGL(int materialIndex){
	if (this->material->index != materialIndex)
		this->material->drawOpenGL();

	GLUquadric* s = gluNewQuadric();
	gluQuadricDrawStyle(s, GL_POLYGON);
	gluQuadricNormals(s, GLU_SMOOTH);
	gluQuadricOrientation(s, GLU_OUTSIDE);
	glShadeModel(GL_SMOOTH);
	gluCylinder(s, this->radius, 0, this->height, this->openGLComplexity, this->openGLComplexity);
	glShadeModel(GL_FLAT);
	gluDisk(s, 0, this->radius, this->openGLComplexity, this->openGLComplexity);
	gluDeleteQuadric(s);

	return this->material->index;
}
