///////////////////////////////////////////
//弾管理と自動消滅を制御するBulletクラスの実装
///////////////////////////////////////////

#include "Bullet.h"
#include "GameTime.h"
#include "GameObject.h"
#include "ScoreData.h"
#include "Enemy.h"

#include "Ray.h"
#include "PhysicsSystem.h"
#include "PhysicsBody.h"
#include "Renderer.h"
#include "RenderingSystem.h"
#include "Billboard.h"

#include "audio.h"
#include <algorithm>


namespace
{
	int g_BulletHitSE = -1;
	bool g_BulletHitSEInitialized = false;
}

void Bullet::ResetAudioState()
{
	g_BulletHitSE = -1;
	g_BulletHitSEInitialized = false;
}

namespace
{
	class BulletSheetAnimator : public ScriptComponent
	{
	public:
		int SheetColumns = 6;
		int SheetRows = 6;
		int SheetFrameCount = 31;
		float AnimationFps = 12.5f;
		bool AnimationLoop = true;
		MeshRenderer* Renderer = nullptr;
	private:
		float m_Timer = 0.0f;
		void ApplyFrame(int frame)
		{
			if (!Renderer)
			{
				return;
			}

			const int columns = (std::max)(1, SheetColumns);
			const int rows = (std::max)(1, SheetRows);
			const int maxFrames = columns * rows;
			const int frameCount = (std::max)(1, (std::min)(SheetFrameCount, maxFrames));
			const int clampedFrame = (std::max)(0, (std::min)(frame, frameCount - 1));

			const int col = clampedFrame % columns;
			const int row = clampedFrame / columns;
			const float width = 1.0f / (float)columns;
			const float height = 1.0f / (float)rows;
			Renderer->material.SetFloat4("uv_rect", { width * (float)col, height * (float)row, width, height });
		}
	public:
		void Start() override
		{
			ApplyFrame(0);
		}

		void Update() override
		{
			m_Timer += (float)Time::DeltaTime();

			const int columns = (std::max)(1, SheetColumns);
			const int rows = (std::max)(1, SheetRows);
			const int maxFrames = columns * rows;
			const int frameCount = (std::max)(1, (std::min)(SheetFrameCount, maxFrames));
			if (frameCount <= 1 || AnimationFps <= 0.0f)
			{
				return;
			}

			int frame = (int)(m_Timer * AnimationFps);
			if (AnimationLoop)
			{
				frame %= frameCount;
			}
			else
			{
				frame = (std::min)(frame, frameCount - 1);
			}
			ApplyFrame(frame);
		}
	};

	void DestroyWithChildren(GameObject& obj)
	{
		for (auto* child : obj.GetChildren())
		{
			if (child) DestroyWithChildren(*child);
		}
		obj.SetActive(false);
		obj.Destroy();
	}

	DirectX::XMFLOAT4 MakeSheetUVRect(int frame, int columns, int rows, int frameCount)
	{
		const int safeColumns = (std::max)(1, columns);
		const int safeRows = (std::max)(1, rows);
		const int maxFrames = safeColumns * safeRows;
		const int safeFrameCount = (std::max)(1, (std::min)(frameCount, maxFrames));
		const int clampedFrame = (std::max)(0, (std::min)(frame, safeFrameCount - 1));
		const int col = clampedFrame % safeColumns;
		const int row = clampedFrame / safeColumns;
		const float width = 1.0f / (float)safeColumns;
		const float height = 1.0f / (float)safeRows;
		return { width * (float)col, height * (float)row, width, height };
	}
}

void Bullet::Start()
{
	m_StartPos = gameObject().transform().position();
	if (!g_BulletHitSEInitialized)
	{
		g_BulletHitSE = LoadAudio("sound/bullet_hit.wav");
		g_BulletHitSEInitialized = true;
	}
	m_HitSE = g_BulletHitSE;

	GameObject* bulletVisual = gameObject().CreateGameObject();
	if (bulletVisual)
	{
		gameObject().SetChild(*bulletVisual);
		bulletVisual->transform().scale() = VisualScale;

		auto* renderer = bulletVisual->AddComponent<MeshRenderer>();
		renderer->mesh = bulletVisual->rendering().CreateBillboardQuad().get();
		renderer->material.texturePath = TexturePath;
		renderer->material.texture = nullptr;
		renderer->material.vsPath = "BillboardVS.cso";
		renderer->material.psPath = "BillboardPS.cso";
		renderer->material.shaderProgram = nullptr;
		renderer->renderQueue = RenderQueue::Transparent;
		renderer->material.SetColor({ VisualColorScale, VisualColorScale, VisualColorScale, VisualAlpha });

		auto* animator = bulletVisual->AddComponent<BulletSheetAnimator>();
		animator->SheetColumns = TrailSheetColumns;
		animator->SheetRows = TrailSheetRows;
		animator->SheetFrameCount = TrailSheetFrameCount;
		animator->AnimationFps = TrailAnimationFps;
		animator->AnimationLoop = TrailAnimationLoop;
		animator->Renderer = renderer;

		auto* billboard = bulletVisual->AddComponent<Billboard>();
		billboard->uvRotation = EffectUVRotation;
	}
}

void Bullet::Update()
{
	float dt = (float)Time::DeltaTime();
	m_LifeTimer += dt;
	if (m_LifeTimer >= LifeTime)
	{
		DestroyWithChildren(gameObject());
		return;
	}

	Vector3 diff = gameObject().transform().position() - m_StartPos;
	if (diff.Length() >= MaxDistance)
	{
		DestroyWithChildren(gameObject());
		return;
	}

	Vector3 dir = Direction.IsZero() ? Vector3(0.0f, 0.0f, 1.0f) : Direction.Normalize();
	Vector3 curPos = gameObject().transform().position();
	float stepDist = Speed * dt + HitRadius;

	// RayCastで進行方向のヒット判定（BVH最適化済み）
	Ray ray(curPos, dir);
	physics().RayCast(ray, stepDist);

	if (ray.IsHit && ray.HitObject)
	{
		auto& hitGameObject = ray.HitObject->gameObject();
		if (hitGameObject.CompareTag("Enemy") && hitGameObject.IsActiveInHierarchy())
		{
			auto* enemyComponent = hitGameObject.GetComponent<Enemy>();
			if (enemyComponent && enemyComponent->IsDefeated())
			{
				return;
			}

			// ヒットSE再生
			if (m_HitSE >= 0)
			{
				PlayAudio(m_HitSE, false);
				SetAudioVolume(m_HitSE, HitSEVolume);
			}

			if (enemyComponent)
			{
				ScoreData::Instance().killedEnemies++;
				enemyComponent->OnDefeated();
				DestroyWithChildren(gameObject());
				return;
			}
		}
	}

	Vector3 nextPos = curPos + dir * (Speed * dt);
	gameObject().transform().position() = nextPos;
}
