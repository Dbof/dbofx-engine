#include "DbofX.h"
namespace dbx 
{
namespace particle
{
	 dbBasicTimeUpdater::dbBasicTimeUpdater()
	 {
	 }

	 void dbBasicTimeUpdater::Update(double delta, dbParticleData* p)
	 {
		 unsigned int endId = p->count_alive_;
		 const float localDt = (float)delta;

		 for (size_t i = 0; i < endId; i++)
		 {
			 p->time_[i].x -= localDt*1000;

			 if (p->time_[i].x <= 0.f)
			 {
				 p->Kill(i);
				 endId = p->count_alive_ < p->count_ ? p->count_alive_ : p->count_;
			 }
		 }
	 }
}}	// namespace
