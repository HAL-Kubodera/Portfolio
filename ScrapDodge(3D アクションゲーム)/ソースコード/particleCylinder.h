/*============================================================
	[particleCylinder.h]
	Å•ëËñº : â~íåÉpÅ[ÉeÉBÉNÉã
	Å•DATE : 2026/01/29
	Å•AUTHOR : ãvï€éõ èrâÓ
============================================================*/
#ifndef _PARTICLECYLINDER_H_
#define _PARTICLECYLINDER_H_

#include "Particle.h"

class ParticleCylinder : public Particle
{
private:
	float m_Distance = 0.0f;

	static const int DEFAULT_VEL = 50;
	static const int ROOP_PARTICLE_NUM = 72;

	static constexpr int OBJECT_LIFE = 300;
	static constexpr int PARTICLE_LIFE = 20;
	
	static constexpr float REDUCE_FACTOR = 100.0f;

	static constexpr float ADD_ROT = 10;
public:
	static const int PARTICLE_MAX = 10000;
	static constexpr Vector3 PARTICLE_SCALE = { 0.1f,0.1f,0.1f };

	void SetDistance(float distance) { m_Distance = distance; }
	PARTICLE* GetParticle() { return m_Particle; }

	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	const char* GetName() override
	{
		return "ParticleCylinder";
	}
};

#endif //_PARTICLECYLINDER_H_