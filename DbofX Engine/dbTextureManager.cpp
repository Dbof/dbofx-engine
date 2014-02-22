#include "DbofX.h"

using std::string;
using std::vector;
using std::unordered_map;

namespace dbx
{
namespace res
{
	using namespace core;
//********** INITIALIZATION OF STATIC VARIABLES **********
	dbTextureManager* dbTextureManager::instance_ = NULL;

//**********************************************************************
// FUNCTIONS

dbTextureManager::dbTextureManager()
{
	num_max_textures_ = num_textures_ = 0;
}

dbTextureManager* dbTextureManager::GetInstance()
{
	if (!instance_)
		instance_ = new dbTextureManager();
	return instance_; 
}


bool dbTextureManager::Init(int MaxNumTextures)
{
	if (!dbDevice::isInitialized())
	{
		DBX_ERROR("Could not initiate Texture Manager. Device not initialized yet");
		return false;
	}

	num_max_textures_ = MaxNumTextures;
	is_init_ = true;

	DBX_INFO("Texture Manager initialized!");
	return true;
}

//**********************************************************************

void dbTextureManager::Exit()
{
	if (is_init_)
	{
		for (unordered_map<string, TEXTURE_ITEM>::iterator it = texture_item_map_.begin(); it != texture_item_map_.end(); it++)
		{
			if (it->second.Exists)
				DB_MEM_RELEASE(it->second.Texture);
		}

		texture_item_map_.clear();
		is_init_ = false;
		DBX_INFO("Texture Manager was shut down.");
	}
}

//**********************************************************************

void dbTextureManager::Lost()
{
	for (unordered_map<string, TEXTURE_ITEM>::iterator it = texture_item_map_.begin(); it != texture_item_map_.end(); it++)
	{
		if (it->second.Exists && !it->second.isManaged)
			DB_MEM_RELEASE(it->second.Texture);
	}
}

bool dbTextureManager::Reset()
{
	HRESULT hr;

	// Load them now
	for (unordered_map<string, TEXTURE_ITEM>::iterator it = texture_item_map_.begin(); it != texture_item_map_.end(); it++)
	{
		TEXTURE_ITEM* tex = &it->second;
		if (tex->Exists)
		{
			if (!tex->isManaged && !tex->Texture)
			{
				switch (tex->Type)
				{
				case TEX_2D:
					hr = D3DXCreateTextureFromFileEx(	dbDevice::getDevice(),
										tex->Filename.c_str(),
										D3DX_DEFAULT,
										D3DX_DEFAULT, 
										tex->MipMaps,
										NULL,
										D3DFMT_UNKNOWN,
										(tex->isManaged) ? D3DPOOL_MANAGED : D3DPOOL_DEFAULT,
										D3DX_FILTER_NONE,
										(tex->MipMaps) ? D3DX_DEFAULT : D3DX_FILTER_NONE,
										tex->ColorKey,
										NULL, NULL,
										(LPDIRECT3DTEXTURE9*)&tex->Texture);

					if (FAILED(hr))
					{
						DBX_ERROR("Error while restoring 2D texture!");
						return false;
					}
				break;

				case TEX_CUBE:
					hr = D3DXCreateCubeTextureFromFileEx(	dbDevice::getDevice(), tex->Filename.c_str(), 
												D3DX_DEFAULT,
												tex->MipMaps, 
												NULL,
												D3DFMT_UNKNOWN, 
												(tex->isManaged) ? D3DPOOL_MANAGED : D3DPOOL_DEFAULT,
												D3DX_FILTER_NONE,
												(tex->MipMaps) ? D3DX_DEFAULT : D3DX_FILTER_NONE,
												tex->ColorKey,
												NULL, NULL,
												(LPDIRECT3DCUBETEXTURE9*)&tex->Texture);
					if (FAILED(hr))
					{
						DBX_ERROR("Error while restoring cube texture!");
						return false;
					}
				break;
				}
			}
		}
	}

	return true;
}

//**********************************************************************
// LIST METHODS
TEXTURE_ITEM* dbTextureManager::AddTexture(TEX_TYPE Type, const string &filepath, bool ManagedPool, bool MipMaps, dbColor ColorKey)
{
	D3DPOOL tmp_pool = D3DPOOL_DEFAULT;
	DWORD	mipFilter = D3DX_FILTER_NONE;
	int		mipLevels = 1;
	TEXTURE_ITEM* item = NULL;
	D3DCOLOR colorkey = (ColorKey.r == -1) ? 0 : ColorKey;	// disable Colorkey if -1 was passed
	string filename = filepath.substr(filepath.find_last_of("/\\")+1);

	if (!is_init_)
	{
		DBX_ERROR("Could not load texture. Texture Manager not initiated.");
		return false;
	}
	
	if (!GetFreeEntry())
	{
		DBX_ERROR("Could not load texture. Max number of textures exceeded.");
		return false;
	}

	if (!FileExist((char*)filepath.c_str()))
	{
		DBX_ERROR("Texture does not exist or could not be found.");
		return false;
	}

	if (MipMaps)
	{
		mipLevels = D3DX_DEFAULT;
		mipFilter = D3DX_DEFAULT;
	}

	TEXTURE_ITEM* tmp_itm = SearchTexture(filename);
	if (tmp_itm)		//Texture already exists
	{
		DBX_WARN("Texture already exists!");
		return tmp_itm;
	}

	if (ManagedPool)
		tmp_pool = D3DPOOL_MANAGED;

	HRESULT hr;
	LPDIRECT3DTEXTURE9 tmp_tex = NULL;
	LPDIRECT3DCUBETEXTURE9 tmp_tex_cube = NULL;
	D3DSURFACE_DESC	SurfaceDesc;

	switch (Type)
	{
	case TEX_2D:
		hr = D3DXCreateTextureFromFileEx(	dbDevice::getDevice(),
											filepath.c_str(),
											D3DX_DEFAULT,
											D3DX_DEFAULT, 
											mipLevels,
											NULL,
											D3DFMT_UNKNOWN,
											tmp_pool,
											D3DX_FILTER_NONE,
											mipFilter,
											colorkey,
											NULL, NULL,
											&tmp_tex);

		if (FAILED(hr))
		{
			DBX_ERROR("Error while creating 2D texture from file.");
			return false;
		}

		tmp_tex->GetLevelDesc(0, &SurfaceDesc);

		item = AddEntry(	filename,
							tmp_tex,
							filepath,
							SurfaceDesc.Width,
							SurfaceDesc.Height,
							1,
							SurfaceDesc.Format, 
							ManagedPool,
							MipMaps,
							colorkey,
							Type);

	break;
	
	case TEX_CUBE:
		hr = D3DXCreateCubeTextureFromFileEx(	dbDevice::getDevice(), filepath.c_str(), D3DX_DEFAULT,
												mipLevels, 
												NULL,
												D3DFMT_UNKNOWN, 
												tmp_pool,
												D3DX_FILTER_NONE,
												mipFilter,
												colorkey,
												NULL, NULL,
												&tmp_tex_cube);
		if (FAILED(hr))
		{
			DBX_ERROR("Error while creating cube texture from file.");
			return false;
		}

		tmp_tex_cube->GetLevelDesc(0, &SurfaceDesc);		
		
		item = AddEntry(	filename,
							tmp_tex_cube,
							filepath,
							SurfaceDesc.Width,
							SurfaceDesc.Height,
							1,
							SurfaceDesc.Format, 
							ManagedPool,
							MipMaps,
							colorkey,
							Type);
	break;
	}

	if (item == NULL)
	{
		DBX_ERROR("Could not load texture to list.");
		return false;
	}

	num_textures_++;
	return item;
}

//**********************************************************************

TEXTURE_ITEM* dbTextureManager::AddEntry(std::string identifier,
						LPDIRECT3DBASETEXTURE9 texture,
						string filename,
						int width, int heigth, int depth,
						D3DFORMAT format, 
						bool poolManaged,
						bool MipMaps,
						D3DCOLOR ColorKey,
						TEX_TYPE iType)
{
	TEXTURE_ITEM tex;

	// copy data
	tex.Texture = texture;
	tex.Type = iType;
	tex.Width = width;
	tex.Heigth = heigth;
	tex.Format = format;
	tex.ColorKey = ColorKey;
	tex.isManaged = poolManaged;
	tex.MipMaps = MipMaps;
	tex.Filename = filename;
	tex.Exists = true;

	// add to map
	texture_item_map_[identifier] = tex;

	return &texture_item_map_[identifier];
}

//**********************************************************************

int	dbTextureManager::GetFreeEntry()
{
	if (num_textures_ >= num_max_textures_)
		return false;
	return true;
}

//**********************************************************************

TEXTURE_ITEM* dbTextureManager::SearchTexture(const string &filename)
{
	std::unordered_map<string, TEXTURE_ITEM>::iterator it = texture_item_map_.find(filename);
	if (it == texture_item_map_.end())
		return NULL;
	return &it->second;
}

//**********************************************************************

bool dbTextureManager::DeleteTexture(const string &filename)
{
	TEXTURE_ITEM* tex = SearchTexture(filename);
	if (!tex)
		return false;

	DB_MEM_RELEASE(tex->Texture);
	tex->Exists = false;
	texture_item_map_.erase(filename);
	return true;
}

//**********************************************************************

}}	// namespace