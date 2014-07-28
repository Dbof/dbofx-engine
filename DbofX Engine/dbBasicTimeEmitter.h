
namespace dbx
{
namespace particle
{

	class DBX_API dbBasicTimeEmitter : public dbParticleEmitter
	{
	public:
		explicit dbBasicTimeEmitter(float emitPerSecond);
		~dbBasicTimeEmitter(void);
		
		// calls all generators and activates particles
		void Emit(double delta, dbParticleData* p) override;
	};

}}	// namespace