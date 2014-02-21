#ifndef _MESH_H
#define _MESH_H

namespace dbx
{
namespace scene
{
	// little enumeration for standard shapes
	enum MESH_STANDARD 
	{ 
		MS_BOX,
		MS_SPHERE,
		MS_TEAPOT
	};

	class DBX_API dbMesh : public dbSceneNode
	{
	public:
		/*
		struct DBMESH_VERTEX
		{
			core::dbVector3	position;
			core::dbVector3	normal;
			float		tu1, tv1;
			float		tu2, tv2;
		};
		*/

		~dbMesh();

		// "constructors"
		bool Init(const std::string& filename);		//TODO - later add XFile class
		bool InitStandard(MESH_STANDARD shape);
		bool IsInitialized()	{ return is_init_; };

		// render
		bool Render(int iSubset, bool useMaterials, bool useTextures);
		bool Render(int iSubset);	// specify -1 for every subset
	
		// alpha blending
		void EnableAlphaBlending(bool enabled) { use_alpha_ = enabled; }

		// change mesh
		void Scale(const core::dbVector3 &factor);

		// getter methods
		core::dbMatrix GetMatrix();	// used for transformation

	private:
		friend class dbScene;

		// private constructor
		dbMesh(const std::string& identifier);

		bool						is_init_;
		bool						use_materials_;
		bool						use_textures_;
		bool						use_alpha_;

		D3DMATERIAL9*				d3d_material_;			// Materials array
		LPDIRECT3DTEXTURE9*			d3d_texture_;			// Textures array
		LPD3DXMESH					d3d_mesh_;				// D3DX Mesh object
		DWORD						num_of_materials_;
		DWORD						d3d_mesh_fvf_;

		core::dbMatrix				view_matrix_, mesh_matrix_;
		core::dbVector3				mesh_scale_;

		std::string					x_filename_;		// full path of .x
		std::vector<std::string>	tex_names_;		// full paths of textures
	};

}}	// namespace
#endif