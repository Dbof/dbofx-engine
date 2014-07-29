#include "DbofX.h"

namespace dbx
{
namespace particle
{
	void dbParticleData::Generate(size_t maxSize)
	{
		count_ = maxSize;
		count_alive_ = 0;
 
		position_.reset(new core::dbVector3[maxSize]);
		color_.reset(new core::dbColor[maxSize]);
		startColor_.reset(new core::dbVector3[maxSize]);
		endColor_.reset(new core::dbVector3[maxSize]);
		velocity_.reset(new core::dbVector3[maxSize]);
		acceleration_.reset(new core::dbVector3[maxSize]);
		time_.reset(new core::dbVector3[maxSize]);
		alive_.reset(new bool[maxSize]);
	}
	
		void dbParticleData::Kill(size_t id)
		{
			if (count_alive_ > 0)
			{
				alive_[id] = false;
				SwapData(id, count_alive_ -1);
				count_alive_--;
			}
		}
		
		void dbParticleData::Wake(size_t id)
		{
			if (count_alive_ < count_)
			{
				alive_[id] = true;
				SwapData(id, count_alive_);
				count_alive_++;
			}
		}

		void dbParticleData::SwapData(size_t a, size_t b)
		{
			if (a == b)
				return;
			std::swap(position_[a], position_[b]);
			std::swap(color_[a], color_[b]);
			std::swap(startColor_[a], startColor_[b]);
			std::swap(endColor_[a], endColor_[b]);
			std::swap(velocity_[a], velocity_[b]);
			std::swap(acceleration_[a], acceleration_[b]);
			std::swap(time_[a], time_[b]);
			std::swap(alive_[a], alive_[b]);
		}

	////////////////////////////////////////////////////////////////////////////////
	// dbParticleSystem class 

	dbPS::dbPS(size_t maxCount)
	{
		count_ = maxCount;
		particles_.Generate(maxCount);
		aliveParticles_.Generate(maxCount);

		for (size_t i = 0; i < maxCount; ++i)
			particles_.alive_[i] = false;
	}

	void dbPS::Update(double delta)
	{
		for (auto& em : emitters_)
		{
			em->Emit(delta, &particles_);
		}

		for (size_t i = 0; i < count_; ++i)
		{
			core::dbVector3 zero;
			particles_.acceleration_[i] = zero;
		}

		for (auto& up : updaters_)
		{
			up->Update(delta, &particles_);
		}
	}

	void dbPS::Reset()
	{
		particles_.count_alive_ = 0;
	}

}}	// namespace
