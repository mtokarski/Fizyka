/** @file mesh_test.cpp
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

#include "material_point.h"
#include "mesh_point_sets.h"

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

#pragma endregion

SphereArea g_sphere;
MeshPointSet g_mesh(15, 15);

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
		glutSolidSphere(0.15f, 8, 8);
	glEndList();

	srand(time(NULL));

	if (utLoadAndBuildShader("data\\light.vert", "data\\light.frag", &g_shaderLight) == false)
		return false;

	g_camera.m_zoom = 10.0f;

	InitSimulation();

	return true;
}

void InitSimulation()
{
	ResetSimulation();

	g_sphere.m_center = Vec3d(2.0, -1.0, 0.0);
	g_sphere.m_radius = 1.0;
	g_sphere.m_bounceFactor = 0.1;
	g_sphere.m_frictionFactor = 0.1;

	g_mesh.m_prohibitedArea = &g_sphere;
}

void ResetSimulation()
{
	g_mesh.m_k = 100.0f;
	g_mesh.m_t = 0.15;
	g_mesh.m_tt = 0.15;
	g_mesh.m_s = 100.01f;
	g_mesh.g.y = -0.75;
	g_mesh.Create(0.3, 0.3);
	g_mesh.SetBound(0, 0, true);
	g_mesh.SetBound(14, 0, true);
	g_mesh.SetBound(1, 0, true);
	g_mesh.SetBound(13, 0, true);
	g_mesh.SetBound(0, 1, true);
	g_mesh.SetBound(14, 1, true);
	//g_mesh.SetBound(14, 14, true);
	//g_mesh.SetBound(0, 14, true);
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
	else if (key == 'w')
	{
		g_sphere.m_center.z += 0.1f;
	}
	else if (key == 's')
	{
		g_sphere.m_center.z -= 0.1f;
	}
	else if (key == 'a')
	{
		g_sphere.m_center.y += 0.1f;
	}
	else if (key == 'd')
	{
		g_sphere.m_center.y -= 0.1f;
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
	g_mesh.Update(deltaTime);



	////Vec3d force(0.0, -5.1*testPoint.m_pos.y, 0.0);
	////if (updateCounter % 30 == 0)
	//{
	//	//testPoint.Update(deltaTime, force);
	//	//utLOG("y: %2.2f\n", testPoint.m_pos.y);
	//}

	//double k = 0.1;
	//double h = 1.9/k;

	//if (updateCounter % 10 == 0)
	//{
	//testPoint.m_nextPos = testPoint.m_pos;
	//testPoint.m_nextPos.y = (1.0 - h*k)*testPoint.m_pos.y;	 
	//testPoint.UpdateParams();
	//testPoint.m_pos.x += 0.05;

	//utLOG("y: %2.2f\n", testPoint.m_pos.y);
	//}

	///*if (testPoint.m_pos.y < -2.0)
	//{
	//	testPoint.m_pos = Vec3d(0.0, 4.0, 0.0);
	//	testPoint.m_vel = Vec3d(0.0, 1.0, 0.0);
	//	testPoint.m_stepCounter = 0;
	//}*/

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

	glColor3f(0.5f, 0.5f, 0.7f);
	for (unsigned int i = 0; i < g_mesh.PointCount(); ++i)
	{
		glPushMatrix();
			glTranslatef(g_mesh.Point(i)->m_pos.x, g_mesh.Point(i)->m_pos.y, g_mesh.Point(i)->m_pos.z);
			glCallList(g_meshDisplayList);
		glPopMatrix();
	}

	glColor3f(1.0f, 0.0f, 0.0f);
	glPushMatrix();
		glTranslatef(g_sphere.m_center.x, g_sphere.m_center.y, g_sphere.m_center.z);
		glutSolidSphere(g_sphere.m_radius, 16, 16);
	glPopMatrix();

	glUseProgram(0);

	//glColor3f(1.0f, 0.0f, 0.0f);
	//glBegin(GL_LINE_STRIP);
	//for (unsigned int i = 0; i < g_line.PointCount(); ++i)
	//{
	//	glVertex3f(g_line.Point(i)->m_pos.x, g_line.Point(i)->m_pos.y, g_line.Point(i)->m_pos.z);
	//}
	//glEnd();

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
		if (MaterialPoint::s_algType == MaterialPoint::atEuler)
			utDrawText2D(10,100, "Euler Integration");
		else
			utDrawText2D(10,100, "Verlet Integration");
	utResetPerspectiveProjection();

	glutSwapBuffers();
}

#pragma endregion
