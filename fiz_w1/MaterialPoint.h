#include "TVector3.h"

class MaterialPoint
{
public:
	Vec3d m_nextPos;
	Vec3d m_pos;
	Vec3d m_prevPos;
	Vec3d m_nextVel;
	Vec3d m_vel;
	Vec3d m_acceleration;
	double m_mass;
	double m_radius;
	unsigned int m_stepCounter;
public:
	MaterialPoint();
	~MaterialPoint();
	void PrepareMove(double deltaTime, const Vec3d &force);
	void UpdateMove();
	void Reset();
protected:
	void CalcEuler(double delta);
	void CalcVerlet(double delta);
};