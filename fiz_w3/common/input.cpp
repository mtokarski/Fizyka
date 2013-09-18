/** @file input.cpp
 *  @brief common input functions
 *
 *	@author Bartlomiej Filipek
 *	@date April 2011
 */

#include "stdafx.h"
#include "input.h"

Camera::Camera()
{
	m_isLeftPressed = false;
	m_isMiddlePressed = false;

	m_angleX = 0.0f;
	m_angleY = 0.0f;
	m_zoom = 0.0f;
	m_deltaAngX = 0.0f;
	m_deltaAngY = 0.0f;
	m_deltaZoom = 0.0f;

	m_lastX = m_lastY = 0;
}

void Camera::ChangeViewportSize(int w, int h)
{
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;

	m_screenWidth = w;
	m_screenHeight = h;
	m_screenRatio = 1.0f * w / h;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set the clipping volume
	gluPerspective(45, m_screenRatio, 0.1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Camera::PressSpecialKey(int key, int x, int y)
{
	switch (key) {
		case GLUT_KEY_LEFT  : m_deltaAngX = -1.1f;break;
		case GLUT_KEY_RIGHT : m_deltaAngX = 1.1f;break;
		case GLUT_KEY_UP    : m_deltaAngY = 1.1f;break;
		case GLUT_KEY_DOWN  : m_deltaAngY = -1.1f; break;
	}
}

void Camera::ReleaseSpecialKey(int key, int x, int y)
{
	switch (key) {
		case GLUT_KEY_LEFT  : m_deltaAngX = 0.0f; break;
		case GLUT_KEY_RIGHT : m_deltaAngX = 0.0f; break;
		case GLUT_KEY_UP    : m_deltaAngY = 0.0f; break;
		case GLUT_KEY_DOWN  : m_deltaAngY = 0.0f; break;
	}
}

void Camera::ProcessMouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{		
		if (state == GLUT_DOWN)
		{
			m_isLeftPressed = true;
			m_lastX = x;
			m_lastY = y;
			//printf("left mouse button pressed...\n");
		}
		else if (state == GLUT_UP)
		{
			m_isLeftPressed = false;
			//printf("left mouse button up...\n");
		}
	}
	else if (button == GLUT_MIDDLE_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			m_isMiddlePressed = true;
			m_lastX = x;
			m_lastY = y;
			//printf("left mouse button pressed...\n");
		}
		else if (state == GLUT_UP)
		{
			m_isMiddlePressed = false;
			//printf("left mouse button up...\n");
		}	
	}	
}

void Camera::ProcessMouseMotion(int x, int y)
{
	int dx = m_lastX - x;
	int dy = m_lastY - y;
	m_lastX = x;
	m_lastY = y;

	if (m_isLeftPressed)
	{
		m_angleX -= dx*0.5f;
		m_angleY -= dy*0.5f;
	}
	if (m_isMiddlePressed)
	{
		m_zoom += dy*0.1f;
	}
}

void Camera::Update(double deltaTime)
{
	m_zoom += m_deltaZoom;
	m_angleX += m_deltaAngX;
	m_angleY += m_deltaAngY;
}

void Camera::SetSimpleView()
{
	glTranslatef(0.0f, 0.0f, -m_zoom);
	glRotatef(m_angleX, 0.0f, 1.0f, 0.0f);
	glRotatef(m_angleY, 1.0f, 0.0f, 0.0f);
}