/*============================================================
	[result.h]
	▼題名 : リザルトシーン
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _RESULT_H_
#define _RESULT_H_

#include "scene.h"

class Result : public Scene
{
private:
	void Init() override;
	void Update() override;
};

#endif // _RESULT_H_