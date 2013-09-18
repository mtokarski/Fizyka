/** @file collision_shapes.cpp
 *  @brief implementation of collision shapes
 *
 *	@author Bartlomiej Filipek
 *	@date May 2011
 */

#include "stdafx.h"
#include "collision_shapes.h"

const double EPSILON = 0.0001;
const double NEAR_ONE = 0.0009;

///////////////////////////////////////////////////////////////////////////////
// BoundingSphere
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
bool TestCollision(const BoundingSphere &a, const BoundingSphere &b)
{
	// if (collision) return true

	return false;
}

///////////////////////////////////////////////////////////////////////////////
// AABB
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
void AABB::Reset() 
{ 
	/* todo */ 

	/* sets min to max values 
	 * sets max to min values 
	 *
	 * then one can use AddPoint to
	 * update and calculate proper AABB */
};

///////////////////////////////////////////////////////////////////////////////
void AABB::AddPoint(const Vec3d &pt) 
{
	/* todo */ 

	/* if pt is outside the box then update box's min and max */
}

///////////////////////////////////////////////////////////////////////////////
bool TestCollision(const AABB &a, const AABB &b)
{
	// if (collision) return true

	return false;
}

///////////////////////////////////////////////////////////////////////////////
// OBB
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
OBB::OBB(double half_w, double half_h, double half_d) 
{ 
	/* todo */ 
	
	/* create box aligned to axes, centered at (0, 0, 0) */
};

///////////////////////////////////////////////////////////////////////////////
void OBB::Rotate(const Matrix3d& rot) 
{
	/* todo */ 
	this->u = rot;
//	this->axes[0] = Vec3d(rot.M00, rot.M01, rot.M02);
//	this->axes[1] = Vec3d(rot.M10, rot.M11, rot.M12);
//	this->axes[2] = Vec3d(rot.M20, rot.M21, rot.M22);

	this->axes[0] = Vec3d(u.M00, u.M01, u.M02);
	this->axes[1] = Vec3d(u.M10, u.M11, u.M12);
	this->axes[2] = Vec3d(u.M20, u.M21, u.M22);
}

///////////////////////////////////////////////////////////////////////////////
void OBB::Translate(const Vec3d& t) 
{
	/* todo */ 
}

