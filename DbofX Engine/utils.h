#pragma once

DBX_API inline bool FileExist(const std::string& filename)
{
	return GetFileAttributes(filename.c_str()) != 0xFFFFFFFF;
}

DBX_API void clear_screen();

DBX_API std::string toString(float x);
DBX_API std::string toString(int x);
DBX_API std::string toString(double x);
DBX_API std::string toString(dbx::core::dbVector3 vector);
DBX_API void SplitPath(std::string filepath, std::string* out_filename, std::string* out_filedir, std::string* out_fileext, std::string* out_filename_no_ext);

DBX_API int pow(int x, int n);
DBX_API void InitRandom();
DBX_API int getRandomInt(int llimit, int hlimit);
DBX_API float getRandomFloat(float llimit, float hlimit, int numOfDigits = 1);
