#ifndef PARTICLE_EMITTER_H
#define PARTICLE_EMITTER_H

namespace dbx
{
	namespace particle
	{
		class DBX_API dbParticleEmitter
		{
		protected:
			std::vector<std::shared_ptr<dbParticleGenerator>> generators_;
			float emit_rate_;
		public:
			explicit dbParticleEmitter(float emitrate) : emit_rate_(emitrate) {};
			~dbParticleEmitter() {};

			// calls all generators and activates particles
			virtual void Emit(double delta, dbParticleData* p);
			void AddGenerator(std::shared_ptr<dbParticleGenerator> generator) { generators_.push_back(generator); };
		};
	}}	// namespace  
#endif // !PARTICLE_EMITTER_H