///////////////////////////////////////////////////////////////////////////////
bool TestCollision(const OBB &a, const OBB &b)
{
	// if (collision) return true
	double ra, rb;
	double R[3][3];/*  = {	a.u.M00*b.u.M00 + a.u.M01*b.u.M01 + a.u.M02*b.u.M02, a.u.M00*b.u.M10 + a.u.M01*b.u.M11 + a.u.M02*b.u.M12, a.u.M00*b.u.M20 + a.u.M01*b.u.M21 + a.u.M02*b.u.M22,
						a.u.M10*b.u.M00 + a.u.M11*b.u.M01 + a.u.M12*b.u.M02, a.u.M10*b.u.M10 + a.u.M11*b.u.M11 + a.u.M12*b.u.M12, a.u.M10*b.u.M20 + a.u.M11*b.u.M21 + a.u.M12*b.u.M22,
						a.u.M20*b.u.M00 + a.u.M21*b.u.M01 + a.u.M22*b.u.M02, a.u.M20*b.u.M10 + a.u.M21*b.u.M11 + a.u.M22*b.u.M12, a.u.M20*b.u.M20 + a.u.M21*b.u.M21 + a.u.M22*b.u.M22 };
*/	double absR[3][3];
	bool existsParallel = false;
/*	
	for(int i=0; i<3; i++) {
		for(int j=0; j<3; j++) {
			R[i][j] = a.axes[i].x*b.axes[j].x + a.axes[i].y*b.axes[j].y + a.axes[i].z*b.axes[j].z;
			// return a.x*b.x + a.y*b.y + a.z*b.z;
		}
	}
*/
	R[0][0] = a.u.M00*b.u.M00 + a.u.M01*b.u.M01 + a.u.M02*b.u.M02;
	R[0][1] = a.u.M00*b.u.M10 + a.u.M01*b.u.M11 + a.u.M02*b.u.M12;
	R[0][2] = a.u.M00*b.u.M20 + a.u.M01*b.u.M21 + a.u.M02*b.u.M22;
	R[1][0] = a.u.M10*b.u.M00 + a.u.M11*b.u.M01 + a.u.M12*b.u.M02;
	R[1][1] = a.u.M10*b.u.M10 + a.u.M11*b.u.M11 + a.u.M12*b.u.M12;
	R[1][2] = a.u.M10*b.u.M20 + a.u.M11*b.u.M21 + a.u.M12*b.u.M22;
	R[2][0] = a.u.M20*b.u.M00 + a.u.M21*b.u.M01 + a.u.M22*b.u.M02;
	R[2][1] = a.u.M20*b.u.M10 + a.u.M21*b.u.M11 + a.u.M22*b.u.M12;
	R[2][2] = a.u.M20*b.u.M20 + a.u.M21*b.u.M21 + a.u.M22*b.u.M22;



	Vec3d t = b.c-a.c;
	// t = Vector(Dot(t, a.u[0]), Dot(t, a.u[1]), Dot(t, a.u[2]));
	t = Vec3d(a.axes[0].x*t.x + a.axes[0].y*t.y + a.axes[0].z*t.z, a.axes[1].x*t.x + a.axes[1].y*t.y + a.axes[1].z*t.z, a.axes[2].x*t.x + a.axes[2].y*t.y + a.axes[2].z*t.z);
	// t = Vec3d(fabs(a.axes[0].x*t.x + a.axes[0].y*t.y + a.axes[0].z*t.z), fabs(a.axes[1].x*t.x + a.axes[1].y*t.y + a.axes[1].z*t.z), fabs(a.axes[2].x*t.x + a.axes[2].y*t.y + a.axes[2].z*t.z));

	for(int i=0; i<3; i++) {
		for(int j=0; j<3; j++) {
			absR[i][j] = fabs(R[i][j]);// + EPSILON;
			if(absR[i][j] > NEAR_ONE) { existsParallel = true; }
		}
	}

	// Test axes L = A0, L = A1, L = A2
	for(int i=0; i<3; i++) {
		rb = b.e.x*absR[i][0] + b.e.y*absR[i][1] + b.e.z*absR[i][2];
		switch(i) {
			case 0:
				ra = a.e.x;
				if(fabs(t.x) > ra + rb) return false;
				break;
			case 1:
				ra = a.e.y;
				if(fabs(t.y) > ra + rb) return false;
				break;
			case 2:
				ra = a.e.z;
				if(fabs(t.z) > ra + rb) return false;
				break;
		}
	}

	// Test axes L = B0, L = B1, L = B2
	for(int i=0; i<3; i++) {
		ra = a.e.x*absR[0][i] + a.e.y*absR[1][i] + a.e.z*absR[2][i];
		switch(i) {
			case 0:
				rb = b.e.x;
				break;
			case 1:
				rb = b.e.y;
				break;
			case 2:
				rb = b.e.z;
				break;
		}
		if(fabs(t.x*R[0][i] + t.y*R[1][i] + t.z*R[2][i]) > ra + rb) return false;
	}

	if(existsParallel) return true;

	// Test axis L = A0 x B0
	ra = a.e.y*absR[2][0] + a.e.z*absR[1][0];
	rb = b.e.y*absR[0][2] + b.e.z*absR[0][1];
	if(fabs(t.z*R[1][0] - t.y*R[2][0]) > ra + rb) return false;

	// Test axis L = A0 x B1
	ra = a.e.y*absR[2][1] + a.e.z*absR[1][1];
	rb = b.e.x*absR[0][2] + b.e.z*absR[0][0];
	if(fabs(t.z*R[1][1] - t.y*R[2][1]) > ra + rb) return false;

	// Test axis L = A0 x B2
	ra = a.e.y*absR[2][2] + a.e.z*absR[1][2];
	rb = b.e.x*absR[0][1] + b.e.y*absR[0][0];
	if(fabs(t.z*R[1][2] - t.y*R[2][2]) > ra + rb) return false;

	// Test axis L = A1 x B0
	ra = a.e.x*absR[2][0] + a.e.z*absR[0][0];
	rb = b.e.y*absR[1][2] + b.e.z*absR[1][1];
	if(fabs(t.x*R[2][0] - t.z*R[0][0]) > ra + rb) return false;

	// Test axis L = A1 x B1
	ra = a.e.x*absR[2][1] + a.e.z*absR[0][1];
	rb = b.e.x*absR[1][2] + b.e.z*absR[1][0];
	if(fabs(t.x*R[2][1] - t.z*R[0][1]) > ra + rb) return false;

	// Test axis L = A1 x B2
	ra = a.e.x*absR[2][2] + a.e.z*absR[0][2];
	rb = b.e.x*absR[1][1] + b.e.y*absR[1][0];
	if(fabs(t.x*R[2][2] - t.z*R[0][2]) > ra + rb) return false;

	// Test axis L = A2 x B0
	ra = a.e.x*absR[1][0] + a.e.y*absR[0][0];
	rb = b.e.y*absR[2][2] + b.e.z*absR[2][1];
	if(fabs(t.y*R[0][0] - t.x*R[1][0]) > ra + rb) return false;

	// Test axis L = A2 x B1
	ra = a.e.x*absR[1][1] + a.e.y*absR[0][1];
	rb = b.e.x*absR[2][2] + b.e.z*absR[2][0];
	if(fabs(t.y*R[0][1] - t.x*R[1][1]) > ra + rb) return false;

	// Test axis L = A2 x B2
	ra = a.e.x*absR[1][2] + a.e.y*absR[0][2];
	rb = b.e.x*absR[2][1] + b.e.y*absR[2][0];
	if(fabs(t.y*R[0][2] - t.x*R[1][2]) > ra + rb) return false;

	return true;
}