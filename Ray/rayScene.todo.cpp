#include "rayScene.h"
#include <GL/glew.h>
#include <GL/glut.h>
#include <math.h>

#define REF_EPSILON 0.00001

///////////////////////
// Ray-tracing stuff //
///////////////////////

Point3D RayScene::Reflect(Point3D v,Point3D n){
	v = -v.unit();
	n = n.unit();

	Point3D projected = n * v.dot(n);

	return (projected - (v - projected)).unit();
}

int RayScene::Refract(Point3D v,Point3D n,double ir,Point3D& refract){
	Point3D incidence = -v.unit();
	Point3D normal = n.unit();
	double sin_incidence = n.crossProduct(v).length() / ir;

	if (sin_incidence > 1) return 0;
	double theta_reflected = asin(sin_incidence);

	refract = normal * ((1 / ir) * incidence.dot(normal) - cos(theta_reflected)) - incidence * (1 / ir);
	return 1;
}

Ray3D RayScene::GetRay(RayCamera* camera,int i,int j,int width,int height){


	Ray3D result;
	result.position = camera->position;

	Point3D forward = camera->direction.unit();
	Point3D up = camera->up.unit();
	Point3D right = forward.crossProduct(up);
	double planeHeight = 2 * tan(camera->heightAngle / 2);
	double planeWidth = 2 * tan(camera->heightAngle * camera->aspectRatio / 2);

	result.direction = forward + up * (((double)j / (height - 1) - 0.5) * planeHeight )
							   + right * (((double)i / (width - 1) - 0.5) * planeWidth);

	return result;
}

Point3D RayScene::GetColor(Ray3D ray, int rDepth, Point3D cLimit) {

	RayIntersectionInfo iInfo;
	double iResult = group->intersect(ray, iInfo, -1);
	if (iResult < 0) {
		int fromCamera = 1;
		for (int i = 0; i < 3; i++)
			if (ray.position[i] != camera->position[i])
				{ fromCamera = 0; break; }
		return fromCamera ? this->background : Point3D(0,0,0);
	}

	// Rays "pass through" the backsides of shapes 
	if (ray.direction.dot(iInfo.normal) > 0) {
		ray.position = iInfo.iCoordinate + ray.direction * REF_EPSILON;
		return GetColor(ray, rDepth, cLimit);
	}

	rDepth--;
	Point3D color = iInfo.material->ambient + iInfo.material->emissive;

	// Add reflection
	int cutOffReflect = 1;
	for (int i = 0; i < 3; i++)
		if (iInfo.material->specular[i] > cLimit[i])
			cutOffReflect = 0;


	if (rDepth > 0 && !cutOffReflect) {
		Ray3D reflect;
		reflect.direction = Reflect(ray.direction.unit(), iInfo.normal.unit());
		reflect.position = iInfo.iCoordinate + reflect.direction * REF_EPSILON;
		color += (Point3D(1,1,1) - iInfo.material->transparent) * iInfo.material->specular * this->GetColor(reflect, rDepth, cLimit / iInfo.material->specular);
	}


	// Add refraction
	int cutOffRefract = 1;
	for (int i = 0; i < 3; i++)
		if (iInfo.material->transparent[i] > cLimit[i])
			cutOffRefract = 0;

	int belowCriticalAngle = 0;
	if (rDepth > 0 && !cutOffRefract) {
		Ray3D refract;
		belowCriticalAngle = Refract(ray.direction.unit(), iInfo.normal.unit(), iInfo.material->refind, refract.direction);
		if (belowCriticalAngle) {
			refract.position = iInfo.iCoordinate + refract.direction * REF_EPSILON;
			color += iInfo.material->transparent * this->GetColor(refract, rDepth, cLimit / iInfo.material->transparent);
		}
	}

	for (int i = 0; i < this->lightNum; i++) {
		RayLight& l = *this->lights[i];

		Point3D t = Point3D(1,1,1);
		for (int j = 0; j < group->shapeNum(); j++) {
			t *= l.transparency(iInfo, group->shapes[j], cLimit / iInfo.material->transparent);
		}

		if (iInfo.material->tex == NULL) {
			color += t * l.getDiffuse(camera->position, iInfo);
		}
		else {
			Pixel texVal = Pixel(iInfo.material->tex->img->BilinearSample(iInfo.texCoordinate[0], iInfo.texCoordinate[1]));
			color += t * Point3D(texVal.r, texVal.g, texVal.b);
		}
		color += t * l.getSpecular(camera->position, iInfo);

	}

	for (int i = 0; i < 3; i++) {
		if (color[i] < 0) color[i] = 0;
		else if (color[i] > 1) color[i] = 1;
	}

	return color;
}

//////////////////
// OpenGL stuff //
//////////////////
void RayMaterial::drawOpenGL(void){
	float alpha = 1 - (this->transparent[0] + this->transparent[1] + this->transparent[2]) / 3;
	GLfloat d[] = { this->diffuse[0], this->diffuse[1], this->diffuse[2], alpha };
	GLfloat s[] = { this->specular[0], this->specular[1], this->specular[2], alpha };
	GLfloat e[] = { this->emissive[0], this->emissive[1], this->emissive[2], alpha };
	GLfloat a[] = { this->ambient[0], this->ambient[1], this->ambient[2], alpha };
	GLfloat one[] = { 1, 1, 1, 1 };

	// TODO: transparency?

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, d);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, s);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, e);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, a);

	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, this->specularFallOff);

	// TODO: textures?
}
void RayTexture::setUpOpenGL(void){
}
