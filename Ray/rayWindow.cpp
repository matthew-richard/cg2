#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <iostream>
#include <Util/time.h>
#include "rayWindow.h"

using std::cout;
using std::endl;

const char RayWindow::KEY_ESCAPE='\033';
RayScene* RayWindow::scene=NULL;
Mouse RayWindow::mouse;
int RayWindow::curveFit=LINEAR;
int RayWindow::isVisible;
int RayWindow::frameCount;
double RayWindow::startTime;
double RayWindow::radius;
double RayWindow::frameRate;
double RayWindow::frameCountStart;
Point3D RayWindow::center;
const double RayWindow::videoFrameRate = 5.0;
int RayWindow::numRenderedFrames;
int RayWindow::totalFrames;
char RayWindow::video[1024];

/** This function prints out the state of the OpenGL error. */
int RayWindow::PrintError(const int& showNoError){
	int x,y;
	int e=1;
	switch(glGetError()){
	case GL_NO_ERROR:
		if(showNoError){printf("No error\n");}
		e=0;
		break;
	case GL_INVALID_ENUM:
		printf("Invalid Enum\n");
		break;
	case GL_INVALID_VALUE:
		printf("Invalid Value\n");
		break;
	case GL_INVALID_OPERATION:
		printf("Invalid Operation\n");
		break;
	case GL_STACK_OVERFLOW:
		printf("Stack Overflow\n");
		break;
	case GL_STACK_UNDERFLOW:
		printf("Stack Underflow\n");
		break;
	case GL_OUT_OF_MEMORY:
		printf("Out of memory\n");
		break;
	}
	if(!showNoError && !e){return 0;}

	glGetIntegerv(GL_MATRIX_MODE,&x);
	if(x==GL_MODELVIEW){
		glGetIntegerv(GL_MODELVIEW_STACK_DEPTH,&x);
		glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH,&y);
		printf("Modelview Matrix: %d/%d\n",x,y);
	}
	else if(x==GL_PROJECTION){
		glGetIntegerv(GL_PROJECTION_STACK_DEPTH,&x);
		glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH,&y);
		printf("Projection Matrix: %d/%d\n",x,y);
	}
	return e;
}

/** This function writes out the specified string, left-aligned, at the specified location, onto the OpenGL window. */
void RayWindow::WriteLeftString(const int& x,const int& y,const char* str){
	GLint vp[4];
	GLint dt=glIsEnabled(GL_DEPTH_TEST);
	GLint lm=glIsEnabled(GL_LIGHTING);
	GLint mm;

	glGetIntegerv(GL_VIEWPORT,vp);
	glGetIntegerv(GL_MATRIX_MODE,&mm);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0,vp[2],0,vp[3],0,1);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glColor4f(0,0,0,1);
	glRasterPos2f(x,y);
	int len=(int)strlen(str);
	for(int i=0;i<len;i++){glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,str[i]);}
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	if(dt){glEnable(GL_DEPTH_TEST);}
	if(lm){glEnable(GL_LIGHTING);}
	glMatrixMode(mm);
}
/** This function writes out the specified string, right-aligned, at the specified location, onto the OpenGL window. */
void RayWindow::WriteRightString(const int& x,const int& y,const char* str){
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT,vp);

	WriteLeftString(vp[2]-x-glutBitmapLength(GLUT_BITMAP_HELVETICA_18,(unsigned char*) str),y,str);
}

/** This function reads the current frame buffer and sets the pixels of the image accordingly. */
int RayWindow::TakeSnapshot(Image32& img){
	GLfloat *pixels;
	int i,j,temp;
	Pixel32 p;
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT,vp);

	if(!img.setSize(vp[2],vp[3])){return 0;}
	pixels=new GLfloat[vp[2]*vp[3]*3];
	if(!pixels){return 0;}
	glReadBuffer(GL_FRONT);
	glReadPixels(vp[0],vp[1],vp[2],vp[3],GL_RGB,GL_FLOAT,pixels);

	for(i=0;i<vp[3];i++){
		for(j=0;j<vp[2];j++){
			temp=0+j*3+(vp[3]-i-1)*(vp[2])*3;
			p.r=255*pixels[temp];
			temp=1+j*3+(vp[3]-i-1)*(vp[2])*3;
			p.g=255*pixels[temp];
			temp=2+j*3+(vp[3]-i-1)*(vp[2])*3;
			p.b=255*pixels[temp];
			img(j,i)=p;
		}
	}
	delete[] pixels;
	return 1;
}

