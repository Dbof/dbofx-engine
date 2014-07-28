#ifndef PS_H
#define PS_H

namespace dbx
{
namespace particle
{
	class DBX_API dbParticleData
	{
	public:
		size_t count_;
		size_t count_alive_;
	
		std::unique_ptr<core::dbVector3[]>	position_;
		std::unique_ptr<core::dbColor[]>	color_;
		std::unique_ptr<core::dbVector3[]>	startColor_;
		std::unique_ptr<core::dbVector3[]>	endColor_;
		std::unique_ptr<core::dbVector3[]>	velocity_;
		std::unique_ptr<core::dbVector3[]>	acceleration_;
		std::unique_ptr<core::dbVector3[]>	time_;
		std::unique_ptr<bool[]>				alive_;
	
	public:
		dbParticleData() : count_(0), count_alive_(0) {};
		explicit dbParticleData(size_t maxCount) { Generate(maxCount); };
		~dbParticleData() {};

		//dbParticleData(const dbParticleData &) = delete;
		//dbParticleData &operator= (const dbParticleData &) = delete;
		
		size_t GetCount() const { return count_; };
		size_t GetCountAlive() const { return count_alive_; };
		
		void Generate(size_t maxSize);
		void Kill(size_t id);
		void Wake(size_t id);
		void SwapData(size_t a, size_t b);
	};

	class DBX_API dbParticleGenerator
	{
	public:
		dbParticleGenerator() {};
		virtual ~dbParticleGenerator() {};

		virtual void Generate(double delta, dbParticleData* p, size_t startId, size_t endId) = 0;
	};

	class DBX_API dbParticleEmitter
	{
	protected:
		std::vector<std::shared_ptr<dbParticleGenerator>> generators_;
		float emitRate_;
	public:
		dbParticleEmitter() : emitRate_(0.f) {};
		~dbParticleEmitter() {};

		// calls all generators and activates particles
		virtual void Emit(double delta, dbParticleData* p);
		void AddGenerator(std::shared_ptr<dbParticleGenerator> generator) { generators_.push_back(generator); };
	};

	class DBX_API dbParticleUpdater
	{
	public:
		dbParticleUpdater() {};
		virtual ~dbParticleUpdater() {};

		virtual void Update(double delta, dbParticleData* p) = 0;
	};


	class DBX_API dbPS
	{
	protected:
		dbParticleData particles_;
		dbParticleData aliveParticles_;

		size_t count_;

		std::vector<std::shared_ptr<dbParticleEmitter>> emitters_;
		std::vector<std::shared_ptr<dbParticleUpdater>> updaters_;
	public:
		explicit dbPS(size_t maxCount);
		virtual ~dbPS() {};

		//dbPS(const dbPS &) = delete;
		//dbPS &operator=(const dbPS &) = delete;

		virtual void Update(double delta);
		virtual void Reset();

		virtual size_t GetParticleCount() const { return particles_.GetCount(); };
		virtual size_t GetAliveParticleCount() const { return particles_.GetCountAlive(); };
		
		void AddEmitter(std::shared_ptr<dbParticleEmitter> em) { emitters_.push_back(em); }
		void AddUpdater(std::shared_ptr<dbParticleUpdater> up) { updaters_.push_back(up); }

		dbParticleData* GetData() { return &particles_; }
	};

}}	// namespace

#endif