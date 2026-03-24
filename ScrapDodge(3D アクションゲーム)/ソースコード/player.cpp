/*============================================================
	[player.cpp]
	▼題名 : プレイヤー
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#include "main.h"
#include "renderer.h"
#include "player.h"
#include "modelRenderer.h"
#include "audio.h"
#include "mouseManager.h"
#include "manager.h"
#include "scene.h"
#include "camera.h"
#include "enemy.h"
#include "meshField.h"
#include "Boxcollision.h"
#include "Result.h"
#include "playerHp.h"
#include "playerLaser.h"
#include "speedManager.h"
#include "pauseManager.h"
#include "fade.h"
#include "targetPoint.h"
#include "particleAura.h"
#include "shadow.h"
#include "DamageScreen.h"

#include "text3d.h"

AnimationModel* Player::m_AnimationModel;
ID3D11VertexShader* Player::m_VertexShader;
ID3D11PixelShader* Player::m_PixelShader;
ID3D11InputLayout* Player::m_VertexLayout;
Audio* Player::m_JumpSE;
bool Player::m_IsLoad;

void Player::Load()
{
	m_AnimationModel = new AnimationModel();
	m_AnimationModel->Load("asset\\model\\player_run.fbx");
	m_AnimationModel->LoadAnimation("asset\\model\\player_Idle.fbx", "Idle");
	m_AnimationModel->LoadAnimation("asset\\model\\player_Run.fbx", "Run");
	m_AnimationModel->LoadAnimation("asset\\model\\player_Jump.fbx", "Jump");
	m_AnimationModel->LoadAnimation("asset\\model\\player_Dead.fbx", "Dead");
	m_AnimationModel->LoadAnimation("asset\\model\\player_Shot.fbx", "Shot");

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");

	m_JumpSE = new Audio();
	m_JumpSE->Load("asset\\audio\\jump.wav");

	m_IsLoad = true;
}

void Player::Init()
{
	if (!m_IsLoad) Load();

	SetTag(Tag_Player);

	BoxCollision* bc = AddComponent<BoxCollision>();
	bc->SetLocalPosition(COLLISION_LOCALPOS);
	bc->SetScale(COLLISION_SCALE);
	bc->SetTargetTag(Tag_Damage);
	bc->SetTargetTag(Tag_Wall);
	bc->SetTargetTag(Tag_Spike);

	Shadow* shadow = AddComponent<Shadow>();
	shadow->SetParent(this);
	shadow->SetDefaultScale(SHADOW_SCALE);

	m_Camera = Manager::GetScene()->GetGameObject<Camera>();
}

void Player::Update()
{
	if (Camera::m_BuildMode) return;
	if (PauseManager::GetPause()) return;

	if (SpeedManager::GetMode() == Speed_Dead)
	{
		UpdateDeadAnimation();

		return;
	}

	if (SpeedManager::GetSpeed() == 0.0f) return;

	//速さの計算
	m_Speed = SpeedManager::GetSpeed();
	if (SpeedManager::GetMode() == Speed_Slow)
		m_Speed = 1.0f;

	m_IsRun = false;
	m_IsInputMulti = false;

	UpdateInput();
	UpdateShot();

	//移動量の加算
	m_Velocity.y += m_Gravity;
	m_Position += m_Velocity * m_Speed;

	//着地判定(軽いからこの処理だけど別のオブジェクトが追加された場合変更)
	if (m_Position.y < Manager::GetScene()->GetGameObject<MeshField>()->GetHeight(m_Position))
	{
		m_Position.y = Manager::GetScene()->GetGameObject<MeshField>()->GetHeight(m_Position);
		m_IsJump = false;
	}

	m_Velocity *= m_Resist;

	UpdateAnimation();
	UpdateInvincible();
	UpdateRotation();

	UpdateComponent();
	UpdateHitCollision();

	if (m_Hp <= 0)
		SetDeadAnimation();

	UpdateAfterEffect();

	m_OldPos = m_Position;
}

void Player::Draw()
{
	//入力レイアウト設定
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//シェーダ設定
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	DrawAfterImage();

	Vector3 shakePos = GetShakePosition();

	{
		//マトリクス設定
		XMMATRIX world, scale, rot, trans;
		scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
		rot = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
		trans = XMMatrixTranslation(shakePos.x, shakePos.y, shakePos.z);
		world = scale * rot * trans;

		Renderer::SetWorldMatrix(world);

		if (m_IsInvincible && SpeedManager::GetMode() != Speed_Dead)
			m_AnimationModel->Draw(DAMAGE_COLOR);
		else
			m_AnimationModel->Draw();
	}


#ifdef _DEBUG

	DrawDebugSystem();

#endif

	DrawComponent();
}

void Player::Uninit()
{
	UninitComponent();
}

//入力のアップデート
void Player::UpdateInput()
{
	if (!m_CanInput)return;

	Move();

	if (MouseManager::IsMouseLeftDown() && !m_IsJump)
	{
		Shot();
	}

	if (MouseManager::IsMouseRightDown())
	{
		//スロウ中はスロウゲージが0になるまで、通常から半分はないと起動不可
		if(m_IsSlow && m_SlowCharge > 0 || !m_IsSlow && m_SlowCharge > SLOW_MAX_CHARGE / 2)
			Slow();
		else
			SlowCharge();
	}
	else
	{
		SlowCharge();
	}

	if (Keyboard_IsKeyDownTrigger(KK_SPACE) && !m_IsJump)
	{
		Jump();
	}
}

//移動
void Player::Move()
{
	float rotation = m_Camera->GetRotation().y;

	if (Keyboard_IsKeyDown(KK_A))
	{
		Vector3 velocity = { -1.0f,0.0f,0.0f };
		velocity.y = 0;
		velocity.normalize();
		m_Velocity += velocity * m_WalkVel;

		if (m_IsInputMulti)
		{
			m_Direction += rotation + XM_PI + XM_PIDIV2;
			m_Direction /= 2;
		}
		else
		{
			m_IsRotate = true;
			m_IsInputMulti = true;
			m_OldDirection = m_Rotation.y;
			m_RotationFrame = 0;
			m_Direction = rotation + XM_PI + XM_PIDIV2;
		}

		m_IsRun = true;
	}
	if (Keyboard_IsKeyDown(KK_D))
	{
		Vector3 velocity = { 1.0f,0.0f,0.0f };
		velocity.y = 0;
		velocity.normalize();
		m_Velocity += velocity * m_WalkVel;

		if (m_IsInputMulti)
		{
			m_Direction += rotation + XM_PIDIV2;
			m_Direction /= 2;
		}
		else
		{
			m_IsRotate = true;
			m_IsInputMulti = true;
			m_OldDirection = m_Rotation.y;
			m_RotationFrame = 0;
			m_Direction = rotation + XM_PIDIV2;
		}

		m_IsRun = true;
	}
	if (Keyboard_IsKeyDown(KK_W))
	{
		Vector3 velocity = { 0.0f,0.0f,1.0f };
		velocity.y = 0;
		velocity.normalize();
		m_Velocity += velocity * m_WalkVel;

		if (m_IsInputMulti)
		{
			if (m_Direction == rotation + XM_PI + XM_PIDIV2 || m_Direction == (rotation + rotation - XM_PIDIV2) / 2)
			{
				//後で綺麗な方法考える
				m_Direction = rotation + rotation - XM_PIDIV2;
				m_Direction /= 2;
			}
			else
			{
				m_Direction += rotation;
				m_Direction /= 2;
			}
		}
		else
		{
			m_IsRotate = true;
			m_IsInputMulti = true;
			m_OldDirection = m_Rotation.y;
			m_RotationFrame = 0;
			m_Direction = rotation;
		}

		m_IsRun = true;
	}
	if (Keyboard_IsKeyDown(KK_S))
	{
		Vector3 velocity = { 0.0f,0.0f,-1.0f };
		velocity.y = 0;
		velocity.normalize();
		m_Velocity += velocity * m_WalkVel;

		if (m_IsInputMulti)
		{
			m_Direction += rotation + XM_PI;
			m_Direction /= 2;
		}
		else
		{
			m_IsRotate = true;
			m_IsInputMulti = true;
			m_OldDirection = m_Rotation.y;
			m_RotationFrame = 0;
			m_Direction = rotation + XM_PI;
		}

		m_IsRun = true;
	}
}

//スロウスキル
void Player::Slow()
{
	SpeedManager::SetMode(Speed_Slow);
	SpeedManager::SetSpeed(0.5f);

	m_IsSlow = true;
	m_SlowCharge--;

	if (m_Aura == nullptr)
	{
		m_Aura = Manager::GetScene()->AddGameObject<ParticleAura>(object);
		m_Aura->SetColor({ 1.0f, 0.0f, 1.0f, 1.0f });
	}

	for (int num = 0; num < AFTER_IMAGE_NUM; num++)
	{
		m_AfterImagePos[num] = m_Position;
		m_AfterImageRot[num] = m_Rotation;
		m_AfterFrame = 0;
	}
}

void Player::SlowCharge()
{
	//前回スロウなら
	if (m_IsSlow)
	{
		m_IsSlow = false;
		m_SlowCoolDown = SLOW_MAX_COOLDOWN;
		SpeedManager::SetMode(Speed_Default);
		SpeedManager::SetSpeed(1.0f);
	}

	if (m_SlowCoolDown != 0)
		m_SlowCoolDown -= SpeedManager::GetSpeed();

	if (m_SlowCoolDown <= 0)
		m_SlowCoolDown = 0;

	if (m_SlowCoolDown == 0)
		m_SlowCharge++;

	if (m_SlowCharge >= SLOW_MAX_CHARGE)
		m_SlowCharge = SLOW_MAX_CHARGE;
}

//攻撃
void Player::Shot()
{
	if (m_IsShot)return;

	TargetPoint* pTPoint = Manager::GetScene()->GetGameObject<TargetPoint>();

	if (pTPoint != nullptr)
	{
		float rotation = m_Camera->GetRotation().y;

		m_IsShot = true;

		m_IsRotate = true;
		m_OldDirection = m_Rotation.y;
		m_RotationFrame = 0;
		m_Direction = rotation;

		m_CanInput = true;
	}
}

//ジャンプ
void Player::Jump()
{
	m_Velocity.y += m_JumpVel;

	m_AnimFrame = 0;
	m_IsJump = true;

	m_JumpSE->Play();
}

void Player::UpdateShot()
{
	if (!m_IsShot) return;

	TargetPoint* pTPoint = Manager::GetScene()->GetGameObject<TargetPoint>();

	if (pTPoint != nullptr)
	{
		PlayerLaser* pLaser = Manager::GetScene()->GetGameObject<PlayerLaser>();

		if (m_ShotFrame != SHOT_MAX_FRAME)
			m_ShotFrame++;

		if (pLaser == nullptr)
		{
			PlayerLaser* pLaser = Manager::GetScene()->AddGameObject<PlayerLaser>(object);

			pLaser->SetPosition(m_Position);
			pLaser->SetTarget(pTPoint);
			pLaser->SetCollision();
		}
	}
	else
	{
		m_ShotFrame--;

		if (m_ShotFrame < 0)
		{
			m_ShotFrame = 0;
			m_IsShot = false;
		}
	}
}

//攻撃を食らったときにプレイヤーを揺らす
void Player::DamageShake(Vector3 shakeVector)
{
	m_ShakeVector = shakeVector;
	m_ShakeTime = 0.0f;
}

Vector3 Player::GetShakePosition()
{
	Vector3 position = m_Position + (m_ShakeVector * cosf(m_ShakeTime));

	//ヒット時シェイク
	m_ShakeTime += SHAKE_FACTOR;
	m_ShakeVector *= SHAKE_FACTOR;

	return position;
}

//ゲームオーバーアニメーションのセット
void Player::SetDeadAnimation()
{
	SpeedManager::SetMode(Speed_Dead);
	SpeedManager::SetSpeed(1.0f / DEAD_STOPTIME);

	SpeedManager::PlayerDead(HANDUP_ANIMTIME, 1.0f / DEAD_STOPTIME);
	SetAnimation("Dead");
	m_AnimFrame = 0;
	m_AnimationBlend = 1.0f;
}

//ゲームオーバーアニメーションのアップデート
void Player::UpdateDeadAnimation()
{
	if (m_AnimFrame >= DEAD_ANIMTIME)
	{
		m_AnimFrame = DEAD_ANIMTIME;
		return;
	}

	//演出時はセカイのスピードに合わせる
	if (m_AnimFrame < HANDUP_ANIMTIME)
		m_AnimFrame += (1.0f / DEAD_STOPTIME) * HANDUP_ANIMTIME;
	else
		m_AnimFrame++;

	m_AnimationModel->Update(m_AnimationName.c_str(), (int)m_AnimFrame, m_AnimationNameNext.c_str(), (int)m_AnimFrame, m_AnimationBlend);
}

//アニメーションのアップデート
void Player::UpdateAnimation()
{
	if (m_IsJump)
	{
		SetAnimation("Jump");
		m_AnimFrame += m_Speed;
	}
	else
	{
		if (m_IsShot)
		{
			SetAnimation("Shot");
			m_AnimFrame = m_ShotFrame;
		}
		else
		{
			if (m_IsRun)
			{
				SetAnimation("Run");
				m_AnimFrame += m_Speed * 2;
			}
			else
			{
				SetAnimation("Idle");
				m_AnimFrame += m_Speed * 2;
			}
		}
	}

	m_AnimationModel->Update(m_AnimationName.c_str(), (int)m_AnimFrame, m_AnimationNameNext.c_str(), (int)m_AnimFrame, m_AnimationBlend);

	m_AnimationBlend += BLEND_PER_FRAME;
	if (m_AnimationBlend > 1.0f)
		m_AnimationBlend = 1.0f;
}

//アニメーションのセット
void Player::SetAnimation(std::string animName)
{
	if (m_AnimationNameNext != animName)
	{
		m_AnimationName = m_AnimationNameNext;
		m_AnimationNameNext = animName;
		m_AnimationBlend = 0.0f;
	}
}

void Player::UpdateInvincible()
{
	if (!m_IsInvincible) return;

	m_InvincibleFrame -= m_Speed;

	if (m_InvincibleFrame <= 0)
	{
		m_IsInvincible = false;
	}
}

//回転のアップデート
void Player::UpdateRotation()
{
	if (!m_IsRotate)return;

	m_RotationFrame += m_Speed;

	float delta = m_Direction - m_OldDirection;

	while (delta > XM_PI)  delta -= XM_2PI;
	while (delta < -XM_PI) delta += XM_2PI;

	float t = (float)m_RotationFrame / ROT_MAX_FRAME;
	m_Rotation.y = m_OldDirection + delta * t;

	if (m_RotationFrame >= ROT_MAX_FRAME)
	{
		m_IsRotate = false;
		m_RotationFrame = 0;
	}
}

//当たり判定のアップデート
void Player::UpdateHitCollision()
{
	BoxCollision* bc = GetComponent<BoxCollision>();


	if (bc != nullptr)
	{
		std::vector<bool> isHit = bc->GetIsHit();

		if (isHit[0])//弱攻撃
		{
			if (!m_IsInvincible)
			{
				m_Hp-= LOW_REDUCE_HP;

				Camera::Shake(LOW_CAMERA_SHAKE);
				SpeedManager::HitStop(LOW_HITSTOP);
				DamageScreen::Damage();
				Player::DamageShake(LOW_PLAYER_SHAKE);

				m_InvincibleFrame = INVINCIBLE_MAX_FRAME;
				m_IsInvincible = true;
			}
		}

		if (isHit[1])//壁
		{
			std::vector<GameObject*> hits = bc->GetHitObjects(1);

			for (GameObject* hit : hits)
			{
				BoxCollision* bc = hit->GetComponent<BoxCollision>();

				Vector3 wallPos = bc->GetPosition();
				Vector3 wallScale = bc->GetScale();

				Vector3 direction = m_Position - wallPos;

				Vector3 axisX = bc->GetRight(); //x方向分離軸
				float dotX = Vector3::dot(direction, axisX);

				Vector3 axisZ = bc->GetForward();
				float dotZ = Vector3::dot(direction, axisZ);

				float radius = 1.0f;

				if (-radius * wallScale.x < dotX && dotX < radius * wallScale.x &&
					-radius * wallScale.z < dotZ && dotZ < radius * wallScale.z)
				{
					float dx = wallScale.x - fabs(dotX);
					float dz = wallScale.z - fabs(dotZ);

					if (dx < dz)
					{
						if (dotX > 0.0)
						{
							m_Position += axisX * dx;
						}
						else
						{
							m_Position -= axisX * dx;
						}

					}
					else
					{
						if (dotZ > 0.0)
						{
							m_Position += axisZ * dz;
						}
						else
						{
							m_Position -= axisZ * dz;
						}
					}
				}

			}
		}


		if (isHit[2])//強攻撃
		{
			if (!m_IsInvincible)
			{
				m_Hp -= HIGH_REDUCE_HP;

				Camera::Shake(HIGH_CAMERA_SHAKE);
				SpeedManager::HitStop(HIGH_HITSTOP);
				DamageScreen::Damage();
				Player::DamageShake(HIGH_PLAYER_SHAKE);

				m_InvincibleFrame = INVINCIBLE_MAX_FRAME;
				m_IsInvincible = true;
			}
		}
	}
}

//残像のアップデート
void Player::UpdateAfterEffect()
{
	if (m_IsSlow)
	{
		for (int num = AFTER_IMAGE_NUM - 1; num > 0; num--)
		{
			m_AfterImagePos[num] = m_AfterImagePos[num - 1];
			m_AfterImageRot[num] = m_AfterImageRot[num - 1];
			m_AfterImageAnimFrame[num] = m_AfterImageAnimFrame[num - 1];
			m_AfterImageAnimBlend[num] = m_AfterImageAnimBlend[num - 1];
		}

		m_AfterImagePos[0] = m_Position;
		m_AfterImageRot[0] = m_Rotation;
		m_AfterImageAnimFrame[0] = m_AnimFrame;
		m_AfterImageAnimBlend[0] = m_AnimationBlend;

		m_Aura->SetPosition(m_Position);
	}
	else
	{
		if (m_Aura != nullptr)
		{
			m_Aura->SetDestroy();
			m_Aura = nullptr;
		}
	}
}

void Player::DrawAfterImage()
{
	if (!m_IsSlow) return;

	for (int num = 0; num < AFTER_IMAGE_NUM; num++)
	{
		//マトリクス設定
		XMMATRIX world, scale, rot, trans;
		scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
		rot = XMMatrixRotationRollPitchYaw(m_AfterImageRot[num].x, m_AfterImageRot[num].y, m_AfterImageRot[num].z);
		trans = XMMatrixTranslation(m_AfterImagePos[num].x, m_AfterImagePos[num].y, m_AfterImagePos[num].z);
		world = scale * rot * trans;

		Renderer::SetWorldMatrix(world);
		m_AnimationModel->Draw(AFTERIMAGE_COLOR);
	}

	m_AnimationModel->Update(m_AnimationName.c_str(), (int)m_AnimFrame, m_AnimationNameNext.c_str(), (int)m_AnimFrame * 3, m_AnimationBlend);
}

void Player::DrawDebugSystem()
{
	ImGui::Begin("Debug");

	if (ImGui::Button("Dead")) {
		SetDeadAnimation();
	}

	ImGui::End();
}
