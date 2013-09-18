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

#include "input.h"

#include "material_point.h"
#include "prohibited_area.h"

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

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class FountainPointSet : public MaterialPointSetWithProhibitedArea
{
public:
	FountainPointSet(unsigned int count, ProhibitedArea *area) : MaterialPointSetWithProhibitedArea(count, area)
	{
		ResetAll();
	}
	~FountainPointSet() { }

	virtual Vec3d Force(unsigned int i)
	{
		return Vec3d(0.0, -9.5, 0.0);
	}

	void ResetPoint(unsigned int i)
	{
		m_points[i].m_pos.x = m_points[i].m_prevPos.x = -2;
		m_points[i].m_pos.y = m_points[i].m_prevPos.y = 1;
		m_points[i].m_pos.z = m_points[i].m_prevPos.z = -2;
		m_points[i].m_vel.x = Rand(-1.5, 1.5);
		m_points[i].m_vel.y = Rand(6.1, 10.0);
		m_points[i].m_vel.z = Rand(-1.5, 1.5);
		m_points[i].m_mass = 1.0;
		m_points[i].m_radius = 0.25;
		m_points[i].Reset();
	}

	void ResetAll()
	{
		for (unsigned int i = 0; i < m_count; ++i)
			ResetPoint(i);
	}
};

#define POINT_COUNT 3000
#define NUM_SPHERE_AREAS 20

FloorArea g_floorArea(0.0);
SphereArea g_sphereAreas[NUM_SPHERE_AREAS];
PlaneArea g_planeArea(Vec3d(1.0, 0.0, 0.0), Vec3d(-10.0, 0.0, 0.0));
ComplexArea g_complexArea(0.0999, 0.3);
FountainPointSet g_fountain(POINT_COUNT, &g_complexArea);

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
		glutSolidSphere(0.25f, 10, 10);
	glEndList();

	srand(time(NULL));

	g_camera.m_angleX = 0.0f;
	g_camera.m_angleY = 30.0f;
	g_camera.m_zoom = 20.0f;

	if (utLoadAndBuildShader("data\\light.vert", "data\\light.frag", &g_shaderLight) == false)
		return false;

	InitSimulation();

	return true;
}

void InitSimulation()
{
	g_complexArea.Add(&g_floorArea);
	//g_complexArea.Add(&g_planeArea);
	for (int i = 0; i < NUM_SPHERE_AREAS; ++i)
	{
		g_sphereAreas[i].m_center.x = Rand(-8.0, 8.0);
		g_sphereAreas[i].m_center.y = Rand(0.5, 2.0);;
		g_sphereAreas[i].m_center.z = Rand(-8.0, 8.0);
		g_sphereAreas[i].m_radius = g_sphereAreas[i].m_center.y;
		g_complexArea.Add(&g_sphereAreas[i]);
	}

	ResetSimulation();
}

void ResetSimulation()
{
	g_fountain.ResetAll();
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
		g_camera.m_angleY = 30.0f;
		g_camera.m_zoom = 17.0f;
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
		g_camera.m_angleY = 30.0f;
		g_camera.m_zoom = 17.0f;
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
	g_fountain.Update(deltaTime);

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
	
	glColor3f(0.5f, 0.5f, 0.7f);
	for (unsigned int i = 0; i < g_fountain.PointCount(); ++i)
	{
		glPushMatrix();
			glTranslatef(g_fountain.Point(i)->m_pos.x, g_fountain.Point(i)->m_pos.y, g_fountain.Point(i)->m_pos.z);
			glCallList(g_meshDisplayList);
		glPopMatrix();
	}

	// draw sphere area:
	for (int i = 0; i < NUM_SPHERE_AREAS; ++i)
	{
		glPushMatrix();
			glColor3f(0.1f, 0.1f, 0.7f);
			glTranslatef(g_sphereAreas[i].m_center.x, g_sphereAreas[i].m_center.y, g_sphereAreas[i].m_center.z);
			glutSolidSphere(g_sphereAreas[i].m_radius, 16, 16);
		glPopMatrix();
	}

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

	/*glColor3f(0.8f, 0.4f, 0.1f);
	glBegin(GL_QUADS);
		glVertex3f(g_planeArea.m_point.x, 10.0f, -10.0f);
		glVertex3f(g_planeArea.m_point.x, -10.0f, -10.0f);
		glVertex3f(g_planeArea.m_point.x, -10.0f, 10.0f);
		glVertex3f(g_planeArea.m_point.x, 10.0f, 10.0f);
	glEnd();*/

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
