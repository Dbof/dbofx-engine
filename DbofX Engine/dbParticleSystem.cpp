#include "DbofX.h"

namespace dbx
{
namespace video
{
	using namespace core;
dbParticleSystem* dbParticleSystem::instance_ = NULL;
DWORD dbParticleVertex::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;

dbParticleSystem::dbParticleSystem(void)
{
}

dbParticleSystem::~dbParticleSystem(void)
{
}

dbParticleSystem* dbParticleSystem::GetInstance()
{
	if (!instance_)
		instance_ = new dbParticleSystem();
	return instance_;
}


void dbParticleSystem::Move(const float fTime)
{
	if (fTime <= 0)
		return;

	dbParticle* particle = db_particle_;

	for (int i = 0; i < num_particles_; i++)
	{
		// reduce lifetime
		particle->lifetime -= fTime;

		if (particle->lifetime <= 0.f)
		{
			if (num_particles_ > 1) *particle = db_particle_[num_particles_-1];
			num_particles_--;

			particle++;
			continue;
		}

		particle->position += particle->velocity * fTime;

		if (particle->friction != 1.f)
		{
			particle->velocity = particle->velocity * powf(particle->friction, fTime);
		}

		particle->size += particle->delta_size * fTime;
		particle->color += particle->delta_color * fTime;
		particle->rotation += particle->delta_rotation * fTime;

		particle++;
	}
	Update();
}

void dbParticleSystem::Render()
{
	if (num_particles_ <= 0)
		return;

	LPDIRECT3DDEVICE9 d3ddev = dbDevice::getDevice();
	HRESULT hr;
	
	dbDevice::enableLighting(false);
	dbDevice::SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	
	dbDevice::setTextureStageStateAlpha(0, D3DTA_DIFFUSE, D3DTA_TEXTURE, D3DTOP_MODULATE);

	// modulate texture with alpha value
	dbDevice::SetRenderState(D3DRS_ALPHABLENDENABLE, true);

	hr = d3ddev->SetFVF(dbParticleVertex::FVF);
	hr = d3ddev->SetStreamSource(0, vertex_buffer_.GetVB(), 0, sizeof(dbParticleVertex));
	hr = d3ddev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, num_particles_*2);
	
	// reset settings
	dbDevice::enableLighting(true);

