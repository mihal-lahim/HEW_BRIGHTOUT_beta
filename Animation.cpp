#include "Animation.h"
#include "Model.h"
#include "Time.h"
#include <algorithm>
#include <cmath>

std::vector<LocalPose> AnimationClip::Sample(float time) const
{
	std::vector<LocalPose> poses{};
	poses.resize(boneTracks.size());

	for (size_t i = 0; i < boneTracks.size(); i++)
	{
		const BoneKeyframes& keyframes = boneTracks[i];
		LocalPose& pose = poses[i];
		pose.position = SampleVec3(keyframes.positionKeyframes, time);
		pose.rotation = SampleQuat(keyframes.rotationKeyframes, time);
		pose.scale = SampleVec3(keyframes.scaleKeyframes, time);
	}

	return poses;
}

DirectX::XMFLOAT3 AnimationClip::SampleVec3(const std::vector<keyframeVec3>& keyframes, float time) const
{
	if (keyframes.empty())
	{
		return { 0.0f,0.0f,0.0f };
	}
	if (time <= keyframes.front().time)
	{
		return keyframes.front().value;
	}
	if (time >= keyframes.back().time)
	{
		return keyframes.back().value;
	}

	// ê¸å`ï‚ä‘
	for (int i = 0; i < static_cast<int>(keyframes.size()) - 1; i++)
	{
		if (time < keyframes[i + 1].time)
		{
			float t0 = keyframes[i].time;
			float t1 = keyframes[i + 1].time;
			float alpha = (time - t0) / (t1 - t0);

			DirectX::XMVECTOR a = DirectX::XMLoadFloat3(&keyframes[i].value);
			DirectX::XMVECTOR b = DirectX::XMLoadFloat3(&keyframes[i + 1].value);
			DirectX::XMVECTOR r = DirectX::XMVectorLerp(a, b, alpha);

			DirectX::XMFLOAT3 result{};
			DirectX::XMStoreFloat3(&result, r);
			return result;
		}
	}

	return keyframes.back().value;
}

DirectX::XMFLOAT4 AnimationClip::SampleQuat(const std::vector<keyframeQuat>& keyframes, float time) const
{
	if (keyframes.empty())
	{
		return { 0.0f,0.0f,0.0f,1.0f };
	}
	if (time <= keyframes.front().time)
	{
		return keyframes.front().value;
	}
	if (time >= keyframes.back().time)
	{
		return keyframes.back().value;
	}

	// ãÖñ ê¸å`ï‚ä‘
	for (int i = 0; i < static_cast<int>(keyframes.size()) - 1; i++)
	{
		if (time < keyframes[i + 1].time)
		{
			float t0 = keyframes[i].time;
			float t1 = keyframes[i + 1].time;
			float alpha = (time - t0) / (t1 - t0);

			DirectX::XMVECTOR a = DirectX::XMLoadFloat4(&keyframes[i].value);
			DirectX::XMVECTOR b = DirectX::XMLoadFloat4(&keyframes[i + 1].value);
			DirectX::XMVECTOR r = DirectX::XMQuaternionSlerp(a, b, alpha);

			DirectX::XMFLOAT4 result{};
			DirectX::XMStoreFloat4(&result, r);
			return result;
		}
	}

	return keyframes.back().value;
}

void AnimationController::ChangeAnimation(const std::string& clipName)
{
	if (!m_model)
	{
		return;
	}

	const auto& clips = m_model->GetAnimationClips();
	for (const auto& clip : clips)
	{
		if (clip.name == clipName)
		{
			m_currentClip = &clip;
			m_currentTime = 0.0f;
			return;
		}
	}
}

void AnimationController::StartAnimation()
{
	if (!m_currentClip && m_model)
	{
		const auto& clips = m_model->GetAnimationClips();
		if (!clips.empty())
		{
			m_currentClip = &clips.front();
		}
	}

	if (m_currentClip)
	{
		m_isPlaying = true;
		m_currentTime = 0.0f;
	}
}

void AnimationController::StopAnimation()
{
	m_isPlaying = false;
}

void AnimationController::SetLoop(bool isLoop)
{
	m_isLoop = isLoop;
}

void AnimationController::SetSpeed(float speed)
{
	m_speed = speed;
}

void AnimationController::Setup(Model* model, const std::vector<Transform*>& boneTransforms)
{
	m_model = model;
	m_skeleton = model ? &model->GetSkeleton() : nullptr;
	m_boneTransforms = boneTransforms;
	m_currentClip = nullptr;
	m_currentTime = 0.0f;

	if (m_model)
	{
		const auto& clips = m_model->GetAnimationClips();
		if (!clips.empty())
		{
			m_currentClip = &clips.front();
		}
	}
}

void AnimationController::Update()
{
	if (!m_isPlaying || !m_currentClip)
	{
		return;
	}

	if (m_currentClip->duration <= 0.0f || m_boneTransforms.empty())
	{
		return;
	}

	const float delta = static_cast<float>(Time::DeltaTime());
	m_currentTime += delta * m_speed * m_currentClip->ticksPerSecond;

	if (m_isLoop)
	{
		m_currentTime = std::fmod(m_currentTime, m_currentClip->duration);
	}
	else if (m_currentTime > m_currentClip->duration)
	{
		m_currentTime = m_currentClip->duration;
		m_isPlaying = false;
	}

	const auto& tracks = m_currentClip->boneTracks;
	const size_t count = std::min(tracks.size(), m_boneTransforms.size());
	for (size_t i = 0; i < count; ++i)
	{
		Transform* boneTransform = m_boneTransforms[i];
		if (!boneTransform)
		{
			continue;
		}

		const BoneKeyframes& keyframes = tracks[i];
		if (!keyframes.positionKeyframes.empty())
		{
			boneTransform->position().FromXMFLOAT3(m_currentClip->SampleVec3(keyframes.positionKeyframes, m_currentTime));
		}
		if (!keyframes.rotationKeyframes.empty())
		{
			boneTransform->rotation().FromXMFLOAT4(m_currentClip->SampleQuat(keyframes.rotationKeyframes, m_currentTime));
		}
		if (!keyframes.scaleKeyframes.empty())
		{
			boneTransform->scale().FromXMFLOAT3(m_currentClip->SampleVec3(keyframes.scaleKeyframes, m_currentTime));
		}
	}
}
