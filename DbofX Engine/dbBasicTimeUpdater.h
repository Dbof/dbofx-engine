#ifndef BASICTIME_UPDATER_H
#define BASICTIME_UPDATER_H


namespace dbx 
{
	namespace particle
	{
		class DBX_API dbBasicTimeUpdater : public dbParticleUpdater
		{
		public:
			dbBasicTimeUpdater();
			~dbBasicTimeUpdater(void) {};

			void Update(double delta, dbParticleData* p) override;
		};
	}}	// namespace  
#endif // !BASICTIME_UPDATER_H