	dbDevice::resetTextureStageState(0);
}

void dbParticleSystem::Init(int maxParticle)
{
	if (maxParticle <= 0)
		return;

	db_particle_ = new dbParticle[maxParticle];
	ZeroMemory(db_particle_, maxParticle * sizeof(dbParticle));
	vertex_buffer_.Init(sizeof(dbParticleVertex) * 6 * maxParticle, sizeof(dbParticleVertex), dbParticleVertex::FVF, D3DUSAGE_WRITEONLY, D3DPOOL_MANAGED);

	max_particles_ = maxParticle;
	num_particles_ = 0;
}

bool dbParticleSystem::AddParticle(const float& lifetime, const core::dbVector3& position, const core::dbVector3& velocity, const float& friction,
								   const float& startsize, const float& endsize, const core::dbColor& startcolor, 
								   const core::dbColor& endcolor, const float& startrotation, const float& endrotation)
{
	dbParticle* particle;
	
	if (num_particles_ >= max_particles_) return false;
	if (lifetime <= 0.f)				return false;

	particle = &db_particle_[num_particles_];

	// color
	particle->color = startcolor;
	particle->delta_color = (endcolor - startcolor) / lifetime;
	// rotation
	particle->rotation = startrotation;
	particle->delta_rotation = (endrotation - startrotation) / lifetime;
	// size
	particle->size = startsize;
	particle->delta_size = (endsize - startsize) / lifetime;
	// pos
	particle->velocity = velocity;
	particle->position = position;
	particle->friction = friction;
	particle->lifetime = lifetime;

	num_particles_++;
	return true;
}

bool dbParticleSystem::AddParticle( const float& lifetime, const core::dbVector3& position, const core::dbVector3& velocity, const float& friction )
{
	return AddParticle(lifetime, position, velocity, friction, 1.f, 1.f, dbColor(1.f), dbColor(1.f), 0.f, 0.f);
}

bool dbParticleSystem::AddParticle( const float& lifetime, const core::dbVector3& position, const core::dbVector3& velocity, const float& friction, const float& startsize, const float& endsize )
{
	return AddParticle(lifetime, position, velocity, friction, startsize, endsize, dbColor(1.f), dbColor(1.f), 0.f, 0.f);
}

bool dbParticleSystem::AddParticle( const float& lifetime, const core::dbVector3& position, const core::dbVector3& velocity, const float& friction,
								   const float& startsize, const float& endsize, 
								   const core::dbColor& startcolor, const core::dbColor& endcolor )
{
	return AddParticle(lifetime, position, velocity, friction, startsize, endsize, startcolor, endcolor, 0.f, 0.f);
}

void dbParticleSystem::Update()
{
	dbParticle* particle = db_particle_;
	vertex_buffer_.SetCursor(0);

	for (int i = 0; i < num_particles_; i++)
	{
		// add to vertex buffer
		dbParticleVertex vertex[4];

		// test
		dbMatrix mView;
		dbDevice::getDevice()->GetTransform(D3DTS_VIEW, (D3DMATRIX*) &mView);
		dbVector3 m_vCamXAxis, m_vCamYAxis, m_vCamZAxis;

		m_vCamXAxis = dbVector3Norm(dbVector3(mView._11, mView._21, mView._31));
		m_vCamYAxis = dbVector3Norm(dbVector3(mView._12, mView._22, mView._32));
		m_vCamZAxis = dbVector3Norm(dbVector3(mView._13, mView._23, mView._33));

		dbVector3 vSizeX = (m_vCamXAxis * cosf(-particle->rotation) + m_vCamYAxis * sinf(particle->rotation)) * particle->size * 0.5f;	//size.x
		dbVector3 vSizeY = (m_vCamXAxis * sinf(-particle->rotation) + m_vCamYAxis * cosf(-particle->rotation)) * particle->size * 0.5f;

		vertex[0].position = particle->position - vSizeX + vSizeY;	// Links oben
		vertex[1].position = particle->position + vSizeX + vSizeY;	// Rechts oben
		vertex[2].position = particle->position + vSizeX - vSizeY;	// Rechts unten
		vertex[3].position = particle->position - vSizeX - vSizeY;	// Links unten
		/*
		vertex[0].position = particle->position + dbVector3(-particle->size /2, +particle->size /2, 0);
		vertex[1].position = particle->position + dbVector3(+particle->size /2, +particle->size /2, 0);
		vertex[2].position = particle->position + dbVector3(+particle->size /2, -particle->size /2, 0);
		vertex[3].position = particle->position + dbVector3(-particle->size /2, -particle->size /2, 0);
		*/
		vertex[0].color = vertex[1].color = vertex[2].color = vertex[3].color = particle->color;
		vertex[0].u = 0.f; vertex[0].v = 0.f;
		vertex[1].u = 1.f; vertex[1].v = 0.f;
		vertex[2].u = 1.f; vertex[2].v = 1.f;
		vertex[3].u = 0.f; vertex[3].v = 1.f;

		vertex_buffer_.AddVertex(&vertex[0]);
		vertex_buffer_.AddVertex(&vertex[1]);
		vertex_buffer_.AddVertex(&vertex[2]);
		vertex_buffer_.AddVertex(&vertex[0]);
		vertex_buffer_.AddVertex(&vertex[2]);
		vertex_buffer_.AddVertex(&vertex[3]);

		particle++;
	}
	
	vertex_buffer_.Update();
}
void dbParticleSystem::Clear()
{
	ZeroMemory(db_particle_, sizeof(dbParticle) * max_particles_);
	num_particles_ = 0;
}


void dbParticleSystem::Exit()
{
	DB_MEM_DELETE(instance_);
}

}}	// namespace