#include "StdAfx.h"
#include "MaterialPoint.h"

MaterialPoint::MaterialPoint(void) {}

MaterialPoint::~MaterialPoint(void) {}

void MaterialPoint::PrepareMove(double deltaTime, const Vec3d &force)
{
	m_acceleration = force*(1.0/m_mass);

	/*
	if (m_algType == atVolr || m_stepCounter == 0) CalcEuler(deltaTime);    
	else CalcVerlet(deltaTime);
	*/
	CalcEuler(deltaTime);
}

void MaterialPoint::UpdateMove()
{
	m_prevPos = m_pos;
	m_pos = m_nextPos;
	m_vel = m_nextVel;
	
	m_stepCounter++;
}

void MaterialPoint::CalcEuler(double delta)
{
	m_nextVel = m_vel + m_acceleration*delta;
	m_nextPos = m_pos + m_nextVel*delta;
}

void MaterialPoint::CalcVerlet(double delta){
	m_nextPos = 2*m_pos - m_prevPos + m_acceleration*(delta*delta);
	m_nextVel = (m_nextPos - m_prevPos)/(2*delta);
}