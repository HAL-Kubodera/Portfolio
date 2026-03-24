/*============================================================
	[particleDamage.h]
	▼題名 : ダメージパーティクル
	▼DATE : 2026/01/14
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _PARTICLEDAMAGE_H_
#define _PARTICLEDAMAGE_H_

#include "particle.h"

class ParticleDamage : public Particle
{
private:
	static constexpr int OBJECT_LIFE = 120;
	static constexpr int PARTICLE_LIFE = 90;

	static constexpr int PARTICLE_RANGE = 100;
	static constexpr float REDUCE_FACTOR = 500.0f;
public:

	void Init() override;
	void Update() override;
	void Uninit() override;

	const char* GetName() override
	{
		return "ParticleDamage";
	}
};

#endif //_PARTICLEDAMAGE_H