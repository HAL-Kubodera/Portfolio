/*============================================================
	[particleAura.h]
	▼題名 : オーラパーティクル
	▼DATE : 2026/01/14
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _PARTICLEDAMAGE_H_
#define _PARTICLEDAMAGE_H_

#include "particle.h"

class ParticleAura : public Particle
{
private:
	static constexpr int PARTICLE_LIFE = 300;
	static constexpr Vector3 PARTICLE_VELOCITY = { 0.03f, 0.12f, 0.03f };
	static constexpr Vector3 PARTICLE_SCALE = { 0.1f,0.1f,0.1f };

	static constexpr int COUNT_PER_FRAME = 2;
	static constexpr int PARTICLE_RANGE_X = 50;
	static constexpr int PARTICLE_RANGE_Y = 20;
	static constexpr int PARTICLE_RANGE_Z = 50;

	static constexpr float REDUCE_FACTOR_X = 50.0f;
	static constexpr float REDUCE_FACTOR_Y = 100.0f;
	static constexpr float REDUCE_FACTOR_Z = 50.0f;
public:

	void Init() override;
	void Update() override;
	void Uninit() override;

	const char* GetName() override
	{
		return "ParticleAura";
	}
};

#endif //_PARTICLEDAMAGE_H