/** This function is called when no events need to be processed. */
void RayWindow::IdleFunction(void) {
	if (!strlen(video)) {
		// Update the parameter values
		scene->setCurrentTime(GetTime() - startTime, curveFit);
	}
	// Just draw the scene again
	if (strlen(video) || isVisible) { glutPostRedisplay(); }
}
/** This function is called when the visibility state of the window changes. */
void RayWindow::VisibilityFunction(int state){
	if(state==GLUT_VISIBLE){isVisible=1;}
	else if(state==GLUT_NOT_VISIBLE){isVisible=0;}
}

//////////////////////////
// Mouse event handlers //
//////////////////////////
/** This function is called when the state of a mouse button is changed */
void RayWindow::MouseFunction( int button, int state, int x, int y ){
	mouse.update(button,state,x,y);

	if (state == GLUT_DOWN) {
		printf("No longer ray tracing.\n");
		glutDisplayFunc(DisplayFunction);
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// check if ray intersects with clickable switch
		int w = glutGet(GLUT_WINDOW_WIDTH);
		int h = glutGet(GLUT_WINDOW_HEIGHT);
		Ray3D ray = RayScene::GetRay(RayWindow::scene->camera, x, h - y, w, h);
		printf("Left click event (%d, %d).\n", x, y);

		RayIntersectionInfo iInfo;
		if (RayWindow::scene->group->intersect(ray, iInfo) >= 0) {
			cout << "Click pointed at scene geometry." << endl;
			if (strstr(iInfo.material->foo, "clickable") != NULL) {
				cout << "Switch was clicked!." << endl;

				// Randomize color of first light
				RayWindow::scene->lights[0]->color = Point3D(rand() / (float) RAND_MAX, rand() / (float) RAND_MAX, rand() / (float) RAND_MAX);
			}
		}

	}
}
/** This function is called when one of the mouse buttons is depressed and the mouse is moved. */
void RayWindow::MotionFunction( int x, int y ){
	Point2D d=mouse.move(x,y);


	if(mouse.middleDown || (mouse.leftDown && mouse.shiftDown)){
		scene->camera->rotateUp(center,0.01*d[0]);
		scene->camera->rotateRight(center,0.01*d[1]);
	}
	if(mouse.scrollDown || (mouse.leftDown && mouse.ctrlDown)){
		scene->camera->moveForward(d[1] * 0.01);//radius/15*d[1]);
	}
	if(mouse.leftDown && !mouse.shiftDown && !mouse.ctrlDown){
		scene->camera->moveRight(-d[0] * 0.01);// -radius / 250 * d[0]);
		scene->camera->moveUp(d[1] * 0.01);// radius / 250 * d[1]);
	}

	glutPostRedisplay();
}
/** This function is called when the mouse is moved moved but no buttons are depressed. */
void RayWindow::PassiveMotionFunction( int x, int y ){
	mouse.move(x,y);
	glutPostRedisplay();
}

/////////////////////////////
// Keyboard event handlers //
/////////////////////////////
/** This function is called when the user presses a key. */
void RayWindow::KeyboardFunction( unsigned char c, int x, int y ){
	char temp[500];
	Image32 img;

	switch( c ){
		case KEY_ESCAPE:
			exit( 0 );
			break;		
		case 'I':
			printf("Image Name: ");
			fgets( temp , 499 , stdin );
			TakeSnapshot(img);
			img.WriteImage(temp);
			printf("Wrote to file: %s\n",temp);
			break;
		case 'p':
			fprintf(stderr,"\nPos: (%g,%g,%g)\n Dir: (%g,%g,%g)\n Up: (%g,%g,%g)\n",
				scene->camera->position[0],scene->camera->position[1],scene->camera->position[2],
				scene->camera->direction[0],scene->camera->direction[1],scene->camera->direction[2],
				scene->camera->up[0],scene->camera->up[1],scene->camera->up[2]);
			break;


	}
}
/** This function is called when the user presses one of the special keys. */
void RayWindow::SpecialFunction( int key, int x, int y ){
	switch( key ){
	case GLUT_KEY_F1:
		break;
	case GLUT_KEY_F2:
		break;
	case GLUT_KEY_F3:
		break;
	case GLUT_KEY_F4:
		break;
	case GLUT_KEY_F5:
		break;
	case GLUT_KEY_F6:
		break;
	case GLUT_KEY_F7:
		break;
	case GLUT_KEY_F8:
		break;
	case GLUT_KEY_F9:
		break;
	case GLUT_KEY_F10:
		break;
	case GLUT_KEY_F11:
		break;
	case GLUT_KEY_F12:
		break;
	case GLUT_KEY_UP:
		break;
	case GLUT_KEY_DOWN:
		break;
	case GLUT_KEY_LEFT:
		break;
	case GLUT_KEY_RIGHT:
		break;
	case GLUT_KEY_PAGE_UP:
		break;
	case GLUT_KEY_PAGE_DOWN:
		break;
	case GLUT_KEY_HOME:
		break;
	case GLUT_KEY_END:
		break;
	case GLUT_KEY_INSERT:
		break;
	}
	glutPostRedisplay();
}

