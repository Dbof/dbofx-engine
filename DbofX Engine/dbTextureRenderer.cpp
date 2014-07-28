#include "DbofX.h"

namespace dbx
{
namespace particle
{
	using namespace core;
	
	DWORD dbTextureParticle::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
	void dbTextureRenderer::Generate(dbPS *sys, bool useQuads)
	{
		assert(sys != nullptr);
		
		system_ = sys;
		const size_t count = sys->GetParticleCount();

		vertex_buffer_.Init(sizeof(dbTextureParticle) * 6 * count, sizeof(dbTextureParticle), dbTextureParticle::FVF, D3DUSAGE_WRITEONLY, D3DPOOL_MANAGED);
	}

	void dbTextureRenderer::Update()
	{
	}

	void dbTextureRenderer::Destroy()
	{
	}

	void dbTextureRenderer::Render()
	{
		assert(system_ != nullptr);
		if (system_->GetAliveParticleCount() == 0)
			return;

		// update vertex buffer
		UpdateBuffer();

		LPDIRECT3DDEVICE9 d3ddev = dbDevice::getDevice();
		HRESULT hr;
	
		dbDevice::enableLighting(false);
		dbDevice::SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	
		dbDevice::setTextureStageStateAlpha(0, D3DTA_DIFFUSE, D3DTA_TEXTURE, D3DTOP_MODULATE);

		// modulate texture with alpha value
		dbDevice::SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	
		hr = d3ddev->SetFVF(dbTextureParticle::FVF);
		hr = d3ddev->SetStreamSource(0, vertex_buffer_.GetVB(), 0, sizeof(dbTextureParticle));
		hr = d3ddev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, system_->GetAliveParticleCount());
		
		dbDevice::SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		dbDevice::enableLighting(true);
		dbDevice::resetTextureStageState(0);
	}

	void dbTextureRenderer::UpdateBuffer()
	{
		vertex_buffer_.SetCursor(0);
		size_t count = system_->GetAliveParticleCount();
		for (size_t i = 0; i < count; i++)
		{
			// add to vertex buffer
			dbTextureParticle vertex[4];
			
			float size = 1.f;
			dbVector3 position = system_->GetData()->position_[i];
			dbColor color = system_->GetData()->color_[i];
			
			dbMatrix mView;
			dbDevice::getDevice()->GetTransform(D3DTS_VIEW, (D3DMATRIX*) &mView);
			dbVector3 m_vCamXAxis, m_vCamYAxis, m_vCamZAxis;

			m_vCamXAxis = dbVector3Norm(dbVector3(mView._11, mView._21, mView._31));
			m_vCamYAxis = dbVector3Norm(dbVector3(mView._12, mView._22, mView._32));
			m_vCamZAxis = dbVector3Norm(dbVector3(mView._13, mView._23, mView._33));
			dbVector3 vSizeX = (m_vCamXAxis * 1 + m_vCamYAxis * 0) * size * 0.5f;	//size.x
			dbVector3 vSizeY = (m_vCamXAxis * 0 + m_vCamYAxis * 1) * size * 0.5f;

			vertex[0].position = position - vSizeX + vSizeY;	// Links oben
			vertex[1].position = position + vSizeX + vSizeY;	// Rechts oben
			vertex[2].position = position + vSizeX - vSizeY;	// Rechts unten
			vertex[3].position = position - vSizeX - vSizeY;	// Links unten
			
			vertex[0].color = vertex[1].color = vertex[2].color = vertex[3].color = color;
			
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
		}
	
		vertex_buffer_.Update();
	}

}}	// namespace