/** @file point_sets.cpp
 *  @brief implementation of different point sets
 *
 *	@author Bartlomiej Filipek
 *	@date April 2011
 */

#include "stdafx.h"
#include "material_point.h"
#include "point_sets.h"

///////////////////////////////////////////////////////////////////////////////
// OscilatorPointSet
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
OscilatorPointSet::OscilatorPointSet(unsigned int count) : MaterialPointSet(count)
{
	m_baseLen = 0.0;
	m_k = 0.0;
	m_resistance = 0.0;
	m_suppression = 0.0;
	m_forceOnStretch = false;
}

///////////////////////////////////////////////////////////////////////////////
Vec3d OscilatorPointSet::Force(unsigned int i)
{
	/* pseudokod: ************************
	
	sila = 0; sila_prawy = 0; sila_lewy = 0;

	// policz sile dla lewy:
	vec r = points[i-1].pos - points[i]
	gdy dlugosc "r" wieksza od dlugosci spoczynkowej
		znormalizuj "r"
		sila_lewy = k*(dlugosc "r" - dlugosc_spoczynkowa) * r
		// tu wstaw sily tlumienia

	// analogicznie dla prawy
	...

	// tu dodaj sile oporu

	sila = sila_prawy + sila_lewy
	return sila;

	*************************************/ 
	
	Vec3d force(0.0), forceLeft(0.0), forceRight(0.0);

	if (i > 0)
	{
		Vec3d r = m_points[i-1].m_pos-m_points[i].m_pos;
		double dist = r.Length();
		if (m_forceOnStretch == false || dist > m_baseLen)
		{
			r.Normalize();
			forceLeft = m_k*(dist-m_baseLen)*r;

			Vec3d velDiff = m_points[i-1].m_vel - m_points[i].m_vel;
			forceLeft += m_suppression * DotProduct(r,velDiff)*r;
		}
	}
	if (i < m_count-1)
	{
		Vec3d r = m_points[i+1].m_pos-m_points[i].m_pos;
		double dist = r.Length();
		if (m_forceOnStretch == false || dist > m_baseLen)
		{
			r.Normalize();
			forceRight = m_k*(dist-m_baseLen)*r;

			Vec3d velDiff = m_points[i+1].m_vel - m_points[i].m_vel;
			forceLeft += m_suppression * DotProduct(r,velDiff)*r;
		}
	}
		
	Vec3d res = -2.0 * m_resistance * m_points[i].m_vel;
	force = forceLeft + forceRight + res;

	return force;
}

///////////////////////////////////////////////////////////////////////////////
// StiffOscilatorPointSet
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
StiffOscilatorPointSet::StiffOscilatorPointSet(unsigned int count) :
	OscilatorPointSet(count)
{
	m_stiffnessForces = new Vec3d[count];
	m_forceOnStretch = true;
}

///////////////////////////////////////////////////////////////////////////////
Vec3d StiffOscilatorPointSet::Force(unsigned int i)
{
	Vec3d force = OscilatorPointSet::Force(i);
	if (m_stiffness > 0.0) force += m_stiffnessForces[i];

	return force;
}

///////////////////////////////////////////////////////////////////////////////
void StiffOscilatorPointSet::BeforeStep(double deltaTime)
{
	/* pseudokod: ************************
	
	function BeforeStep()
	{
		for each point: sila_sztywnosci[point] = 0;

		for each point:
			do_lewego = point[i-1].pos - point[i].pos
			do_prawego = point[i+1].pos - point[i].pos
			sila = s*(do_prawego + do_lewego)/2

			sila_sztywnosci[i] += sila;
			sila_sztywnosci[i-1] += sila/2;
			sila_sztywnosci[i+1] += sila/2;
	}

	function Sila(i)
	{
		return sila_wlasciwa(i) + sila_sztywnosci[i]
	}

	*************************************/ 
	
	for (unsigned int i = 0; i < m_count; ++i) m_stiffnessForces[i] = Vec3d(0.0, 0.0, 0.0);

	if (m_stiffness <= 0.0) return;

	for (unsigned int i = 1; i < m_count-1; ++i)
	{
		Vec3d toLeft  = m_points[i-1].m_pos - m_points[i].m_pos;
		Vec3d toRight = m_points[i+1].m_pos - m_points[i].m_pos;
		Vec3d stiffnessForce = m_stiffness *(toRight + toLeft)*0.5;
		m_stiffnessForces[i] += stiffnessForce;
		m_stiffnessForces[i-1]-=stiffnessForce/2;
		m_stiffnessForces[i+1]-=stiffnessForce/2;
	}
}

///////////////////////////////////////////////////////////////////////////////
// LinePointSet
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
LinePointSet::LinePointSet(unsigned int count) :
	StiffOscilatorPointSet(count),
	m_gForce(0.0, -9.0, 0.0)
{
	m_forceOnStretch = true;
}

///////////////////////////////////////////////////////////////////////////////
void LinePointSet::Reset(double basicLen, const Vec3d &start, const Vec3d &dir)
{
	m_baseLen = basicLen;

	m_bounds[0] = true;
	//m_bounds[1] = true;

	for (unsigned int i = 0; i < m_count; ++i)
	{
		m_points[i].Reset();
		m_points[i].m_pos = start + (double)i * basicLen * dir;
		m_points[i].m_vel = Vec3d(0.0, 0.0, 0.0);
	}
	m_bounds[0] = true;
}

///////////////////////////////////////////////////////////////////////////////
Vec3d LinePointSet::Force(unsigned int i)
{
	Vec3d force = StiffOscilatorPointSet::Force(i);

	force += m_points[i].m_mass * m_gForce;

	return force;
}