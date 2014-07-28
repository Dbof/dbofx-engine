
namespace dbx
{
namespace particle
{
	struct DBX_API dbTextureParticle
	{
		core::dbVector3	position;
		D3DCOLOR		color;
		float			u, v;
		static DWORD FVF;
	};

	class DBX_API dbTextureRenderer : public dbParticleRenderer
	{
	private:
		dbPS* system_;
		core::dbVertexBuffer vertex_buffer_;

		void UpdateBuffer();
	public:
		dbTextureRenderer(void) {};
		~dbTextureRenderer(void) {};
		
		void Generate(dbPS *sys, bool useQuads) override;
		void Render() override;
		void Update() override;
		void Destroy() override;
	};

}}	// namespace