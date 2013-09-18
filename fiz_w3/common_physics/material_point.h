/** @file material_point.h
 *  @brief declaration of material point and material poin set used in physics simulation
 *
 *	@author Bartlomiej Filipek
 *	@date March 2011
 */

#pragma once

#include <assert.h>
#include "utils_math.h"

///////////////////////////////////////////////////////////////////////////////
/** most of fields, for simplicity, are public - this migh couse some errors when
  * used without care */
class MaterialPoint
{
public:
	enum AlgorithmType { atEuler, atVerlet };
	static AlgorithmType s_algType;
public:
	/** next position calculated by the algorithm */
	Vec3d m_nextPos;
	/** this pos will be used to render point */
	Vec3d m_pos;
	/** position from step back */
	Vec3d m_prevPos;

	Vec3d m_nextVel;
	Vec3d m_vel;

	Vec3d m_acceleration;

	/** by default it is set to 1.0 */
	double m_mass;
	/** by default it is set to 0 */
	double m_radius;

	unsigned int m_stepCounter;
public:
	MaterialPoint();
	~MaterialPoint();

	/** calculates next step of simulation - calculates "next" params */
	void PrepareMove(double deltaTime, const Vec3d &force);
	void PrepareMoveEuler(double deltaTime, const Vec3d &force);
	/** updates simulation */
	void UpdateMove();
	
	/** important method if you want to reset whole simulation */
	void Reset() { m_nextPos = m_pos; m_prevPos = m_pos; m_nextVel = m_vel; m_stepCounter = 0; }
protected:
	void CalcEuler(double delta);
	void CalcVerlet(double delta);
};

///////////////////////////////////////////////////////////////////////////////
class MaterialPointSet
{
protected:
	MaterialPoint *m_points;
	/** if true then smulation is not performed for given point, 
	 *  by default it is false for every point */
	bool *m_bounds;
	unsigned int m_count;
public:
	MaterialPointSet(unsigned int count);
	virtual ~MaterialPointSet();

	virtual Vec3d Force(unsigned int id) = 0;
	virtual void BeforeStep(double deltaTime) { };
	virtual void AfterStep(double deltaTime)  { };
	virtual void Update(double deltaTime);
public:
	unsigned int PointCount() const { return m_count; }
	MaterialPoint *Point(unsigned int i) { assert(i < m_count); return &m_points[i]; } 
	void SetBound(unsigned int id, bool bound) { assert(id < m_count); m_bounds[id] = bound; }
};