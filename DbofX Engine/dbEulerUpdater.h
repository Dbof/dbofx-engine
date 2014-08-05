#ifndef EULER_UPDATER_H
#define EULER_UPDATER_H

namespace dbx
{
	namespace particle
	{
		class DBX_API dbEulerUpdater : public dbParticleUpdater
		{
		private:
			core::dbVector3 global_acceleration_;
		public:
			explicit dbEulerUpdater(const core::dbVector3 acceleration) 
				: global_acceleration_(acceleration) {}
			virtual void Update(double dt, dbParticleData *p) override;

			void SetAcceleration(const core::dbVector3 acc) { global_acceleration_ = acc; }
			core::dbVector3 GetAcceleration() const { return global_acceleration_; }
		};
	}}	// namespace  
#endif // !EULER_UPDATER_H
