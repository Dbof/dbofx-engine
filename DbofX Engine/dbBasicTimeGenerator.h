#ifndef BASICTIME_GENERATOR_H
#define BASICTIME_GENERATOR_H

namespace dbx
{
	namespace particle
	{
		class DBX_API dbBasicTimeGenerator : public dbParticleGenerator
		{
		private:
			float life_time_millis;
		public:
			explicit dbBasicTimeGenerator(float emitPerSecond);
			~dbBasicTimeGenerator(void);

			// calls all generators and activates particles
			void Generate(double delta, dbParticleData* p, size_t startId, size_t endId) override;
		};

	}}	// namespace  
#endif // !BASICTIME_GENERATOR_H
