#pragma once

namespace dbx
{
namespace particle
{
	class DBX_API dbRoundPosGen : public dbParticleGenerator
	{
	private:
		core::dbVector3 center_;
		float radX_;
		float radY_;
	public:
		dbRoundPosGen(void) {};
		dbRoundPosGen(const core::dbVector3 &center, double radX, double radY) : center_(center), radX_((float)radX), radY_((float)radY) {}
 
		void SetCenter(const core::dbVector3& center) { center_ = center; }
		void SetRadiusX(const float& radius) { radX_ = radius; }
		void SetRadiusY(const float& radius) { radY_ = radius; }

		virtual void Generate(double dt, dbParticleData *p, size_t startId, size_t endId) override;
	};

}}	// namespace