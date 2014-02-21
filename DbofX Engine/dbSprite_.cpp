#include "DbofX.h"

namespace dbx
{
namespace scene
{

bool dbSprite::isInit;	// ID3DXSPRITE initiated
bool dbSprite::isSetup;	// setup() was called
LPD3DXSPRITE dbSprite::sprite;		// you only need one instance to draw x sprites
dbVector3 dbSprite::center;

dbSprite::dbSprite()
{
}

dbSprite::~dbSprite()
{
	DB_MEM_RELEASE(sprite);
}

//**********************************************************************

bool dbSprite::init()
{
	if (!sprite)
	{
		if (FAILED( D3DXCreateSprite(dbDevice::getDevice(), &sprite) ))
		{
			DBX_ERROR("Sprite could not be created!");
			return false;
		}
	}
	isInit = isSetup = true;
	return true;
}

//**********************************************************************

void dbSprite::setPosition(dbVector3 position, float rotation, dbVector3 scaling)
{
	if (!isInit)
		init();
	
	float width = (float) (curr_texture.Width * scaling.x / 2);
	float height = (float) (curr_texture.Heigth * scaling.y / 2);
	
	center = dbVector3(position.x + width, position.y + height, 0);
	//center = dbVector3(0, 0, 0);
	sprite->SetTransform((const D3DXMATRIX*) &(dbMatrixScaling(scaling) * dbMatrix2DRotation(rotation) * dbMatrixTranslation(position)));

	/*
	position -= rcenter;
	
	dbMatrix rotationMatrix = dbMatrixTranslation(-1 * rcenter) * dbMatrix2DRotation(rotation) * dbMatrixTranslation( rcenter); 
	dbMatrix transformMatrix = dbMatrixScaling(scaling) * rotationMatrix * dbMatrixTranslation(position);
	
	dbDevice::getDevice()->SetTransform(D3DTS_WORLD, (const D3DXMATRIX*) &transformMatrix);

	D3DXMATRIX world, view;
	dbDevice::getDevice()->GetTransform(D3DTS_VIEW, &view);
	dbDevice::getDevice()->GetTransform(D3DTS_WORLD, &world);
	//sprite->SetWorldViewLH(&world, &view);

	*/

	isSetup = true;
}

void dbSprite::setPosition(dbMatrix transMatrix)
{
	sprite->SetTransform( (const D3DXMATRIX*) &transMatrix);
	isSetup = true;
}

/*
void dbSprite::setup(TEXTURE_ITEM& texture, dbVector3 position, float rotation, dbVector3 scaling)
{
	if (!isInit)
		init();

	position.z = scaling.z = 0.f;
	curr_texture = texture.Texture;
	
	float width = (float) (texture.Width * scaling.x / 2);
	float height = (float) (texture.Heigth * scaling.y / 2);

	dbMatrix rotationMatrix = dbMatrixTranslation(dbVector3(-width, - height, 0.f)) * dbMatrix2DRotation(rotation) * dbMatrixTranslation(dbVector3(width, height, 0.f));
	dbMatrix transformMatrix = dbMatrixScaling(scaling) * rotationMatrix * dbMatrixTranslation(position) ;
	
	sprite->SetTransform( (const D3DXMATRIX*) &transformMatrix);
	isSetup = true;
}*/
/*
void setup(dbMatrix transMatrix)
{
	if (!isInit)
		init();

	dbMatrix transformMatrix = transMatrix;
	sprite->SetTransform( (const D3DXMATRIX*) &transformMatrix);
	isSetup = true;
}*/

//**********************************************************************

bool dbSprite::render(RECT* sourceRect, dbColor color)
{
	if (!isSetup)
	{
		DBX_ERROR("Sprite is not set up for rendering!");
		return false;
	}

	if (FAILED( sprite->Begin(D3DXSPRITE_OBJECTSPACE | D3DXSPRITE_ALPHABLEND) ))
	{
		DBX_ERROR("Could not begin sprite rendering!");
		return false;
	}

	if (FAILED( sprite->Draw((LPDIRECT3DTEXTURE9) curr_texture.Texture, sourceRect, (const D3DXVECTOR3*) &center, (const D3DXVECTOR3*) &dbVector3(0, 0, 0), color) ))
	{
		DBX_ERROR("Could not draw sprite!");
	}

	if (FAILED( sprite->End() ))
	{
		DBX_ERROR("Could not end sprite rendering!");
	}
	return true;
}

//**********************************************************************

}}