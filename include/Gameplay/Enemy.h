#ifndef ENEMY_H
#define ENEMY_H

#include "Movement.h"

#include "GameObject.h"
#include "GameTime.h"
#include "Health.h"
#include "Ray.h"
#include "Renderer.h"
#include <vector>

class GameObject;

inline void SetEnemyActiveRecursive(GameObject* obj, bool active)
{
	if (!obj) return;
	obj->SetActive(active);
	for (auto* child : obj->GetChildren())
	{
		SetEnemyActiveRecursive(child, active);
	}
}

inline void DestroyEnemyRecursive(GameObject* obj)
{
	if (!obj) return;
	for (auto* child : obj->GetChildren())
	{
		DestroyEnemyRecursive(child);
	}
	obj->SetActive(false);
	obj->Destroy();
}

#include "audio.h"


class Enemy : public Movement
{
public:
	float MoveSpeed = 3.0f;
	bool UseGravity = true;
	float Gravity = -30.0f;
	float FallDestroyY = -20.0f;
	float GroundDetectOffset = 0.6f;
	float GroundRayLength = 2.0f;
	GameObject* modelObject = nullptr;
	std::vector<GameObject*> idleModelObjects{};
	std::vector<GameObject*> moveModelObjects{};
	std::vector<GameObject*> attackModelObjects{};
	std::vector<GameObject*> deadModelObjects{};
	float IdleAnimationInterval = 0.35f;
	float MoveAnimationInterval = 0.12f;
	float AttackAnimationInterval = 0.12f;
	float DeadAnimationInterval = 0.1f;
	float DeadDuration = 0.5f;
	float AttackRange = 1.8f;
	float AttackDamage = 10.0f;
	float AttackInterval = 2.0f;
	GameObject* attackEffectObject = nullptr;
	float AttackEffectDuration = 0.2f;
	float AttackEffectForwardDistance = 0.6f;
	float AttackEffectHeightOffset = 0.0f;
	float AttackEffectPitch = 180.0f;

	bool IsDefeated() const { return m_IsDefeated; }
	void OnDefeated()
	{
		if (m_IsDefeated)
			return;

		m_IsDefeated = true;
		m_DeadTimer = 0.0f;
		if (attackEffectObject)
		{
			SetEnemyActiveRecursive(attackEffectObject, false);
		}
		SetAnimationMode(AnimationMode::Dead);
	}

	void Start() override
	{
		Movement::Start();
		m_target = GetGameObjectByTag("Player");

		SetAnimationMode(AnimationMode::Idle);


		// Ž€–SSE“Ç‚Ýž‚Ý
		m_DeathSE = LoadAudio("sound/enemy_die.wav"); 
		if (m_DeathSE >= 0)
		{
			SetAudioVolume(m_DeathSE, 0.7f);
		}

		// i•K—v‚È‚ç‘«‰¹‚à‚±‚±‚Å“Ç‚Ýž‚Þj
		// m_MoveSE = LoadAudio("sound/enemy_step.wav");
		if (m_MoveSE >= 0)
		{
			SetAudioVolume(m_MoveSE, 0.4f);
		}

	}

	void Update() override
	{
		float deltaTime = (float)Time::DeltaTime();
		if (m_AttackCooldownTimer > 0.0f)
		{
			m_AttackCooldownTimer -= deltaTime;
			if (m_AttackCooldownTimer < 0.0f)
			{
				m_AttackCooldownTimer = 0.0f;
			}
		}
		UpdateAttackEffect(deltaTime);

		if (gameObject().transform().position().y < FallDestroyY)
		{
			m_DestroyedByFall = true;
			DestroyEnemyRecursive(&gameObject());
			return;
		}

		bool isOnGround = IsOnGround();
		if (UseGravity && !isOnGround)
		{
			ForceVec.y += Gravity;
		}
		else if (isOnGround && VelocityVec.y < 0.0f)
		{
			VelocityVec.y = 0.0f;
		}

		if (m_IsDefeated)
		{
			m_DeadTimer += deltaTime;
			UpdateAnimation(deltaTime);
			if (m_DeadTimer >= DeadDuration)
			{
				DestroyEnemyRecursive(&gameObject());
			}
			return;
		}

		if (!m_target)
		{
			m_target = GetGameObjectByTag("Player");
			if (!m_target)
			{
				return;
			}
		}

		Vector3 direction = m_target->transform().position() - gameObject().transform().position();
		direction.y = 0.0f;

		float length = direction.Length();
		if (length <= AttackRange)
		{
			MoveVec = { 0.0f, 0.0f, 0.0f };

			if (length > 0.001f)
			{
				MoveVec = direction.Normalize();
				RotateByMoveVec();
				MoveVec = { 0.0f, 0.0f, 0.0f };
			}

			SetAnimationMode(AnimationMode::Attack);
			UpdateAnimation(deltaTime);
			TryAttackTarget();

			if (m_isMoving && m_MoveSE >= 0)
			{
				StopAudio(m_MoveSE);
				m_isMoving = false;
			}

			return;
		}

		if (length <= 0.1f)
		{
			MoveVec = { 0.0f, 0.0f, 0.0f };

			SetAnimationMode(AnimationMode::Idle);
			UpdateAnimation(deltaTime);

			// ’âŽ~‚µ‚½‚çSE‚ðŽ~‚ß‚é
			if (m_isMoving && m_MoveSE >= 0)
			{
				StopAudio(m_MoveSE);
				m_isMoving = false;
			}

			return;
		}

		// ˆÚ“®ŠJŽnŽž‚ÉSE‚ðƒ‹[ƒv‚ÅÄ¶
		if (!m_isMoving && m_MoveSE >= 0)
		{
			PlayAudio(m_MoveSE, true);
			m_isMoving = true;
		}

		MoveVec = direction.Normalize() * MoveSpeed;
		RotateByMoveVec();
		SetAnimationMode(AnimationMode::Move);
		UpdateAnimation(deltaTime);
	}

