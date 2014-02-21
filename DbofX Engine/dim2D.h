#pragma once



template <class T>
class dim2D
{
public:
	T x;
	T y;

	dim2D() : x(0), y(0) {}
	dim2D(T wid, T hei) : x(wid), y(hei) {}
	
	dim2D operator+ (T num)
	{
		x += num;
		y += num;
		return *this;
	};
	
	dim2D operator= (dim2D ob)
	{
		x = ob.x;
		y = ob.y;
		return *this;
	}
	
};


template <typename T>
bool operator== (dim2D<T> ob1, dim2D<T> ob2)
{
	return (ob1.x == ob2.x ) && (ob1.y == ob2.y);
}