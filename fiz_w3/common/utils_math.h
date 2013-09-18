/** @file utils_math.h
 *  @brief math utilities
 *
 *	@author Bartlomiej Filipek
 *	@date March 2011
 */

#pragma once

#include <stdio.h>

template <typename T>
class TVector3
{
public:
	union
	{
		T m[3];
		
		struct
		{
			T x, y, z;
		};
	};
public:
	TVector3() { }
	TVector3(const T& a): x(a), y(a), z(a) { }
	TVector3(const T& a, const T& b, const T& c): x(a), y(b), z(c) { }
	TVector3(const TVector3<T> & a) { x = a.x; y = a.y; z = a.z; } 

	void operator += (const TVector3<T> & a) { x += a.x; y += a.y; z += a.z; }
	void operator -= (const TVector3<T> & a) { x -= a.x; y -= a.y; z -= a.z; }
	void operator *= (const T &v) { x *= v; y *= v; z *= v; }
	
	friend TVector3<T> operator+ (const TVector3<T> & a, const TVector3<T> & b) { return TVector3<T>(a.x+b.x, a.y+b.y, a.z+b.z); }
	friend TVector3<T> operator- (const TVector3<T> & a, const TVector3<T> & b) { return TVector3<T>(a.x-b.x, a.y-b.y, a.z-b.z); }
	friend TVector3<T> operator- (const TVector3<T> & a) { return TVector3<T>(-a.x, -a.y, -a.z); }

	friend TVector3<T> operator* (const TVector3<T> & a, const T &v) { return TVector3<T>(a.x*v, a.y*v, a.z*v); }
	friend TVector3<T> operator* (const T &v, const TVector3<T> & a) { return TVector3<T>(a.x*v, a.y*v, a.z*v); }

	friend TVector3<T> operator/ (const TVector3<T> & a, const T &v) { return TVector3<T>(a.x/v, a.y/v, a.z/v); }

	friend T Distance(const TVector3<T> & a, const TVector3<T> & b) { TVector3<T> d = a-b; return d.Length(); }
	friend T DotProduct(const TVector3<T> & a, const TVector3<T> & b) { return a.x*b.x + a.y*b.y + a.z*b.z; }

	T Length() const { return sqrt(x*x + y*y + z*z); }
	void Normalize() { T f = 1.0/Length(); x *= f; y *= f; z *=f; }
};

typedef TVector3<float> Vec3f;
typedef TVector3<double> Vec3d;
typedef TVector3<short> Vec3i;

template <typename T> T Rand(const T &a, const T &b) { return ((T)rand() / RAND_MAX) * (b - a) + a; }