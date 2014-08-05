#ifndef TERRAIN_H
#define TERRAIN_H

namespace dbx
{
namespace scene
{
	enum HMAP_TYPE { HT_BMP, HT_RAW };
	
	class DBX_API dbTerrain
	{
	public:
		dbTerrain();
		~dbTerrain();

		bool Init(int numX, int numY);
		bool Init(const char* filename, HMAP_TYPE hType, float normValue = 1.f);
		void Render();

		bool PosOnTerrain(const core::dbVector3 &position);
		float GetHeight(const core::dbVector3 &position);
		
	private:
		struct TERRAIN_VERTEX
		{
			core::dbVector3	pos;
			core::dbVector3 normal;
			DWORD			color;
			float			u, v;
			static DWORD	FVF;
		};

		struct HEIGHTMAP
		{
			int		width;
			int		height;
			float*	data;
		};


		bool InitVB();
		bool InitIB();
		bool LoadBMPHeightMap(const char* fname, float normValue);
		bool LoadRAWHeightMap(const char* fname, float normValue);
		int	ConvertToArrayPos(const int x, const int y, const HEIGHTMAP &hmap);

		core::dbVertexBuffer	vertex_buffer_;
		core::dbIndexBuffer		index_buffer_;
		int						max_vertex_;
		int						max_index_;
		int						num_vertices_x_;
		int						num_vertices_y_;
		HEIGHTMAP				height_map_;	// height map
	};

}}

#endif