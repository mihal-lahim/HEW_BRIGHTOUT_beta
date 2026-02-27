#ifndef ENEMY_H
#define ENEMY_H

#include "Movement.h"
#include "GameObject.h"
#include "Time.h"
#include "Ray.h"
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
	std::vector<GameObject*> deadModelObjects{};
	float IdleAnimationInterval = 0.35f;
	float MoveAnimationInterval = 0.12f;
	float DeadAnimationInterval = 0.1f;
	float DeadDuration = 0.5f;

	bool IsDefeated() const { return m_IsDefeated; }
	void OnDefeated()
	{
		if (m_IsDefeated)
			return;

		m_IsDefeated = true;
		m_DeadTimer = 0.0f;
		SetAnimationMode(AnimationMode::Dead);
	}

	void Start() override
	{
		Movement::Start();
		m_target = GetGameObjectByTag("Player");
		SetAnimationMode(AnimationMode::Idle);
	}

	void Update() override
	{
		if (gameObject().transform().position().y < FallDestroyY)
		{
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
			m_DeadTimer += (float)Time::DeltaTime();
			UpdateAnimation((float)Time::DeltaTime());
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
		if (length <= 0.1f)
		{
			MoveVec = { 0.0f, 0.0f, 0.0f };
			SetAnimationMode(AnimationMode::Idle);
			UpdateAnimation((float)Time::DeltaTime());
			return;
		}

		MoveVec = direction.Normalize() * MoveSpeed;
		RotateByMoveVec();
		SetAnimationMode(AnimationMode::Move);
		UpdateAnimation((float)Time::DeltaTime());
	}

private:
	enum class AnimationMode
	{
		Idle,
		Move,
		Dead,
	};

	GameObject* m_target = nullptr;
	AnimationMode m_AnimationMode = AnimationMode::Idle;
	bool m_IsDefeated = false;
	float m_DeadTimer = 0.0f;
	float m_IdleAnimationTimer = 0.0f;
	float m_MoveAnimationTimer = 0.0f;
	float m_DeadAnimationTimer = 0.0f;
	size_t m_IdleAnimationIndex = 0;
	size_t m_MoveAnimationIndex = 0;
	size_t m_DeadAnimationIndex = 0;

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

	void AdvanceAnimationGroup(std::vector<GameObject*>& models, size_t& index, float& timer, float interval, float deltaTime)
	{
		if (models.size() <= 1 || interval <= 0.0f)
			return;

		timer += deltaTime;
		if (timer < interval)
			return;

		timer -= interval;
		if (models[index])
			SetEnemyActiveRecursive(models[index], false);

		index = (index + 1) % models.size();
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
		else if (m_AnimationMode == AnimationMode::Dead)
		{
			if (!deadModelObjects.empty())
				AdvanceAnimationGroup(deadModelObjects, m_DeadAnimationIndex, m_DeadAnimationTimer, DeadAnimationInterval, deltaTime);
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
};

#endif
