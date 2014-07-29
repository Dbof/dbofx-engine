namespace dbx 
{
namespace particle
{
	class DBX_API dbBasicTimeUpdater : public dbParticleUpdater
	{
	private:
		unsigned long time_millis_;
	public:
		explicit dbBasicTimeUpdater(unsigned long timeMillis);
		~dbBasicTimeUpdater(void) {};

		void Update(double delta, dbParticleData* p) override;
	};
}}	// namespace