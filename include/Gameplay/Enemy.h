#ifndef ENEMY_H
#define ENEMY_H

#include "Movement.h"

#include "GameObject.h"
#include "GameTime.h"
#include "Health.h"
#include "Ray.h"
#include "Renderer.h"
#include <DirectXMath.h>
#include <algorithm>
#include <cmath>
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

inline void SetEnemyUVRotationRecursive(GameObject* obj, float uvRotation)
{
	if (!obj) return;

	auto meshRenderers = obj->GetComponents<MeshRenderer>();
	for (auto* renderer : meshRenderers)
	{
		renderer->material.SetFloat("uv_rotation", uvRotation);
	}

	auto skinnedRenderers = obj->GetComponents<SkinnedMeshRenderer>();
	for (auto* renderer : skinnedRenderers)
	{
		renderer->material.SetFloat("uv_rotation", uvRotation);
	}

	for (auto* child : obj->GetChildren())
	{
		SetEnemyUVRotationRecursive(child, uvRotation);
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

inline void SetEnemyUVRectRecursive(GameObject* obj, const DirectX::XMFLOAT4& uvRect)
{
	if (!obj) return;

	auto meshRenderers = obj->GetComponents<MeshRenderer>();
	for (auto* renderer : meshRenderers)
	{
		renderer->material.SetFloat4("uv_rect", uvRect);
	}

	auto skinnedRenderers = obj->GetComponents<SkinnedMeshRenderer>();
	for (auto* renderer : skinnedRenderers)
	{
		renderer->material.SetFloat4("uv_rect", uvRect);
	}

	for (auto* child : obj->GetChildren())
	{
		SetEnemyUVRectRecursive(child, uvRect);
	}
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
	int AttackEffectDirectionColumns = 1;
	int AttackEffectDirectionRows = 1;
	int AttackEffectDirectionFrameCount = 1;
	float AttackEffectDirectionYawOffset = 0.0f;

	// ê⁄ínîªíËÇÃä‘à¯Ç´ä‘äuÅiïbÅj
	static constexpr float GroundCheckInterval = 0.1f;

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


		// éÄñSSEì«Ç›çûÇ›
		m_DeathSE = LoadAudio("sound/enemy_die.wav"); 
		if (m_DeathSE >= 0)
		{
			SetAudioVolume(m_DeathSE, 0.7f);
		}

		// ÅiïKóvÇ»ÇÁë´âπÇ‡Ç±Ç±Ç≈ì«Ç›çûÇﬁÅj
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
			// É^Å[ÉQÉbÉgåüçıÇä‘à¯Ç´Åi0.5ïbä‘äuÅj
			m_targetSearchTimer -= deltaTime;
			if (m_targetSearchTimer <= 0.0f)
			{
				m_targetSearchTimer = 0.5f;
				m_target = GetGameObjectByTag("Player");
			}
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

			// í‚é~ÇµÇΩÇÁSEÇé~ÇﬂÇÈ
			if (m_isMoving && m_MoveSE >= 0)
			{
				StopAudio(m_MoveSE);
				m_isMoving = false;
			}

			return;
		}

		// à⁄ìÆäJénéûÇ…SEÇÉãÅ[ÉvÇ≈çƒê∂
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

	// ì|Ç≥ÇÍÇΩ/DestroyéûÇ…åƒÇŒÇÍÇÈ
	void OnDestroy() override
	{
		if (attackEffectObject)
		{
			DestroyEnemyRecursive(attackEffectObject);
			attackEffectObject = nullptr;
		}

		// à⁄ìÆíÜÇÃÉãÅ[ÉvSEÇé~ÇﬂÇÈ
		if (m_isMoving && m_MoveSE >= 0)
		{
			StopAudio(m_MoveSE);
			m_isMoving = false;
		}

		// éÄñSSEÇÉèÉìÉVÉáÉbÉgÇ≈çƒê∂
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

		ApplyAttackEffectDirectionUV(attackDirection);
	}

	void ApplyAttackEffectDirectionUV(const Vector3& attackDirection)
	{
		if (!attackEffectObject || AttackEffectDirectionColumns <= 0 || AttackEffectDirectionRows <= 0)
			return;

		const int maxFrames = AttackEffectDirectionColumns * AttackEffectDirectionRows;
		const int frameCount = (std::max)(1, (std::min)(AttackEffectDirectionFrameCount, maxFrames));
		Vector3 dir = attackDirection;
		if (dir.IsZero())
		{
			dir = Vector3(0.0f, 0.0f, 1.0f);
		}

		const float yaw = std::atan2(dir.x, dir.z) + DirectX::XMConvertToRadians(AttackEffectDirectionYawOffset);
		SetEnemyUVRotationRecursive(attackEffectObject, -yaw + DirectX::XM_PI);
		float normalized = (yaw + DirectX::XM_PI) / DirectX::XM_2PI;
		normalized -= std::floor(normalized);

		int frame = (int)std::floor(normalized * (float)frameCount + 0.5f);
		frame %= frameCount;

		const int column = frame % AttackEffectDirectionColumns;
		const int row = frame / AttackEffectDirectionColumns;

		const float width = 1.0f / (float)AttackEffectDirectionColumns;
		const float height = 1.0f / (float)AttackEffectDirectionRows;
		const float offsetX = width * (float)column;
		const float offsetY = height * (float)row;

		SetEnemyUVRectRecursive(attackEffectObject, { offsetX, offsetY, width, height });
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

	bool m_cachedIsOnGround = true;
	float m_groundCheckTimer = 0.0f;

	bool IsOnGround()
	{
		m_groundCheckTimer -= (float)Time::DeltaTime();
		if (m_groundCheckTimer <= 0.0f)
		{
			m_groundCheckTimer = GroundCheckInterval;
			Vector3 from = gameObject().transform().position();
			Ray ray(from, { 0.0f, -1.0f, 0.0f });
			physics().RayCast(ray, GroundRayLength);
			m_cachedIsOnGround = ray.IsHit && ray.HitDistance >= 0.0f && ray.HitDistance <= GroundDetectOffset;
		}
		return m_cachedIsOnGround;
	}

	int m_MoveSE = -1;
	bool m_isMoving = false;

	// ì|Ç≥ÇÍÇΩÇ∆Ç´ÇÃSE
	int m_DeathSE = -1;
	bool m_DestroyedByFall = false;

	float m_targetSearchTimer = 0.0f;

};

#endif
