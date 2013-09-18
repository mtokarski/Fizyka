/** @file prohibited_area.cpp
 *  @brief implementation of prohibited areas, used for simple collisions
 *
 *	@author Bartlomiej Filipek
 *	@date April 2011
 */

#include "stdafx.h"
#include "prohibited_area.h"

///////////////////////////////////////////////////////////////////////////////
// ProhibitedArea
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// FloorArea
///////////////////////////////////////////////////////////////////////////////
bool FloorArea::IsInArea(Vec3d &pos, double margin, Vec3d *normal)
{
	///////////////////
	// TODO
	///////////////////

	bool inside = (pos.y<m_level + margin);
	if(inside && normal!=NULL) *normal = Vec3d(0.0, 1.0, 0.0);
	return inside;

	//return false;
}

///////////////////////////////////////////////////////////////////////////////
// PlaneArea
///////////////////////////////////////////////////////////////////////////////
bool PlaneArea::IsInArea(Vec3d &pos, double margin, Vec3d *normal)
{
	///////////////////
	// TODO
	///////////////////

	return false;
}

///////////////////////////////////////////////////////////////////////////////
// SphereArea
///////////////////////////////////////////////////////////////////////////////
bool SphereArea::IsInArea(Vec3d &pos, double margin, Vec3d *normal)
{
	///////////////////
	// TODO
	///////////////////

	return false;
}

///////////////////////////////////////////////////////////////////////////////
// InsideBoxArea
///////////////////////////////////////////////////////////////////////////////
bool InsideBoxArea::IsInArea(Vec3d &pos, double margin, Vec3d *normal)
{
	///////////////////
	// TODO
	///////////////////
	double min_axesX = m_min.x + margin;
	double max_axesX = m_max.x + margin;

	double min_axesY = m_min.y + margin;
	double max_axesY = m_max.y + margin;

	double min_axesZ = m_min.z + margin;
	double max_axesZ = m_max.z + margin;

	bool inside = false;

	if(pos.x < min_axesX || pos.x > max_axesX || pos.y < min_axesY || pos.y > max_axesY || pos.z < min_axesZ || pos.z > max_axesZ) inside = true;

	if(inside && normal!=NULL) {
		if(pos.x < min_axesX) normal->x = 1;
		if(pos.x > max_axesX) normal->x = -1;

		if(pos.y < min_axesY) normal->y = 1;
		if(pos.y > max_axesY) normal->y = -1;

		if(pos.z < min_axesZ) normal->z = 1;
		if(pos.z > max_axesZ) normal->z = -1;
	}
	return inside;
	//return false;
}

///////////////////////////////////////////////////////////////////////////////
// ComplesArea
///////////////////////////////////////////////////////////////////////////////
bool ComplexArea::IsInArea(Vec3d &pos, double margin, Vec3d *normal)
{
	for (size_t i = 0; i < m_areas.size(); ++i)
	{
		if (m_areas[i]->IsInArea(pos, margin, normal)) return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
// MaterialPointSetWithProhibitedArea
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
void MaterialPointSetWithProhibitedArea::Update(double deltaTime)
{
	BeforeStep(deltaTime);
	
	for (unsigned int i = 0; i < m_count; ++i)
	{
		if (m_bounds[i] == false)
		{
			m_points[i].PrepareMove(deltaTime, Force(i));

			if (m_prohibitedArea)
			{
				if (m_prohibitedArea->IsInArea(m_points[i].m_nextPos, m_points[i].m_radius, NULL))
				{
					UpdateOnCollision(i, deltaTime);
				}
			}
		}
	}

	AfterStep(deltaTime);

	for (unsigned int i = 0; i < m_count; ++i)
	{
		if (m_bounds[i] == false)
			m_points[i].UpdateMove();
	}
}

///////////////////////////////////////////////////////////////////////////////
void MaterialPointSetWithProhibitedArea::UpdateOnCollision(int id, double deltaTime)
{
	MaterialPoint *point = &m_points[id];
	Vec3d normal(0, 0, 0);
	if (m_prohibitedArea->IsInArea(point->m_nextPos, point->m_radius, &normal))
	{
		Vec3d force = Force(id);
		
		double normFactor = DotProduct(force, normal);
		if(normFactor < 0) {
			force -= normal*normFactor;
		}

		double velFactor = DotProduct(point->m_vel, normal);
		point->m_vel -= normal*(1 + m_prohibitedArea->m_bounceFactor)*velFactor;

		if(normFactor < 0.0 && point->m_vel.Length() > 0.0) { //m_vel - need to get length
			Vec3d friction = -point->m_vel;
			friction.Normalize();
			friction *= fabs(m_prohibitedArea->m_frictionFactor*normFactor);
			force += friction;
		}
		/* pseudokod: ************************
	
		wspolczynnik_normalej = dot_product (force, normal)
		gdy wspolczynnik_normalej < 0
			sila -= normal * wspolczynnik_normalej

		wspolczynnik_predkosci = dot_product(point->vel, normal)
		point->vel -= normal * (1+wspolczynnik_odbicia_obszaru) * wspolczynnik_predkosci

		// policz tarcie:
		if (wspolczynnik_normalej < 0.0 oraz wartosc point->m_vel > 0.0)
		{
			tarcie = -point->m_vel;
			tarcie.Normalize();
			tacie *= fabs(wspolczynnik_tarcia_obszaru * wspolczynnik_normalej);
			sila += tarcie
		}

		*************************************/ 
		
		///////////////////
		// TODO
		///////////////////


		// policz jeszcze raz ruch
		point->PrepareMoveEuler(deltaTime, force);
	}
}