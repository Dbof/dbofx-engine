
namespace dbx
{
namespace particle
{

	class DBX_API dbParticleRenderer
	{
	public:
		dbParticleRenderer() {};
		virtual ~dbParticleRenderer(void) {};

		virtual void Generate(dbPS *sys, bool useQuads) = 0;
		virtual void Destroy() = 0;
		virtual void Update() = 0;
		virtual void Render() = 0;
	};
	
}}	// namespace

