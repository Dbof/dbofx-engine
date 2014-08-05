#include "DbofX.h"

namespace dbx
{
namespace particle
{
 	void dbParticleEmitter::Emit(double delta, dbParticleData *p)
	{
		const size_t maxNewParticles = static_cast<size_t>(delta * emit_rate_);
		const size_t startId = p->GetCountAlive();
		const size_t endId = min(startId + maxNewParticles, p->GetCount() - 1);
 
		for (auto &gen : generators_)
			gen->Generate(delta, p, startId, endId);
 
		for (size_t i = startId; i < endId; ++i)
		{
			p->Wake(i);
		}
	}
}}	// namespace