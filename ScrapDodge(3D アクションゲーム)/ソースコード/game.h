/*============================================================
	[game.h]
	Å•ëËñº : ÉQÅ[ÉÄÉVÅ[Éì
	Å•DATE : 2026/03/18
	Å•AUTHOR : ãvï€éõ èrâÓ
============================================================*/
#ifndef _GAME_H_
#define _GAME_H_

#include "scene.h"

class Game : public Scene
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

#endif //_GAME_H_