#include "DbofX.h"


namespace dbx
{
namespace particle
{
	dbBasicTimeGenerator::dbBasicTimeGenerator(float lifetimeMillis)
	{
		life_time_millis = lifetimeMillis;
	}

	dbBasicTimeGenerator::~dbBasicTimeGenerator()
	{
	}

	void dbBasicTimeGenerator::Generate(double delta, dbParticleData* p, size_t startId, size_t endId)
	{
		for (size_t i = startId; i < endId; ++i)
		{
			p->time_[i].x = life_time_millis;
			p->Wake(i);
		}
	}
}}	// namespace