/** @file utils.cpp
 *  @brief impleentation of useful utilities for opengl based apps
 *
 *	@author Bartlomiej Filipek
 *	@date March 2011
 */

#include "stdafx.h"
#include "utils.h"

///////////////////////////////////////////////////////////////////////////////
bool utInitOpenGL20()
{
	// init OpenGL extensions (like shader support, framebuffer object, etc)
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		utLOG_ERROR("GLEW Error %s\n", glewGetErrorString(err));
		return false;
	}
	utLOG("Using GLEW %s\n", glewGetString(GLEW_VERSION));

	if (glewIsSupported("GL_VERSION_2_0"))
	{
		utLOG("Ready for OpenGL 2.0\n");
	}
	else 
	{
		utLOG_ERROR("OpenGL 2.0 not supported\n");
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
void utSetOrthographicProjection(int scrW, int scrH) {

	// switch to projection mode
	glMatrixMode(GL_PROJECTION);
	// save previous matrix which contains the 
	//settings for the perspective projection
	glPushMatrix();
	// reset matrix
	glLoadIdentity();
	// set a 2D orthographic projection
	gluOrtho2D(0, scrW, 0, scrH);
	// invert the y axis, down is positive
	glScalef(1, -1, 1);
	// mover the origin from the bottom left corner
	// to the upper left corner
	glTranslatef(0, -(float)scrH, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

///////////////////////////////////////////////////////////////////////////////
void utResetPerspectiveProjection() {
	// set the current matrix to GL_PROJECTION
	glMatrixMode(GL_PROJECTION);
	// restore previous settings
	glPopMatrix();
	// get back to GL_MODELVIEW matrix
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

///////////////////////////////////////////////////////////////////////////////
void utDrawText2D(float x, float y, void *font, char *string)
{
  char *c;
  // set position to start drawing fonts
  glRasterPos2f(x, y);
  // loop all the characters in the string
  for (c=string; *c != '\0'; c++) {
    glutBitmapCharacter(font, *c);
  }
}

///////////////////////////////////////////////////////////////////////////////
void utCalculateAndPrintFps(float x, float y)
{
	static char fpsStr[16];
	static unsigned int frame = 0;
	static int timeBase = 0;
	
	frame++;

	int t = glutGet(GLUT_ELAPSED_TIME);
	if (t - timeBase > 1000) {
		sprintf(fpsStr, "FPS: %4.2f", frame*1000.0/(t - timeBase));
		timeBase = t;		
		frame = 0;
	}

	utDrawText2D(x, y, fpsStr);
}

///////////////////////////////////////////////////////////////////////////////
void utDrawTransparentBox(float x, float y, float z, float w, float h, float d)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPushMatrix();		
		glTranslatef(x, y, z);
		glScaled(w, h, d);

		glDepthMask(false);
		glutSolidCube(1.0f);
		glDepthMask(true);
		
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		// red lines:
		glColor3f(1.0f, 0.0f, 0.0f);
		glutSolidCube(1.0f);
		glEnable(GL_CULL_FACE);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPopMatrix();
	glDisable(GL_BLEND);
}

///////////////////////////////////////////////////////////////////////////////
void utDrawTransparentSphere(float x, float y, float z, float r)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPushMatrix();		
		glTranslatef(x, y, z);

		glDepthMask(false);
		glutSolidSphere(r, 16, 16);
		glDepthMask(true);
		
		//glDisable(GL_CULL_FACE);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//// red lines:
		//glColor3f(1.0f, 0.0f, 0.0f);
		//glutSolidSphere(r, 16, 6);
		//glEnable(GL_CULL_FACE);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPopMatrix();
	glDisable(GL_BLEND);
}