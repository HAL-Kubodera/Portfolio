/*============================================================
	[manager.h]
	▼題名 : ゲームループマネージャー
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _MANAGER_H_
#define _MANAGER_H_

class Manager
{
private:
	static class Scene* m_Scene;
	static class Scene* m_SceneNext;

public:
	static void Init();
	static void Uninit();
	static void Update();
	static void Draw();

	static Scene* GetScene() { return m_Scene; }

	template<typename T>
	static void SetScene()
	{
		m_SceneNext = new T();
	}
};

#endif // _MANAGER_H_