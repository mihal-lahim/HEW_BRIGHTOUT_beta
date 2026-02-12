#ifndef ANIMATION_H
#define ANIMATION_H

#include <DirectXMath.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "Component.h"
#include "StructuredBuffer.h"

// ボーン情報構造体
struct Bone
{
	std::string name = {};
	int parentIndex = -1;
	std::vector<int> childIndexes = {};
	// 補正用行列
	DirectX::XMMATRIX offsetMatrix = {};
};

// スケルトン構造体
struct Skeleton
{
	std::vector<Bone> bones = {};
	// ボーン名からインデックスへのマッピング
	std::unordered_map<std::string, int> boneNameToIndexMap = {};
};

// キーフレーム(ベクター3)構造体
struct KeyframeVec3
{
	float time = -1;
	DirectX::XMFLOAT3 value = {};
};

// キーフレーム(クォータニオン)構造体
struct KeyframeQuat
{
	float time = -1;
	// クォータニオン(x, y, z, w)
	DirectX::XMFLOAT4 value = {};
};

// 特定のボーンの特定のアニメーションデータ構造体
struct BoneAnimation
{
	std::vector<KeyframeVec3> positionKeyframes = {};
	std::vector<KeyframeQuat> rotationKeyframes = {};
	std::vector<KeyframeVec3> scaleKeyframes = {};
};

// アニメーション構造体
struct AnimationClip
{
	std::string name = {};
	// アニメーションの総時間（秒）
	float duration = -1;
	// アニメーションの時間単位（ティック/秒）
	float ticksPerSecond = -1;

	// ボーンインデックスからアニメーションデータへのマッピング
	std::unordered_map<int, BoneAnimation> boneAnimations = {};
};

// ポーズ構造体
struct Pose
{
	// ローカル行列群
	std::vector<DirectX::XMMATRIX> localMatrixes = {};
	// グローバル行列群
	std::vector<DirectX::XMMATRIX> globalMatrixes = {};
	// スキン行列群
	std::vector<DirectX::XMMATRIX> skinMatrixes = {};
};

class Model;

class Animator : public Component
{
public:
	Animator(const Model* model);

	// アニメーションの再生
	void Play();
	// アニメーションの停止
	void Stop();

	// アニメーションの切り替え
	void ChangeAnimation(const std::string& clipName);

	// アニメーションの更新
	void UpdateAnimation();

	// ボーン行列用構造化バッファのバインド
	void Bind(GraphicsDevice& device);

	// ループ設定の変更
	void SetModel(Model* model);

	// アニメーションクリップの設定
	void SetAnimationClip(const AnimationClip* clip);

	// ループ設定の変更
	bool& isLoop()
	{
		return m_isLoop;
	}

	// 再生速度の設定
	float& speed()
	{
		return m_speed;
	}

	// 再生状態の取得
	bool isPlaying() const
	{
		return m_isPlaying;
	}

	// 現在の再生時間の取得
	float currentTime() const
	{
		return m_currentTime;
	}

	// 現在のポーズの取得
	const Pose& currentPose() const
	{
		return m_currentPose;
	}


	void InitializeByContext() override;
private:
	// キーフレームのサンプリングメソッド
	DirectX::XMFLOAT3 SampleVec3(const std::vector<KeyframeVec3>& keyframes, float time);
	DirectX::XMFLOAT4 SampleQuat(const std::vector<KeyframeQuat>& keyframes, float time);

	// ローカルポーズのサンプリングメソッド
	void SampleLocalPose(float time);

	// グローバルポーズの計算メソッド
	void ComputeGlobalPose();
	void RecursiveComputeGlobalPose(int boneIndex, const DirectX::XMMATRIX& parentMatrix);

	// スキンポーズの計算メソッド
	void ComputeSkinPose();

	// スケルトンへの参照
	const Skeleton* m_skeleton = nullptr;
	// 現在のアニメーションクリップ
	const AnimationClip* m_currentClip = nullptr;

	// 現在のポーズ
	Pose m_currentPose = {};

	// 再生時間
	float m_currentTime = 0.0f;
	// 再生速度
	float m_speed = 1.0f;
	// 再生状態
	bool m_isPlaying = false;
	// ループ設定
	bool m_isLoop = true;

	// モデルへの参照
	const Model* m_model = nullptr;

	// ボーン行列用構造化バッファ
	StructuredBuffer m_boneMatrixBuffer = {};
};



#endif
