/** @file rigid_body_test.cpp
 *  @brief man code for <app name>
 *
 *	@author <author>
 *	@date <day-month> 2011
 */

#include "stdafx.h"
#include "utils.h"
#include "utils_shaders.h"
#include "utils_math.h"

#include "input.h"

#include "rigid_body.h"

#include "collision_shapes.h"

#pragma region globals
///////////////////////////////////////////////////////////////////////////////
// globals
Camera g_camera;

// shaders:
GLuint g_shaderLight;
GLuint g_shaderParticle;

// geometry to render
GLint g_meshDisplayList;

// time:
double g_appTime = 0.0;	// global app time in seconds

// menu:
#define MENU_EXIT 1
#define MENU_RESET 2
#define MENU_EULER 3
#define MENU_VERLET 4

Box tmp1, tmp2;
OBB obb1, obb2;

float red = 0.0f;
float green = 1.0f;
float blue = 0.0f;

#pragma endregion


///////////////////////////////////////////////////////////////////////////////
// function prototypes
bool InitApp();
void InitSimulation();
void ResetSimulation();
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
	if (utInitOpenGL20() == false) return 1;

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
	glutAddMenuEntry("euler integration", MENU_EULER);
	glutAddMenuEntry("verlet integration", MENU_VERLET);
	glutAddMenuEntry("Exit", MENU_EXIT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glEnable(GL_DEPTH_TEST);
	
	g_meshDisplayList = glGenLists(1);

	glNewList(g_meshDisplayList,GL_COMPILE);
		glutSolidSphere(0.20f, 12, 12);
	glEndList();

	srand((unsigned int)time(NULL));

	if (utLoadAndBuildShader("data\\light.vert", "data\\light.frag", &g_shaderLight) == false)
		return false;

	g_camera.m_zoom = 10.0f;

	InitSimulation();

	return true;
}

void InitSimulation()
{
	ResetSimulation();
}

void ResetSimulation()
{
	tmp1.m_width = 3.0;
	tmp1.m_height = 0.5;
	tmp1.m_length = 1.0;
	tmp1.m_density = 1.0;
	tmp1.CalculateInertiaTensor();

	tmp1.m_massCenter.m_pos = Vec3d(0.0, 0.0, 0.0);
	tmp1.m_angularVel = Vec3d(0.0, 0.0, 0.5);

	tmp2.m_width = 1.5;
	tmp2.m_height = 0.5;
	tmp2.m_length = 1.5;
	tmp2.m_density = 1.0;
	tmp2.CalculateInertiaTensor();

	tmp2.m_massCenter.m_pos = Vec3d(2.0, 0.0, 0.0);
	tmp2.m_angularVel = Vec3d(0.0, 0.0, 0.0);

	obb1.c = tmp1.m_massCenter.m_pos;
	obb1.e = Vec3d(tmp1.m_width/2, tmp1.m_height/2, tmp1.m_length/2);
	obb1.u = tmp1.m_rotation;

	obb2.c = tmp2.m_massCenter.m_pos;
	obb2.e = Vec3d(tmp2.m_width/2, tmp2.m_height/2, tmp2.m_length/2);
	obb2.u = tmp2.m_rotation;
}

///////////////////////////////////////////////////////////////////////////////
void CleanUp()
{
	glDeleteLists(g_meshDisplayList, 1);
	utDeleteAllUsedShaders();
}

#pragma endregion

#pragma region Input

///////////////////////////////////////////////////////////////////////////////
void ChangeSize(int w, int h)
{
	g_camera.ChangeViewportSize(w, h);
}

///////////////////////////////////////////////////////////////////////////////
void ProcessMenu(int option)
{
	if (option == MENU_EXIT)
	{
		exit(0);
	}
	else if (option == MENU_RESET)
	{
		g_camera.m_angleX = 0.0f;
		g_camera.m_angleY = 0.0f;
		g_camera.m_zoom = 10.0f;
	}
	else if (option == MENU_EULER)
	{
		MaterialPoint::s_algType = MaterialPoint::atEuler;
	}
	else if (option == MENU_VERLET)
	{
		MaterialPoint::s_algType = MaterialPoint::atVerlet;
	}
}

///////////////////////////////////////////////////////////////////////////////
void ProcessNormalKeys(unsigned char key, int x, int y) {

	if (key == 27) 
		exit(0);
	else if (key == ' ')
	{
		g_camera.m_angleX = 0.0f;
		g_camera.m_angleY = 0.0f;
		g_camera.m_zoom = 10.0f;
	}
	else if (key == 'r')
	{
		ResetSimulation();
	}
}

///////////////////////////////////////////////////////////////////////////////
void PressSpecialKey(int key, int x, int y) 
{
	g_camera.PressSpecialKey(key, x, y);
}


