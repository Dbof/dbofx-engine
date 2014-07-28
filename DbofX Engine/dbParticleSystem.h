#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

namespace dbx
{
namespace video
{
	struct DBX_API dbParticleVertex
	{
		core::dbVector3	position;
		D3DCOLOR	color;
		float		u, v;
		static DWORD FVF;
	};

	struct DBX_API dbParticle
	{
		float			lifetime;
		core::dbVector3	position;
		core::dbVector3	velocity;
		float			friction;
		float			size;
		float			delta_size;
		core::dbColor	color;
		core::dbColor	delta_color;
		float			rotation;
		float			delta_rotation;
	};


	class DBX_API dbParticleSystem
	{
	public:
		static dbParticleSystem* GetInstance();
		static void Exit();

		void Move(const float fTime);
		void Render();
		void Update();
		void Clear();

		int GetParticleNumber()	{ return num_particles_; }

		void Init(int maxParticle);
		
		// AddParticle variants

		bool AddParticle(const float& lifetime, const core::dbVector3& position, const core::dbVector3& velocity, const float& friction);
		bool AddParticle(const float& lifetime, const core::dbVector3& position, const core::dbVector3& velocity, const float& friction,
			const float& startsize, const float& endsize);
		bool AddParticle(const float& lifetime, const core::dbVector3& position, const core::dbVector3& velocity, const float& friction, 
			const float& startsize, const float& endsize, 
			const core::dbColor& startcolor, const core::dbColor& endcolor);
		bool AddParticle(const float& lifetime, const core::dbVector3& position, const core::dbVector3& velocity, const float& friction, 
			const float& startsize, const float& endsize, 
			const core::dbColor& startcolor, const core::dbColor& endcolor,
			const float& startrotation, const float& endrotation);

	private:
		dbParticleSystem(void);
		~dbParticleSystem(void);
		
		dbParticle* db_particle_;
		int		max_particles_;
		int		num_particles_;

		core::dbVertexBuffer vertex_buffer_;
		static dbParticleSystem* instance_;
	};

}}	// namespace

#endif