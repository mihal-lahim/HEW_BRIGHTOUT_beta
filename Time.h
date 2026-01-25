
#ifndef TIME_H
#define TIME_H

#include <chrono>


using clock = std::chrono::steady_clock;

class Time
{
private:
	// 初期化フラグ
    static inline bool s_Initialized = false;

	// 最終更新時刻
    static inline clock::time_point s_Last{};

	// 経過時間
    static inline float s_DeltaTime = 0.0f;

	// 固定更新間隔
    static inline float s_FixedDeltaTime = 1.0f / 60.0f; // デフォルト60FPS

	// 時間スケール（倍率）
    static inline float s_TimeScale = 1.0f;

	// 固定ステップ用アキュムレータ
    static inline float s_Accumulator = 0.0f;

public:

    // 経過時間（秒）
    static float DeltaTime() { return s_DeltaTime; }
    static float FixedDeltaTime() { return s_FixedDeltaTime; }
    static float TimeScale() { return s_TimeScale; }

    static void SetTimeScale(float scale) { s_TimeScale = scale; }
    static void SetFixedDeltaTime(float seconds) { s_FixedDeltaTime = seconds; }

    // フレーム開始時に呼ぶ
    static void BeginFrame()
    {
		// 現在時刻を取得
        auto now = clock::now();

		// 初期化済みなら経過時間を計算
        if (s_Initialized) 
        {
            std::chrono::duration<float> dt = now - s_Last;
            s_DeltaTime = dt.count() * s_TimeScale;
        } 
		// 初回フレームなら初期化
        else 
        {
            s_Initialized = true;
            s_DeltaTime = 0.0f;
        }

		// 最終更新時刻を更新
        s_Last = now;

		// アキュムレータに経過時間を加算
        s_Accumulator += s_DeltaTime;
    }

    // 固定ステップが溜まっているか
    static bool HasFixedStep()
    {
        return s_Accumulator >= s_FixedDeltaTime;
    }

    // 固定ステップを1消費
    static void ConsumeFixedStep()
    {
        if (s_Accumulator >= s_FixedDeltaTime)
            s_Accumulator -= s_FixedDeltaTime;
    }
};

#endif
