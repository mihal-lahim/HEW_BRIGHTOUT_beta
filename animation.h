//////////////////////////////////
// animation.h
// アニメーション構造体とユーティリティ (Assimp用)
//////////////////////////////////
#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <DirectXMath.h>

// アニメーション キーフレーム構造体
struct KeyVec3
{
    double time = 0.0;
    DirectX::XMFLOAT3 value = { 0.0f, 0.0f, 0.0f };
};

struct KeyQuat
{
    double time = 0.0;
    DirectX::XMFLOAT4 value = { 0.0f, 0.0f, 0.0f, 1.0f };  // XMVECTOR の代わりに XMFLOAT4 を使用
};

// ボーン キーフレーム アニメーション データ
struct BoneKeyframes
{
    std::vector<KeyVec3> trans;  // 移動 キーフレーム
    std::vector<KeyVec3> scale;  // スケール キーフレーム
    std::vector<KeyQuat> rot;    // 回転 キーフレーム
};

// アニメーション クリップ（全ボーンアニメーションを含む）
struct AnimationClip
{
    double duration = 0.0;       // アニメーション期間（秒単位）
    double tps = 1.0;            // 1秒あたりのティック数
    std::vector<BoneKeyframes> tracks;  // ボーンごとに1つのエントリ
};

// 現在のアニメーション再生状態
struct AnimationState
{
    const AnimationClip* clip = nullptr;
    double time = 0.0;
    bool play = false;
    bool loop = true;
};

#endif // ANIMATION_H
