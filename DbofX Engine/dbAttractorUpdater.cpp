#include "DbofX.h"

namespace dbx 
{
namespace particle
{
	 dbAttractorUpdater::dbAttractorUpdater() {}
	 dbAttractorUpdater::~dbAttractorUpdater() {}

	 void dbAttractorUpdater::Update(double delta, dbParticleData* p)
	 {
		const float localDT = (float)delta;
		const size_t endId = p->count_;
		const size_t countAttractors = attractors_.size();
		core::dbVector3 off;

		for (size_t i = 0; i < endId; ++i)
		{
			for (size_t a = 0; a < countAttractors; ++a)
			{
				off = attractors_[a] - p->position_[i];
				p->acceleration_[i] += off * localDT;
			}
		}
	 }
}}	// namespace
