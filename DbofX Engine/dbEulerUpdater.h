
namespace dbx
{
namespace particle
{
	class DBX_API dbEulerUpdater : public dbParticleUpdater
	{
	private:
		core::dbVector3 global_acceleration_;
	public:
		dbEulerUpdater();
		virtual void Update(double dt, dbParticleData *p) override;
	};
}}	// namespace