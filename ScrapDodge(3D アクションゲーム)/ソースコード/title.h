/*============================================================
	[title.h]
	▼題名 : タイトルシーン
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _TITLE_H_
#define _TITLE_H_

#include "scene.h"

class Title : public Scene
{
private:
	int m_Frame = 0;
	int m_MovieStep = 0;

	class TitleCamera* pCamera;
	std::vector<class TitleStone*> pStones;

	class Audio* m_BGM = nullptr;
	static constexpr float BGM_VOLUME = 0.2f;

	static constexpr int FIRST_CUT = 290;
	static constexpr int SECOND_CUT = 300;
	static constexpr int THIRD_CUT = 400;

	struct StoneData
	{
		Vector3 pos;
		Vector3 scale;
		float frame;
		float delayFrame;
	};

	static constexpr StoneData STONES[] =
	{
		{ {-22.5f, 2.0f, 1.0f}, {2.0f, 1.0f, 2.0f}, 50.0f, 0.0f },
		{ {-24.5f, 2.0f, 4.0f}, {2.0f, 1.0f, 2.0f}, 50.0f, 80.0f }
	};

	static constexpr StoneData MOVIE_STONES[] =
	{
		{ {-1.5f, 2.3f, -8.0}, {1.0f, 1.0f, 2.0f}, 0.0f, 200.0f },
		{ {-25.0f, 2.0f, -1.0f}, {2.0f, 1.0f, 1.0f}, 40.0f, 200.0f },
		{ { -28.0f, 2.0f, 4.0f }, {3.0f, 1.0f, 2.0f}, 100.0f, 200.0f }
	};

	static constexpr int M_STONE_NUM = 2;

	static constexpr Vector3 FIRST_LASER_POS = { -1.5f, 2.3f, -8.0f };
	static constexpr Vector3 FIRST_LASER_VEL = { 0.1f,0.0f,1.0f };
	static constexpr float FIRST_LASER_FRAME = 70;
	static constexpr Vector3 SECOND_LASER_POS = { -5.5f, 1.4f, 8.0f };
	static constexpr Vector3 SECOND_LASER_VEL = { 0.1f,0.0f,-1.0f };
	static constexpr float SECOND_LASER_FRAME = 90;
	static constexpr Vector3 THIRD_LASER_POS = { -8.5f, 0.1f, -8.0f };
	static constexpr Vector3 THIRD_LASER_VEL = { 0.1f,0.0f,1.0f };
	static constexpr float THIRD_LASER_FRAME = 40;

	void Init() override;
	void Update() override;
	void Uninit() override;

	void InitStone();
	void SetMovieStone();
};

#endif // _TITLE_H_