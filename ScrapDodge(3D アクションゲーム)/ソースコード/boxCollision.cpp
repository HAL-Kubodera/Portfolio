/*============================================================
    [boxCollision.cpp]
    Å•ëËñº : î†å^ÇÃìñÇΩÇËîªíË
    Å•DATE : 2026/03/18
    Å•AUTHOR : ãvï€éõ èrâÓ
============================================================*/
#include "main.h"
#include "renderer.h"
#include "boxCollision.h"
#include "model.h"
#include "manager.h"
#include "scene.h"
#include "camera.h"

void BoxCollision::Init()
{
	Collision::Init();

	m_Model = ModelLoad("asset\\model\\collision.fbx", true);
	SetModelColor(m_Model, 0.0f, 1.0f, 0.0f, 0.2f);

    m_GuiName = m_GuiName + "BoxCollider";
}

void BoxCollision::Update()
{
	Collision::Update();
  
    std::vector<GameObject*> gameObjects = Manager::GetScene()->GetGameObjects<GameObject>();

    for (auto obj : gameObjects)
    {
        Collision* col = obj->GetComponent<Collision>();

        if (col != nullptr && col != this)
        {
            int i = 0;

            for (Tag tag : m_TargetTag)
            {
                if (tag == col->GetTag() && ObbCheck(col))
                {
                    m_IsHit[i] = true;
                    m_HitObjects[i].push_back(col->GetParent());
                }

                i++;
            }
        }
    }
}

void BoxCollision::Draw()
{
    SetModelColor(m_Model, 0.0f, 1.0f, 0.0f, 0.2f);
	
    for (bool hit : m_IsHit)
    {
        if(hit == true) SetModelColor(m_Model, 1.0f, 0.0f, 0.0f, 0.2f);
    }
		

	Collision::Draw();
}

void BoxCollision::Uninit()
{
	Collision::Uninit();
}

bool BoxCollision::ObbCheck(Collision* col)
{
    Vector3 posA = m_Position;
    Vector3 posB = col->GetPosition();

    Vector3 rightA = GetRight();
    Vector3 upA = GetUp();
    Vector3 forwardA = GetForward();

    Vector3 rightB = col->GetRight();
    Vector3 upB = col->GetUp();
    Vector3 forwardB = col->GetForward();

    Vector3 axesA[AXIS_COUNT] = { rightA, upA, forwardA };
    Vector3 axesB[AXIS_COUNT] = { rightB, upB, forwardB };

    Vector3 radiusA = GetScale() * 0.5f;
    Vector3 radiusB = col->GetScale() * 0.5f;

    Vector3 direction = posB - posA;

    float dot[AXIS_COUNT][AXIS_COUNT], absDot[AXIS_COUNT][AXIS_COUNT];
    for (int i = 0; i < AXIS_COUNT; ++i)
    {
        for (int j = 0; j < AXIS_COUNT; ++j)
        {
            dot[i][j] = Vector3::dot(axesA[i], axesB[j]);
            absDot[i][j] = std::abs(dot[i][j]);
        }
    }

    direction = Vector3(Vector3::dot(direction, axesA[0]),
        Vector3::dot(direction, axesA[1]),
        Vector3::dot(direction, axesA[2]));

    float ra, rb;

    for (int i = 0; i < AXIS_COUNT; ++i)
    {
        ra = radiusA[i];
        rb = radiusB.x * absDot[i][0] + radiusB.y * absDot[i][1] + radiusB.z * absDot[i][2];
        if (std::abs(direction[i]) > ra + rb) return false;
    }

    for (int i = 0; i < AXIS_COUNT; ++i)
    {
        ra = radiusA.x * absDot[0][i] + radiusA.y * absDot[1][i] + radiusA.z * absDot[2][i];
        rb = radiusB[i];
        if (std::abs(direction.x * dot[0][i] + direction.y * dot[1][i] + direction.z * dot[2][i]) > ra + rb)
            return false;
    }

    for (int i = 0; i < AXIS_COUNT; ++i)
    {
        for (int j = 0; j < AXIS_COUNT; ++j)
        {
            ra = radiusA[(i + 1) % AXIS_COUNT] * absDot[(i + 2) % AXIS_COUNT][j] + radiusA[(i + 2) % AXIS_COUNT] * absDot[(i + 1) % AXIS_COUNT][j];
            rb = radiusB[(j + 1) % AXIS_COUNT] * absDot[i][(j + 2) % AXIS_COUNT] + radiusB[(j + 2) % AXIS_COUNT] * absDot[i][(j + 1) % AXIS_COUNT];
            float t = std::abs(direction[(i + 2) % AXIS_COUNT] * dot[(i + 1) % AXIS_COUNT][j] - direction[(i + 1) % AXIS_COUNT] * dot[(i + 2) % AXIS_COUNT][j]);
            if (t > ra + rb) return false;
        }
    }

    return true; // ëSé≤Ç≈ï™ó£Ç™Ç»ÇØÇÍÇŒè’ìÀ
}

std::vector<bool> BoxCollision::GetIsHit()
{
	return m_IsHit;
}



