#ifndef _VECTOR3D_H_
#define _VECTOR3D_H_

#include <cmath>

class Vector2D
{
public:
	double x, y;

public:
	Vector2D(){x = 0; y = 0;}
	Vector2D(double x, double y){this->x = x; this->y = y;}
};

class Vector3D
{
public:
	double x, y, z;

public:
	Vector3D(){x = 0; y = 0; z = 0;}
	Vector3D(double x, double y, double z){this->x = x; this->y = y; this->z = z;}

	Vector3D operator+ (const Vector3D& v)
	{
		return Vector3D(x+v.x, y+v.y, z+v.z);
	}

	Vector3D operator- (const Vector3D& v)
	{
		return Vector3D(x-v.x, y-v.y, z-v.z);
	}

	double operator* (const Vector3D& v)
	{
		return x*v.x+y*v.y+z*v.z;
	}

	Vector3D operator* (double v)
	{
		return Vector3D(x*v, y*v, z*v);
	}

	Vector3D operator^ (const Vector3D& v)
	{
		return Vector3D(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x);
	}

	Vector3D operator/(double v)
	{
		return Vector3D(x/v, y/v, z/v);
	}

	Vector3D& operator+=(const Vector3D& v)
	{
		x += v.x; y += v.y; z += v.z;
		return *this;
	}

	Vector3D& operator=(const Vector3D& v)
	{
		x = v.x; y = v.y; z = v.z;
		return *this;
	}

	double length()
	{
		return sqrt(x*x+y*y+z*z);
	}

	void norm()
	{
		double len = length();
		x /= len; y /= len; z /= len;
	}

	double& operator[](unsigned int i)
	{
		switch(i)
		{
			case 0: return x;
			case 1: return y;
			case 2: return z;
		}
	}
};

#endif