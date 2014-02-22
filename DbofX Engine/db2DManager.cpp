#include "DbofX.h"

namespace dbx
{
namespace video
{
db2DManager* db2DManager::instance_ = NULL;

using namespace core;

db2DManager::db2DManager(void)
{
	d3dx_sprite_ = NULL;

	if (FAILED(D3DXCreateSprite(dbDevice::getDevice(), &d3dx_sprite_)))
	{
			DBX_ERROR("Could not create sprite object!");
			return;
	}
	batched_draw_2D_ = batched_draw_3D_ = false;
	ResetSettings();
}

void db2DManager::Exit()
{
	DB_MEM_RELEASE(d3dx_sprite_);
}

db2DManager* db2DManager::GetInstance()
{
	if (!instance_)
		instance_ = new db2DManager();
	return instance_;
}


void db2DManager::SetRotation(const float& radian)
{
	sprite_rotation_ = radian;
}

void db2DManager::SetScaling(dbVector3& scaling)
{
	// since z-coordinate is not used
	scaling.z = 1.f;
	this->sprite_scaling_ = scaling;
}

// Begin draw image
void db2DManager::Begin2D()
{
	if (batched_draw_2D_)
		return;

	DWORD flags = D3DXSPRITE_ALPHABLEND;

	if (FAILED( d3dx_sprite_->Begin(flags) ))
	{
		DBX_ERROR("Could not begin sprite rendering!");
		return;
	}
	batched_draw_2D_ = true;
}

void db2DManager::Begin3D()
{
	if (batched_draw_3D_ || d3dx_sprite_ == NULL)
		return;

	DWORD flags = D3DXSPRITE_OBJECTSPACE | D3DXSPRITE_ALPHABLEND;

	if (FAILED( d3dx_sprite_->Begin(flags) ))
	{
		DBX_ERROR("Could not begin sprite rendering!");
		return;
	}
	batched_draw_3D_ = true;
}

void db2DManager::DrawImage2D(res::TEXTURE_ITEM* item, RECT* sourceRect, dbVector3 destination, dbColor color, bool useAlpha)
{
	if (batched_draw_3D_)
	{
		DBX_WARN("Drawing 2D image not possible in Begin3D or BeginBillboard block!");
		return;
	}

	float width = (float) (item->Width * sprite_scaling_.x / 2);
	float height = (float) (item->Heigth * sprite_scaling_.y / 2);
	
	dbVector3 center = dbVector3(destination.x + width, destination.y + height, 0);
	d3dx_sprite_->SetTransform((const D3DXMATRIX*) &(dbMatrixScaling(sprite_scaling_) * dbMatrix2DRotation(sprite_rotation_) * dbMatrixTranslation(destination)));

	// drawing part
	DWORD flags = (useAlpha ? D3DXSPRITE_ALPHABLEND : 0);
	bool result = Draw(flags, item, sourceRect, &center, &destination, color);
}

void db2DManager::DrawImage2D( res::TEXTURE_ITEM* item, RECT* sourceRect, core::dbVector3 destination )
{
	DrawImage2D(item, sourceRect, destination, dbColor(1.f), false);
}

void db2DManager::DrawImage3D(res::TEXTURE_ITEM* item, RECT* sourceRect, dbVector3 destination, dbColor color, bool useAlpha)
{
	if (batched_draw_2D_)
	{
		DBX_WARN("Drawing 3D image not possible in Begin2D or BeginBillboard block!");
		return;
	}

	float width = (float) (item->Width * sprite_scaling_.x / 2);
	float height = (float) (item->Heigth * sprite_scaling_.y / 2);

	sprite_scaling_.z = 1.f;
	
	dbVector3 rcenter = dbVector3(width, height, 0);
	destination -= rcenter;
	
	// since texture is upside down
	if (sprite_rotation_ == 0.f)
		sprite_rotation_ = DB_TO_RAD(180);

	dbMatrix rotationMatrix = dbMatrixTranslation(-1 * rcenter) * dbMatrix2DRotation(sprite_rotation_) * dbMatrixTranslation(rcenter); 
	dbMatrix transformMatrix = dbMatrixScaling(sprite_scaling_) * rotationMatrix * dbMatrixTranslation(destination);
	
	dbDevice::getDevice()->SetTransform(D3DTS_WORLD, (const D3DXMATRIX*) &transformMatrix);

	// drawing part
	DWORD flags = D3DXSPRITE_OBJECTSPACE | (useAlpha ? D3DXSPRITE_ALPHABLEND : 0);
	bool result = Draw(flags, item, sourceRect, NULL, NULL, color);
}

void db2DManager::DrawImage3D( res::TEXTURE_ITEM* item, RECT* sourceRect, core::dbVector3 destination )
{
	DrawImage3D(item, sourceRect, destination, dbColor(1.f), false);
}

void db2DManager::DrawBillboard(res::TEXTURE_ITEM* item, RECT* sourceRect, dbVector3 destination, dbColor color, bool useAlpha)
{
	if (item == NULL)
	{
		DBX_ERROR("Texture item is NULL");
		return;
	}

	if (batched_draw_2D_ || batched_draw_3D_)
	{
		DBX_WARN("Drawing billboard not possible in Begin2D or Begin3D block!");
		return;
	}

	float width = (float) (item->Width * sprite_scaling_.x / 2);
	float height = (float) (item->Heigth * sprite_scaling_.y / 2);
	
	dbVector3 rcenter = dbVector3(width, height, 0.f);
	dbVector3 origCenter = dbVector3(item->Width / 2.f, item->Heigth / 2.f, 0);

	dbMatrix viewMatrix, spriteMatrix, transformMatrix;
	
	// rotate sprite
	spriteMatrix = dbMatrix2DRotation(180);
	d3dx_sprite_->SetTransform((D3DXMATRIX*) &spriteMatrix);
	
	// Transformation (new world matrix)
	transformMatrix = dbMatrixScaling(sprite_scaling_)* dbMatrixTranslation(destination);

	dbMatrix world, view;
	dbDevice::getDevice()->GetTransform(D3DTS_WORLD, (D3DMATRIX*) &world);
	dbDevice::getDevice()->GetTransform(D3DTS_VIEW, (D3DMATRIX*) &view);
	world = transformMatrix;
	d3dx_sprite_->SetWorldViewLH((D3DXMATRIX*)&world, (D3DXMATRIX*)&view);

	// drawing part
	DWORD flags = D3DXSPRITE_BILLBOARD | D3DXSPRITE_OBJECTSPACE | (useAlpha ? D3DXSPRITE_ALPHABLEND : 0);
	bool result = Draw(flags, item, sourceRect, &origCenter, NULL, color);
}

void db2DManager::DrawBillboard( res::TEXTURE_ITEM* item, RECT* sourceRect, core::dbVector3 destination )
{
	DrawBillboard(item, sourceRect, destination, dbColor(1.f), false);
}

bool db2DManager::Draw(DWORD Flags, const res::TEXTURE_ITEM* item, RECT* &sourceRect, dbVector3* center, dbVector3* dest, const dbColor &color)
{
	if (!d3dx_sprite_)
	{
		DBX_ERROR("Could not draw image! Sprite object is not initialized!");
		return false;
	}

	if (!batched_draw_2D_ && !batched_draw_3D_)
	{
		if (FAILED( d3dx_sprite_->Begin(Flags) ))
		{
			DBX_ERROR("Could not begin sprite rendering!");
			return false;
		}
	}

	if (FAILED( d3dx_sprite_->Draw((LPDIRECT3DTEXTURE9) item->Texture, sourceRect, (const D3DXVECTOR3*) center, (const D3DXVECTOR3*) dest, color) ))
	{
		DBX_ERROR("Could not draw sprite!");
		d3dx_sprite_->End();
		return false;
	}

	if (!batched_draw_2D_ && !batched_draw_3D_)
	{
		if (FAILED( d3dx_sprite_->End() ))
		{
			DBX_ERROR("Could not end sprite rendering!");
			return false;
		}
	}

	// reset rotation/scaling
	if (!batched_draw_2D_ && !batched_draw_3D_)
		ResetSettings();
	return true;
}

void db2DManager::ResetSettings()
{
	sprite_rotation_ = 0.f;
	sprite_scaling_ = dbVector3(1.f, 1.f, 1.f);
	d3dx_sprite_->SetTransform((const D3DXMATRIX*) &dbMatrixIdentity());
}


void db2DManager::OnLostDevice()
{
	if (d3dx_sprite_)
		d3dx_sprite_->OnLostDevice();
}

void db2DManager::OnResetDevice()
{
	if (d3dx_sprite_)
		d3dx_sprite_->OnResetDevice();
}

void db2DManager::End()
{
	if (batched_draw_2D_ || batched_draw_3D_)
	{
		if (SUCCEEDED( d3dx_sprite_->End() ))
		{
			batched_draw_2D_ = batched_draw_3D_ = false;
		}
		else
			DBX_ERROR("Could not end sprite rendering!");
	}
}


}}	// namespace