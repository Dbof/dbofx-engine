#include "DbofX.h"

namespace dbx
{
namespace particle
{

	dbBasicSpeedGenerator::dbBasicSpeedGenerator(core::dbVector3& velocity)
	{
		global_vel_ = velocity;
	}

	dbBasicSpeedGenerator::~dbBasicSpeedGenerator(void)
	{
	}

	void dbBasicSpeedGenerator::Generate(double delta, dbParticleData* p, size_t startId, size_t endId)
	{
		for (size_t i = startId; i < endId; ++i)
		{
			p->velocity_[i] = global_vel_;
		}
	}

	void dbBasicSpeedGenerator::SetSpeed(core::dbVector3& velocity)
	{
		global_vel_ = velocity;
	}
}}	// namespace