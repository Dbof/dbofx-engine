#include "DbofX.h"

using std::string;

namespace dbx
{
namespace scene
{
	using namespace core;
//********** CONSTRUCTORS / DESTRUCTORS **********
dbMesh::dbMesh(const string& id) : dbSceneNode(id)
{
	ZeroMemory(this, sizeof(dbMesh));
	
	identifier_					= id;
	mesh_matrix_					= dbMatrixIdentity();
	node_pos_					= dbVector3(0.f, 0.f, 0.f);
	node_right_					= dbVector3(1.f, 0.f, 0.f);
	node_up_						= dbVector3(0.f, 1.f, 0.f);
	node_direction_				= dbVector3(0.f, 0.f, 1.f);
	node_movespeed_				= 1.f;
	node_rotationspeed_			= 1.f;
	node_rotationx_ = node_rotationy_ = node_rotationz_ = 0.f;
	mesh_scale_ = dbVector3(1.f, 1.f, 1.f);

	mat_rotationx_ = mat_rotationy_ = mat_rotationz_ = dbMatrixIdentity();

	x_filename_	= "";
	num_of_materials_	= 1;
	d3d_material_			= NULL;	
	d3d_texture_			= NULL;
	d3d_mesh_		= NULL;
	is_init_		= use_materials_ = use_textures_ = false;
	d3d_mesh_fvf_		= D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2;
}


dbMesh::~dbMesh()
{
	if (!is_init_)
		return;
	
	if (d3d_texture_)
	{
		for (DWORD i = 0; i < num_of_materials_; i++)
			DB_MEM_RELEASE(d3d_texture_[i]);
			
		DB_MEM_DELETE_ARRAY(d3d_texture_);
	}

	DB_MEM_DELETE_ARRAY(d3d_material_);
	DB_MEM_RELEASE(d3d_mesh_);
}

//**********************************************************************

bool dbMesh::Init(const string &filename)
{
	if (!dbDevice::isInitialized())
	{
		DBX_ERROR("Mesh could not be loaded. Device not initialized yet");
		return false;
	}

	// check parameters
	if (filename == "")
	{
		DBX_ERROR("Mesh could not be loaded. An invalid name or an invalid file path was specified.");
		return false;
	}

	x_filename_ = filename;

	if (!FileExist(x_filename_.c_str()))
	{
		DBX_ERROR("File not found");
		return false;
	}

	LPD3DXBUFFER matBuffer;
	LPD3DXBUFFER adjacencyBuffer;

	// TODO: Add selection managed/unmanaged
	HRESULT hr = D3DXLoadMeshFromX(x_filename_.c_str(), D3DXMESH_MANAGED, dbDevice::getDevice(), &adjacencyBuffer, &matBuffer, NULL, &num_of_materials_, &d3d_mesh_);
	if (FAILED(hr))
	{
		DBX_ERROR("File found, but mesh could not be loaded.");
		return false;
	}


	if (num_of_materials_ >= 1)
		use_materials_ = use_textures_ = true;

	D3DXMATERIAL* tmp = (D3DXMATERIAL*)matBuffer->GetBufferPointer();
	d3d_material_ = new D3DMATERIAL9[num_of_materials_];
	d3d_texture_ = new LPDIRECT3DTEXTURE9[num_of_materials_];

	for (DWORD i = 0; i < num_of_materials_; i++)
	{
		d3d_material_[i] = tmp[i].MatD3D;
		
		//Since D3DX does not do it
		d3d_material_[i].Ambient = d3d_material_[i].Diffuse;
		d3d_material_[i].Power = 10.f;
	
		d3d_texture_[i] = NULL;
		if (tmp[i].pTextureFilename != NULL && strlen(tmp[i].pTextureFilename) > 0)
		{
			string tmp_tex = static_cast<string>(tmp[i].pTextureFilename);	//TODO
			if (FileExist((char*)tmp_tex.c_str()))
			{
				if (FAILED( D3DXCreateTextureFromFile(dbDevice::getDevice(), tmp_tex.c_str(), &d3d_texture_[i]) ))
				{
					DBX_WARN("Texture \"" + tmp_tex + "\" could not be loaded");
					d3d_texture_[i] = NULL;
				}
				else
				{
					tex_names_.push_back(tmp_tex);
					DBX_INFO("Texture \"" + tmp_tex + "\" loaded");
				}	
			}
			else
				DBX_ERROR("Texture \"" + tmp_tex + "\" could not be found");			
		}
	}

	DB_MEM_RELEASE(matBuffer);
	DB_MEM_RELEASE(adjacencyBuffer); 

	// calculate tangents and normals
	D3DXComputeNormals(d3d_mesh_, NULL);
	d3d_mesh_->OptimizeInplace(D3DXMESHOPT_COMPACT, NULL, NULL, NULL, NULL);

	DBX_INFO("Mesh loaded successfully");

	is_init_ = true;
	return true;
}

//**********************************************************************

bool dbMesh::InitStandard(MESH_STANDARD shape)
{
	if (!dbDevice::isInitialized())
	{
		DBX_ERROR("Mesh could not be loaded. Device not initialized yet");
		return false;
	}
 
	switch(shape)
	{
		case MS_BOX:
			D3DXCreateBox(dbDevice::getDevice(), 10, 10, 10, &d3d_mesh_, NULL);
		break;
	
		case MS_SPHERE:
			D3DXCreateSphere(dbDevice::getDevice(), 5, 12, 12, &d3d_mesh_, NULL);
		break;

		case MS_TEAPOT:
			D3DXCreateTeapot(dbDevice::getDevice(), &d3d_mesh_, NULL);
		break;
	}
	
	
	use_materials_ = false;
	num_of_materials_ = 0;
	use_textures_ = false;

	D3DXComputeNormals(d3d_mesh_, NULL);
	d3d_mesh_->OptimizeInplace(D3DXMESHOPT_COMPACT, NULL, NULL, NULL, NULL);

	DBX_INFO("Standard mesh loaded successfully");

	is_init_ = true;
	return true;
}

//**********************************************************************

void dbMesh::Scale(const dbVector3 &factor)
{
	mesh_scale_ = factor;
}

//**********************************************************************

bool dbMesh::Render(int iSubset)
{
	return Render(iSubset, true, true);
}

bool dbMesh::Render(int iSubset, bool useMaterials, bool useTextures)
{
	if (!is_init_)
	{
		DBX_ERROR("Mesh is not initialized and could not be rendered");
		return false;
	}

	if (iSubset < 0 || iSubset > (int)num_of_materials_)
	{	// draw all subsets

		// Loop through the number of materials for this mesh
		for(DWORD i = 0; i < num_of_materials_; i++)
		{
			// Set the material for this mesh subset 
			if (useMaterials && use_materials_)
				dbDevice::getDevice()->SetMaterial(&d3d_material_[i]);		//TODO
		
			// Set the texture for this mesh subset
			if (useTextures && use_textures_)
				dbDevice::getDevice()->SetTexture(0, d3d_texture_[i]);		//TODO
			
				dbDevice::SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
			if (FAILED(d3d_mesh_->DrawSubset(i)))
				DBX_ERROR("Could not draw subset");

			dbDevice::SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
			// Draw the current mesh subset 
			if (FAILED(d3d_mesh_->DrawSubset(i)))
				DBX_ERROR("Could not draw subset");
		}

	}
	else
	{
		if (useMaterials)
			dbDevice::getDevice()->SetMaterial(&d3d_material_[iSubset]);
		if (useTextures)
				dbDevice::getDevice()->SetTexture(0, d3d_texture_[iSubset]);	

		dbDevice::SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		d3d_mesh_->DrawSubset(iSubset);

	}

	return true;
}

//**********************************************************************

dbMatrix dbMesh::GetMatrix()
{
	dbMatrix viewMatrix = dbMatrixIdentity();

	viewMatrix._11 = node_right_.x; viewMatrix._12 = node_up_.x; viewMatrix._13 = node_direction_.x;
	viewMatrix._21 = node_right_.y; viewMatrix._22 = node_up_.y; viewMatrix._23 = node_direction_.y;
	viewMatrix._31 = node_right_.z; viewMatrix._32 = node_up_.z; viewMatrix._33 = node_direction_.z;

	viewMatrix._41 = - dbVector3Dot(node_pos_, node_right_ );
	viewMatrix._42 = - dbVector3Dot(node_pos_, node_up_ );
	viewMatrix._43 = - dbVector3Dot(node_pos_, node_direction_ );
	
	viewMatrix = dbMatrixScaling(mesh_scale_) * dbMatrixInvert(viewMatrix);
	return viewMatrix;	// SRT = scale, rotate, translate
};

}} // namespace