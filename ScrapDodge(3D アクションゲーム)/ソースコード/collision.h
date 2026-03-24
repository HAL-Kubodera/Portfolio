/*============================================================
	[collision.h]
	Å•ëËñº : ìñÇΩÇËîªíË
	Å•DATE : 2026/03/18
	Å•AUTHOR : ãvï€éõ èrâÓ
============================================================*/
#ifndef _COLLISION_H_
#define _COLLISION_H_

#include "myComponent.h"
#include "GameObject.h"

class Collision : public MyComponent
{
protected:
	class MODEL_FBX* m_Model;

	ID3D11VertexShader* m_VertexShader;
	ID3D11PixelShader* m_PixelShader;
	ID3D11InputLayout* m_VertexLayout;
	class ModelRenderer* m_ModelRenderer;

	Vector3 m_localPosition;

	std::vector<std::vector<GameObject*>> m_HitObjects;

	Tag m_Tag = Tag_Normal;
	std::vector<Tag> m_TargetTag;

	bool m_IsDisp = false;
	std::vector<bool> m_IsHit;

public:
	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	void SetLocalPosition(Vector3 pos)
	{
		m_localPosition = pos;
	}

	void SetIsDisp(bool flg) { m_IsDisp = flg; }
	bool* GetIsDisp() { return &m_IsDisp; }

	void SetTag(Tag tag) { m_Tag = tag; }
	Tag GetTag() { return m_Tag; }

	void SetTargetTag(Tag tag);
	std::vector<Tag> GetTargetTag() { return m_TargetTag; }

	std::vector<GameObject*> GetHitObjects(int i) { return m_HitObjects[i]; }

	virtual std::vector<bool> GetIsHit() = 0;
	virtual const char* GetName() = 0;
};

#endif // _COLLISION_H_