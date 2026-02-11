#include "Animation.h"
#include "Model.h"
#include "GameObject.h"
#include "Time.h"
#include "RenderingSystem.h"

using namespace DirectX;

Animator::Animator(const Model* model)
	: m_model(model)
{
	m_skeleton = &m_model->GetSkeleton();
}

void Animator::Play()
{
	m_isPlaying = true;
}

void Animator::Stop()
{
	m_isPlaying = false;
}

void Animator::ChangeAnimation(const std::string& clipName)
{
	if (!m_model)
	{
		return;
	}
	const auto& animations = m_model->GetAnimations();
	for (const auto& clip : animations)
	{
		if (clip.name == clipName)
		{
			SetAnimationClip(&clip);
			return;
		}
	}
}

void Animator::UpdateAnimation()
{
	if (!m_isPlaying || !m_currentClip || !m_skeleton)
	{
		return;
	}
	// 再生時間の更新
	m_currentTime += m_speed * (float)Time::DeltaTime();

	// ループ処理
	if (m_isLoop && m_currentTime > m_currentClip->duration)
	{
		m_currentTime = fmodf(m_currentTime, m_currentClip->duration);
	}
	else if (m_currentTime > m_currentClip->duration)
	{
		m_currentTime = m_currentClip->duration;
		m_isPlaying = false;
	}

	// ローカルポーズのサンプリング
	SampleLocalPose(m_currentTime);
	// グローバルポーズの計算
	ComputeGlobalPose();
	// スキンポーズの計算
	ComputeSkinPose();
}

void Animator::Bind(GraphicsDevice& device)
{
	// ボーン行列用構造化バッファの更新とバインド
	if (m_boneMatrixBuffer.GetElementCount() != m_skeleton->bones.size())
	{
		m_boneMatrixBuffer.CreateBuffer(device, sizeof(XMMATRIX), (UINT)m_skeleton->bones.size(), USAGE_TYPE::DEFAULT, VIEW_TYPE::SRV);
	}

	m_boneMatrixBuffer.UpdateBuffer(device, m_currentPose.skinMatrixes.data());
	m_boneMatrixBuffer.BindVS(device, 0);
}

void Animator::SetModel(Model* model)
{
	m_model = model;
	if (m_model)
	{
		m_skeleton = &m_model->GetSkeleton();
	}
}
void Animator::SetAnimationClip(const AnimationClip* clip)
{
	m_currentClip = clip;
	m_currentTime = 0.0f;
}

XMFLOAT3 Animator::SampleVec3(const std::vector<KeyframeVec3>& keyframes, float time)
{
	// キーフレームが存在しない場合や1つしかない場合の処理
	if (keyframes.empty()) 
		return {};
	if (keyframes.size() == 1)
		return keyframes[0].value;

	for (size_t i = 0; i < keyframes.size() - 1; i++)
	{
		// 指定時間がこの区間にある場合
		if (time >= keyframes[i].time && time <= keyframes[i + 1].time)
		{
			// 線形補間
			float t = (time - keyframes[i].time) / (keyframes[i + 1].time - keyframes[i].time);
			XMFLOAT3 start = keyframes[i].value;
			XMFLOAT3 end = keyframes[i + 1].value;
			XMFLOAT3 result{};
			XMVECTOR r = XMVectorLerp(XMLoadFloat3(&start), XMLoadFloat3(&end), t);
			XMStoreFloat3(&result, r);

			return result;
		}
	}
}

XMFLOAT4 Animator::SampleQuat(const std::vector<KeyframeQuat>& keyframes, float time)
{
	// キーフレームが存在しない場合や1つしかない場合の処理
	if (keyframes.empty())
		return {};
	if (keyframes.size() == 1)
		return keyframes[0].value;

	for (size_t i = 0; i < keyframes.size() - 1; i++)
	{
		// 指定時間がこの区間にある場合
		if (time >= keyframes[i].time && time <= keyframes[i + 1].time)
		{
			// 線形補間
			float t = (time - keyframes[i].time) / (keyframes[i + 1].time - keyframes[i].time);
			XMFLOAT4 start = keyframes[i].value;
			XMFLOAT4 end = keyframes[i + 1].value;
			XMFLOAT4 result{};
			XMVECTOR r = XMQuaternionSlerp(XMLoadFloat4(&start), XMLoadFloat4(&end), t);
			XMStoreFloat4(&result, r);

			return result;
		}
	}
}

void Animator::SampleLocalPose(float time)
{
	for (int i = 0; i < m_skeleton->bones.size(); ++i)
	{
		// ボーンアニメーションの取得
		const auto& boneAnimIt = m_currentClip->boneAnimations.find(i);
		if (boneAnimIt != m_currentClip->boneAnimations.end())
		{
			const BoneAnimation& boneAnim = boneAnimIt->second;

			// サンプリング
			XMFLOAT3 position = SampleVec3(boneAnim.positionKeyframes, time);
			XMFLOAT4 rotation = SampleQuat(boneAnim.rotationKeyframes, time);
			XMFLOAT3 scale = SampleVec3(boneAnim.scaleKeyframes, time);

			XMVECTOR posV = XMLoadFloat3(&position);
			XMVECTOR rotV = XMLoadFloat4(&rotation);
			XMVECTOR scaleV = XMLoadFloat3(&scale);

			// ローカルポーズに設定
			XMMATRIX localMatrix = 
				  XMMatrixScalingFromVector(scaleV) 
				* XMMatrixRotationQuaternion(rotV) 
				* XMMatrixTranslationFromVector(posV);

			m_currentPose.localMatrixes[i] = localMatrix;
		}
	}
}

void Animator::ComputeGlobalPose()
{
	// ルートボーンから再帰的にグローバルポーズを計算
	for (int i = 0; i < m_skeleton->bones.size(); ++i)
	{
		if (m_skeleton->bones[i].parentIndex == -1)
		{
			RecursiveComputeGlobalPose(i, gameObject().transform().GetWorldMatrix());
		}
	}
}

void Animator::RecursiveComputeGlobalPose(int boneIndex, const DirectX::XMMATRIX& parentMatrix)
{
	// ローカル行列を取得して親行列と掛け合わせる
	XMMATRIX current = m_currentPose.localMatrixes[boneIndex];
	m_currentPose.globalMatrixes[boneIndex] = current * parentMatrix;

	// 子ボーンに対して再帰的に処理
	for (int i = 0; i < m_skeleton->bones.size(); ++i)
	{
		if (m_skeleton->bones[i].parentIndex == boneIndex)
		{
			RecursiveComputeGlobalPose(i, m_currentPose.globalMatrixes[boneIndex]);
		}
	}
}

void Animator::ComputeSkinPose()
{
	for (int i = 0; i < m_skeleton->bones.size(); ++i)
	{
		// スキン行列 = 補正行列 * グローバル行列
		XMMATRIX offsetMatrix = m_skeleton->bones[i].offsetMatrix;
		XMMATRIX globalMatrix = m_currentPose.globalMatrixes[i];
		m_currentPose.skinMatrixes[i] = XMMatrixTranspose(offsetMatrix * globalMatrix);
	}
}
