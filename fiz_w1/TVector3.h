#pragma once

template <typename T>
class TVector3
{
public:
	// dziêki unii bêdziemy mogli siê dostawaæ do Pol klasy zarówno po
	// nazwie wspó³rzêdnej, jak i poprzez adres w tablicy...
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
	TVector3(const T& a, const T& b, const T& c): x(a), y(b), z(c) { }
	// przyk³adowa implementacja dodawania
	friend TVector3<T> operator+ (const TVector3<T> & a, const TVector3<T> & b)
	{ return TVector3<T>(a.x+b.x, a.y+b.y, a.z+b.z); }

	// niezaimplementowane operatory – do zrobienia!
	friend TVector3<T> operator- (const TVector3<T> & a, const TVector3<T> & b)
	{ return TVector3<T>(a.x-b.x, a.y-b.y, a.z-b.z); }

	friend TVector3<T> operator* (const TVector3<T> & a, const T &v) { return TVector3<T>(a.x*v, a.y*v, a.z*v); }
	friend TVector3<T> operator* (const T &v, const TVector3<T> & a) { return TVector3<T>(v* a.x,v*a.y, v*a.z); }
	friend TVector3<T> operator/ (const TVector3<T> & a, const T &v) { return TVector3<T>(a.x / v, a.y / v, a.z / v); }

	T Length() const { return sqrt(x*x + y*y + z*z); }
	void Normalize() { T f = 1.0/Length(); x *= f; y *= f; z *=f; }
};
	// specyfikacja szablonu dla typu float oraz double:
	typedef TVector3<float> Vec3f;
	typedef TVector3<double> Vec3d;