///////////////////////////////////////////////////////////////////////////////
void ReleaseSpecialKey(int key, int x, int y) 
{
	g_camera.ReleaseSpecialKey(key, x, y);
}

///////////////////////////////////////////////////////////////////////////////
void ProcessMouse(int button, int state, int x, int y)
{
	g_camera.ProcessMouse(button, state, x, y);
}

///////////////////////////////////////////////////////////////////////////////
void ProcessMouseMotion(int x, int y)
{
	g_camera.ProcessMouseMotion(x, y);
}
#pragma endregion

#pragma region Update & Render

///////////////////////////////////////////////////////////////////////////////
void Idle()
{
	static double lastDeltas[3] = { 0.0, 0.0, 0.0 };
	static const double REFRESH_TIME = 1.0/60.0;	// monitor with 60 Hz
	
	// in milisec
	int t = glutGet(GLUT_ELAPSED_TIME);
	double newTime = (double)t*0.001;

	double deltaTime = newTime - g_appTime;
	if (deltaTime > REFRESH_TIME) deltaTime = REFRESH_TIME;

	// average:
	deltaTime = (deltaTime + lastDeltas[0] + lastDeltas[1] + lastDeltas[2]) * 0.25;

	g_appTime = g_appTime+deltaTime;

	// call Update:
	UpdateScene(deltaTime);

	// render frame:
	RenderScene();

	// save delta:
	lastDeltas[0] = lastDeltas[1];
	lastDeltas[1] = lastDeltas[2];
	lastDeltas[2] = deltaTime;
}

///////////////////////////////////////////////////////////////////////////////
void UpdateScene(double deltaTime) 
{
	if(TestCollision(obb1, obb2)) {
		red = 1.0f;
		green = 0.0f;
		blue = 0.0f;
	} else {
		red = 0.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	tmp1.PrepareMove(Vec3d(0.0), Vec3d(0.0, 0.0, 0.0), deltaTime);
	tmp1.UpdateMove();
	obb1.Rotate(tmp1.m_rotation);

	tmp2.PrepareMove(Vec3d(0.0), Vec3d(0.0, 0.0, 0.0), deltaTime);
	tmp2.UpdateMove();
	obb2.Rotate(tmp2.m_rotation);

	g_camera.Update(deltaTime);
}

///////////////////////////////////////////////////////////////////////////////
void RenderScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// setup camera:
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	g_camera.SetSimpleView();

	// render something
	glUseProgram(g_shaderLight);
	//float col = 0.5f+0.5f*sinf(meshAnim);
	//glColor3f(col, 0.0f, 1.0f-col);

	glPushMatrix();
		double m[16];
		glMultMatrixd(tmp1.GetRotationMatrix4x4(m));
		glTranslatef(tmp1.m_massCenter.m_pos.x, tmp1.m_massCenter.m_pos.y, tmp1.m_massCenter.m_pos.z);
		glScaled(tmp1.m_width, tmp1.m_height, tmp1.m_length);
		glColor4f(red, green, blue, 0.2f);
		glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
		double n[16];
		glMultMatrixd(tmp2.GetRotationMatrix4x4(n));
		glTranslatef(tmp2.m_massCenter.m_pos.x, tmp2.m_massCenter.m_pos.y, tmp2.m_massCenter.m_pos.z);
		glScaled(tmp2.m_width, tmp2.m_height, tmp2.m_length);
		glColor4f(red, green, blue, 0.2f);
		glutSolidCube(1.0);
	glPopMatrix();

	glUseProgram(0);

	//glColor3f(1.0f, 0.0f, 0.0f);

	//	// draw ground:
	//glColor3f(0.8f, 0.5f, 0.3f);
	//glBegin(GL_QUADS);
	//	glNormal3f(0.0f, 1.0f, 0.0f);
	//	glVertex3f(-10.0f, 0.0f, -10.0f);
	//	glVertex3f(-10.0f, 0.0f, 10.0f);
	//	glVertex3f(10.0f, 0.0f, 10.0f);
	//	glVertex3f(10.0f, 0.0f, -10.0f);
	//glEnd();

	// draw text & fps:
	glColor3f(0.0f,1.0f,0.0f);
	utSetOrthographicProjection(g_camera.m_screenWidth, g_camera.m_screenHeight);
		utCalculateAndPrintFps(10, 35);
		utDrawText2D(10,55, "Esc - Quit");
		utDrawText2D(10,70, "R - Reset");
		utDrawText2D(10,85, "Space - Reset Cam");
		char text[16];
		sprintf(text, "steps: %d", tmp1.m_stepCounter);
		//sprintf(text, "rot: %f", obb1.u.M00);
		utDrawText2D(10, 100, text);
	utResetPerspectiveProjection();

	glutSwapBuffers();
}

#pragma endregion
