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
	if (pos.y < m_level+margin)
	{
		if (normal) *normal = Vec3d(0, 1, 0);

		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
// PlaneArea
///////////////////////////////////////////////////////////////////////////////
bool PlaneArea::IsInArea(Vec3d &pos, double margin, Vec3d *normal)
{
	double dist = m_normal.x * (pos.x - m_point.x) +
				  m_normal.y * (pos.y - m_point.y) +
				  m_normal.z * (pos.z - m_point.z);

	if (dist-margin < 0.0)
	{
		if (normal) *normal = m_normal;
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
// SphereArea
///////////////////////////////////////////////////////////////////////////////
bool SphereArea::IsInArea(Vec3d &pos, double margin, Vec3d *normal)
{
	Vec3d norm = pos - m_center;
	double dist = norm.Length();

	if (dist < m_radius+margin)
	{
		if (normal)
		{
			norm.Normalize();
			*normal = norm;
		}
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
// InsideBoxArea
///////////////////////////////////////////////////////////////////////////////
bool InsideBoxArea::IsInArea(Vec3d &pos, double margin, Vec3d *normal)
{
	Vec3d norm(0);
	bool ret = false;

	if (pos.x < m_min.x+margin)
	{
		norm.x = -1.0f;
		ret = true;
	}
	if (pos.x > m_max.x-margin)
	{
		norm.x = 1.0f;
		ret = true;
	}
	if (pos.y < m_min.y+margin)
	{
		norm.y = -1.0f;
		ret = true;
	}
	if (pos.y > m_max.y-margin)
	{
		norm.y = 1.0f;
		ret = true;
	}
	if (pos.z < m_min.z+margin)
	{
		norm.z = -1.0f;
		ret = true;
	}
	if (pos.z > m_max.z-margin)
	{
		norm.z = 1.0f;
		ret = true;
	}

	if (normal) *normal = norm;

	return ret;
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
		double normalFactor = DotProduct(force, normal);
		if (normalFactor < 0.0)
			force -= normal * normalFactor;

		double velFactor = DotProduct(point->m_vel, normal);
		//if (velFactor < 0.0)
			point->m_vel -= normal * (1+m_prohibitedArea->m_bounceFactor) * velFactor;

		if (normalFactor < 0.0 && point->m_vel.Length() > 0.0)
		{
			Vec3d friction = -point->m_vel;
			friction.Normalize();
			friction *= fabs(m_prohibitedArea->m_frictionFactor * normalFactor);
			force += friction;
		}

		point->PrepareMoveEuler(deltaTime, force);
	}
}