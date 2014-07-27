
namespace dbx
{
namespace particle
{

	class dbParticleRenderer
	{
	public:
		dbParticleRenderer() {};
		virtual ~dbParticleRenderer(void);

		virtual void generate(dbPS *sys, bool useQuads) = 0;
		virtual void destroy() = 0;
		virtual void update() = 0;
		virtual void render() = 0;
	};
	
}}	// namespace

