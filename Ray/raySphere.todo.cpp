#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "rayScene.h"
#include "raySphere.h"

////////////////////////
//  Ray-tracing stuff //
////////////////////////
double RaySphere::intersect(Ray3D ray,RayIntersectionInfo& iInfo,double mx){
	Point3D rToC =  this->center - ray.position;
	double d = rToC.dot(ray.direction.unit());

	// No intersection if sphere center is "behind" ray
	if (d < 0)
		return -1;

	double f = sqrt(rToC.squareNorm() - d * d);

	// No intersection if ray line is further than sphere radius away from circle center
	if (f > this->radius)
		return -1;

	// Ignore intersections further away than mx
	double rayDist = d - sqrt(this->radius * this->radius - f * f);
	if (mx > 0 && rayDist >= mx)
		return -1;

	// Calculate intersection info
	iInfo.iCoordinate = ray(rayDist) * ray.direction.length();
	iInfo.normal = (iInfo.iCoordinate - this->center).unit();
	iInfo.material = this->material;

	// latitude (between 0 and 1)
	iInfo.texCoordinate[0] = (asin(iInfo.normal[1]) / (PI / 2)) + 1;

	// longitude (between 0 and 1)
	iInfo.texCoordinate[1] = (acos(iInfo.normal[2]) + PI) / (2 * PI);

	return rayDist;
}
BoundingBox3D RaySphere::setBoundingBox(void){
	return bBox;
}

//////////////////
// OpenGL stuff //
//////////////////
int RaySphere::drawOpenGL(int materialIndex){
	GLUquadric* s = gluNewQuadric();
	gluQuadricDrawStyle(s, GL_POLYGON);
	gluQuadricNormals(s, GLU_SMOOTH);
	gluQuadricOrientation(s, GLU_OUTSIDE);
	gluSphere(s, this->radius, 100, 100);

	return -1;
}
