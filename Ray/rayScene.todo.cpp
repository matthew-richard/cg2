#include "rayScene.h"
#include <GL/glew.h>
#include <GL/glut.h>
#include <math.h>


///////////////////////
// Ray-tracing stuff //
///////////////////////

Point3D RayScene::Reflect(Point3D v,Point3D n){
	return Point3D();
}

int RayScene::Refract(Point3D v,Point3D n,double ir,Point3D& refract){
	return 0;
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

Point3D RayScene::GetColor(Ray3D ray,int rDepth,Point3D cLimit){
	RayIntersectionInfo iInfo;
	double result = group->intersect(ray, iInfo, -1);
	return result < 0 ? this->background : (iInfo.material->ambient + iInfo.material->emissive);
}

//////////////////
// OpenGL stuff //
//////////////////
void RayMaterial::drawOpenGL(void){
}
void RayTexture::setUpOpenGL(void){
}
