/** @file main.cpp
 *  @brief man code for <app name>
 *
 *	@author <author>
 *	@date <day-month> 2011
 */

#include "stdafx.h"
#include "utils.h"
#include "utils_shaders.h"
#include "utils_math.h"

#include "material_point.h"
#include "prohibited_area.h"

///////////////////////////////////////////////////////////////////////////////
// globals

// camera:
float camAngleX, camAngleY, camZoom;
float camDeltaAngX, camDeltaAngY, camDeltaZoom;

// input:
bool isMouseLeftPressed = false;
bool isMouseMiddlePressed = false;
int lastMouseX = 0;
int lastMouseY = 0;

// screen params
int screenWidth;
int screenHeight;
float screenRatio;

// geometry to render
GLint meshDisplayList;

// simple animation:
float meshAnim = 0.0f;

// shaders:
GLuint shaderLight;
GLuint shaderParticle;

// time:
double g_appTime = 0.0;	// global app time in seconds

// for fps:
int frame, time, timebase = 0;

// menu:
#define MENU_EXIT 1
#define MENU_RESET 2
#define MENU_OPTION 3

///////////////////////////////////////////////////////////////////////////////
// function prototypes
bool InitApp();
void CleanUp();

// callbacks:
void ChangeSize(int w, int h);
void ProcessMenu(int option);

// keyboard:
void ProcessNormalKeys(unsigned char key, int x, int y);
void PressSpecialKey(int key, int x, int y);
void ReleaseSpecialKey(int key, int x, int y);

// mouse:
void ProcessMouse(int button, int state, int x, int y);
void ProcessMouseMotion(int x, int y);

void Idle();
void UpdateScene(double deltaSeconds);
void RenderScene();

///////////////////////////////////////////////////////////////////////////////
// entry point
int main(int argc, char **argv)
{
	// init GLUT
	glutInit(&argc, argv);
	// use depth buffer, double buffering and standard RGBA mode
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200,200);
	// standard resolution:
	glutInitWindowSize(640, 480);
	// title:
	glutCreateWindow("<add title>");

	// set callbacks:
	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(ProcessNormalKeys);
	glutSpecialFunc(PressSpecialKey);
	glutSpecialUpFunc(ReleaseSpecialKey);
	glutMouseFunc(ProcessMouse);
	glutMotionFunc(ProcessMouseMotion);
	glutDisplayFunc(RenderScene);
	glutIdleFunc(Idle);
	glutReshapeFunc(ChangeSize);
	
	// init OpenGL extensions (like shader support, framebuffer object, etc)
	glewInit();

	// init whole application:
	if (InitApp() == false)
	{
		utLOG_ERROR("cannot init application...");
		return 1;
	}

	// start rendering and animation:
	glutMainLoop();

	// finish:
	CleanUp();

	return(0);
}

