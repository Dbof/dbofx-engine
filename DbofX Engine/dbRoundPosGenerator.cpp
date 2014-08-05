#include "DbofX.h"

namespace dbx
{
namespace particle
{
	void dbRoundPosGenerator::Generate(double dt, dbParticleData *p, size_t startId, size_t endId)
	{
		for (size_t i = startId; i < endId; ++i)
		{
			double ang = getRandomFloat(0.0f, DB_PI*2.0f, 2);
			p->position_[i] = center_ + core::dbVector3(radX_*(float)sin(ang), radY_*(float)cos(ang), 0.f);
			p->velocity_[i] = core::dbVector3();
		}
	}

}}	// namespace
