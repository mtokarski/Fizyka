/** @file mesh_point_sets.cpp
 *  @brief implementation of different point sets
 *
 *	@author Bartlomiej Filipek
 *	@date April 2011
 */

#include "stdafx.h"
#include "material_point.h"
#include "mesh_point_sets.h"

static Vec3i g_vecDir[8] =
{
	Vec3i(0,1,0),
	Vec3i(1,1,0),
	Vec3i(1,0,0),
	Vec3i(1,-1,0),
	Vec3i(0,-1,0),
	Vec3i(-1,-1,0),
	Vec3i(-1,0,0),
	Vec3i(-1,1,0)
};

///////////////////////////////////////////////////////////////////////////////
// MeshPointSet
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
MeshPointSet::MeshPointSet(unsigned int nx, unsigned ny) :
	MaterialPointSetWithProhibitedArea(nx*ny)
{
	m_Nx = nx;
	m_Ny = ny;

	m_stiffnessForces = new Vec3d[nx*ny];
}

///////////////////////////////////////////////////////////////////////////////
void MeshPointSet::Create(double lx, double ly)
{
	m_lx = lx;
	m_ly = ly;

	for(int ix=0;ix<m_Nx;++ix)
		for(int iy=0;iy<m_Ny;++iy)
		{
			int i=ix+m_Nx*iy;
			m_stiffnessForces[i]=Vec3d(0);
			m_points[i].m_pos.x = ix*lx;
			m_points[i].m_pos.y = 0.0;
			m_points[i].m_pos.z = iy*ly;

			m_points[i].m_vel = Vec3d(0);
			m_points[i].Reset();
		}	
}

///////////////////////////////////////////////////////////////////////////////
Vec3d MeshPointSet::Force(unsigned int i)
{
	Vec3d force(0.0);

	int nx = i % m_Nx;
	int ny = (i-nx)/m_Nx;
			
	for(int dir = 0; dir < 8; dir++)
	{
		int nxNeighbour = nx + g_vecDir[dir].x;
		int nyNeighbour = ny + g_vecDir[dir].y;				
		int neighbourID = nxNeighbour + nyNeighbour*m_Nx;

		if(nxNeighbour >= 0 && nxNeighbour < m_Nx && nyNeighbour >= 0 && nyNeighbour < m_Ny)
		{
			Vec3d to_neighbour = m_points[neighbourID].m_pos-m_points[i].m_pos;
			double baseLen = sqrt((g_vecDir[dir].x*m_lx)*(g_vecDir[dir].x*m_lx)+(g_vecDir[dir].y*m_ly)*(g_vecDir[dir].y*m_ly));
			double diff = to_neighbour.Length() - baseLen;
			
			if (m_forceOnStretch == false || diff > 0.0)
			{
				to_neighbour.Normalize();
				force += m_k *diff * to_neighbour;
				Vec3d velDiff = m_points[neighbourID].m_vel - m_points[i].m_vel;
				force += m_tt * DotProduct(to_neighbour, velDiff) * to_neighbour;
			}
		}
	}
	if (m_t!=0) force -= m_points[i].m_vel*m_t; //tlumienie
	if (m_s!=0) force+=m_stiffnessForces[i];
	force+=m_points[i].m_mass*g;

	return force;
}

///////////////////////////////////////////////////////////////////////////////
void MeshPointSet::BeforeStep(double deltaTime)
{
	MaterialPointSet::BeforeStep(deltaTime);
			
	for(int i=0; i < m_count; i++) m_stiffnessForces[i] = Vec3d(0);
	if (m_s == 0) return;

	for(int ix = 1; ix < m_Nx - 1; ++ix)
		for(int iy = 1; iy < m_Ny - 1; ++iy)
		{
			int id = ix + m_Nx * iy;
					
			Vec3d toNeighbours = Vec3d(0);
			for(int dir = 0; dir < 8; dir += 1)
			{
				int nxNeighbour = ix + g_vecDir[dir].x;
				int nyNeighbour = iy + g_vecDir[dir].y;				
				int neighbourID = nxNeighbour + nyNeighbour*m_Nx;
				if(nxNeighbour >= 0 && nxNeighbour < m_Nx && nyNeighbour >= 0 && nyNeighbour < m_Ny)
				{					
					Vec3d diff = m_points[neighbourID].m_pos - m_points[id].m_pos;
					toNeighbours+=diff;
				}						
			}
			toNeighbours *= (1.0/8.0);
			Vec3d force = m_s * toNeighbours;

			m_stiffnessForces[id] += force;
			for(int dir = 0; dir < 8; dir += 1)
			{
				int nxNeighbour = ix + g_vecDir[dir].x;
				int nyNeighbour = iy + g_vecDir[dir].y;				
				int neighbourID = nxNeighbour + nyNeighbour*m_Nx;
				m_stiffnessForces[neighbourID] -= force*(1.0/8.0);
			}
		}
}