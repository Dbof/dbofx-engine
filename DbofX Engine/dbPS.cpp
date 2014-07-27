#include "DbofX.h"

namespace dbx
{
namespace particle
{
	void dbParticleData::Generate(size_t maxSize)
	{
		_count = maxSize;
		_countAlive = 0;
 
		_position.reset(new core::dbVector3[maxSize]);
		_color.reset(new core::dbColor[maxSize]);
		_startColor.reset(new core::dbVector3[maxSize]);
		_endColor.reset(new core::dbVector3[maxSize]);
		_velocity.reset(new core::dbVector3[maxSize]);
		_acceleration.reset(new core::dbVector3[maxSize]);
		_time.reset(new core::dbVector3[maxSize]);
		_alive.reset(new bool[maxSize]);
	}
	
		void dbParticleData::Kill(size_t id)
		{
			if (_countAlive > 0)
			{
				_alive[id] = false;
				SwapData(id, _countAlive -1);
				_countAlive--;
			}
		}
		
		void dbParticleData::Wake(size_t id)
		{
			if (_countAlive < _count)
			{
				_alive[id] = true;
				SwapData(id, _countAlive);
				_countAlive++;
			}
		}

		void dbParticleData::SwapData(size_t a, size_t b)
		{
			std::swap(_position[a], _position[b]);
			std::swap(_color[a], _color[b]);
			std::swap(_startColor[a], _startColor[b]);
			std::swap(_endColor[a], _endColor[b]);
			std::swap(_velocity[a], _velocity[b]);
			std::swap(_acceleration[a], _acceleration[b]);
			std::swap(_time[a], _time[b]);
			std::swap(_alive[a], _alive[b]);
		}
	////////////////////////////////////////////////////////////////////////////////
	// dbParticleEmitter class
 	void dbParticleEmitter::Emit(double delta, dbParticleData *p)
	{
		const size_t maxNewParticles = static_cast<size_t>(delta * _emitRate);
		const size_t startId = p->GetCountAlive();
		const size_t endId = min(startId + maxNewParticles, p->GetCount() -1);
 
		for (auto &gen : _generators)
		gen->Generate(delta, p, startId, endId);
 
		for (size_t i = startId; i < endId; ++i)
		{
			p->Wake(i);
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	// dbParticleSystem class 

	dbPS::dbPS(size_t maxCount)
	{
		_count = maxCount;
		_particles.Generate(maxCount);
		_aliveParticles.Generate(maxCount);

		for (size_t i = 0; i < maxCount; ++i)
			_particles._alive[i] = false;
	}

	void dbPS::Update(double delta)
	{
		for (auto& em : _emitters)
		{
			em->Emit(delta, &_particles);
		}

		for (size_t i = 0; i < _count; ++i)
		{
			_particles._acceleration[i] = core::dbVector3();
		}

		for (auto& up : _updaters)
		{
			up->Update(delta, &_particles);
		}
	}

	void dbPS::Reset()
	{
		_particles._countAlive = 0;
	}

}}	// namespace
