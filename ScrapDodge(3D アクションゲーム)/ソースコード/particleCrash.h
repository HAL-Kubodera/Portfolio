/*============================================================
	[particleCrash.h]
	▼題名 : クラッシュパーティクル
	▼DATE : 2026/01/24
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _PARTICLECRASH_H_
#define _PARTICLECRASH_H_

#include "Particle.h"

class ParticleCrash : public Particle
{
protected:
	static const int PARTICLE_MAX = 100;
	static const int DEFAULT_VEL = 50;
private:
	static constexpr int PARTICLE_LIFE = 120;
	static constexpr Vector3 PARTICLE_VELOCITY = { 0.0f, -0.01f, 0.0f };
	static constexpr Vector3 PARTICLE_SCALE = { 0.3f,0.3f,0.3f };

	static constexpr float REDUCE_FACTOR = 100.0f;
	
public:

	void Init() override;
	void Update() override;
	void Uninit() override;

	const char* GetName() override
	{
		return "ParticleCrash";
	}
};

#endif //_PARTICLECRASH_H_