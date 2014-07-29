
namespace dbx
{
namespace particle
{
	class DBX_API dbBasicPositionGenerator : public dbParticleGenerator
	{
	private:
		core::dbVector3 position_;
	public:
		explicit dbBasicPositionGenerator(const core::dbVector3& pos) : position_(pos) {}
		~dbBasicPositionGenerator(void);
		
		void SetPosition(const core::dbVector3& pos) { position_ = pos; }

		// calls all generators and activates particles
		void Generate(double delta, dbParticleData* p, size_t startId, size_t endId) override;
	};

}}	// namespace