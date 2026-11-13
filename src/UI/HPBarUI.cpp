#include "HPBarUI.h"
#include "SceneSystem.h"
#include "Renderer.h"
#include "UIQuad.h"
#include "Mesh.h"
#include "RenderingSystem.h"
#include "Health.h"
#include "Player.h"
#include <algorithm>
#include <sstream>
#include <string>
#include <Windows.h>
#include <iomanip>

using namespace std;

static inline std::string Narrow(const std::wstring& ws)
{
    if (ws.empty()) return std::string();
    int required = ::WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (required <= 0) return std::string();
    std::string result;
    result.resize(required - 1); // required は終端ヌルを含む
    ::WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, &result[0], required, nullptr, nullptr);
    return result;
};

void HPBarUI::Start()
{
    auto* scene = gameObject().scenePtr();
    auto& rendering = gameObject().rendering();

    // GraphicsDevice を取得
    GraphicsDevice& device = rendering.GetGraphicsDevice();
    float screenW = static_cast<float>(device.GetBackBufferWidth());
    float screenH = static_cast<float>(device.GetBackBufferHeight());

    // レガシー互換: ユーザーが position のみを設定している場合のフォールバック
    // （背景・ゲージ両方が position と等しいときに既存の単一 position を尊重）
    if (backgroundPosition == position && fillPosition == position)
    {
        backgroundPosition = position;
        fillPosition = position;
    }

    // =========================
    // 背景
    // =========================
    m_bgObj = scene->CreateGameObject();
    m_bgObj->SetName("HPBar_Background");
    m_bgObj->transform().position() = backgroundPosition;
    m_bgObj->transform().scale() =
        (!backgroundScale.IsZero()) ? backgroundScale : scale;

    m_bgRenderer = m_bgObj->AddComponent<MeshRenderer>();

    // CreateUIQuad の shared_ptr を保持しておく（以前は .get() のみで一時が破棄されていた）
    m_fullMesh = rendering.CreateUIQuad();
    m_bgRenderer->mesh = m_fullMesh.get();

    m_bgRenderer->renderQueue = RenderQueue::UI;
    m_bgRenderer->material.texturePath = backgroundTexture;
    m_bgRenderer->material.vsPath = vsPath;
    m_bgRenderer->material.psPath = psPath;

    // スクリーンスペースフラグと画面サイズを設定
    m_bgRenderer->material.SetFloat("UseScreenSpace", 1.0f);
    m_bgRenderer->material.SetFloat("ScreenWidth", screenW);
    m_bgRenderer->material.SetFloat("ScreenHeight", screenH);

    // =========================
    // ゲージ
    // =========================
    m_fillObj = scene->CreateGameObject();
    m_fillObj->SetName("HPBar_Fill");
    m_fillObj->transform().position() = fillPosition;
    m_fillObj->transform().scale() =
        (!fillScale.IsZero()) ? fillScale : scale;

    m_fillRenderer = m_fillObj->AddComponent<MeshRenderer>();

    m_fillMesh = rendering.CreateUIQuad();
    m_fillRenderer->mesh = m_fillMesh.get();

    m_fillRenderer->renderQueue = RenderQueue::UI;
    m_fillRenderer->material.texturePath = fillTexture;
    m_fillRenderer->material.vsPath = vsPath;
    m_fillRenderer->material.psPath = psPath;

    m_fillRenderer->material.SetFloat("UseScreenSpace", 1.0f);
    m_fillRenderer->material.SetFloat("ScreenWidth", screenW);
    m_fillRenderer->material.SetFloat("ScreenHeight", screenH);

    // 初期位置を保持（Update で左端固定する補正に使う）
    m_fillOriginalPosition = m_fillObj->transform().position();

    // m_fillObj を作成して位置・スケールを設定した直後に追加
    m_fillObj->transform().position() = fillPosition;
    m_fillObj->transform().scale() =
        (!fillScale.IsZero()) ? fillScale : scale;

    // 左に10移動（必要なだけ値を変更）
    m_fillObj->transform().position().x -= 10.0f;

    // 元位置も更新しておく（Update での補正に使う）
    m_fillOriginalPosition = m_fillObj->transform().position();

    auto healths = scene->GetComponents<Health>();
    for (auto* h : healths)
    {
        if (!h) continue;
        if (h->gameObject().GetComponent<Player>() != nullptr)
        {
            m_playerHealth = h;
            break;
        }
    }
    if (!m_playerHealth && !healths.empty())
        m_playerHealth = healths.front();

    m_bgOriginalScale = m_bgObj->transform().scale();
    m_fillOriginalScale = m_fillObj->transform().scale();
}

void HPBarUI::Update()
{
    if (!m_playerHealth || !m_fillObj)
        return;

    float cur = m_playerHealth->GetCurrentHealth();
    float max = m_playerHealth->GetMaxHealth();

    float percent = (max > 0.0f)
        ? std::clamp(cur / max, 0.0f, 1.0f)
        : 0.0f;

    // 整数パーセント単位で変化があった時だけメッシュを再生成（毎フレーム生成を避ける）
    int curPercent = static_cast<int>(percent * 100.0f + 0.5f);
    if (curPercent != m_lastPercent)
    {
     m_lastPercent = curPercent;

        // 左端固定で右側だけ縮める:
     // スケールXを percent 倍に縮小し、UV の U 範囲も 0~percent にクリッピングする。
    // これによりテクスチャが横方向に圧縮されず、左端から正しく表示される。
        Vector3 newScale = m_fillOriginalScale;
        newScale.x = m_fillOriginalScale.x * percent;
  m_fillObj->transform().scale() = newScale;

        // UV を 0~percent にしたメッシュを再生成してテクスチャの圧縮を防ぐ
        if (percent > 0.0f)
        {
       GraphicsDevice& device = gameObject().rendering().GetGraphicsDevice();
      m_fillMesh = ::CreateUIQuadWithUV(device, 0.0f, percent);
    m_fillRenderer->mesh = m_fillMesh.get();
        }
    }

    m_fillObj->transform().position() = m_fillOriginalPosition;

    // HP0で完全に消す
    m_fillRenderer->SetEnable(percent > 0.0f);
}

// ランタイムで背景位置を変更（インスペクタから変更された直後に呼ぶ場合やスクリプトから変更する場合に使用）
void HPBarUI::SetBackgroundPosition(const Vector3& pos)
{
    backgroundPosition = pos;
    if (m_bgObj)
    {
        m_bgObj->transform().position() = pos;
    }
}

// ランタイムでゲージ位置を変更（左端固定の基準位置も更新）
void HPBarUI::SetFillPosition(const Vector3& pos)
{
    fillPosition = pos;
    if (m_fillObj)
    {
        m_fillObj->transform().position() = pos;
        m_fillOriginalPosition = m_fillObj->transform().position();
    }
}