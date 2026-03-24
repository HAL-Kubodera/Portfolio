/*============================================================
	[component.h]
	▼題名 : アニメーションモデル用コンポーネント
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _COMPONENT_H_
#define _COMPONENT_H_

class Component
{
protected:
	class  GameObject* m_GameObject = nullptr;
public:
	Component() {};
	Component(GameObject* Object) { m_GameObject = Object; }
	virtual ~Component() {}

	virtual void Init() {};
	virtual void Uninit() {};
	virtual void Update() {};
	virtual void Draw() {};
};

#endif // _COMPONENT_H_