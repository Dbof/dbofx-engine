#ifndef COLOR_H
#define COLOR_H

namespace dbx
{
namespace core
{

	class DBX_API dbColor
	{
	public:
		union
		{
			struct
			{
				float r;
				float g;
				float b;
				float a;
			};
		
			float c[4];
			D3DCOLORVALUE d3dColorValue;
			D3DCOLOR d3dColor;
		};

		// Constructors
		dbColor(void) : r(1.f), g(1.f), b(1.f), a(1.f) {};
		dbColor(const dbColor& c) : r(c.r), g(c.g), b(c.b), a(c.a) {};
		dbColor(const float f) : r(f), g(f), b(f), a(1.f) {};
		dbColor(const float _r, const float _g, const float _b)	: r(_r), g(_g), b(_b), a(1.0f) {};
		dbColor(const float _r, const float _g, const float _b, const float _a) : r(_r), g(_g), b(_b), a(_a) {};
		dbColor(const BYTE _r, const BYTE _g, const BYTE _b) : r(_r / 255.f), g(_g / 255.f), b(_b / 255.f), a(1.f) {}; 
		dbColor(const BYTE _r, const BYTE _g, const BYTE _b, const BYTE _a) : r(_r / 255.f), g(_g / 255.f), b(_b / 255.f), a(_a / 255.f) {}; 
		dbColor(const DWORD c) : r((float)(BYTE)(c >> 16) / 255.f), g((float)(BYTE)(c >> 8) / 255.f), b((float)(BYTE)(c) / 255.f), a((float)(BYTE)(c >> 24) / 255.f) {};
		dbColor(const D3DCOLORVALUE& c) : d3dColorValue(c) {};
	
		// Casting operators
		operator DWORD () const
		{
			return ((a >= 1.0f ? 255 : a <= 0.0f ? 0 : (DWORD)(a * 255.0f)) << 24) |
				   ((r >= 1.0f ? 255 : r <= 0.0f ? 0 : (DWORD)(r * 255.0f)) << 16) |
				   ((g >= 1.0f ? 255 : g <= 0.0f ? 0 : (DWORD)(g * 255.0f)) << 8)  |
					(b >= 1.0f ? 255 : b <= 0.0f ? 0 : (DWORD)(b * 255.0f));
		}

		operator D3DCOLORVALUE& ()	{ return d3dColorValue; }
		operator float* () { return (float*)c; }

		// Assign operators
	
		// Zuweisungsoperatoren
		dbColor& operator = (const dbColor& c)	{ d3dColorValue = c.d3dColorValue; return *this; }
		dbColor& operator += (const dbColor& c)	{ r += c.r; g += c.g; b += c.b; a += c.a; return *this; }
		dbColor& operator -= (const dbColor& c)	{ r -= c.r; g -= c.g; b -= c.b; a -= c.a; return *this; }
		dbColor& operator *= (const dbColor& c)	{ r *= c.r; g *= c.g; b *= c.b; a *= c.a; return *this; }
		dbColor& operator *= (const float f)	{ r *= f; g *= f; b *= f; a *= f; return *this;}
		dbColor& operator /= (const dbColor& c)	{ r /= c.r; g /= c.g; b /= c.b; a /= c.a; return *this; }
		dbColor& operator /= (const float f)	{ r /= f; g /= f; b /= f; a /= f; return *this; }
	};


	//	***** EXTERNAL FUNCTIONS *****

	// arithmetics operators
	inline dbColor operator + (const dbColor& a, const dbColor& b)	{ return dbColor(a.r + b.r, a.g + b.g, a.b + b.b, a.a + b.a); }
	inline dbColor operator - (const dbColor& a, const dbColor& b)	{ return dbColor(a.r - b.r, a.g - b.g, a.b - b.b, a.a - b.a); }
	inline dbColor operator - (const dbColor& c)					{ return dbColor(-c.r, -c.g, -c.b, -c.a); }
	inline dbColor operator * (const dbColor& a, const dbColor& b)	{ return dbColor(a.r * b.r, a.g * b.g, a.b * b.b, a.a * b.a); }
	inline dbColor operator * (const dbColor& c, const float f)		{ return dbColor(c.r * f, c.b * f, c.g * f, c.a * f); }
	inline dbColor operator * (const float f, const dbColor& c)		{ return dbColor(c.r * f, c.b * f, c.g * f, c.a * f); }
	inline dbColor operator / (const dbColor& a, const dbColor& b)	{ return dbColor(a.r / b.r, a.g / b.g, a.b / b.b, a.a / b.a); }
	inline dbColor operator / (const dbColor& c, const float f)		{ return dbColor(c.r / f, c.b / f, c.g / f, c.a / f); }


	// comparison operators
	inline bool operator == (const dbColor& a, const dbColor& b) { return ((a.d3dColorValue == b.d3dColorValue) ? true : false); }
	inline bool operator != (const dbColor& a, const dbColor& b) { return ((a.d3dColorValue == b.d3dColorValue) ? false : true); }

	inline dbColor dbColorNegative(const dbColor& c) { return dbColor(1.f - c.r, 1.f - c.g, 1.f - c.b); }
	inline dbColor dbColorNegativeA(const dbColor& c) { return dbColor(1.f - c.r, 1.f - c.g, 1.f - c.b, 1.f - c.a); }
	inline float dbColorBrightness(const dbColor& c) { return ( c.r * 0.3f + c.g * 0.6f + c.b * 0.1f); }
	inline dbColor dbColorRandom(float alpha = -1) { 
		return dbColor(getRandomFloat(0.f, 1.f), getRandomFloat(0.f, 1.f), getRandomFloat(0.f, 1.f), (alpha < 0.0f) ? getRandomFloat(0.f, 1.f) : alpha); }

}}	// namespace
#endif