/** @file input.h
 *  @brief common input functions
 *
 *	@author Bartlomiej Filipek
 *	@date April 2011
 */

#pragma once

class Camera
{
public:
	float m_angleX, m_angleY, m_zoom;
	float m_deltaAngX, m_deltaAngY, m_deltaZoom;

	bool m_isLeftPressed;
	bool m_isMiddlePressed;
	int	m_lastX;
	int m_lastY;

	int m_screenWidth;
	int m_screenHeight;
	float m_screenRatio;
public:
	Camera();
	~Camera() { }

	void ChangeViewportSize(int w, int h);
	void PressSpecialKey(int key, int x, int y);
	void ReleaseSpecialKey(int key, int x, int y);
	void ProcessMouse(int button, int state, int x, int y);
	void ProcessMouseMotion(int x, int y);

	void Update(double deltaTime);
	void SetSimpleView();
};



		