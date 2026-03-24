/*============================================================
    [speedManager.h]
    ¥‘è–¼ : ¢ŠE‚Ì‘¬‚³‚ðŠÇ—‚·‚éƒ}ƒl[ƒWƒƒ[
    ¥DATE : 2026/03/18
    ¥AUTHOR : ‹v•ÛŽ› r‰î
============================================================*/
#ifndef SPEEDMANAGER_H_
#define SPEEDMANAGER_H_

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = nullptr; } }

enum SPEEDMODE
{
    Speed_Default,
    Speed_Slow,
    Speed_Dead,
    Speed_Clear,
};

class SpeedManager
{
private:
    static float m_Speed;
    static constexpr float m_SlowSpeed = 0.3f;

    static int m_Frame;

    static bool m_StopTime;
    static SPEEDMODE m_Mode;

public:
    static constexpr int m_SlowMaxFrame = 10;
    static constexpr int m_SlowEndFrame = 120;

    static void Init();
    static void Update();

    static void SetSpeed(float speed) { m_Speed = speed; }
    static float GetSpeed() { return m_Speed; }
    static void SetMode(SPEEDMODE mode) { m_Mode = mode; }
    static SPEEDMODE GetMode() { return m_Mode; }

    static float GetTimeLimit();

    static void HitStop(int frame);
    static void PlayerDead(float slowTime,float speed);
};

#endif //SPEEDMANAGER_H_
