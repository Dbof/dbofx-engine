namespace dbx 
{
namespace particle
{
	class DBX_API dbAttractorUpdater : public dbParticleUpdater
	{
	protected:
		std::vector<core::dbVector3> attractors_;
	public:
		dbAttractorUpdater(void);
		~dbAttractorUpdater(void);
		
		virtual void Update(double dt, dbParticleData *p) override;
 
		void				Add(const core::dbVector3& attr) { attractors_.push_back(attr); }
		core::dbVector3&	Get(size_t id) { return attractors_[id]; }
		size_t				GetCount() const { return attractors_.size(); }
		void				Clear() { attractors_.clear(); }
	};
}}	// namespace