#ifndef PARTICLE_UPDATER_H
#define PARTICLE_UPDATER_H

namespace dbx 
{
	namespace particle
	{
		class DBX_API dbParticleUpdater
		{
		public:
			dbParticleUpdater() {};
			virtual ~dbParticleUpdater() {};

			virtual void Update(double delta, dbParticleData* p) = 0;
		};

	}}	// namespace 
#endif // !PARTICLE_UPDATER_H