/////////////////////////
// Menu event handlers //
/////////////////////////
/** This function is called when the user updates the draw mode in the drop-down menu. */
void RayWindow::DrawModeMenu(int entry){
	glPolygonMode( GL_FRONT_AND_BACK, entry );
	glutPostRedisplay();

}
/** This function is called when the user updates the cull mode in the drop-down menu. */
void RayWindow::CullModeMenu(int entry){
	if(entry==NO_CULL){glDisable(GL_CULL_FACE);}
	else{
		glEnable(GL_CULL_FACE);
		if(entry==CULL_BACK_FACE){glCullFace(GL_BACK);}
		if(entry==CULL_FRONT_FACE){glCullFace(GL_FRONT);}
	}
	glutPostRedisplay();
}
/** This function is called when the user updates the curve fitting method in the drop-down menu. */
void RayWindow::CurveFitMenu(int entry){
	curveFit=entry;
	startTime=GetTime();
	glutPostRedisplay();
}
/** This function is called when the user selects one of the main menu options in the drop-down menu. */
void RayWindow::MainMenu(int entry){
switch (entry) {
case 0:
	exit(0);
	break;
case -1:
	cout << "Ray tracing." << endl;
	glClear(GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
	glutDisplayFunc(RayTraceFunction);
	glutPostRedisplay();
	break;
default:
	cout << "Unrecognized menu option. Quitting." << endl;
	exit(1);
	break;
}
}

/////////////////////////


/**  This function draws the OpenGL window. */
void RayWindow::DisplayFunction(void){
	char temp[500];
	double t;

	// Clear the color and depth buffers
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	float d=radius+(scene->camera->position-center).length();

	// Draw the perspective projection (to get good front and back clipping planes, we need to know the rough
	// size of the model)
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective(scene->camera->heightAngle*180.0/PI,scene->camera->aspectRatio,0.1,10000*d);

	// Draw the RayScene
	GLint drawMode[2];
	glGetIntegerv(GL_POLYGON_MODE,drawMode);
	if(drawMode[0]==GL_FILL){scene->drawOpenGL();}
	else{
		glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glPolygonOffset(1,2);
		glDisable(GL_BLEND);
		scene->drawOpenGL();
		glDisable(GL_POLYGON_OFFSET_FILL);
		glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
		glPolygonMode(GL_FRONT_AND_BACK,drawMode[0]);
		glDisable(GL_BLEND);
		glEnable(GL_LINE_SMOOTH);
		glLineWidth(2);
		scene->drawOpenGL();
	}


	// Update the frame rate value on the tenth frame
	frameCount++;
	if(frameCount==10){
		frameCount=0;
		t=frameCountStart;
		frameCountStart=GetTime();
		frameRate=10/(frameCountStart-t);
	}
	sprintf(temp,"%.1f fs",frameRate);
	WriteLeftString(1,2,temp);

	// Write out the mouse position
	sprintf(temp,"(%3d , %3d)",mouse.endX,mouse.endY);
	WriteRightString(1,2,temp);

	// Swap the back and front buffers
	glutSwapBuffers();

	// Save to file
	if (strlen(video)) {
		Image32 img;
		char fileName[1024];
		numRenderedFrames++;
		scene->setCurrentTime(numRenderedFrames / videoFrameRate, curveFit);
		sprintf(fileName, "%s_%d.bmp", video, numRenderedFrames);

		TakeSnapshot(img);
		img.WriteImage(fileName);

		if (numRenderedFrames >= totalFrames) {
			printf("All %d frames rendered. Exiting.\n", totalFrames);
			exit(0);
		}
	}
}

/**  This function draws the OpenGL window. */
void RayWindow::RayTraceFunction(void) {
	Image32 img;
	GLuint tex;
	
	// Perform ray tracing
	scene->RayTrace(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 5, 0.0001, img);
	//img.WriteImage("img.bmp");

	// Provide image as texture to OpenGL
	int w = img.width();
	int h = img.height();
	GLubyte* rawImage = new GLubyte[w * h * 4];
	for (int i = 0; i < w; i++)
	for (int j = 0; j < h; j++) {
		Pixel32& p = img.pixel(i, j);
		GLubyte* b = &(rawImage[(j * w + i) * 4]);

		b[0] = p.r;
		b[1] = p.g;
		b[2] = p.b;
		b[3] = p.a;
	}
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rawImage);
	delete[] rawImage;

	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset material ambience
	GLfloat ones[] = { 1, 1, 1, 1 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ones);


	// As described on SO: http://gamedev.stackexchange.com/questions/61080/how-do-i-draw-a-full-resolution-texture-to-a-window-of-the-same-resolution
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, 0, h, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glBegin(GL_QUADS);
		glNormal3f(0, 0, -1.0f);

		glTexCoord2f(0, 1);
		glVertex3f(0, 0, 0); // Upper left

		glTexCoord2f(1, 1);
		glVertex3f(w, 0, 0); // Upper right

		glTexCoord2f(1, 0);
		glVertex3f(w, h, 0); // Lower right

		glTexCoord2f(0, 0);
		glVertex3f(0, h, 0); // Lower left
	glEnd();
	glDeleteTextures(1, &tex);

	WriteRightString(1, 2, "Click to stop ray-tracing.");

	// Swap the back and front buffers
	glutSwapBuffers();
}

