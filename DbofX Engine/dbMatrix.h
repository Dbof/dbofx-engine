#ifndef _MATRIX_H
#define _MATRIX_H

namespace dbx
{
namespace core
{
	class DBX_API dbMatrix
	{
	public:
		union
		{
			struct
			{
				float _11, _12, _13, _14,
					  _21, _22, _23, _24,
					  _31, _32, _33, _34,
					  _41, _42, _43, _44;
			};

			float		m[4][4];			// two-dimensional array
			float		n[16];				// Eindimensionales Array der Elemente
			D3DMATRIX	mD3DMatrix;			// Matrix als D3DMATRIX
		};

	
		// Constructors
		inline dbMatrix()	{ D3DXMatrixIdentity((D3DXMATRIX*)&mD3DMatrix); };
		inline dbMatrix(float f);
		inline dbMatrix(const dbMatrix& a);
		inline dbMatrix(const D3DMATRIX& d3dmat);
		inline dbMatrix(float _m11, float _m12, float _m13, float _m14,
				 float _m21, float _m22, float _m23, float _m24,
				 float _m31, float _m32, float _m33, float _m34,
				 float _m41, float _m42, float _m43, float _m44);

		// operators
		dbMatrix& operator= (const dbMatrix& a);
		dbMatrix& operator+= (const dbMatrix& a);
		dbMatrix& operator-= (const dbMatrix& a);
		dbMatrix& operator*= (const dbMatrix& a);
		dbMatrix& operator/= (const dbMatrix& a);
		dbMatrix& operator*= (const float& f);
		dbMatrix& operator/= (const float& f);
	
		dbMatrix operator* (const dbMatrix& a);
		dbMatrix operator* (const float& f);
		dbMatrix operator/ (const dbMatrix& a);
		dbMatrix operator/ (const float& f);

		bool operator== (const dbMatrix& a);
		bool operator!= (const dbMatrix& a);
	private:

	};

	DBX_API dbMatrix	dbMatrixIdentity();
	DBX_API float		dbMatrixDet(const dbMatrix& m);
	DBX_API dbMatrix	dbMatrixInvert(const dbMatrix& a);
	DBX_API dbMatrix	dbMatrixTranslation(const dbVector3& v);
	DBX_API dbMatrix	dbMatrixRotationX(const float f);
	DBX_API dbMatrix	dbMatrixRotationY(const float f);
	DBX_API dbMatrix	dbMatrixRotationZ(const float f);
	DBX_API dbMatrix	dbMatrixRotationXYZ(const float x, const float y, const float z);
	DBX_API dbMatrix	dbMatrixRotationXYZ(const dbVector3& v);
	DBX_API dbMatrix	dbMatrixRotationAxis(const dbVector3& axis, const float f);
	DBX_API dbMatrix	dbMatrixScaling(const dbVector3& v);
	DBX_API dbMatrix	dbMatrixAxes(const dbVector3& xAxis, const dbVector3& yAxis, const dbVector3& zAxis);
	DBX_API dbMatrix	dbMatrixProjection(float FOV, float aspect, float nearPlane, float farPlane);
	DBX_API dbMatrix	dbMatrix2DRotation(const float f);

}}	// namespace
#endif

