#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rayTriangle.h"

////////////////////////
//  Ray-tracing stuff //
////////////////////////
void RayTriangle::initialize(void){
	v1 = v[2]->position - v[0]->position;
	v2 = v[1]->position - v[0]->position;
	plane.normal = -(v1.crossProduct(v2).unit());
	plane.distance = -(v[2]->position.dot(plane.normal));
}
double RayTriangle::intersect(Ray3D ray,RayIntersectionInfo& iInfo,double mx){
	// Distance along ray
	double t = (-plane.distance - plane.normal.dot(ray.position)) / plane.normal.dot(ray.direction);

	if (t < 0 || (mx >= 0 && t >= mx))
		return -1;

	// Intersection point in plane
	Point3D world_p = ray(t);
	Point3D p = world_p - v[0]->position;

	// No intersection when ray hits back of plane
	// (disabled to enable directional lights to cast shadows)
	/* if (plane.normal.dot(ray.position - world_p) < 0)
		return -1; */

	// Solve for barycentric coords
	double det, gamma, beta, alpha;

	if ((v1[0] * v2[1] - v2[0] * v1[1]) != 0) {
		det = (v1[0] * v2[1] - v2[0] * v1[1]);
		gamma = (1 / det) * (v2[1] * p[0] - v2[0] * p[1]);
		beta = (1 / det) * (-v1[1] * p[0] + v1[0] * p[1]);
	}
	else if ((v1[0] * v2[2] - v2[0] * v1[2]) != 0) {
		det = (v1[0] * v2[2] - v2[0] * v1[2]);
		gamma = (1 / det) * (v2[2] * p[0] - v2[0] * p[2]);
		beta = (1 / det) * (-v1[2] * p[0] + v1[0] * p[2]);
	}
	else {
		det = (v1[1] * v2[2] - v2[1] * v1[2]);
		gamma = (1 / det) * (v2[2] * p[1] - v2[1] * p[2]);
		beta = (1 / det) * (-v1[2] * p[1] + v1[1] * p[2]);
	}

	alpha = 1 - (gamma + beta);

	if ((alpha < 0 || alpha > 1) || (beta < 0 || beta > 1) || (gamma < 0 || gamma > 1)) {
		// Ray does not intersect with triangle
		return -1;
	}

	iInfo.iCoordinate = world_p;
	iInfo.material = this->material;
	iInfo.normal = this->plane.normal;
	iInfo.texCoordinate =
		this->v[0]->texCoordinate * alpha + this->v[1]->texCoordinate * beta + this->v[2]->texCoordinate * gamma;

	return t;
}
BoundingBox3D RayTriangle::setBoundingBox(void){
	return bBox;
}

//////////////////
// OpenGL stuff //
//////////////////
int RayTriangle::drawOpenGL(int materialIndex){
	glShadeModel(GL_FLAT);
	this->material->drawOpenGL();

	glBegin(GL_TRIANGLES);
		glVertex3f(v[0]->position[0], v[0]->position[1], v[0]->position[2]);
		glVertex3f(v[1]->position[0], v[1]->position[1], v[1]->position[2]);
		glVertex3f(v[2]->position[0], v[2]->position[1], v[2]->position[2]);
	glEnd();

	return 0;
}