/** This function is called when the window is resized. */
void RayWindow::ReshapeFunction(int width,int height)
{
	GLint viewPort[4];
	
	glViewport( 0, 0, width, height );
	glGetIntegerv(GL_VIEWPORT, viewPort);
	scene->camera->aspectRatio=(float)width/(float)height;
	glutPostRedisplay();
}

/** This function instantiates the OpenGL window, reading in the RayScene from the specified file
  * and setting the initial OpenGL window size. The function never returns! */
void RayWindow::RayView(RayScene* s,int width,int height,int cplx, char* video, float duration){
	int drawMenu;
	int cullMenu;
	int curveFitMenu;
	int argc=1;
	strcpy(RayWindow::video, video);
	char* argv[]={"foo"};


	startTime=GetTime();

	scene=s;
	BoundingBox3D b=scene->group->setBoundingBox();
	center=(b.p[0]+b.p[1])/2;
	radius=(b.p[0]-b.p[1]).length()/2;
	frameCountStart=GetTime();

	if (strlen(video)) {
		numRenderedFrames = 0;
		totalFrames = duration * videoFrameRate;
	}

	// Initialize the OpenGL context
	glutInit(&argc,argv);
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Intro to CG");
	glutInitWindowSize(width,height);
	glutInitWindowPosition(0,0);
	glClearColor(scene->background[0],scene->background[1],scene->background[2],1.0);
	glutReshapeWindow(width,height);

	// Initialize the event handlers
	glutDisplayFunc			(DisplayFunction);
	glutReshapeFunc			(ReshapeFunction);
	glutKeyboardFunc		(KeyboardFunction);
	glutSpecialFunc			(SpecialFunction);
	glutMouseFunc			(MouseFunction);
	glutMotionFunc			(MotionFunction);
	glutPassiveMotionFunc	(PassiveMotionFunction);
	glutVisibilityFunc		(VisibilityFunction);
	glutIdleFunc(IdleFunction);

	// Initialize the menus
	drawMenu = glutCreateMenu(DrawModeMenu);
	glutAddMenuEntry(" Polygons ",GL_FILL);
	glutAddMenuEntry(" Lines ",GL_LINE);
	glutAddMenuEntry(" Points ",GL_POINT);

	cullMenu = glutCreateMenu(CullModeMenu);
	glutAddMenuEntry(" None ", NO_CULL);
	glutAddMenuEntry(" Back ", CULL_BACK_FACE);
	glutAddMenuEntry(" Front ", CULL_FRONT_FACE);

	curveFitMenu = glutCreateMenu(CurveFitMenu);
	glutAddMenuEntry(" Linear Interpolation ",LINEAR);
	glutAddMenuEntry(" Catmull-Rom Interpolation ",CATMULL_ROM);
	glutAddMenuEntry(" Uniform Cubic B-Spline Approximation ", UNIFORM_CUBIC_B_SPLINE);

	glutCreateMenu(MainMenu);
	glutAddSubMenu(" Polygon mode ", drawMenu);
	glutAddSubMenu(" Cull mode ", cullMenu);
	glutAddSubMenu(" Animation Fitting ",curveFitMenu);
	glutAddMenuEntry(" Quit ",0);
	glutAddMenuEntry(" RayTrace", -1);

	// Attach the drop-down menu to the right button
	glutAttachMenu( GLUT_RIGHT_BUTTON );

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);


	// Set up all the necessary stuff for the RayScene
	scene->setUpOpenGL(cplx);

	// Fall into the main loop
	glutMainLoop();
}
