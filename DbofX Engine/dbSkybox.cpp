#include "DbofX.h"

using std::string;

namespace dbx
{
namespace scene
{
	using namespace core;

dbSkybox::dbSkybox(void)
{
	index_arr_ = 0;
	vertex_buffer_ = 0;
}

dbSkybox::~dbSkybox(void)
{
	DB_MEM_RELEASE(vertex_buffer_);
	DB_MEM_RELEASE(index_buffer_);
}

//**********************************************************************
// METHODS
bool dbSkybox::Init(const string& fname)
{
	sky_size_ = 10.f;

	if (!InitCube())
		return false;

	this->sky_size_	= sky_size_;
	texture_name_	= fname;
	texture_item_	= res::dbTextureManager::GetInstance()->AddTexture(res::TEX_CUBE, fname, false, false, dbColor(0.f));	//load texture
	if (texture_item_ == NULL)
		return false;
	return true;
}

bool dbSkybox::InitCube()
{
	/*    V2  _______ V3
	 *      /|      /|
	 *  V1 /_|_____/ V4
	 *     | |  .  | |
	 *    V5 |_____|_| V6
	 *     | /     | /
	 *  V8 |/______|/ V7
	 */

	SKYBOX_VERTEX vertices[8] = 
	{
		{ dbVector3(-1.f,  1.f,  1.f)},
		{ dbVector3(-1.f,  1.f, -1.f)},
		{ dbVector3( 1.f,  1.f, -1.f)},
		{ dbVector3( 1.f,  1.f,  1.f)},
		{ dbVector3(-1.f, -1.f, -1.f)},
		{ dbVector3( 1.f, -1.f, -1.f)},
		{ dbVector3( 1.f, -1.f,  1.f)},
		{ dbVector3(-1.f, -1.f,  1.f)}
	};

	for (int i = 0; i < 8; i++)
	{
		vertices[i].normal = vertices[i].xyz;
		vertices[i].xyz = vertices[i].xyz * sky_size_;	// using standard value
		//vertices[i].x *= 15.f;
		//vertices[i].y *= 15.f;
		//vertices[i].z *= 15.f;
	}

	if (FAILED( dbDevice::getDevice()->CreateVertexBuffer(	sizeof(SKYBOX_VERTEX) * 8, 
												D3DUSAGE_WRITEONLY, 
												D3DFVF_CUSTOMVERTEX, 
												D3DPOOL_MANAGED,
												&vertex_buffer_, NULL)))
	return false;

	void* pVertex = NULL;
	if (FAILED( vertex_buffer_->Lock(0, sizeof(vertices), (void**)&pVertex, 0)))
		return false;

	memcpy(pVertex, vertices, sizeof(vertices));
	vertex_buffer_->Unlock();

	if (FAILED( dbDevice::getDevice()->CreateIndexBuffer(	sizeof(short)*36,
															D3DUSAGE_WRITEONLY,
															D3DFMT_INDEX16,
															D3DPOOL_MANAGED,
															&index_buffer_, NULL)))
		return false;

	short Indices[] =
    {
		0, 2, 1,	0, 3, 2,
		7, 5, 4,	7, 6, 5,
		0, 4, 7,	0, 1, 4,
		3, 5, 6,	3, 2, 5,
		0, 6, 3,	0, 7, 6,
		2, 4, 5,	2, 1, 4
    };
	
	/*    V2  _______ V3
	 *      /|      /|
	 *  V1 /_|_____/ V4
	 *     | |  .  | |
	 *    V5 |_____|_| V6
	 *     | /     | /
	 *  V8 |/______|/ V7
	 */
	
	void* index = NULL;
	if (FAILED( index_buffer_->Lock(0, 0, &index, 0)))
		return false;

	memcpy(index, (void*) &Indices, sizeof(Indices));
	index_buffer_->Unlock();
	
	return true;
}

//**********************************************************************

void dbSkybox::Render(const dbVector3& position)
{
	if (dbDevice::canRender() != D3D_OK)
		return;
	LPDIRECT3DDEVICE9 dev = dbDevice::getDevice();

	dbDevice::SetRenderState(D3DRS_ZENABLE, false);
	dbDevice::SetRenderState( D3DRS_LIGHTING, false );

	dev->SetStreamSource(0, vertex_buffer_, 0, sizeof(SKYBOX_VERTEX));
	dev->SetIndices(index_buffer_);
	dev->SetFVF(D3DFVF_CUSTOMVERTEX);
	dbDevice::setTexture(texture_item_);

	dbDevice::setTransform(dbMatrixTranslation(position));
	
	dbDevice::SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
	dbDevice::SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

	dbDevice::SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	dbDevice::SetRenderState(D3DRS_ZENABLE, true);
	dbDevice::SetRenderState( D3DRS_LIGHTING, true );
}

void dbSkybox::OnLostDevice()
{
	DB_MEM_RELEASE(vertex_buffer_);
	DB_MEM_RELEASE(index_buffer_);
}
void dbSkybox::OnResetDevice()
{
	InitCube();
}

}}	// namespace