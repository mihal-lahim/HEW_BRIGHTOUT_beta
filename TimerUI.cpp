#include "TimerUI.h"
#include "GameObject.h"
#include "SceneSystem.h"
#include "Renderer.h"
#include "RenderingSystem.h"
#include "UIQuad.h"
#include "Mesh.h"
#include "Result.h"


#include <chrono>
#include <cmath>

using namespace std::chrono;

TimerUI::TimerUI()
{
    // 初期化は Start() へ
}

void TimerUI::Start()
{
    auto* scene = gameObject().scenePtr();
    auto& rendering = gameObject().rendering();
    GraphicsDevice& device = rendering.GetGraphicsDevice();

    // 番号テクスチャは 0..9 を横に並べた1枚画像。U幅は 1/10
    const float digitCount = 10.0f;
    const float uWidth = 1.0f / digitCount;

    // 0..9 の切り出しメッシュを作成（再利用する）
    m_digitMeshes.resize(10);
    for (int i = 0; i < 10; ++i)
    {
        float u0 = uWidth * i;
        float u1 = uWidth * (i + 1);
        m_digitMeshes[i] = CreateUIQuadWithUV(device, u0, u1);
    }

    // 桁数：MM:SS を想定して 4 桁（M10 M1 S10 S1）
    const int digits = 4;
    m_digitRenderers.resize(digits, nullptr);

    // タイマーの基準位置（画面座標, 調整可）
    Vector3 basePos = gameObject().transform().position(); // TimerUI を作成したオブジェクトの位置を基準にする想定

    // 横並びオフセット（
    const float digitSpacing = 60.0f;   // 調整可
    const float digitWidth = 50.0f;     // スケール値（x方向
    const float digitHeight = 100.0f;   // スケール値（y方向）

    // 左から tens of minutes ... ones of seconds (M10 M1 S10 S1)
    float startX = basePos.x - (digitSpacing * 1.5f);

    for (int i = 0; i < digits; ++i)
    {
        GameObject* digitObj = scene->CreateGameObject();
        digitObj->SetName(std::string("TimerDigit") + std::to_string(i));
        digitObj->transform().position() = Vector3(startX + i * digitSpacing, basePos.y, basePos.z);
        digitObj->transform().scale() = Vector3(digitWidth, digitHeight, 1.0f);

        MeshRenderer* renderer = digitObj->AddComponent<MeshRenderer>();
        // 初期メッシュ
        renderer->mesh = m_digitMeshes[0].get();
        renderer->renderQueue = RenderQueue::UI;

        // 既存の UiVS を使い、スクリーンスペースモードを有効化する（UiVS が UseScreenSpace を扱う前提）
        renderer->material.vsPath = "UiVS.cso";
        renderer->material.psPath = "UiPS.cso";
        renderer->material.texturePath = L"texture/BRIGHTOUT_Number_0-9.png";

        // 画面サイズを渡す + スクリーンスペースフラグ
        renderer->material.SetFloat("ScreenWidth", static_cast<float>(device.GetBackBufferWidth()));
        renderer->material.SetFloat("ScreenHeight", static_cast<float>(device.GetBackBufferHeight()));
        renderer->material.SetFloat("UseScreenSpace", 1.0f);

        m_digitRenderers[i] = renderer;
    }

    m_lastTime = steady_clock::now();
}

void TimerUI::Update()
{

    // 経過時間を計算して残り時間をデクリメント
    auto now = steady_clock::now();
    duration<float> delta = now - m_lastTime;
    m_lastTime = now;

    m_totalSeconds -= delta.count();
  if (m_totalSeconds < 0.0f) m_totalSeconds = 0.0f;

    // タイマーが0になったらリザルトシーンに遷移
    if (m_totalSeconds <= 0.0f && !m_hasTransitioned)
    {
 m_hasTransitioned = true;
        scene().ChangeScene<Result>();
        return;
    }

    int totalSec = static_cast<int>(std::floor(m_totalSeconds + 0.0001f));

    int minutes = totalSec / 60;
    int seconds = totalSec % 60;

    int m10 = (minutes / 10) % 10;
    int m1 = minutes % 10;
    int s10 = (seconds / 10) % 10;
    int s1 = seconds % 10;

    int digits[4] = { m10, m1, s10, s1 };

    // 各レンダラにメッシュを差し替え
    for (int i = 0; i < 4; ++i)
    {
        if (m_digitRenderers[i] && m_digitMeshes.size() > static_cast<size_t>(digits[i]))
        {
            m_digitRenderers[i]->mesh = m_digitMeshes[digits[i]].get();
            // マテリアルのパスが確実でない場合再設定（安全対策）
            m_digitRenderers[i]->material.texturePath = L"texture/BRIGHTOUT_Number_0-9.png";
            m_digitRenderers[i]->material.vsPath = "UiVS.cso";
            m_digitRenderers[i]->material.psPath = "UiPS.cso";
        }
    }
}