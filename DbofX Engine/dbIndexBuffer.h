#ifndef INDEX_BUFFER_H
#define INDEX_BUFFER_H

namespace dbx
{
namespace core
{
	class DBX_API dbIndexBuffer
	{
	public:
		dbIndexBuffer(void);
		~dbIndexBuffer(void);
		
		bool Init(int ibsize, int indexsize, D3DFORMAT d3dformat, DWORD dwUsage = D3DUSAGE_WRITEONLY, D3DPOOL Pool = D3DPOOL_DEFAULT);
		bool Init(LPDIRECT3DINDEXBUFFER9 indexbuffer, int indexsize);
		
		LPDIRECT3DINDEXBUFFER9		GetIB()				{ return index_buffer_; }
		void*						GetBuffer()			{ return back_buffer_; }
		DWORD						GetBufferSize()		{ return index_buffer_size_; }
		DWORD						GetIndexSize()		{ return index_size_; }
		DWORD						GetMaxIndices()		{ return max_indices_; }
		DWORD						GetUsage()			{ return d3d_usage_; }
		D3DPOOL						GetPool()			{ return d3d_pool_; }
		D3DFORMAT					GetFormat()			{ return index_format_; }
		DWORD						GetFirstIndex()		{ return first_index_; }
		DWORD						GetLastIndex()		{ return last_index_; }
		DWORD						GetCursor()			{ return cursor_; }

		int							AddIndex(void* pIndex);
		int							AddIndices(int numIndices, void* pIndex);
		bool						SetCursor(int newpos);
		bool						Update();
		
		static void					OnLostDevice();
		static void					OnResetDevice();
	private:
		LPDIRECT3DINDEXBUFFER9	index_buffer_;
		void*					back_buffer_;
		int						index_buffer_size_;		// in bytes
		int						index_size_;	
		int						index_number_;
		int						max_indices_;
		DWORD					d3d_usage_;
		D3DPOOL					d3d_pool_;
		D3DFORMAT				index_format_;
		int						first_index_;
		int						last_index_;	
		int						cursor_;
		bool					is_init_;

		static std::vector<dbIndexBuffer*> index_buffer_arr_;
		DISALLOW_COPY_AND_ASSIGN(dbIndexBuffer);
	};

}}	// namespace
#endif