

namespace dbx
{
namespace particle
{
	struct DBX_API dbPointParticle
	{
		core::dbVector3	position;
		D3DCOLOR		color;
		static DWORD FVF;
	};

	class DBX_API dbPointRenderer : public dbParticleRenderer
	{
	private:
		dbPS* system_;
		core::dbVertexBuffer vertex_buffer_;

		void UpdateBuffer();
		
	public:
		dbPointRenderer(void);
		~dbPointRenderer(void);

		void Generate(dbPS *sys, bool useQuads) override;
		void Render() override;
		void Update() override;
		void Destroy() override;
	};

}}	// namespace

