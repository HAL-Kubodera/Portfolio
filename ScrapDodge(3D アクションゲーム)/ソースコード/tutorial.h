/*============================================================
	[tutorial.h]
	▼題名 : チュートリアルシーン
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _TUTORIAL_H_
#define _TUTORIAL_H_

#include "scene.h"

class Tutorial : public Scene
{
private:
	class Audio* m_BGM;
	static constexpr float BGM_VOLUME = 0.05f;
public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
};

#endif //_TUTORIAL_H_