	// “|‚³‚ê‚½/DestroyŽž‚ÉŒÄ‚Î‚ê‚é
	void OnDestroy() override
	{
		if (attackEffectObject)
		{
			DestroyEnemyRecursive(attackEffectObject);
			attackEffectObject = nullptr;
		}

		// ˆÚ“®’†‚Ìƒ‹[ƒvSE‚ðŽ~‚ß‚é
		if (m_isMoving && m_MoveSE >= 0)
		{
			StopAudio(m_MoveSE);
			m_isMoving = false;
		}

		// Ž€–SSE‚ðƒƒ“ƒVƒ‡ƒbƒg‚ÅÄ¶
		if (!m_DestroyedByFall && m_DeathSE >= 0)
		{
			PlayAudioOneShot(m_DeathSE, 0.9f);
		}

		Movement::OnDestroy();
	}

private:
	enum class AnimationMode
	{
		Idle,
		Move,
		Attack,
		Dead,
	};

	GameObject* m_target = nullptr;

	AnimationMode m_AnimationMode = AnimationMode::Idle;
	bool m_IsDefeated = false;
	float m_DeadTimer = 0.0f;
	float m_IdleAnimationTimer = 0.0f;
	float m_MoveAnimationTimer = 0.0f;
	float m_AttackAnimationTimer = 0.0f;
	float m_DeadAnimationTimer = 0.0f;
	float m_AttackCooldownTimer = 0.0f;
	float m_AttackEffectTimer = 0.0f;
	size_t m_IdleAnimationIndex = 0;
	size_t m_MoveAnimationIndex = 0;
	size_t m_AttackAnimationIndex = 0;
	size_t m_DeadAnimationIndex = 0;

	void UpdateAttackEffect(float deltaTime)
	{
		if (!attackEffectObject)
			return;

		if (m_AttackEffectTimer > 0.0f)
		{
			UpdateAttackEffectTransform();
			m_AttackEffectTimer -= deltaTime;
			if (m_AttackEffectTimer <= 0.0f)
			{
				m_AttackEffectTimer = 0.0f;
				SetEnemyActiveRecursive(attackEffectObject, false);
			}
		}
	}

	void TriggerAttackEffect()
	{
		if (!attackEffectObject)
			return;
		UpdateAttackEffectTransform();

		SetEnemyActiveRecursive(attackEffectObject, true);

		m_AttackEffectTimer = AttackEffectDuration > 0.0f ? AttackEffectDuration : 0.05f;
	}

	void UpdateAttackEffectTransform()
	{
		if (!attackEffectObject)
			return;

		Vector3 attackDirection = Vector3(0.0f, 0.0f, 1.0f);
		if (m_target)
		{
			attackDirection = m_target->transform().position() - gameObject().transform().position();
			attackDirection.y = 0.0f;
		}

		if (!attackDirection.IsZero())
		{
			attackDirection = attackDirection.Normalize();
		}

		attackEffectObject->transform().position() = gameObject().transform().position()
			+ (attackDirection * AttackEffectForwardDistance)
			+ Vector3(0.0f, AttackEffectHeightOffset, 0.0f);
	}

