#include "DbofX.h"

namespace dbx
{
namespace particle
{
	dbBasicPositionGenerator::~dbBasicPositionGenerator(void)
	{
	}

	void dbBasicPositionGenerator::Generate(double delta, dbParticleData* p, size_t startId, size_t endId)
	{
		for (size_t i = startId; i < endId; ++i)
		{
			p->position_[i] = position_;
			p->Wake(i);
		}
	}
}}	// namespace