#ifndef BASICSPEED_GENERATOR_H
#define BASICSPEED_GENERATOR_H

namespace dbx
{
	namespace particle
	{
		class DBX_API dbBasicSpeedGenerator : public dbParticleGenerator
		{
		private:
			core::dbVector3 global_vel_;
		public:
			explicit dbBasicSpeedGenerator(core::dbVector3& velocity);
			~dbBasicSpeedGenerator(void);

			void SetSpeed(core::dbVector3& velocity);
			void Generate(double delta, dbParticleData* p, size_t startId, size_t endId) override;
		};

	}}	// namespace  
#endif // !1
