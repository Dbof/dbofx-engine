#include "DbofX.h"


namespace dbx
{
namespace core
{

dbVector3::dbVector3(void)
{
	x = y = z = 0;
}

dbVector3::~dbVector3(void)
{
}

//operators
dbVector3& dbVector3::operator= (const dbVector3& a)
{
	x = a.x;
	y = a.y;
	z = a.z;
	return *this;
}



dbVector3& dbVector3::operator+= (const dbVector3& a)
{
	x += a.x;
	y += a.y;
	z += a.z;
	return *this;
}

dbVector3& dbVector3::operator-= (const dbVector3& a)
{
	x -= a.x;
	y -= a.y;
	z -= a.z;
	return *this;
}

dbVector3& dbVector3::operator*= (const dbVector3& a)
{
	x *= a.x;
	y *= a.y;
	z *= a.z;
	return *this;
}

dbVector3& dbVector3::operator/= (const dbVector3& a)
{
	x /= a.x;
	y /= a.y;
	z /= a.z;
	return *this;
}

//conversions
dbVector3::operator D3DVECTOR()
{
	return *((D3DVECTOR*)(this));
}

dbVector3::operator D3DXVECTOR3()
{
	return D3DXVECTOR3(D3DVECTOR(*this));
}

}}	// namespace