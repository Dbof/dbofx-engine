#include "DbofX.h"

using std::vector;
using std::string;
using std::unordered_map;

namespace dbx
{
namespace scene
{
	using namespace core;

dbScene::dbScene(const string& identifier)
{
	active_camera_ = NULL;
	active_skybox_ = NULL;
	dbSceneMgr::GetInstance()->AddScene(this);
}

void dbScene::Release()
{
	DB_MEM_DELETE(active_camera_);
	DB_MEM_DELETE(active_skybox_);

	// mesh cleanup
	for (std::unordered_map<string, dbMesh*>::iterator it = meshes_map_.begin(); it != meshes_map_.end(); it++)
		DB_MEM_DELETE(it->second);
	// mesh cleanup
}


void dbScene::OnLostDevice()
{ 
	// meshes
	for (std::unordered_map<string, dbMesh*>::iterator it = meshes_map_.begin(); it != meshes_map_.end(); it++)
	{
		DB_MEM_RELEASE(it->second->d3d_mesh_);
		it->second->is_init_ = false;
	}
}

void dbScene::OnResetDevice()
{ 
	// meshes
	for (std::unordered_map<string, dbMesh*>::iterator it = meshes_map_.begin(); it != meshes_map_.end(); it++)
	{
		it->second->Init(it->second->x_filename_);
	}
}

// ********** MESHES FUNCTION **********
dbMesh* dbScene::AddMesh(const string& identifier, const string file)
{
	if (!FileExist(file.c_str()))
	{
		DBX_ERROR("Mesh file \"" + file + "\" not found");
		return NULL;
	}

	if (meshes_map_.count(identifier) > 0)
		return NULL;	// already exists
	
	dbMesh* tmp = new dbMesh(identifier);
	if (!tmp->Init(file))
	{
		delete tmp;
		return NULL;	// FAILED to load
	}

	meshes_map_[identifier] = tmp;
	return tmp;
}

dbMesh* dbScene::AddTeapot(const string& identifier)
{
	if (meshes_map_.count(identifier) > 0)
		return NULL;	// already exists
	
	dbMesh* tmp = new dbMesh(identifier);
	if (!tmp->InitStandard(MS_TEAPOT))
	{
		delete tmp;
		return NULL;	// FAILED to load
	}
	
	meshes_map_[identifier] = tmp;
	return tmp;
}

dbMesh* dbScene::GetMesh(const string& name)
{
	std::unordered_map<string, dbMesh*>::iterator it = meshes_map_.find(name);
	if (it == meshes_map_.end())
	{
		#ifdef _DEBUG
		DBX_ERROR("Could not get mesh \"" + name + "\"");
		#endif

		return NULL;
	}
	return it->second;
}



// ********** LIGHTING FUNCTIONS **********
dbDirectionalLight* dbScene::AddDirectionalLight(const string& identifier, dbVector3 direction, dbColor Diffuse, dbColor Specular, dbColor Ambient)
{
	dbDirectionalLight* dbl = dbLightManager::GetInstance()->InitDirectional(identifier, direction, Diffuse, Specular, Ambient);
	return dbl;
}

dbPointLight* dbScene::AddPointLight( const std::string& identifier, core::dbVector3 pos, float range, core::dbColor Diffuse, core::dbColor Specular /*= core::dbColor(1.f)*/, core::dbColor Ambient /*= core::dbColor(1.f)*/ )
{
	dbPointLight* dbl = dbLightManager::GetInstance()->InitPoint(identifier, pos, range, Diffuse, Specular, Ambient);
	return dbl;
}

dbSpotLight* dbScene::AddSpotLight(const std::string& identifier, core::dbVector3 pos, core::dbVector3 direction, 
								   float range, float angleInnerCircle, float angleOuterCircle, 
								   core::dbColor Diffuse, core::dbColor Specular /*= core::dbColor(1.f)*/, core::dbColor Ambient /*= core::dbColor(1.f)*/ )
{
	dbSpotLight* dbl = dbLightManager::GetInstance()->InitSpot(identifier, pos, direction, range, Diffuse, Specular, Ambient, angleInnerCircle, angleOuterCircle);
	return dbl;
}

dbLight* dbScene::GetLight(const string& id)
{
	return dbLightManager::GetInstance()->GetLight(id);
}

// ********** CAMERA FUNCTIONS **********
dbCamera* dbScene::AddCamera(const string identifier, dbVector3 position, dbVector3 direction)
{
	if (active_camera_ != NULL)
		DB_MEM_DELETE(active_camera_);

	active_camera_ = new dbCamera(identifier);
	active_camera_->Setup(position, direction);

	return active_camera_;
}

dbCamera* dbScene::GetCamera()
{
	return active_camera_;	//This can be NULL, so we need to care about in the called function
}


dbCamera* dbScene::SetCamera(dbCamera* cam)
{
	//camera->setup(*cam);
	return active_camera_;
}

// ********** SCENENODE FUNCTIONS **********
dbSceneNode* dbScene::AddSceneNode(const string& identifier)
{
	if (scene_nodes_map_.count(identifier) > 0)	// already exists
		return NULL;	// return NULL if identifier already exists
	
	dbSceneNode* tmp = new dbSceneNode(identifier);
	scene_nodes_map_[identifier] = tmp;
	return tmp;
}

dbSceneNode* dbScene::GetSceneNode(const string& identifier)
{
	std::unordered_map<string, dbSceneNode*>::iterator it = scene_nodes_map_.find(identifier);

	if (it == scene_nodes_map_.end())	// not found
		return NULL;
	return it->second;
}

// ********** SKYBOX FUNCTIONS **********

dbSkybox* dbScene::AddSkybox(const string& filename)
{
	if (active_skybox_ != NULL)
		DB_MEM_DELETE(active_skybox_);

	active_skybox_ = new dbSkybox();
	if (! active_skybox_->Init(filename))
		return NULL;

	return active_skybox_;
}

dbSkybox* dbScene::GetSkybox()
{
	return active_skybox_;
}

// ********** SKYBOX FUNCTIONS **********

void dbScene::SaveScreenshot(const string& filename)
{
	string fext, fdir, fname;
	SplitPath(filename, NULL, &fdir, &fext, &fname);

	_D3DXIMAGE_FILEFORMAT format;
	if (fext == "bmp")
		format = D3DXIFF_BMP;
	else if (fext == "jpg" || fext == "jpeg")
		format = D3DXIFF_JPG;
	else if (fext == "png")
		format = D3DXIFF_PNG;
	else if (fext == "dds")
		format = D3DXIFF_DDS;
	else if (fext == "tga")
		format = D3DXIFF_TGA;
	else
	{
		DBX_WARN("File extension not supported or recognized! Saving as BMP..");
		fext = ("bmp");
		format = D3DXIFF_BMP;
	}

	string result_name = fname;
	for (int i = 0; FileExist(fdir + result_name + "." + fext) == true; i++)
	{
		result_name = fname + toString(i);
	}
	result_name = fdir + result_name + "." + fext;

	LPDIRECT3DSURFACE9 bbuffer;
	dbDevice::getDevice()->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &bbuffer);	// this adds to ref counter, so release after
	D3DXSaveSurfaceToFile(result_name.c_str(), format, bbuffer, NULL, NULL);
	bbuffer->Release();
}

}}	// namespace