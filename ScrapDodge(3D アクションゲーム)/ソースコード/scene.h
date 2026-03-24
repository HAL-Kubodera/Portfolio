/*============================================================
	[scene.h]
	▼題名 : シーンの基底クラス
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _SCENE_H_
#define _SCENE_H_

#include <list>
#include <vector>
#include "GameObject.h"
#include "myComponent.h"

enum Layer
{
	camera = 0,
	field,
	object,
	laser,
	area,
	screen,
	ui,
	Layer_Num,
};

class Scene
{
private:
	std::list<GameObject*> m_GameObject[Layer_Num];

protected:
	bool m_IsFade = false;

public:
	
	virtual void Init();
	virtual void Uninit();
	virtual void Update();
	virtual void Draw();
	virtual void LayerDraw(Layer layer);

	bool GetIsFade() { return m_IsFade; }
	void SetIsFade(bool flg) { m_IsFade = flg; }

	template<typename T>
	T* AddGameObject(int Layer)
	{
		T* GameObject = new T();

		GameObject->Init();
		m_GameObject[Layer].push_back(GameObject);

		return GameObject;
	}

	template<typename T>
	T* GetGameObject()
	{
		for (int i = 0; i < Layer_Num; i++)
		{
			for (auto gameObject : m_GameObject[i])
			{
 				T* find = dynamic_cast<T*>(gameObject);

				if (find != nullptr)
					return find;
			}
		}

		return nullptr;
	}

	template<typename T>
	std::vector<T*> GetGameObjects()
	{
		std::vector<T*> finds;

		for (int i = 0; i < Layer_Num; i++)
		{
			for (auto gameObject : m_GameObject[i])
			{
				T* find = dynamic_cast<T*>(gameObject);

				if (find != nullptr)
					finds.push_back(find);
			}
		}

		return finds;
	}
};

#endif //_SCENE_H_