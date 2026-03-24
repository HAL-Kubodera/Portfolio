/*
ファイル : pauseManager.h
  名 前  : Shunsuke Kubodera
  日 付  : 2026/1/15
*/

#ifndef PAUSEMANAGER_H_
#define PAUSEMANAGER_H_

#include <vector>

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = nullptr; } }

class PauseManager
{
private:
    static std::vector<class GameObject*> m_PauseObject;

    static bool m_IsPause;
public:
    static bool GetPause() { return m_IsPause; }
    static void ChangePause();

    static void Init();
    static void Update();
    static void Uninit();
};

#endif //PAUSEMANAGER_H_
