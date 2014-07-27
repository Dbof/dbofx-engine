#ifndef PS_H
#define PS_H

namespace dbx
{
namespace particle
{
	class dbParticleData
	{
	public:
		size_t _count;
		size_t _countAlive;
	
		std::unique_ptr<core::dbVector3[]>	_position;
		std::unique_ptr<core::dbColor[]>	_color;
		std::unique_ptr<core::dbVector3[]>	_startColor;
		std::unique_ptr<core::dbVector3[]>	_endColor;
		std::unique_ptr<core::dbVector3[]>	_velocity;
		std::unique_ptr<core::dbVector3[]>	_acceleration;
		std::unique_ptr<core::dbVector3[]>	_time;
		std::unique_ptr<bool[]>				_alive;
	
	public:
		dbParticleData() : _count(0), _countAlive(0) {};
		explicit dbParticleData(size_t maxCount) { Generate(maxCount); };
		~dbParticleData() {};

		//dbParticleData(const dbParticleData &) = delete;
		//dbParticleData &operator= (const dbParticleData &) = delete;
		
		size_t GetCount() const { return _count; };
		size_t GetCountAlive() const { return _countAlive; };
		
		void Generate(size_t maxSize);
		void Kill(size_t id);
		void Wake(size_t id);
		void SwapData(size_t a, size_t b);
	};

	class dbParticleGenerator
	{
	public:
		dbParticleGenerator() {};
		virtual ~dbParticleGenerator() {};

		virtual void Generate(double delta, dbParticleData* p, size_t startId, size_t endId) = 0;
	};

	class dbParticleEmitter
	{
	protected:
		std::vector<std::shared_ptr<dbParticleGenerator>> _generators;
	private:
		float _emitRate;
	public:
		dbParticleEmitter() : _emitRate(0.f) {};
		~dbParticleEmitter() {};

		// calls all generators and activates particles
		virtual void Emit(double delta, dbParticleData* p);
		void AddGenerator(std::shared_ptr<dbParticleGenerator> generator) { _generators.push_back(generator); };
	};

	class dbParticleUpdater
	{
	public:
		dbParticleUpdater() {};
		virtual ~dbParticleUpdater() {};

		virtual void Update(double delta, dbParticleData* p) = 0;
	};


	class dbPS
	{
	protected:
		dbParticleData _particles;
		dbParticleData _aliveParticles;

		size_t _count;

		std::vector<std::shared_ptr<dbParticleEmitter>> _emitters;
		std::vector<std::shared_ptr<dbParticleUpdater>> _updaters;
	public:
		explicit dbPS(size_t maxCount);
		virtual ~dbPS() {};

		//dbPS(const dbPS &) = delete;
		//dbPS &operator=(const dbPS &) = delete;

		virtual void Update(double delta);
		virtual void Reset();

		virtual size_t GetParticleCount() const { return _particles.GetCount(); };
		virtual size_t GetAliveParticleCount() const { return _particles.GetCountAlive(); };
		
		void AddEmitter(std::shared_ptr<dbParticleEmitter> em) { _emitters.push_back(em); }
		void AddUpdater(std::shared_ptr<dbParticleUpdater> up) { _updaters.push_back(up); }

		dbParticleData* GetData() { return &_particles; }
	};

}}	// namespace

#endif