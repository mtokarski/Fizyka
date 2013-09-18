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
	zapisz dlugosc "r" w zmieniej nowej "len"
	gdy "len" wieksza od dlugosci spoczynkowej
		znormalizuj "r"
		sila_lewy = k*("len" - dlugosc_spoczynkowa) * r
		// tu wstaw sily tlumienia

	// analogicznie dla prawy
	...

	// tu dodaj sile oporu

	sila = sila_prawy + sila_lewy
	return sila;

	*************************************/ 
	
	Vec3d force(0.0), forceLeft(0.0), forceRight(0.0);

	///////////////////
	// TODO
	///////////////////

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
			sila_sztywnosci[i-1] -= sila/2;
			sila_sztywnosci[i+1] -= sila/2;
	}

	function Sila(i)
	{
		return sila_wlasciwa(i) + sila_sztywnosci[i]
	}

	*************************************/ 
	
	///////////////////
	// TODO
	///////////////////
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


