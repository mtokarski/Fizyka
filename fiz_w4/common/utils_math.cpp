/** @file utils_math.cpp
 *  @brief math utilities, implementation
 *
 *	@author Bartlomiej Filipek
 *	@date May 2011
 */

#include "stdafx.h"
#include "utils_math.h"

//////////////////////////////////////////////////////////////////////////
// Martix4
//////////////////////////////////////////////////////////////////////////

const double Matrix3d::IDENTITY[9] = { 1.0, 0.0, 0.0, 
									   0.0, 1.0, 0.0, 
									   0.0, 0.0, 1.0, };

//////////////////////////////////////////////////////////////////////////
Matrix3d Matrix3d::operator* (const Matrix3d& mt) const
{
	Matrix3d buf;
	int i, j;

	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			buf.M[i + j*3] = M[0 + j*3]*mt.M[i + 0*3] +  M[1 + j*3]*mt.M[i + 1*3] +  M[2 + j*3]*mt.M[i + 2*3];

	return buf;
}

//////////////////////////////////////////////////////////////////////////
Matrix3d Matrix3d::operator* (double v) const
{
	Matrix3d buf;
	int i;

	for (i = 0; i < 9; i++)
		buf.M[i] = M[i]*v;

	return buf;
}

//////////////////////////////////////////////////////////////////////////
Matrix3d Matrix3d::operator+ (const Matrix3d& mt) const
{
	Matrix3d buf;
	int i;

	for (i = 0; i < 9; i++)
		buf.M[i] = M[i] + mt.M[i];

	return buf;
}

//////////////////////////////////////////////////////////////////////////
void Matrix3d::operator*= (const Matrix3d& mt)
{
	Matrix3d buf;
	int i, j;

	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			buf.M[i + j*3] = M[0 + j*3]*mt.M[i + 0*3] +  M[1 + j*3]*mt.M[i + 1*3] +  M[2 + j*3]*mt.M[i + 2*3];

	memcpy((void *)M, (const void *)buf.M, sizeof(Matrix3d));
}

//////////////////////////////////////////////////////////////////////////
void Matrix3d::operator*= (double v)
{
	int i;

	for (i = 0; i < 9; i++)
			M[i] *= v;
}

//////////////////////////////////////////////////////////////////////////
void Matrix3d::operator+= (const Matrix3d& mt)
{
	int i;

	for (i = 0; i < 9; i++)
			M[i] += mt.M[i];
}

//////////////////////////////////////////////////////////////////////////
Vec3d TransformVec3(const Matrix3d& mt, const Vec3d& v)
{
	Vec3d buf;
	buf.x = v.x*mt.M00 + v.y*mt.M10 + v.z*mt.M20;
	buf.y = v.x*mt.M01 + v.y*mt.M11 + v.z*mt.M21;
	buf.z = v.x*mt.M02 + v.y*mt.M12 + v.z*mt.M22;
	return buf;
}

//////////////////////////////////////////////////////////////////////////
void Transpose(const Matrix3d& mt, Matrix3d* out)
{
	out->M00 = mt.M00; out->M01 = mt.M10; out->M02 = mt.M20;
	out->M10 = mt.M01; out->M11 = mt.M11; out->M12 = mt.M21; 
	out->M20 = mt.M02; out->M21 = mt.M12; out->M22 = mt.M22;
}

#define __MATH_OMIT_DIV_BY_ZERO_CHECK

//////////////////////////////////////////////////////////////////////////
int Inverse(const Matrix3d& m, Matrix3d* out)
{
	double d = Det(m);

#ifndef __MATH_OMIT_DIV_BY_ZERO_CHECK
	// are we able to find inverse matrix?
	if (abs(d) < 0.000001)
		return 0;
#endif
	
	d = 1.0 / d;
	out->M00 =  d*(m.M11*m.M22 - m.M21*m.M12);
	out->M10 = -d*(m.M10*m.M22 - m.M20*m.M12);
	out->M20 =  d*(m.M10*m.M21 - m.M20*m.M11);

	out->M01 = -d*(m.M01*m.M22 - m.M21*m.M02);
	out->M11 =  d*(m.M00*m.M22 - m.M20*m.M02);
	out->M21 = -d*(m.M00*m.M21 - m.M20*m.M01);

	out->M02 =  d*(m.M01*m.M12 - m.M11*m.M02);
	out->M12 = -d*(m.M00*m.M12 - m.M10*m.M02);
	out->M22 =  d*(m.M00*m.M11 - m.M10*m.M01);

	return 1;
}

//////////////////////////////////////////////////////////////////////////
// Quaterniond
//////////////////////////////////////////////////////////////////////////

Quaterniond Quaterniond::operator* (const Quaterniond& q)
{
	Quaterniond qprod;
	// standard version:
	//qprod.w = w*q.w - x*q.x - y*q.y - z*q.z;
	//qprod.x = w*q.x + x*q.w + y*q.z - z*q.y;
	//qprod.y = w*q.y - x*q.z + y*q.w - z*q.x;
	//qprod.z = w*q.z + x*q.y - y*q.x + z*q.w;

	// optimalized version:
	double prd_0 = (z - y) * (q.y - q.z);
	double prd_1 = (w + x) * (q.w + q.x);
	double prd_2 = (w - x) * (q.y + q.z);
	double prd_3 = (y + z) * (q.w - q.x);
	double prd_4 = (z - x) * (q.x - q.y);
	double prd_5 = (z + x) * (q.x + q.y);
	double prd_6 = (w + y) * (q.w - q.z);
	double prd_7 = (w - y) * (q.w + q.z);

	double prd_8 = prd_5 + prd_6 + prd_7;
	double prd_9 = 0.5f * (prd_4 + prd_8);

	qprod.w = prd_0 + prd_9 - prd_5;
	qprod.x = prd_1 + prd_9 - prd_8;
	qprod.y = prd_2 + prd_9 - prd_7;
	qprod.z = prd_3 + prd_9 - prd_6;

	return qprod;
}

