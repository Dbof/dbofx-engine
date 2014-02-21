#ifndef _SKYBOX_H
#define _SKYBOX_H

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0))

namespace dbx
{
namespace scene
{
	class DBX_API dbSkybox
	{
	public:
		~dbSkybox(void);

		void Render(const core::dbVector3& position);
		void OnLostDevice();
		void OnResetDevice();

	private:
		friend class dbScene;

		struct SKYBOX_VERTEX
		{
			core::dbVector3 xyz;
			core::dbVector3 normal;
		};

		dbSkybox(void);

		bool InitCube();
		bool Init(const std::string& fname);	// for dbScene

		int*						index_arr_;
		LPDIRECT3DINDEXBUFFER9		index_buffer_;
		LPDIRECT3DVERTEXBUFFER9		vertex_buffer_;

		std::string					texture_name_;
		int							texture_index_;
	
		float						sky_size_;

	};

}}	// namespace
#endif