#pragma region Init & CleanUp
///////////////////////////////////////////////////////////////////////////////
bool InitApp() 
{
	// create GLUT menu:
	int menu = glutCreateMenu(ProcessMenu);
	glutAddMenuEntry("Reset Camera", MENU_RESET);
	glutAddMenuEntry("Option 1", MENU_OPTION);
	glutAddMenuEntry("Option 2", MENU_OPTION);
	glutAddMenuEntry("Exit", MENU_EXIT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glEnable(GL_DEPTH_TEST);
	
	meshDisplayList = glGenLists(1);

	glNewList(meshDisplayList,GL_COMPILE);
		glutSolidSphere(0.25f, 16, 16);
	glEndList();

	camAngleX = 0.0f;
	camAngleY = 0.0f;
	camZoom = 10.0f;
	camDeltaAngX = 0.0f;
	camDeltaAngY = 0.0f;
	camDeltaZoom = 0.0f;

	if (utLoadAndBuildShader("data\\light.vert", "data\\light.frag", &shaderLight) == false)
		return false;

	if (utLoadAndBuildShader("data\\particle.vert", "data\\particle.frag", &shaderParticle) == false)
		return false;

	return true;
}


///////////////////////////////////////////////////////////////////////////////
void CleanUp()
{
	utDeleteAllUsedShaders();
}

#pragma endregion

///////////////////////////////////////////////////////////////////////////////
void ChangeSize(int w, int h)
{
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;

	screenWidth = w;
	screenHeight = h;
	screenRatio = 1.0f * w / h;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set the clipping volume
	gluPerspective(45, screenRatio, 0.1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

#pragma region Input
void ProcessMenu(int option)
{
	if (option == MENU_EXIT)
		exit(0);
	else if (option == MENU_RESET)
	{
		camAngleX = 0.0f;
		camAngleY = 0.0f;
		camZoom = 10.0f;
	}
}

///////////////////////////////////////////////////////////////////////////////
void ProcessNormalKeys(unsigned char key, int x, int y) {

	if (key == 27) 
		exit(0);
	else if (key == ' ')
	{
		camAngleX = 0.0f;
		camAngleY = 0.0f;
		camZoom = 10.0f;
	}
}


///////////////////////////////////////////////////////////////////////////////
void PressSpecialKey(int key, int x, int y) {

	switch (key) {
		case GLUT_KEY_LEFT  : camDeltaAngX = -1.1f;break;
		case GLUT_KEY_RIGHT : camDeltaAngX = 1.1f;break;
		case GLUT_KEY_UP    : camDeltaAngY = 1.1f;break;
		case GLUT_KEY_DOWN  : camDeltaAngY = -1.1f; break;
	}

}


///////////////////////////////////////////////////////////////////////////////
void ReleaseSpecialKey(int key, int x, int y) {

	switch (key) {
		case GLUT_KEY_LEFT  : camDeltaAngX = 0.0f; break;
		case GLUT_KEY_RIGHT : camDeltaAngX = 0.0f; break;
		case GLUT_KEY_UP    : camDeltaAngY = 0.0f; break;
		case GLUT_KEY_DOWN  : camDeltaAngY = 0.0f; break;
	}
}


///////////////////////////////////////////////////////////////////////////////
void ProcessMouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{		
		if (state == GLUT_DOWN)
		{
			isMouseLeftPressed = true;
			lastMouseX = x;
			lastMouseY = y;
			//printf("left mouse button pressed...\n");
		}
		else if (state == GLUT_UP)
		{
			isMouseLeftPressed = false;
			//printf("left mouse button up...\n");
		}
	}
	else if (button == GLUT_MIDDLE_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			isMouseMiddlePressed = true;
			lastMouseX = x;
			lastMouseY = y;
			//printf("left mouse button pressed...\n");
		}
		else if (state == GLUT_UP)
		{
			isMouseMiddlePressed = false;
			//printf("left mouse button up...\n");
		}	
	}	
}

///////////////////////////////////////////////////////////////////////////////
void ProcessMouseMotion(int x, int y)
{
	int dx = lastMouseX - x;
	int dy = lastMouseY - y;
	lastMouseX = x;
	lastMouseY = y;

	if (isMouseLeftPressed)
	{
		camAngleX -= dx*0.5f;
		camAngleY -= dy*0.5f;
	}
	if (isMouseMiddlePressed)
	{
		camZoom += dy*0.1f;
	}
}
#pragma endregion

#pragma region Update & Render

///////////////////////////////////////////////////////////////////////////////
void Idle()
{
	// in milisec
	int t = glutGet(GLUT_ELAPSED_TIME);
	double newTime = (double)t*0.001;

	double deltaTime = newTime - g_appTime;
	g_appTime = newTime;

	// call Update:
	UpdateScene(deltaTime);

	// render frame:
	RenderScene();
}

///////////////////////////////////////////////////////////////////////////////
void UpdateScene(double deltaTime) 
{
	static unsigned int updateCounter = 0;
	updateCounter++;

	meshAnim += 1.0f * (float)deltaTime;
}

///////////////////////////////////////////////////////////////////////////////
void RenderScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// setup camera:
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	camZoom += camDeltaZoom;
	camAngleX += camDeltaAngX;
	camAngleY += camDeltaAngY;

	glTranslatef(0.0f, 0.0f, -camZoom);
	glRotatef(camAngleX, 0.0f, 1.0f, 0.0f);
	glRotatef(camAngleY, 1.0f, 0.0f, 0.0f);


	// render something
	glUseProgram(shaderLight);
	//float col = 0.5f+0.5f*sinf(meshAnim);
	//glColor3f(col, 0.0f, 1.0f-col);
	glColor3f(0.1f, 0.5f, 0.8f);
	glCallList(meshDisplayList);

	glUseProgram(0);

	// draw ground:
	glColor3f(0.8f, 0.5f, 0.3f);
	glBegin(GL_QUADS);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-10.0f, 0.0f, -10.0f);
		glVertex3f(-10.0f, 0.0f, 10.0f);
		glVertex3f(10.0f, 0.0f, 10.0f);
		glVertex3f(10.0f, 0.0f, -10.0f);
	glEnd();

	static char fpsStr[16];
	frame++;
	time = glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) {
		sprintf(fpsStr, "FPS: %4.2f", frame*1000.0/(time-timebase));
		timebase = time;		
		frame = 0;
	}

	glColor3f(0.0f,1.0f,0.0f);
	utSetOrthographicProjection(screenWidth, screenHeight);
		utDrawText2D(10,35, fpsStr);
		utDrawText2D(10,55, "Esc - Quit");
		utDrawText2D(10,75, "Space - Reset");
	utResetPerspectiveProjection();

	glutSwapBuffers();
}

#pragma endregion