//////////////////////////////////////////////////////////////////////////
void Quaterniond::operator*= (const Quaterniond& q)
{
	// standard version:
	//Quaterniond qprod;
	//qprod.w = w*q.w - x*q.x - y*q.y - z*q.z;
	//qprod.x = w*q.x + x*q.w + y*q.z - z*q.y;
	//qprod.y = w*q.y - x*q.z + y*q.w - z*q.x;
	//qprod.z = w*q.z + x*q.y - y*q.x + z*q.w;
	//memcpy((void *)m, (const void *)qprod.m, sizeof(Quaterniond));
	
	// optimalized version:
	double prd_0 = (z - y) * (q.y - q.z);
	double prd_1 = (w + x) * (q.w + q.x);
	double prd_2 = (w - x) * (q.y + q.z);
	double prd_3 = (y + z) * (q.w - q.x);
	double prd_4 = (z - x) * (q.x - q.y);
	double prd_5 = (z + x) * (q.x + q.y);
	double prd_6 = (w + y) * (q.w - q.z);
	double prd_7 = (w - y) * (q.w + q.z);

	double prd_8 = prd_5 + prd_6 + prd_7;
	double prd_9 = 0.5f * (prd_4 + prd_8);

	w = prd_0 + prd_9 - prd_5;
	x = prd_1 + prd_9 - prd_8;
	y = prd_2 + prd_9 - prd_7;
	z = prd_3 + prd_9 - prd_6;
}

//////////////////////////////////////////////////////////////////////////
Matrix3d *Quaterniond::ToMatrix3x3(Matrix3d *out)
{
	double x2 = x * x;
	double y2 = y * y;
	double z2 = z * z;
	double xy = x * y;
	double xz = x * z;
	double yz = y * z;
	double xw = x * w;
	double yw = y * w;
	double zw = z * w;
	
	// X
	out->M[0] = 1.0 - 2.0 * (y2 + z2);
	out->M[1] = 2.0 * (xy - zw);
	out->M[2] = 2.0 * (xz + yw);
	// Y
	out->M[3] = 2.0 * (xy + zw);
	out->M[4] = 1.0 - 2.0 * (x2 + z2);
	out->M[5] = 2.0 * (yz - xw);
	// Z
	out->M[6] = 2.0 * (xz - yw);
	out->M[7] = 2.0 * (yz + xw);
	out->M[8] = 1.0 - 2.0 * (x2 + y2);

	return out;
}

//////////////////////////////////////////////////////////////////////////
double *Quaterniond::ToMatrix4x4(double *out)
{
	double x2 = x * x;
	double y2 = y * y;
	double z2 = z * z;
	double xy = x * y;
	double xz = x * z;
	double yz = y * z;
	double xw = x * w;
	double yw = y * w;
	double zw = z * w;
	
	// X
	out[0] = 1.0 - 2.0 * (y2 + z2);
	out[1] = 2.0 * (xy - zw);
	out[2] = 2.0 * (xz + yw);
	out[3] = 0.0;
	// Y
	out[4] = 2.0 * (xy + zw);
	out[5] = 1.0 - 2.0 * (x2 + z2);
	out[6] = 2.0 * (yz - xw);
	out[7] = 0.0;

	// Z
	out[8] = 2.0 * (xz - yw);
	out[9] = 2.0 * (yz + xw);
	out[10] = 1.0 - 2.0 * (x2 + y2);
	out[11] = 0.0;

	out[12] = 0.0;   
	out[13] = 0.0;  
	out[14] = 0.0; 
	out[15] = 1.0;



	return out;
}

//////////////////////////////////////////////////////////////////////////
void Normalize(Quaterniond* q)
{
	double l = 1.0/(sqrt(q->w*q->w + q->x*q->x + q->y*q->y + q->z*q->z));

	q->w = q->w*l;
	q->x = q->x*l;
	q->y = q->y*l;
	q->z = q->z*l;	
}

//////////////////////////////////////////////////////////////////////////
Quaterniond Inverse(const Quaterniond* q)
{
	double l = 1.0/(q->w*q->w + q->x*q->x + q->y*q->y + q->z*q->z);

	return Quaterniond(q->w*l, -q->x*l, -q->y*l, -q->z*l);
}

//////////////////////////////////////////////////////////////////////////
Quaterniond AxisRotationToQuaternion(const Vec3d* axis, double rot)
{
	double t = 0.5f*rot;

	double st = sin(t);

	return Quaterniond(cos(t), st*axis->x, st*axis->y, st*axis->z);
}

//////////////////////////////////////////////////////////////////////////
void QuaternionToVector3dAndAngle(const Quaterniond* q, Vec3d* axis, double &rot)
{
	rot = (double)acos(q->w);

	double buf = 1.0/sin(rot);

	axis->x = q->x*buf;
	axis->y = q->y*buf;
	axis->z = q->z*buf;

	rot*=2.0f;
}

//////////////////////////////////////////////////////////////////////////
// others
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
Vec3d ClosestPoint(const Vec3d &A, const Vec3d &B, const Vec3d &P, double *t)
{
    Vec3d AB         = B - A;
    double ab_square = DotProduct(AB, AB);
    Vec3d AP         = P - A;
    double ap_dot_ab = DotProduct(AP, AB);

	// this should be between 0 and 1
    *t  = ap_dot_ab / ab_square; 

    // calculate the closest point
	// TODO
	// Q is between A and B, param T
    Vec3d Q = A;

    return Q;
}