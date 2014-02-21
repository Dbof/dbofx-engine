#include "DbofX.h"


namespace dbx
{
namespace core
{

// constructor definitions
dbMatrix::dbMatrix(	float _m11, float _m12, float _m13, float _m14,
			float _m21, float _m22, float _m23, float _m24,
			float _m31, float _m32, float _m33, float _m34,
			float _m41, float _m42, float _m43, float _m44)
{
	_11 = _m11; _12 = _m12; _13 = _m13; _14 = _m14;
	_21 = _m21; _22 = _m22; _23 = _m23; _24 = _m24;
	_31 = _m31; _32 = _m32; _33 = _m33; _34 = _m34;
	_41 = _m41; _42 = _m42; _43 = _m43; _44 = _m44;
}

dbMatrix::dbMatrix(float f)
{
	_11 = _12 = _13 = _14 = f;
	_21 = _22 = _23 = _24 = f;
	_31 = _32 = _33 = _34 = f;
	_41 = _42 = _43 = _44 = f;
}

dbMatrix::dbMatrix(const dbMatrix& a)
{
	*this = a;
}


dbMatrix::dbMatrix(const D3DMATRIX& d3dmat)
{
	mD3DMatrix = d3dmat;
}

//**********************************************************************
// OPERATORS

dbMatrix& dbMatrix::operator= (const dbMatrix& a)
{
	mD3DMatrix = a.mD3DMatrix;
	return *this;
}

dbMatrix& dbMatrix::operator+= (const dbMatrix& a)
{
		_11 += a._11; _12 += a._12; _13 += a._13; _14 += a._14;
		_21 += a._21; _22 += a._22; _23 += a._23; _24 += a._24;
		_31 += a._31; _32 += a._32; _33 += a._33; _34 += a._34;
		_41 += a._41; _42 += a._42; _43 += a._43; _44 += a._44;
		return *this;
}

dbMatrix& dbMatrix::operator-= (const dbMatrix& a)
{
		_11 -= a._11; _12 -= a._12; _13 -= a._13; _14 -= a._14;
		_21 -= a._21; _22 -= a._22; _23 -= a._23; _24 -= a._24;
		_31 -= a._31; _32 -= a._32; _33 -= a._33; _34 -= a._34;
		_41 -= a._41; _42 -= a._42; _43 -= a._43; _44 -= a._44;
		return *this;
}

dbMatrix& dbMatrix::operator*= (const dbMatrix& a)
{
	return *this = (*this * a);
}


dbMatrix& dbMatrix::operator/= (const dbMatrix& a)
{
	return *this *= dbMatrixInvert(a);
}


dbMatrix& dbMatrix::operator*= (const float& f)
{
	return *this = (*this * f);
}

dbMatrix& dbMatrix::operator/= (const float& f)
{
	if (f == 0)
		return *this = dbMatrix(0.f);
	return *this = (*this / f);
}

//**********************************************************************
// ARITHMETIC OPERATORS
dbMatrix dbMatrix::operator* (const dbMatrix& a)
{
	return dbMatrix(a._11 * _11 + a._21 * _12 + a._31 * _13 + a._41 * _14,
					a._12 * _11 + a._22 * _12 + a._32 * _13 + a._42 * _14,
					a._13 * _11 + a._23 * _12 + a._33 * _13 + a._43 * _14,
					a._14 * _11 + a._24 * _12 + a._34 * _13 + a._44 * _14,
					a._11 * _21 + a._21 * _22 + a._31 * _23 + a._41 * _24,
					a._12 * _21 + a._22 * _22 + a._32 * _23 + a._42 * _24,
					a._13 * _21 + a._23 * _22 + a._33 * _23 + a._43 * _24,
					a._14 * _21 + a._24 * _22 + a._34 * _23 + a._44 * _24,
					a._11 * _31 + a._21 * _32 + a._31 * _33 + a._41 * _34,
					a._12 * _31 + a._22 * _32 + a._32 * _33 + a._42 * _34,
					a._13 * _31 + a._23 * _32 + a._33 * _33 + a._43 * _34,
					a._14 * _31 + a._24 * _32 + a._34 * _33 + a._44 * _34,
					a._11 * _41 + a._21 * _42 + a._31 * _43 + a._41 * _44,
					a._12 * _41 + a._22 * _42 + a._32 * _43 + a._42 * _44,
					a._13 * _41 + a._23 * _42 + a._33 * _43 + a._43 * _44,
					a._14 * _41 + a._24 * _42 + a._34 * _43 + a._44 * _44);
}

dbMatrix dbMatrix::operator* (const float& f)
{
	return dbMatrix(_11 * f, _12 * f, _13 * f, _14 * f,
							_21 * f, _22 * f, _23 * f, _24 * f,
					 		_31 * f, _32 * f, _33 * f, _34 * f,
							_41 * f, _42 * f, _43 * f, _44 * f);	
}

dbMatrix dbMatrix::operator/ (const dbMatrix& a)
{
	return *this *= dbMatrixInvert(a);
}

dbMatrix dbMatrix::operator/ (const float& f)
{
	return dbMatrix(_11 / f, _12 / f, _13 / f, _14 / f,
			        _21 / f, _22 / f, _23 / f, _24 / f,
					_31 / f, _32 / f, _33 / f, _34 / f,
					_41 / f, _42 / f, _43 / f, _44 / f);
}

//**********************************************************************
// COMPARISON OPERATORS
bool dbMatrix::operator== (const dbMatrix& a)
{
	if (_11 != a._11) return false;
	if (_12 != a._12) return false;
	if (_13 != a._13) return false;
	if (_14 != a._14) return false;
	if (_21 != a._21) return false;
	if (_22 != a._22) return false;
	if (_23 != a._23) return false;
	if (_24 != a._24) return false;
	if (_31 != a._31) return false;
	if (_32 != a._32) return false;
	if (_33 != a._33) return false;
	if (_34 != a._34) return false;
	if (_41 != a._41) return false;
	if (_42 != a._42) return false;
	if (_43 != a._43) return false;
	return _44 == a._44;
}

bool dbMatrix::operator!= (const dbMatrix& a)
{
	if (_11 != a._11) return true;
	if (_12 != a._12) return true;
	if (_13 != a._13) return true;
	if (_14 != a._14) return true;
	if (_21 != a._21) return true;
	if (_22 != a._22) return true;
	if (_23 != a._23) return true;
	if (_24 != a._24) return true;
	if (_31 != a._31) return true;
	if (_32 != a._32) return true;
	if (_33 != a._33) return true;
	if (_34 != a._34) return true;
	if (_41 != a._41) return true;
	if (_42 != a._42) return true;
	if (_43 != a._43) return true;
	return _44 == a._44;
}

//**********************************************************************

dbMatrix dbMatrixIdentity()
{
	return dbMatrix(1.0f, 0.0f, 0.0f, 0.0f, 
					0.0f, 1.0f, 0.0f, 0.0f, 
					0.0f, 0.0f, 1.0f, 0.0f, 
					0.0f, 0.0f, 0.0f, 1.0f);
}

//**********************************************************************

float dbMatrixDet(const dbMatrix& a)
{
	// Determinante der linken oberen 3x3-Teilmatrix berechnen
	return a._11 * (a._22 * a._33 - a._23 * a._32) -
           a._12 * (a._21 * a._33 - a._23 * a._31) +
           a._13 * (a._21 * a._32 - a._22 * a._31);
}

//**********************************************************************

dbMatrix dbMatrixInvert(const dbMatrix& a)
{
	// Kehrwert der Determinante vorberechnen
	float fInvDet = dbMatrixDet(a);
	if(fInvDet == 0.0f) 
		return dbMatrixIdentity();

	fInvDet = 1.0f / fInvDet;

	// Invertierte Matrix berechnen
	dbMatrix mResult;
	mResult._11 =  fInvDet * (a._22 * a._33 - a._23 * a._32);
	mResult._12 = -fInvDet * (a._12 * a._33 - a._13 * a._32);
	mResult._13 =  fInvDet * (a._12 * a._23 - a._13 * a._22);
	mResult._14 =  0.0f;
	mResult._21 = -fInvDet * (a._21 * a._33 - a._23 * a._31);
	mResult._22 =  fInvDet * (a._11 * a._33 - a._13 * a._31);
	mResult._23 = -fInvDet * (a._11 * a._23 - a._13 * a._21);
	mResult._24 =  0.0f;
	mResult._31 =  fInvDet * (a._21 * a._32 - a._22 * a._31);
	mResult._32 = -fInvDet * (a._11 * a._32 - a._12 * a._31);
	mResult._33 =  fInvDet * (a._11 * a._22 - a._12 * a._21);
	mResult._34 =  0.0f;
	mResult._41 = -(a._41 * mResult._11 + a._42 * mResult._21 + a._43 * mResult._31);
	mResult._42 = -(a._41 * mResult._12 + a._42 * mResult._22 + a._43 * mResult._32);
	mResult._43 = -(a._41 * mResult._13 + a._42 * mResult._23 + a._43 * mResult._33);
	mResult._44 =  1.0f;

	return mResult;
}

//**********************************************************************

dbMatrix dbMatrixTranslation(const dbVector3& v)
{
	return dbMatrix(1.0f, 0.0f, 0.0f, 0.0f,
		            0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					v.x,  v.y,  v.z,  1.0f);
}

//**********************************************************************

dbMatrix dbMatrixRotationX(const float f)
{
	return dbMatrix(1.0f,  0.0f,	0.0f,    0.0f,
		            0.0f,  cosf(f), sinf(f), 0.0f,
					0.0f, -sinf(f), cosf(f), 0.0f,
					0.0f,  0.0f,    0.0f,    1.0f);
}

//**********************************************************************

dbMatrix dbMatrixRotationY(const float f)
{
	return dbMatrix(cosf(f), 0.0f, -sinf(f), 0.0f,
		            0.0f,    1.0f,  0.0f,    0.0f,
					sinf(f), 0.0f,  cosf(f), 0.0f,
					0.0f,    0.0f,  0.0f,    1.0f);
}

//**********************************************************************

dbMatrix dbMatrixRotationZ(const float f)
{
	return dbMatrix( cosf(f), sinf(f), 0.0f, 0.0f,
		            -sinf(f), cosf(f), 0.0f, 0.0f,
					 0.0f,    0.0f,    1.0f, 0.0f,
					 0.0f,    0.0f,    0.0f, 1.0f);
}

//**********************************************************************

dbMatrix dbMatrixRotationXYZ(const float x, const float y, const float z)
{
	return	dbMatrixRotationZ(z) *
			dbMatrixRotationX(x) *
			dbMatrixRotationY(y);
}

//**********************************************************************

dbMatrix dbMatrixRotationXYZ(const dbVector3 &v)
{
	return	dbMatrixRotationZ(v.z) *
			dbMatrixRotationX(v.x) *
			dbMatrixRotationY(v.y);
}

//**********************************************************************

dbMatrix dbMatrixRotationAxis(const dbVector3& axis, const float f)
{
	// calc sin and cos
	float sin = sinf(-f);
	float cos = cosf(-f);
	float cos_tmp = 1.0f - cos;

	// Achsenvektor normalisieren
	const dbVector3 vAxis(dbVector3Norm(axis));

	// Matrix erstellen
	return dbMatrix((vAxis.x * vAxis.x) * cos_tmp + cos,
		            (vAxis.x * vAxis.y) * cos_tmp - (vAxis.z * sin),
				    (vAxis.x * vAxis.z) * cos_tmp + (vAxis.y * sin),
					0.0f,
					(vAxis.y * vAxis.x) * cos_tmp + (vAxis.z * sin),
					(vAxis.y * vAxis.y) * cos_tmp + cos,
					(vAxis.y * vAxis.z) * cos_tmp - (vAxis.x * sin),
					0.0f,
					(vAxis.z * vAxis.x) * cos_tmp - (vAxis.y * sin),
					(vAxis.z * vAxis.y) * cos_tmp + (vAxis.x * sin),
					(vAxis.z * vAxis.z) * cos_tmp + cos,
					0.0f,
					0.0f,
					0.0f,
					0.0f,
					1.0f);
}

//**********************************************************************

dbMatrix dbMatrixScaling(const dbVector3& v)
{
	return dbMatrix(v.x,  0.0f, 0.0f, 0.0f,
					0.0f, v.y,  0.0f, 0.0f,
					0.0f, 0.0f, v.z,  0.0f,
					0.0f, 0.0f, 0.0f, 1.0f);
}

//**********************************************************************

dbMatrix dbMatrixAxes(const dbVector3& xAxis, const dbVector3& yAxis, const dbVector3& zAxis)
{
	return dbMatrix(xAxis.x, xAxis.y, xAxis.z, 0.0f,
		            yAxis.x, yAxis.y, yAxis.z, 0.0f,
					zAxis.x, zAxis.y, zAxis.z, 0.0f,
					   0.0f,    0.0f,    0.0f, 1.0f);
}

//**********************************************************************

dbMatrix dbMatrixProjection(float rFOV, float aspect, float nearPlane, float farPlane)
{
	dbMatrix matResult;
	D3DXMatrixPerspectiveFovLH((D3DXMATRIX*)&matResult, 
								rFOV,    // the horizontal field of view
								aspect, // aspect ratio
								nearPlane,    // the near view-plane
								farPlane);    // the far view-plane
	return matResult;
}

//**********************************************************************

dbMatrix dbMatrix2DRotation(const float f)
{
	return dbMatrix(cosf(f),-sinf(f),	0.f, 0.f,
		           sinf(f),	 cosf(f),	0.f, 0.f,
					0.0f,	0.f,		1.f, 0.f,
					0.0f,	0.0f,		0.f, 1.f);
}

}}	// namespace