	void TryAttackTarget()
	{
		if (!m_target || m_AttackCooldownTimer > 0.0f)
			return;

		auto* targetHealth = m_target->GetComponent<Health>();
		if (!targetHealth || !targetHealth->IsAlive())
			return;

		targetHealth->TakeDamage(AttackDamage);
		TriggerAttackEffect();
		m_AttackCooldownTimer = AttackInterval >= 2.0f ? AttackInterval : 2.0f;
	}

	void HideAllModels()
	{
		for (auto* model : idleModelObjects)
		{
			if (model) SetEnemyActiveRecursive(model, false);
		}
		for (auto* model : moveModelObjects)
		{
			if (model) SetEnemyActiveRecursive(model, false);
		}
		for (auto* model : attackModelObjects)
		{
			if (model) SetEnemyActiveRecursive(model, false);
		}
		for (auto* model : deadModelObjects)
		{
			if (model) SetEnemyActiveRecursive(model, false);
		}
	}

	void ResetAnimationGroup(std::vector<GameObject*>& models, size_t& index, float& timer)
	{
		timer = 0.0f;
		index = 0;
		for (size_t i = 0; i < models.size(); ++i)
		{
			if (models[i])
				SetEnemyActiveRecursive(models[i], i == 0);
		}
	}

	void AdvanceAnimationGroup(std::vector<GameObject*>& models, size_t& index, float& timer, float interval, float deltaTime, bool loop = true)
	{
		if (models.size() <= 1 || interval <= 0.0f)
			return;

		timer += deltaTime;
		if (timer < interval)
			return;

		timer -= interval;
		if (!loop && index + 1 >= models.size())
			return;

		if (models[index])
			SetEnemyActiveRecursive(models[index], false);

		index = loop ? (index + 1) % models.size() : (index + 1);
		if (models[index])
			SetEnemyActiveRecursive(models[index], true);
	}

	void SetAnimationMode(AnimationMode mode)
	{
		if (m_AnimationMode == mode)
			return;

		m_AnimationMode = mode;
		HideAllModels();

		if (m_AnimationMode == AnimationMode::Move)
		{
			ResetAnimationGroup(moveModelObjects, m_MoveAnimationIndex, m_MoveAnimationTimer);
		}
		else if (m_AnimationMode == AnimationMode::Attack)
		{
			ResetAnimationGroup(attackModelObjects, m_AttackAnimationIndex, m_AttackAnimationTimer);
		}
		else if (m_AnimationMode == AnimationMode::Dead)
		{
			ResetAnimationGroup(deadModelObjects, m_DeadAnimationIndex, m_DeadAnimationTimer);
		}
		else
		{
			ResetAnimationGroup(idleModelObjects, m_IdleAnimationIndex, m_IdleAnimationTimer);
		}
	}

	void UpdateAnimation(float deltaTime)
	{
		if (deltaTime <= 0.0f)
			return;

		if (m_AnimationMode == AnimationMode::Move)
		{
			if (!moveModelObjects.empty())
				AdvanceAnimationGroup(moveModelObjects, m_MoveAnimationIndex, m_MoveAnimationTimer, MoveAnimationInterval, deltaTime);
		}
		else if (m_AnimationMode == AnimationMode::Attack)
		{
			if (!attackModelObjects.empty())
				AdvanceAnimationGroup(attackModelObjects, m_AttackAnimationIndex, m_AttackAnimationTimer, AttackAnimationInterval, deltaTime);
		}
		else if (m_AnimationMode == AnimationMode::Dead)
		{
			if (!deadModelObjects.empty())
				AdvanceAnimationGroup(deadModelObjects, m_DeadAnimationIndex, m_DeadAnimationTimer, DeadAnimationInterval, deltaTime, false);
		}
		else
		{
			if (!idleModelObjects.empty())
				AdvanceAnimationGroup(idleModelObjects, m_IdleAnimationIndex, m_IdleAnimationTimer, IdleAnimationInterval, deltaTime);
		}
	}

	bool IsOnGround()
	{
		Vector3 from = gameObject().transform().position();
		Ray ray(from, { 0.0f, -1.0f, 0.0f });
		physics().RayCast(ray, GroundRayLength);
		return ray.IsHit && ray.HitDistance >= 0.0f && ray.HitDistance <= GroundDetectOffset;
	}

	int m_MoveSE = -1;
	bool m_isMoving = false;

	// “|‚³‚ê‚½‚Æ‚«‚ÌSE
	int m_DeathSE = -1;
	bool m_DestroyedByFall = false;

};

#endif
