/*============================================================
    [tutorialManager.h]
    ▼題名 : チュートリアルでの動きを管理
    ▼DATE : 2026/03/18
    ▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef TUTORIALMANAGER_H_
#define TUTORIALMANAGER_H_

class TutorialManager
{
private:
    static int m_Frame;
    static int m_Phase;
    static bool m_SetGuide;
    static bool m_WasSlow;

    static class Guide* m_Guide;
    static class TutorialEnemy* m_Enemy;

    static constexpr int ATTACK_FRAME = 60;
    static constexpr int SHOT_WAIT_FRAME = 240;
    static constexpr int FRONT_WAIT_FRAME = 300;
    static constexpr int FAN_WAIT_FRAME = 300;

public:

    static void Init();
    static void Update();

    static void SetGuide();
    static bool NextGuide();
    static void SetAction();

    static void Retry();

    static int GetPhase() { return m_Phase; }
};

#endif //TUTORIALMANAGER_H_
