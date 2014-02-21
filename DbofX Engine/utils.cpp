#include "DbofX.h"

using std::string;
using namespace dbx::core;

void clear_screen()
{
	HANDLE output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD bytes_write, size;
	COORD coord = {0, 0};
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(output_handle, &csbi);
	
	size = csbi.dwSize.X * csbi.dwSize.Y;
	
	FillConsoleOutputCharacter(output_handle, ' ', size, coord, &bytes_write);
	SetConsoleCursorPosition(output_handle, coord);
}

string toString(float x)
{
	std::stringstream ss (std::stringstream::in | std::stringstream::out);
	ss << x;
	return ss.str();
}

string toString(int x)
{
	std::stringstream ss (std::stringstream::in | std::stringstream::out);
	ss << x;
	return ss.str();
}

string toString(double x)
{
	std::stringstream ss (std::stringstream::in | std::stringstream::out);
	ss << x;
	return ss.str();
}

string toString(dbVector3 vector)
{
	return "x" + toString(vector.x) + " y" + toString(vector.y) + " z" + toString(vector.z);
}

//n. power of x = x^n
int pow(int x, int n) 
{
	int num = x;
	for(int i = 0; i < n && n != 1; i++)	
		x *= num;
	return x;
}

void InitRandom()
{
	srand((int)time(NULL));
}

int getRandomInt(int llimit, int hlimit)
{
	return llimit + (rand() % (hlimit - llimit + 1));
}
	
float getRandomFloat(float llimit, float hlimit, int numOfDigits)
{
	float fac = (float) pow(10, numOfDigits);

	int l = (int) (llimit * fac);
	int h = (int) (hlimit * fac);
	
	return ((l + (rand() % (h-l + 1))) / fac) ;
}


void SplitPath(std::string in_filepath, std::string* out_filename, std::string* out_filedir, 
					  std::string* out_fileext, std::string* out_filename_no_ext)
{
	string fname, ext, dir;

	int point = in_filepath.find_last_of('.');
	if (point == -1)
		ext = "";
	else
		ext = in_filepath.substr(point+1, string::npos);

	int slash = in_filepath.find_last_of('\\');
	if (slash == -1)
		dir = "";
	else
		dir = in_filepath.substr(0, slash+1);

	fname = in_filepath.substr(slash+1, point-slash-1);

	// now check out params
	if (out_filename)
		*out_filename = fname + "." + ext;
	if (out_filedir)
		*out_filedir = dir;
	if (out_fileext)
		*out_fileext = ext;
	if (out_filename_no_ext)
		*out_filename_no_ext = fname;
	return;
}