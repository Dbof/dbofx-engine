#ifndef _VECTOR_H
#define _VECTOR_H

namespace dbx
{
namespace core
{
	class DBX_API dbVector3
	{
	public:
		// constructors
		dbVector3(void);
		dbVector3(const dbVector3& v) : x(v.x), y(v.y), z(v.z) {};
		dbVector3(const float vx, const float vy, const float vz) : x(vx), y(vy), z(vz) {};
		dbVector3(const D3DVECTOR& v) : x(v.x), y(v.y), z(v.z) {};
	
		// operators
		dbVector3& operator= (const dbVector3& a);
		dbVector3& operator+= (const dbVector3& a);
		dbVector3& operator-= (const dbVector3& a);
		dbVector3& operator*= (const dbVector3& a);
		dbVector3& operator/= (const dbVector3& a);
	
		operator D3DVECTOR();
		operator D3DXVECTOR3();
		~dbVector3(void);

		float x, y, z;
	};

	//*****	GLOBAL FUNCTIONS *****
	//Operators
	inline bool operator== (dbVector3& a, dbVector3& b)
	{
		return a.x == b.x && a.y == b.y && a.z == b.z ;
	}

	inline bool operator!= (dbVector3& a, dbVector3& b)
	{
		return a.x != b.x || a.y != b.y || a.z != b.z ;
	}

	inline dbVector3 operator+ (const dbVector3& a, const dbVector3& b)
	{
		return dbVector3(a.x + b.x, a.y + b.y, a.z + b.z);
	}

	inline dbVector3 operator- (const dbVector3& a, const dbVector3& b)
	{
		return dbVector3(a.x - b.x, a.y - b.y, a.z - b.z);
	}

	inline dbVector3 operator* (const dbVector3& a, const dbVector3& b)
	{
		return dbVector3(a.x * b.x, a.y * b.y, a.z * b.z);
	}

	inline dbVector3 operator/ (const dbVector3& a, const dbVector3& b)
	{
		return dbVector3(a.x / b.x, a.y / b.y, a.z / b.z);
	}


	//Cross product
	inline dbVector3 dbVector3Cross(const dbVector3& a, const dbVector3& b)
	{
		return dbVector3(a.y * b.z - a.z * b.y,
						 a.z * b.x - a.x * b.z,
						 a.x * b.y - a.y * b.x);
	}

	//Dot product (Skalar)
	inline dbVector3 operator* (const dbVector3& a, float b)
	{
		return dbVector3(a.x * b, a.y * b, a.z * b);
	}

	inline dbVector3 operator* (float b, const dbVector3& a)
	{
		return dbVector3(a.x * b, a.y * b, a.z * b);
	}

	inline dbVector3 operator/ (const dbVector3& a, float b)
	{
		return dbVector3(a.x / b, a.y / b, a.z / b);
	}

	inline float dbVector3Dot (const dbVector3& a, const dbVector3& b)
	{
		return (a.x * b.x + a.y * b.y + a.z * b.z);
	}

	inline float dbVector3Len(const dbVector3& v)
	{
		return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	}


	inline float dbVector3Angle(const dbVector3& a, const dbVector3& b)
	{
		return acosf( (a.x * b.x + a.y * b.y + a.z * b.z) /
				sqrtf( (a.x * a.x + a.y * a.y + a.z * a.z) *
					  (b.x * b.x + b.y * b.y + b.z * b.z)) );
	}

	inline dbVector3 dbVector3Norm(const dbVector3& v)
	{
		float d;
		if ( (d = dbVector3Len(v)) != 0)
			return (v / d);
		return v;
	}

}}	// namespace
#endif