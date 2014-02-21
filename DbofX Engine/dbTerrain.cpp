#include "DbofX.h"

using std::string;

namespace dbx
{
namespace scene
{
	using namespace core;

	DWORD dbTerrain::TERRAIN_VERTEX::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1;

dbTerrain::dbTerrain()
{
	ZeroMemory(this, sizeof(dbTerrain));
}

dbTerrain::~dbTerrain()
{
}

bool dbTerrain::Init(const char* filename, HMAP_TYPE hType, float normValue)
{
	if (!FileExist(filename))
		return false;

	bool result = false;
	switch (hType)
	{
	case HT_BMP:
		result = LoadBMPHeightMap(filename, normValue);
	break;

	case HT_RAW:
		result = LoadRAWHeightMap(filename, normValue);
	break;
	}

	return result;
}

bool dbTerrain::LoadBMPHeightMap(const char* fname, float normValue)
{
	// from http://rastertek.com/tertut02.html
	FILE* filePtr;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char* bitmapImage;

	// Open the height map file in binary.
	int error = 0;
	error = fopen_s(&filePtr, fname, "rb");
	if(error != 0)
	{
		return false;
	}

	// Read in the file header.
	int count = 0;
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	// Read in the bitmap info header.
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	// Save the dimensions of the terrain.
	height_map_.width = bitmapInfoHeader.biWidth;
	height_map_.height = bitmapInfoHeader.biHeight;

	// Calculate the size of the bitmap image data.
	int imageSize = height_map_.width * height_map_.height * 3;

	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];
	if(!bitmapImage)
	{
		return false;
	}

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if(count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if(error != 0)
	{
		return false;
	}

	// Create the structure to hold the height map data.
	height_map_.data = new float[height_map_.width * height_map_.height];
	if(!height_map_.data)
	{
		return false;
	}

	// Initialize the position in the image data buffer.
	int k = 0;
	int index = 0;

	// Read the image data into the height map.
	for(int y = 0; y < height_map_.height; y++)
	{
		for(int x = 0; x < height_map_.width; x++)
		{
			unsigned char height = bitmapImage[k];

			height_map_.data[index++] = (float)height / normValue;

			k+=3;
		}
	}

	// Release the bitmap image data.
	delete [] bitmapImage;
	bitmapImage = 0;
	
	return Init(height_map_.width, height_map_.height);
}

bool dbTerrain::LoadRAWHeightMap(const char* fname, float normValue)
{
	std::ifstream ifile(fname, std::ifstream::binary);
	if (!ifile.is_open())
		return false;
	
	ifile.seekg (0, ifile.end);
    int length = (int)ifile.tellg();

	// check if RAW file is square (fairly exact)
	{
		int tmp_sqrt = (int) sqrt((float)length);	// truncate
		if ((tmp_sqrt*tmp_sqrt) != length)	// file size does not match
		{
			ifile.close();
			return false;
		}
	}
    ifile.seekg(0, ifile.beg);
	
    char* buffer = new char [length];
    ifile.read (buffer,length);
	
    if (!ifile)
	{
		DBX_ERROR("Could not read file");
		ifile.close();
		return false;
	}
	ifile.close();
	
	// handle hmap values
	height_map_.width = height_map_.height = (int) sqrt((float)length);

	height_map_.data = new float[length];
	for (int i = 0; i < length; i++)
	{
		height_map_.data[i] = (float)(buffer[i]) / normValue;		// also normalize
	}

	return Init(height_map_.width, height_map_.height);
}

bool dbTerrain::Init(int numX, int numY)
{
	num_vertices_x_ = numX;
	num_vertices_y_ = numY;
	max_vertex_ = num_vertices_x_ * num_vertices_y_;

	vertex_buffer_.SetCursor(0);
	index_buffer_.SetCursor(0);
	
	bool res;
	res = InitVB();	
	if (!res)
		return false;
	
	res = InitIB();
	if (!res)
		return false;
	return true;
	//TERRAIN_VERTEX* vert = (TERRAIN_VERTEX*) vbuffer.getBuffer();
	//for (int i = 0; i < (int)vbuffer.getMaxVertices(); i++)
	//{
	//	std::cout << "X: " << vert[i].pos.x << " Y: " << vert[i].pos.y << " Z: " << vert[i].pos.z << std::endl;
	//}

	//short* ind = (short*)ibuffer.getBuffer();
	//for (int i = 0; i < (int)ibuffer.getMaxIndices(); i++)
	//{
	//	if (i % 6 == 0)
	//		std::cout << "\n";
	//	else if (i % 3 == 0)
	//		std::cout << " ";
	//	std::cout << ind[i] << " ";
	//}
}

void dbTerrain::Render()
{
	if (dbDevice::canRender() != D3D_OK)
		return;

	LPDIRECT3DDEVICE9 dev = dbDevice::getDevice();
	HRESULT hr;

	hr = dev->SetStreamSource(0, vertex_buffer_.GetVB(), 0, sizeof(TERRAIN_VERTEX));
	hr = dev->SetIndices(index_buffer_.GetIB());
	hr = dev->SetFVF(TERRAIN_VERTEX::FVF);
	
	dbDevice::SetRenderState( D3DRS_CULLMODE, D3DCULL_CW);
//	dbDevice::SetRenderState( D3DRS_LIGHTING, false );
	
	if (FAILED( dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, max_vertex_, 0, max_index_ / 3)))
		DBX_ERROR("Could not render terrain!");

//	dbDevice::SetRenderState( D3DRS_LIGHTING, true );
}


bool dbTerrain::InitVB()
{
	if (!vertex_buffer_.Init(sizeof(TERRAIN_VERTEX) * max_vertex_, sizeof(TERRAIN_VERTEX), TERRAIN_VERTEX::FVF, D3DUSAGE_WRITEONLY, D3DPOOL_MANAGED))
		return false;
	if (height_map_.width <= 0 || height_map_.height <= 0)
		return false;

	TERRAIN_VERTEX* vertex = new TERRAIN_VERTEX[max_vertex_];
	int vi = 0;

	for (float y = 0; y < num_vertices_y_; y++)
	{
		for (float x = 0; x < num_vertices_x_; x++)
		{
			TERRAIN_VERTEX tvertex;
			tvertex.pos = dbVector3(x, height_map_.data[vi], y);
			tvertex.color = dbColor(1.f);	//dbColor(0.f, getRandomFloat(0.5f, 1.5f, 2), 0.f);
			tvertex.normal = dbVector3(0.f, 1.f, 0.f);
			tvertex.u = x / num_vertices_x_;
			tvertex.v = y / num_vertices_y_;

			vertex[vi++] = tvertex;	// increment vi
		}
	}

	vertex_buffer_.AddVertices(max_vertex_, vertex);
	if (!vertex_buffer_.Update())
		return false;
	return true;
}

bool dbTerrain::InitIB()
{
	int rows	= num_vertices_y_ - 1;
	int column	= num_vertices_x_ - 1;
	max_index_	= 6 * rows * column;

	int value_size = sizeof(short);
	if (max_index_ > 65536)
		value_size = sizeof(int);
	
	if (!index_buffer_.Init(value_size * max_index_, value_size, (value_size == sizeof(int)) ? D3DFMT_INDEX32 : D3DFMT_INDEX16, D3DUSAGE_WRITEONLY))
		return false;
	

	int* indices = new int[max_index_];
	int* index_pointer = &indices[0];
	int i = 0;
	for (int y = 0; y < rows; y++)
	{
		int num1, num2;
		num1 = num_vertices_x_ * y;
		num2 = num1 + num_vertices_x_ + 1;

		for (int x = 0; x < column; x++)
		{
			index_pointer[i]	= num1;
			index_pointer[i+1]	= num1 + 1;
			index_pointer[i+2]	= num2;
			
			index_pointer[i+3]	= num1;
			index_pointer[i+4]	= num2;
			index_pointer[i+5]	= num2 - 1;
			index_pointer += 6;
			num1++;
			num2++;
		}
	}

	index_buffer_.AddIndices(max_index_, indices);
	if (!index_buffer_.Update())
		return false;
	return true;
}

bool dbTerrain::PosOnTerrain(const core::dbVector3 &position)
{
	if (position.x < 0 || position.x >= height_map_.width)
		return false;
	if (position.z < 0 || position.z >= height_map_.height)
		return false;
	return true;
}


float dbTerrain::GetHeight(const core::dbVector3 &position)
{
	static float old = 0.f;
	if (PosOnTerrain(position))
	{
		if (!height_map_.data)
			return old;

		/*     z
			1 ----- ----- 2
			| --          |
		x	|    --       |
			|       --    |
			|          -- |
			3 ----- ----- 4
		*/
		float h1 = height_map_.data[ConvertToArrayPos(static_cast<int>(floor(position.x)), static_cast<int>(floor(position.z)), height_map_)];
		float h2 = height_map_.data[ConvertToArrayPos(static_cast<int>(ceil(position.x)), static_cast<int>(floor(position.z)), height_map_)];
		float h3 = height_map_.data[ConvertToArrayPos(static_cast<int>(floor(position.x)), static_cast<int>(ceil(position.z)), height_map_)];
		float h4 = height_map_.data[ConvertToArrayPos(static_cast<int>(ceil(position.x)), static_cast<int>(ceil(position.z)), height_map_)];

		float dx = position.x - floor(position.x); 
		float dz = position.z - floor(position.z); 
		if (dx > dz)	// left triangle
		{
			old = (old + ( (h1 + ((h3-h1) * dx)) + (h3 + ((h4-h3) * dz)) ) / 2) / 2;
			return old;
		}
		if (dz > dx)
		{
			old = (old + ( (h1 + ((h2-h1) * dz)) + (h2 + ((h4-h2) * dx)) ) / 2)/ 2;
			return old;
		}
		// dz == dx
		old = (h1 + (h4-h1) * dx);
		return old;
	}
	return 0.f;
}


int dbTerrain::ConvertToArrayPos(const int x, const int y, const HEIGHTMAP &hmap)
{
	return y*hmap.width + x;
}


}}	// namespace
