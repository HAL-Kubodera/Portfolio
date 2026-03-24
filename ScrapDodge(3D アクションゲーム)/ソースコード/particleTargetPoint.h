/*============================================================
	[particleTargetPoint.h]
	▼題名 : ダメージパーティクル
	▼DATE : 2026/01/14
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _PARTICLETARGETPOINT_H_
#define _PARTICLETARGETPOINT_H_

#include "particle.h"

class ParticleTargetPoint : public Particle
{
private:
	float m_Rot = 0;

	static constexpr Vector3 DEFAULT_SCALE = { 0.2f,0.2f,0.2f };

	static constexpr float CIRCLE_RADIUS = 0.2f;
	static constexpr float CIRCLE_Y_OFFSET = -0.2f;
	static constexpr float DISTANCE = -0.3f;

	static constexpr int ROOP_PARTICLE_NUM = 10;
	static constexpr int PARTICLE_LIFE = 20;
	static constexpr float ADD_ROT = 18;
public:

	void Init() override;
	void Update() override;
	void Uninit() override;

	const char* GetName() override
	{
		return "ParticleTargetPoint";
	}
};

#endif //_PARTICLETARGETPOINT_H