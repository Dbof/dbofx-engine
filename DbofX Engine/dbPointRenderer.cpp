#include "DbofX.h"

namespace dbx
{
namespace particle
{
	using namespace core;
	
	DWORD dbPointParticle::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

	dbPointRenderer::dbPointRenderer(void)
	{
	}


	dbPointRenderer::~dbPointRenderer(void)
	{
	}

	
	void dbPointRenderer::Generate(dbPS *sys, bool useQuads)
	{
		assert(sys != nullptr);
		
		system_ = sys;
		const size_t count = sys->GetParticleCount();

		vertex_buffer_.Init(sizeof(dbPointParticle) * count, sizeof(dbPointParticle), dbPointParticle::FVF, D3DUSAGE_WRITEONLY, D3DPOOL_MANAGED);
	}
	void dbPointRenderer::Render() 
	{
		assert(system_ != nullptr);
		if (system_->GetAliveParticleCount() == 0)
			return;

		// update vertex buffer
		UpdateBuffer();

		LPDIRECT3DDEVICE9 d3ddev = dbDevice::getDevice();
		HRESULT hr;
	
		dbDevice::enableLighting(false);
	
		dbDevice::setTextureStageStateAlpha(0, D3DTA_DIFFUSE, D3DTA_TEXTURE, D3DTOP_SELECTARG1);
		hr = d3ddev->SetFVF(dbPointParticle::FVF);
		//hr = d3ddev->SetStreamSource(0, vertex_buffer_.GetVB(), 0, sizeof(dbPointParticle));
		hr = d3ddev->DrawPrimitiveUP(D3DPT_POINTLIST, system_->GetAliveParticleCount(), vertex_buffer_.GetBuffer(), sizeof(dbPointParticle) );
		
		dbDevice::enableLighting(true);
		dbDevice::setTextureStageStateAlpha(0, D3DTA_DIFFUSE, D3DTA_TEXTURE, D3DTOP_SELECTARG2);
	}

	void dbPointRenderer::UpdateBuffer()
	{
		vertex_buffer_.SetCursor(0);
		size_t count = system_->GetAliveParticleCount();
		for (size_t i = 0; i < count; i++)
		{
			dbVector3 position = system_->GetData()->position_[i];
			dbColor color = system_->GetData()->color_[i];

			dbPointParticle pa;
			pa.color = color;
			pa.position = position;

			vertex_buffer_.AddVertex(&pa);
		}
		vertex_buffer_.Update();
	}

	void dbPointRenderer::Update() {}
	void dbPointRenderer::Destroy() {}
}} // namespace