#include "DbofX.h"

namespace dbx
{
namespace core
{

std::vector<dbIndexBuffer*> dbIndexBuffer::index_buffer_arr_;

dbIndexBuffer::dbIndexBuffer(void)
{
	ZeroMemory(this, sizeof(dbIndexBuffer));
}

dbIndexBuffer::~dbIndexBuffer(void)
{
	DB_MEM_RELEASE(index_buffer_);
	free(back_buffer_);
}


void dbIndexBuffer::OnLostDevice()
{
	for (int i = 0; i < (int)index_buffer_arr_.size(); i++)
	{
		dbIndexBuffer* buf = index_buffer_arr_[i];
		if (buf->d3d_pool_ == D3DPOOL_DEFAULT)
			DB_MEM_RELEASE(buf->index_buffer_);
	}
}

void dbIndexBuffer::OnResetDevice()
{
	for (int i = 0; i < (int)index_buffer_arr_.size(); i++)
	{
		dbIndexBuffer* buf = index_buffer_arr_[i];
		if (buf->d3d_pool_ == D3DPOOL_DEFAULT)
		{
			void* tmp_buffer = malloc(buf->index_size_ * buf->index_number_);
			memcpy(tmp_buffer, buf->back_buffer_, buf->index_size_ * buf->index_number_);
			int num = buf->index_number_;
			buf->SetCursor(0);
			buf->Init(buf->index_buffer_size_, buf->index_size_, buf->index_format_, buf->d3d_usage_, buf->d3d_pool_);
			buf->AddIndices(num, tmp_buffer);
			buf->Update();

			free(tmp_buffer);
		}
	}
}

bool dbIndexBuffer::Init(int ibsize, int indexsize, D3DFORMAT d3dformat, DWORD d3d_usage_, D3DPOOL Pool)
{
	if (!dbDevice::isInitialized())
		return false;

	if (ibsize <= 0 || indexsize <= 0)
	{
		DBX_ERROR("Could not initialize index buffer. Size is " + toString(ibsize) + " and index size is " + toString(indexsize));
		return false;
	}
	
	// create temporary pointer and initialise with init(LPDIRECT3DINDEXBUFFER9)
	LPDIRECT3DINDEXBUFFER9 tmp_index_buffer_;
	
	d3d_usage_ |= D3DUSAGE_WRITEONLY;
	HRESULT hr = dbDevice::getDevice()->CreateIndexBuffer(ibsize, d3d_usage_, d3dformat, Pool, &tmp_index_buffer_, NULL);
	if FAILED(hr)
	{
		if(d3d_usage_ & D3DUSAGE_DYNAMIC)
		{
			DBX_WARN("Creating dynamic indexbuffer failed. Trying to create static indexbuffer.");
			return Init(ibsize, indexsize, d3dformat, d3d_usage_ ^ D3DUSAGE_DYNAMIC, Pool);
		}
		else
		{
			DBX_ERROR("Could not create index buffer!");
			return false;
		}
	}

	if (!Init(tmp_index_buffer_, indexsize))
	{
		DBX_ERROR("Could not initiate index buffer!");
		return false;
	}

	// this decreases the counter if init() succeeds
	tmp_index_buffer_->Release();

	index_number_ = 0;
	// add to manager
	if (!is_init_)
	{
		index_buffer_arr_.push_back(this);
		is_init_ = true;
	}
	return true;
}

bool dbIndexBuffer::Init(LPDIRECT3DINDEXBUFFER9 indexbuffer, int indexsize)
{
	if (!indexbuffer || indexsize <= 0)
		return false;

	D3DINDEXBUFFER_DESC Desc;

	// increase ref counter
	indexbuffer->AddRef();
	// get description of vb
	indexbuffer->GetDesc(&Desc);

	// copy values
	index_buffer_		= indexbuffer;
	index_buffer_size_	= Desc.Size;
	index_size_			= indexsize;
	index_format_		= Desc.Format;
	d3d_usage_			= Desc.Usage;
	d3d_pool_			= Desc.Pool;

	
	max_indices_	= index_buffer_size_ / index_size_;

	back_buffer_			= malloc(index_buffer_size_);
	if (!back_buffer_)
	{
		DBX_ERROR("Could not allocate memory for vertex buffer!");
		return false;
	}

	if (!(Desc.Usage & D3DUSAGE_WRITEONLY))
	{
		BYTE* bData;

		if (SUCCEEDED(index_buffer_->Lock(0, 0, (void**) &bData, D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY)))
		{
			memcpy(back_buffer_, bData, index_buffer_size_);
			index_buffer_->Unlock();
		}
		else
			DBX_WARN("Could not lock vertex buffer!");
	}

	first_index_ = max_indices_ -1;
	last_index_ = 0;
	return true;
}

int	dbIndexBuffer::AddIndex(void* pIndex)
{
	if (cursor_ >= max_indices_ || cursor_ < 0 || !pIndex)
		return -1;

	memcpy(((BYTE*)back_buffer_) + index_size_*cursor_, pIndex, index_size_);

	if (cursor_ < first_index_) first_index_ = cursor_;
	if (cursor_ > last_index_) last_index_ = cursor_;

	index_number_++;
	return cursor_++;
}

int	dbIndexBuffer::AddIndices(int numIndices, void* pIndex)
{
	if (cursor_ + numIndices > max_indices_ || cursor_ < 0 || !pIndex)
		return -1;

	memcpy(((BYTE*)back_buffer_) + index_size_*cursor_, pIndex, index_size_ * numIndices);
	
	if (cursor_ < first_index_) first_index_ = cursor_;
	if (cursor_+numIndices -1 > last_index_) last_index_ = cursor_+numIndices -1;
	
	cursor_ += numIndices;
	index_number_ += numIndices;
	return cursor_ - numIndices;
}

bool dbIndexBuffer::SetCursor(int newpos)
{
	cursor_ = newpos;
	return true;
}

bool dbIndexBuffer::Update()
{
	if (first_index_ > last_index_)	return true;
	if (first_index_ >= max_indices_ || last_index_ >= max_indices_ || !is_init_)
	{
		DBX_INFO("Could not update index buffer");
		return false;
	}

	void* pIndices = NULL;
	if (FAILED(index_buffer_->Lock(first_index_ * index_size_, (last_index_ - first_index_ +1) * index_size_, &pIndices, (d3d_usage_ & D3DUSAGE_DYNAMIC) ? D3DLOCK_DISCARD : 0)))
	{
		DBX_ERROR("Could not lock index buffer!");
		return false;
	}

	memcpy(pIndices, ((BYTE*)back_buffer_) + first_index_ * index_size_, (last_index_ - first_index_ + 1) * index_size_);
	index_buffer_->Unlock();

	first_index_ = max_indices_ -1;
	last_index_ = 0;

	return true;
}


}}	// namespace