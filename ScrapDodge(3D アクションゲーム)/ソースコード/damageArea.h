/*============================================================
	[damageArea.h]
	▼題名 : ダメージエリア
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _DAMAGE_AREA_H_
#define _DAMAGE_AREA_H_

#include "GameObject.h"

class DamageArea : public GameObject
{
private:
	static struct MODEL_FBX* m_Model;
	static struct MODEL_FBX* m_ModelDeep;
	static ID3D11VertexShader* m_VertexShader;
	static ID3D11PixelShader* m_PixelShader;
	static ID3D11InputLayout* m_VertexLayout;
	static bool m_IsLoad;

	enum AttackMode m_Mode;

	Vector3 m_EnemyPos;
	GameObject* m_Target;
	int m_lifeTime;
	 
	
public:
	static void Load();
	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;
	
	void SetLaserArea(GameObject* target, Vector3 position, Vector3 scale, int frame);
	void SetFanArea(Vector3 position, Vector3 velocity, Vector3 scale, int frame);
	void SetLifeTime(int time) { m_lifeTime = time; }
	void SetMode(AttackMode mode) { m_Mode = mode; }

	const char* GetName() override
	{
		return "DamageArea";
	}
};

#endif //_DAMAGE_AREA_H_