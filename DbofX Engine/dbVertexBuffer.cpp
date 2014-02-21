#include "DbofX.h"

using std::string;
using std::vector;

namespace dbx
{
namespace core
{
	using namespace core;

vector<dbVertexBuffer*> dbVertexBuffer::vertex_buffer_arr_;

dbVertexBuffer::dbVertexBuffer(void)
{
	ZeroMemory(this, sizeof(dbVertexBuffer));
}


dbVertexBuffer::~dbVertexBuffer(void)
{
	DB_MEM_RELEASE(vertex_buffer_);
	free(back_buffer_);
}


void dbVertexBuffer::OnLostDevice()
{
	for (int i = 0; i < (int)vertex_buffer_arr_.size(); i++)
	{
		dbVertexBuffer* buf = vertex_buffer_arr_[i];
		if (buf->d3d_pool == D3DPOOL_DEFAULT)
		{
			void *pIndices = NULL;
			if (FAILED( buf->vertex_buffer_->Lock(0, buf->vertex_buffer_size_, &pIndices, (buf->d3d_usage_ & D3DUSAGE_DYNAMIC) ? D3DLOCK_DISCARD : 0)))
				return;

			memcpy(buf->back_buffer_, pIndices, buf->vertex_buffer_size_);
			buf->vertex_buffer_->Unlock();
			DB_MEM_RELEASE(buf->vertex_buffer_);
		}
	}
}

void dbVertexBuffer::OnResetDevice()
{
	for (int i = 0; i < (int)vertex_buffer_arr_.size(); i++)
	{
		dbVertexBuffer* buf = vertex_buffer_arr_[i];
		if (buf->d3d_pool == D3DPOOL_DEFAULT)
		{
			void* tmp_buffer = malloc(buf->vertex_size_ * buf->vertex_number_);
			memcpy(tmp_buffer, buf->back_buffer_, buf->vertex_size_ * buf->vertex_number_);
			int num = buf->vertex_number_;
			buf->SetCursor(0);
			buf->Init(buf->vertex_buffer_size_, buf->vertex_size_, buf->d3d_fvf_, buf->d3d_usage_, buf->d3d_pool);
			buf->AddVertices(num, tmp_buffer);
			buf->Update();

			free(tmp_buffer);
		}
	}
}


bool dbVertexBuffer::Init(int vbsize, int vertexsize, DWORD d3d_fvf, DWORD d3d_usage, D3DPOOL Pool)
{
	if (!dbDevice::isInitialized())
	{
		DBX_ERROR("Could not initialize vertex buffer. Device is not initialized!");
		return false;
	}
	
	if (vbsize <= 0)
	{
		DBX_ERROR("Could not initialize vertex buffer. Size is " + vbsize);
		return false;
	}

	if (vertexsize <= 0)
	{
		DBX_ERROR("Could not initialize vertex buffer. Vertex size is " + vertexsize);
		return false;
	}

	// create temporary pointer and initialise with init(LPDIRECT3DVERTEXBUFFER9)
	LPDIRECT3DVERTEXBUFFER9 tmp_vertex_buffer_;

	HRESULT hr = dbDevice::getDevice()->CreateVertexBuffer(vbsize, d3d_usage_, d3d_fvf_, Pool, &tmp_vertex_buffer_, NULL);
	if FAILED(hr)
	{
		if(d3d_usage_ & D3DUSAGE_DYNAMIC)
		{
			// Warnung ausgeben
			DBX_WARN("Creating dynamic vertexbuffer failed. Trying to create static vertexbuffer.");
			return Init(vbsize, vertexsize, d3d_fvf_, d3d_usage_ ^ D3DUSAGE_DYNAMIC, Pool);
		}
		else
		{
			DBX_ERROR("Could not create vertex buffer!");
			return false;
		}
	}

	
	vertex_buffer_size_ = vbsize;
	vertex_size_ = vertexsize;

	if (!Init(tmp_vertex_buffer_, vertexsize))
	{
		DBX_ERROR("Could not initiate vertex buffer!");
		return false;
	}

	// this decreases the counter if init() succeeds
	tmp_vertex_buffer_->Release();

	vertex_number_ = 0;
	// add to manager
	if (!is_init_)
	{
		vertex_buffer_arr_.push_back(this);
		is_init_ = true;
	}
	return true;
}

bool dbVertexBuffer::Init(LPDIRECT3DVERTEXBUFFER9 vertexbuffer, int vertexsize)
{
	if (vertexbuffer == NULL || vertexsize <= 0)
		return false;
	
	D3DVERTEXBUFFER_DESC Desc;

	// increase ref counter
	vertexbuffer->AddRef();
	// get description of vb
	vertexbuffer->GetDesc(&Desc);

	// copy values
	vertex_buffer_			= vertexbuffer;
	vertex_buffer_size_	= Desc.Size;
	vertex_size_		= vertexsize;
	d3d_fvf_			= Desc.FVF;
	d3d_usage_			= Desc.Usage;
	d3d_pool			= Desc.Pool;

	max_vertices_	= vertex_buffer_size_ / vertex_size_;

	back_buffer_			= malloc(vertex_buffer_size_);
	if (!back_buffer_)
	{
		DBX_ERROR("Could not allocate memory for vertex buffer!");
		return false;
	}

	if (!(Desc.Usage & D3DUSAGE_WRITEONLY))
	{
		BYTE* bData;

		if (SUCCEEDED(vertex_buffer_->Lock(0, 0, (void**) &bData, D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY)))
		{
			memcpy(back_buffer_, bData, vertex_buffer_size_);
			vertex_buffer_->Unlock();
		}
		else
			DBX_WARN("Could not lock vertex buffer!");
	}

	first_vertex_ = max_vertices_ -1;
	last_vertex_ = 0;
	return true;
}

int dbVertexBuffer::AddVertex(void* pVertex)
{
	if (cursor_ >= max_vertices_ || cursor_ < 0 || !pVertex)
		return -1;

	memcpy(((BYTE*)back_buffer_) + vertex_size_*cursor_, pVertex, vertex_size_);

	if (cursor_ < first_vertex_) first_vertex_ = cursor_;
	if (cursor_ > last_vertex_) last_vertex_ = cursor_;

	vertex_number_++;
	return cursor_++;
}

int dbVertexBuffer::AddVertices(int numVertices, void* pVertex)
{
	if (cursor_+numVertices > max_vertices_ || cursor_ < 0)
		return -1;

	memcpy(((BYTE*)back_buffer_) + vertex_size_*cursor_, pVertex, numVertices*vertex_size_);

	if (cursor_ < first_vertex_) first_vertex_ = cursor_;
	if (cursor_ + numVertices -1 > last_vertex_) last_vertex_ = cursor_ + numVertices - 1;

	cursor_ += numVertices;
	vertex_number_ += numVertices;
	return cursor_ - numVertices;
}

bool dbVertexBuffer::SetCursor(int newpos)
{
	cursor_ = newpos;
	return true;
}

bool dbVertexBuffer::Update()
{
	if (first_vertex_ > last_vertex_)	return true;
	if (first_vertex_ >= max_vertices_ || last_vertex_ >= max_vertices_ || !is_init_)
	{
		DBX_INFO("Could not update vertex buffer");
		return false;
	}

	void* pVertices = NULL;
	if (FAILED(vertex_buffer_->Lock(first_vertex_ * vertex_size_, (last_vertex_ - first_vertex_ +1) * vertex_size_, &pVertices, (d3d_usage_ & D3DUSAGE_DYNAMIC) ? D3DLOCK_DISCARD : 0)))
	{
		DBX_ERROR("Could not lock vertex buffer!");
		return false;
	}

	memcpy(pVertices, ((BYTE*)back_buffer_) + first_vertex_ * vertex_size_, (last_vertex_ - first_vertex_ + 1) * vertex_size_);
	vertex_buffer_->Unlock();

	first_vertex_ = max_vertices_ -1;
	last_vertex_ = 0;

	return true;
}

}}	// namespace