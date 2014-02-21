#include "DbofX.h"

dbMesh::dbMesh(void)
{
}

dbMesh::dbMesh(LPDIRECT3DDEVICE9 dev)
{
	d3ddevice = dev;
	scaling = 1.f;
	rotationX = rotationY = rotationZ = 0.f;
	position = dbVector3(0.0f, 0.0f, 0.0f);

	num_of_materials_ = 1;
	x_filename_ = "";

	useMat = true;
	use_textures_ = true;
	mesh_init = false;
	
	// properties
	visible = lighting = specular = wireframe = alphablend = flatobj = zbuffer = false;		//D3DRS_SHADEMODE
	
	D3DXMatrixIdentity(&mat_rotationx_);
	mat_rotationy_ = mat_rotationz_ = mat_rotationx_;

	ZeroMemory(&d3d_texture_, sizeof(d3d_texture_));
	ZeroMemory(&d3d_material_, sizeof(d3d_material_));
}

dbMesh::~dbMesh(void)
{
	if (d3d_mesh_ && mesh_init)
	{
		d3d_mesh_->Release();
		d3d_mesh_ = NULL;
	}
	
	if (d3d_material_)
	{
		delete[] d3d_material_;
		d3d_material_ = NULL;
	}


}


void dbMesh::initMesh(char *file)
{
	if (this == NULL)		//Mesh does not exist
		return;
	
	x_filename_ = file;
	
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
			if (FileExist(tmp[i].pTextureFilename))
				D3DXCreateTextureFromFile(d3ddevice, tmp[i].pTextureFilename, &d3d_texture_[i]);
				
			else
				DBX_ERROR("Texture does not exist or could not be found");			
		}
	}

	matBuffer->Release();
	adjacencyBuffer->Release(); 

	mesh_init = true;
}

void dbMesh::createTeapot()
{
	if (this == NULL)		//Mesh does not exist
		return;

	D3DXCreateTeapot(d3ddevice, &d3d_mesh_, NULL);
	
	d3d_mesh_->OptimizeInplace(D3DXMESHOPT_COMPACT, NULL, NULL, NULL, NULL);

	useMat = false;
	use_textures_ = false;

	mesh_init = true;
}


// ********** GENERAL SETTINGS **********
void dbMesh::setVisible(bool state)
{
	if (mesh_init)
		visible = state;
}

void dbMesh::setState(DB_STATE state, bool enable)
{
	switch (state)
	{
	case DBS_WIREFRAME:	
		wireframe = enable;
	break;

	case DBS_ALPHABLEND:
		alphablend = enable;
	break;

	case DBS_FLATOBJ:
		flatobj = enable;
	break;

	case DBS_SPECULAR:
		specular = enable;
	break;

	case DBS_ZBUFFER:
		zbuffer = enable;
	break;

	case DBS_STANDARD:
		if (enable)
		{
			alphablend = true;
			lighting = true;
			zbuffer = true;
		}
	break;
	}
}


// ********** TRANSFORMATION FUNCTIONS **********
void dbMesh::RotateX(float angle, bool relative)
{
	if (this == NULL)		//Mesh does not exist
		return;

	if (relative)
		angle += rotationX;
	rotationX = angle;
	D3DXMatrixRotationX(&mat_rotationx_, rotationX);
}

void dbMesh::RotateY(float angle, bool relative)
{
	if (this == NULL)		//Mesh does not exist
		return;

	if (relative)
		angle += rotationY;
	rotationY = angle;
	D3DXMatrixRotationY(&mat_rotationy_, rotationY);
}

void dbMesh::RotateZ(float angle, bool relative)
{
	if (this == NULL)		//Mesh does not exist
		return;

	if (relative)
		angle += rotationZ;
	rotationZ = angle;
	D3DXMatrixRotationZ(&mat_rotationz_, rotationZ);
}

void dbMesh::rotateAroundPoint(float angle, dbVector3 point)
{
	D3DXMATRIX matResult, matRot, matTrans1, matTrans2;
	D3DXMatrixTranslation(&matTrans1, -point.x, -point.y, -point.z);
	D3DXMatrixRotationY(&matRot, angle);
	D3DXMatrixTranslation(&matTrans2, point.x, point.y, point.z);

	matResult = matTrans2 * matRot * matTrans2;
		//TODO
}


void dbMesh::Scale(float factor)
{
	if (this == NULL)		//Mesh does not exist
		return;

	scaling = abs(factor);		//TODO - maybe fix this so you can scale upside down (negative)
}

void dbMesh::Move(dbVector3 pos, bool relative)
{
	if (this == NULL)		//Mesh does not exist
		return;
	
	if (relative)
		pos += position;
	position = pos;
}



void dbMesh::draw()
{
	if (mesh_init != true || (!visible) || (this == NULL))		//Mesh does not exist
		return;


	D3DXMATRIX matResult, matRotation, matScaling, matTranslation;

	matRotation = mat_rotationz_ * mat_rotationx_ * mat_rotationy_ ;

	D3DXMatrixTranslation(&matTranslation, position.x, position.y, position.z);

	D3DXMatrixScaling(&matScaling, scaling, scaling, scaling);

	matResult = matScaling * matRotation * matTranslation;
	d3ddevice->SetTransform(D3DTS_WORLD, &matResult);

	// Check states and set RenderState
	d3ddevice->SetRenderState(D3DRS_LIGHTING, lighting);
	d3ddevice->SetRenderState(D3DRS_SPECULARENABLE, specular);
	d3ddevice->SetRenderState(D3DRS_FILLMODE, (wireframe) ? D3DFILL_WIREFRAME : D3DFILL_SOLID);
	d3ddevice->SetRenderState(D3DRS_ALPHABLENDENABLE, alphablend);
	d3ddevice->SetRenderState(D3DRS_SHADEMODE, (flatobj) ? D3DSHADE_FLAT : D3DSHADE_GOURAUD);
	d3ddevice->SetRenderState(D3DRS_ZWRITEENABLE, zbuffer);
	

	// Loop through the number of materials for this mesh?
	for(DWORD i = 0; i < num_of_materials_; i++)
	{
		// Set the material for this mesh subset 
		if (useMat)
			d3ddevice->SetMaterial(&d3d_material_[i]); 
		
		// Set the texture for this mesh subset
		if (use_textures_)
			d3ddevice->SetTexture(0, d3d_texture_[i]);

		// Draw the current mesh subset 
		d3d_mesh_->DrawSubset(i); 
	}
}