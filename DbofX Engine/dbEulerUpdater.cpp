#include "DbofX.h"

namespace dbx
{
namespace particle
{
	void dbEulerUpdater::Update(double dt, dbParticleData *p)
	{
		const float localDT = (float)dt;
		const core::dbVector3 globalA = core::dbVector3(localDT * global_acceleration_.x, localDT * global_acceleration_.y, localDT * global_acceleration_.z);
 
		const unsigned int endId = p->count_alive_;
		for (size_t i = 0; i < endId; ++i)
			p->acceleration_[i] += globalA;
 
		for (size_t i = 0; i < endId; ++i) 
		{
			p->velocity_[i] += localDT * p->acceleration_[i];
			p->position_[i] += localDT * p->velocity_[i];
		}
		for (size_t i = 0; i < endId; ++i)
			if (p->position_[i].y >= 100)
				p->Kill(i);

	}

}}	// namespace