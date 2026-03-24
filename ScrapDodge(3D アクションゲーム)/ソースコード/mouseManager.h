/*============================================================
    [mouseManager.h]
    Å•ëËñº : É}ÉEÉXä«óù
    Å•DATE : 2026/03/18
    Å•AUTHOR : ãvï€éõ èrâÓ
============================================================*/
#ifndef MOUSEMANAGER_H_
#define MOUSEMANAGER_H_

#include "Enemy.h"
#include <vector>

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = nullptr; } }

class MouseManager
{
private:
	static int m_MouseX;
	static int m_MouseY;
	static bool m_IsDownLeft;
    static bool m_IsDownRight;
public:

    static void SetMousePosition(int x, int y) {
        m_MouseX = x;
        m_MouseY = y;
    }

    static int GetMouseX() { return m_MouseX; }
    static int GetMouseY() { return m_MouseY; }

    static void SetMouseLeftDown(bool down) { m_IsDownLeft = down; }
    static bool IsMouseLeftDown() { return m_IsDownLeft; }

    static void SetMouseRightDown(bool down) { m_IsDownRight = down; }
    static bool IsMouseRightDown() { return m_IsDownRight; }
};

#endif //MOUSEMANAGER_H_
