#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

namespace dbx
{
namespace core
{
	class DBX_API dbVertexBuffer
	{
	public:
		dbVertexBuffer(void);
		~dbVertexBuffer(void);

		bool						Init(int vbsize, int vertexsize, DWORD d3d_fvf, DWORD d3d_usage = D3DUSAGE_WRITEONLY, D3DPOOL Pool = D3DPOOL_DEFAULT);
		bool						Init(LPDIRECT3DVERTEXBUFFER9 vertexbuffer, int vertexsize);

		// getter methods
		LPDIRECT3DVERTEXBUFFER9		GetVB()				{ return vertex_buffer_; }
		void*						GetBuffer()			{ return back_buffer_; }
		DWORD						GetBufferSize()		{ return vertex_buffer_size_; }
		DWORD						GetVertexSize()		{ return vertex_size_; }
		DWORD						GetMaxVertices()	{ return max_vertices_; }
		DWORD						GetFVF()			{ return d3d_fvf_; }
		DWORD						GetUsage()			{ return d3d_usage_; }
		D3DPOOL						GetPool()			{ return d3d_pool; }
		DWORD						GetFirstVertex()	{ return first_vertex_; }
		DWORD						GetLastVertex()		{ return last_vertex_; }
		DWORD						GetCursor()			{ return cursor_; }

		int							AddVertex(void* pVertex);
		int							AddVertices(int numVertices, void* pVertex);
		bool						SetCursor(int newpos);
		bool						Update();
		
		static void					OnLostDevice();
		static void					OnResetDevice();
	private:
		LPDIRECT3DVERTEXBUFFER9 vertex_buffer_;
		void*					back_buffer_;
		int						vertex_buffer_size_;		// in bytes
		int						vertex_size_;			
		int						vertex_number_;
		int						max_vertices_;
		DWORD					d3d_fvf_;
		DWORD					d3d_usage_;
		D3DPOOL					d3d_pool;
		int						first_vertex_;
		int						last_vertex_;	
		int						cursor_;				// current position
		bool					is_init_;

		static std::vector<dbVertexBuffer*> vertex_buffer_arr_;
		DISALLOW_COPY_AND_ASSIGN(dbVertexBuffer);
	};

}}	// namespace
#endif