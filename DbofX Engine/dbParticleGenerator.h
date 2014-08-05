#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H

namespace dbx
{
	namespace particle
	{
		class DBX_API dbParticleGenerator
		{
		public:
			dbParticleGenerator() {};
			virtual ~dbParticleGenerator() {};

			virtual void Generate(double delta, dbParticleData* p, size_t startId, size_t endId) = 0;
		};
	}}	// namespace  
#endif // !PARTICLE_GENERATOR_H
