/*============================================================
	[loading.h]
	▼題名 : ローディングシーン
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _LOADING_H_
#define _LOADING_H_

#include "scene.h"

enum LoadScene
{
	Load_Game,
	Load_Tutorial,

	Load_Max
};

class Loading : public Scene
{
private:
	bool m_IsFinish = false;
	float m_Frame = 0;
	static constexpr float MIN_LOAD_FRAME = 120;
	static LoadScene m_Scene;

	void Init() override;
	void Update() override;
	void Load();
public:
	static void SetLoadNextScene(LoadScene Scene);
};

#endif //_LOADING_H_