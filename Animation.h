#ifndef ANIMATION_H
#define ANIMATION_H

#include <string>
#include <unordered_map>
#include <vector>
#include <DirectXMath.h>
#include "Component.h"
#include "Transform.h"

class Model;

// ボーン構造体
struct Bone
{
	std::string name = {};
	int parentIndex = -1;
	DirectX::XMMATRIX offsetMatrix = {};
	DirectX::XMMATRIX bindPose = {};
};

// スケルトン
struct Skeleton
{
	std::vector<Bone> bones = {};
	std::unordered_map<std::string, int> boneMap = {};

	// ボーン名からインデックスを取得するメソッド
	int FindBoneIndex(const std::string& boneName) const
	{
		auto it = boneMap.find(boneName);
		if (it != boneMap.end())
		{
			return it->second;
		}
		return -1;
	}
};

// ベクター3のキーフレーム構造体
struct keyframeVec3
{
	float time = 0.0f;
	DirectX::XMFLOAT3 value = {};
};

// クォータニオンのキーフレーム構造体
struct keyframeQuat
{
	float time = 0.0f;
	DirectX::XMFLOAT4 value = {};
};

// ボーンごとのキーフレーム群構造体
struct BoneKeyframes
{
	std::vector<keyframeVec3> positionKeyframes = {};
	std::vector<keyframeQuat> rotationKeyframes = {};
	std::vector<keyframeVec3> scaleKeyframes = {};

	// キーフレームが空かどうかを確認するメソッド
	bool IsEmpty() const
	{
		return positionKeyframes.empty() && rotationKeyframes.empty() && scaleKeyframes.empty();
	}
};

// ローカルポーズ構造体
struct LocalPose
{
	DirectX::XMFLOAT3 position = {};
	DirectX::XMFLOAT4 rotation = {};
	DirectX::XMFLOAT3 scale = {};
};

// アニメーションクリップ構造体
struct AnimationClip
{
	std::string name = {};
	float duration = 0.0f;
	float ticksPerSecond = 0.0f;
	std::vector<BoneKeyframes> boneTracks = {};

	// 指定したボーンのキーフレーム群を取得するメソッド
	const BoneKeyframes& GetBoneTrack(int boneIndex) const
	{
		return boneTracks[boneIndex];
	}

	// 指定した時間のローカルポーズをサンプリングするメソッド
	std::vector<LocalPose> Sample(float time) const;

	// ベクター3のキーフレームをサンプリングするヘルパーメソッド
	DirectX::XMFLOAT3 SampleVec3(const std::vector<keyframeVec3>& keyframes, float time) const;

	// クォータニオンのキーフレームをサンプリングするヘルパーメソッド
	DirectX::XMFLOAT4 SampleQuat(const std::vector<keyframeQuat>& keyframes, float time) const;
};


class AnimationController : public Component
{
public:
	void Setup(Model* model, const std::vector<Transform*>& boneTransforms);
	void Update();

	const std::vector<Transform*>& GetBoneTransforms() const
	{
		return m_boneTransforms;
	}

	const Skeleton* GetSkeleton() const
	{
		return m_skeleton;
	}

	// アニメーションの変更
	void ChangeAnimation(const std::string& clipName);

	// アニメーションの開始
	void StartAnimation();

	// アニメーションの停止
	void StopAnimation();

	// ループ設定
	void SetLoop(bool isLoop);

	// アニメーションのスピード設定
	void SetSpeed(float speed);
private:
	// モデル
	Model* m_model = nullptr;

	// 現在再生中のアニメーションクリップ
	const AnimationClip* m_currentClip = nullptr;

	// 管理するスケルトン
	const Skeleton* m_skeleton = nullptr;

	// 管理するボーンのTransform群
	std::vector<Transform*> m_boneTransforms = {};

	// アニメーション再生中かどうかのフラグ
	bool m_isPlaying = false;

	// ループ再生するかどうかのフラグ
	bool m_isLoop = true;

	// 現在の再生時間
	float m_currentTime = 0.0f;

	// 再生速度
	float m_speed = 1.0f;
};


#endif
