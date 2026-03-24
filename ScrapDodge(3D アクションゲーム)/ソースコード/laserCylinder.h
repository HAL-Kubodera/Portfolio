/*============================================================
	[laserCylinder.h]
	¥‘è–¼ : ‰~’Œó‚ÌƒŒ[ƒU[
	¥DATE : 2026/03/18
	¥AUTHOR : ‹v•ÛŽ› r‰î
============================================================*/
#ifndef _LASER_CYLINDER_H_
#define _LASER_CYLINDER_H_

#include "GameObject.h"
#include "model.h"

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = nullptr; } }

class LaserCylinder : public GameObject
{
private:
	ID3D11VertexShader* m_VertexShader = nullptr;
	ID3D11PixelShader* m_PixelShader = nullptr;
	ID3D11InputLayout* m_VertexLayout = nullptr;

	class ParticleCylinder* m_Particle = nullptr;
	class DamageArea* m_DamageArea = nullptr;

	bool m_IsSpawn = false;
	float m_Frame = 0;

	static constexpr float SCALE_MAX_FRAME = 90.0f;
	static constexpr float SCALE_HEIGHT = 10.0f;
	static constexpr float CYLINDER_BASE_HEIGHT = 5.0f;

	void CheckIsSpawn();
	void SetParticle();
public:
	static constexpr float LIFE_FRAME = 120.0f;

	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;
	
	void SetDamageArea(class DamageArea* area) { m_DamageArea = area; }
	ParticleCylinder* GetParticleCylinder() { return m_Particle; }

	const char* GetName() override
	{
		return "LaserCylinder";
	}

	void DispUniqueGui() override
	{
		GameObject::DispUniqueGui();

		if (ImGui::TreeNode(GetName()))
		{
			ImGui::InputFloat("frame", &m_Frame);


			ImGui::TreePop();
		}
	}
};

#endif// _LASER_CYLINDER_H_