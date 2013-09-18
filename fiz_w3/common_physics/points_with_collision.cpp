/** @file points_with_collision.cpp
 *  @brief implementation of of material point sets that collide with each others
 *
 *	@author Bartlomiej Filipek
 *	@date April 2011
 */

#include "stdafx.h"
#include "points_with_collision.h"

///////////////////////////////////////////////////////////////////////////////
// PointSetWithCollisions
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
PointSetWithCollisions::PointSetWithCollisions(int count) :
	MaterialPointSetWithProhibitedArea(count)
{
	m_epsilon = 1.0;
}

///////////////////////////////////////////////////////////////////////////////
void PointSetWithCollisions::CheckCollisions(double deltaTime)
{
	for(unsigned int i = 0; i < m_count; i++)
	{
		double mI = m_points[i].m_mass;

		for(unsigned int j = i + 1; j < m_count; j++)
		{										
			double nextDistance = (m_points[i].m_nextPos - m_points[j].m_nextPos).Length();

			if(nextDistance <= (m_points[i].m_radius + m_points[j].m_radius))
			{						
				/*
				 - calculate N: vector between i and j point's pos
				 - calculte "mi" (mass_i*mass_j/sum masses)
				 - calculate dot product between vel diference and Normal N
				 - calculate output vel...
				*/
				
			
				/* new vel:
				 m_points[i].m_vel = ??;
				 m_points[j].m_vel = ??;
				*/
					
				// prepare move based on new velocities:
				m_points[i].PrepareMoveEuler(deltaTime, Force(i));
				m_points[j].PrepareMoveEuler(deltaTime, Force(j));
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void PointSetWithCollisions::AfterStep(double deltaTime)
{
	CheckCollisions(deltaTime);
}