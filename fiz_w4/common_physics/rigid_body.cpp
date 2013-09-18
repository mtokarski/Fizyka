/** @file rigid_body.cpp
 *  @brief implementation of rigid bodies base structures
 *
 *	@author Bartlomiej Filipek
 *	@date May 2011
 */

#include "stdafx.h"
#include "material_point.h"
#include "rigid_body.h"

///////////////////////////////////////////////////////////////////////////////
// RigidBody
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
RigidBody::RigidBody(Vec3d pos, Vec3d vel, Vec3d angVel, Matrix3d momentOfInertiaTensor) 
{
	m_massCenter.m_pos = pos;
	m_massCenter.m_vel = vel;
	m_massCenter.Reset();

	m_angularVel = angVel;
	m_nextAngularVel = angVel;

#ifdef USE_QUATERNIONS_FOR_ROTATIONS
	m_rotation = m_prevRotation = m_nextRotation = AxisRotationToQuaternion(&vel, 0.0);
#else
	Identity(&m_rotation);
	Identity(&m_prevRotation);
	Identity(&m_nextRotation);
#endif

	m_momentOfInertiaTensor = momentOfInertiaTensor;
	Inverse(m_momentOfInertiaTensor, &m_momentOfInertiaTensorInv);

	m_stepCounter = 0;
}

///////////////////////////////////////////////////////////////////////////////
RigidBody::RigidBody() :
	m_angularVel(0.0, 0.0, 0.0)
{
	m_nextAngularVel = m_angularVel;

#ifdef USE_QUATERNIONS_FOR_ROTATIONS
	m_rotation = m_prevRotation = m_nextRotation = AxisRotationToQuaternion(&m_angularVel, 0.0);
#else
	Identity(&m_rotation);
	Identity(&m_prevRotation);
	Identity(&m_nextRotation);
#endif

	Identity(&m_momentOfInertiaTensor); 
	Identity(&m_momentOfInertiaTensorInv);

	m_stepCounter = 0;
}

///////////////////////////////////////////////////////////////////////////////
RigidBody::~RigidBody()
{

}

///////////////////////////////////////////////////////////////////////////////
void RigidBody::PrepareRotationEuler(const Vec3d& momentOfForce, double deltaTime)
{
	Vec3d temp1=TransformVec3(m_momentOfInertiaTensor, m_angularVel);
	//Vec3d temp2 = MomentOfForce - CrossProduct(m_angularVel, temp2);
	Vec3d temp2=momentOfForce-CrossProduct(m_angularVel, temp1);
	m_nextAngularVel=m_angularVel+(TransformVec3(m_momentOfInertiaTensorInv, temp2))*deltaTime;

	// velocity:
	// pseudocode:
	//
	// temp1 = moment_of_inertia_tensor * omega(t)
	// temp2 = MomentOfForce - CROSS(omega(t), temp2)
	// omega(t + 1) = omega(t) + (moment_of_inertia_tensor_INV * temp2) * delta_time
	// use: TransformVec3 to multiply vector 3d by matrix


	// rotation
#ifdef USE_QUATERNIONS_FOR_ROTATIONS
	// quaternion:
	// pseudocode
	//
	// quat_derviative = Quat(0, omega(t).x, omega(t).y, omega(t).z) * rotation * 0.5
	// next_rotation = rotation + quat_derviative * delta_time
	// normalize next_rotation

	Quaterniond quat_dervitive=(Quaterniond(0, m_angularVel.x, m_angularVel.y, m_angularVel.z))*m_rotation*0.5;
	m_nextRotation=m_rotation + quat_dervitive*deltaTime;
	Normalize(&m_nextRotation);
#else
	// matrix:
	// pseudocode:
	//
	// rot_derviative = StarOperator(omega(t) * rotation
	// next_rotation = rotation + rot_derviative * delta_time
	//
	//
	Matrix3d rot_derviative = StarOperator(m_angularVel)*m_rotation;
	m_nextRotation = m_rotation + rot_derviative*deltaTime;
#endif
	
}

///////////////////////////////////////////////////////////////////////////////
void RigidBody::PrepareMove(const Vec3d& force, const Vec3d& momentOfForce, double deltaTime)
{
	// prepare move for m_massCenter
	m_massCenter.PrepareMove(deltaTime,force);
	//
	// calc rotation, call PrepareRotationEuler
	PrepareRotationEuler(momentOfForce,deltaTime);
}

///////////////////////////////////////////////////////////////////////////////
void RigidBody::UpdateMove()
{
	m_prevRotation = m_rotation;	
	m_rotation = m_nextRotation;

	m_angularVel = m_nextAngularVel;
	m_massCenter.UpdateMove();
	
	++m_stepCounter;
}

///////////////////////////////////////////////////////////////////////////////
// RigidBodySet
///////////////////////////////////////////////////////////////////////////////
RigibBodySet::RigibBodySet(unsigned int count)
{
	m_count = count;

	// allocate memory:
	m_bodies = new RigidBody[count];
	m_bounds = new bool[count];

	// default:
	for (unsigned int i = 0; i < count; ++i)
		m_bounds[i] = false;
}

RigibBodySet::~RigibBodySet()
{
	delete [] m_bodies;
	m_bodies = NULL;
	delete [] m_bounds;
	m_bounds = NULL;
}

void RigibBodySet::Update(double deltaTime)
{
	// to do...
}