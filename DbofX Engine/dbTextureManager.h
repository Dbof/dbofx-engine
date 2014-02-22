#ifndef _TEXTUREMGR_H
#define _TEXTUREMGR_H

namespace dbx
{
namespace res
{
	enum TEX_TYPE { TEX_2D, TEX_CUBE };

	struct DBX_API TEXTURE_ITEM
	{
		LPDIRECT3DBASETEXTURE9	Texture;
		TEX_TYPE				Type;
		int						Width;
		int						Heigth;
		D3DFORMAT				Format;
		core::dbColor			ColorKey;
		bool					isManaged;
		bool					MipMaps;
		std::string				Filename;
		bool					Exists;
	};

	class DBX_API dbTextureManager
	{
	public:
		static dbTextureManager* GetInstance();

		bool Init(int MaxNumTextures);
		void Exit();
		bool IsInitiated() { return is_init_; }

		// list methods
		TEXTURE_ITEM* AddTexture(TEX_TYPE Type, const std::string &filepath, bool ManagedPool, bool MipMaps, core::dbColor ColorKey = core::dbColor(-1.f));
		TEXTURE_ITEM* SearchTexture(const std::string &filename);
		bool DeleteTexture(const std::string &filename);
		
		void Lost();
		bool Reset();

	private:
		dbTextureManager();

		// private methods
		TEXTURE_ITEM* AddEntry(std::string identifier,
						LPDIRECT3DBASETEXTURE9 texture,
						std::string  filename,
						int width, int heigth, int depth,
						D3DFORMAT format, 
						bool poolManaged,
						bool MipMaps,
						D3DCOLOR ColorKey,
						TEX_TYPE iType);
		int GetFreeEntry();

		// private members
		static dbTextureManager* instance_;

		std::unordered_map<std::string, TEXTURE_ITEM> texture_item_map_;
		int num_max_textures_;
		int num_textures_;

		bool is_init_;
		bool square_only_;			// TODO - getCaps and check if non-square text. are supported

		DISALLOW_COPY_AND_ASSIGN(dbTextureManager);
	};

}}	// namespace
#endif