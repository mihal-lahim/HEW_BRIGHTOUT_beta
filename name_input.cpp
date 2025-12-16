#include "name_input.h"
#include "key_logger.h"
#include "texture.h"
#include "sprite.h"
#include "fade.h"
#include "direct3d.h"
#include "math.h"
#include "scene.h"
#include "controller.h"
#include <windows.h>
#include <vector>

// ------------------------------------------------------------
// 選択したキーの履歴を保存
// ------------------------------------------------------------
struct SelectedKey {
    int row;
    int col;
};
static std::vector<SelectedKey> g_SelectedKeys;

// テクスチャ
static int g_TexKeyboardId = -1;
static int g_TexFrameId = -1;

// キーボード表示フラグ（NameInputでは常にTRUE）
static bool g_ShowKeyboard = true;

// カーソル位置
static int cursorRow = 0;
static int cursorCol = 0;

static Controller g_Controller(0);

// ひらがなマップ
static const char* HIRAGANA_MAP[5][10] =
{
    { "わ","ら","や","ま","は","な","た","さ","か","あ" },
    { "を","り","  ","み","ひ","に","ち","し","き","い" },
    { "ん","る","ゆ","む","ふ","ぬ","つ","す","く","う" },
    { "ー","れ","  ","め","へ","ね","て","せ","け","え" },
    { "小","ろ","よ","も","ほ","の","と","そ","こ","お" }
};


void NameInput_Initialize()
{
    g_TexKeyboardId = Texture_Load(L"texture/keyboard.png");
    g_TexFrameId = Texture_Load(L"texture/frame.png");

    cursorRow = 0;
    cursorCol = 0;

    g_SelectedKeys.clear();

    // NameInput シーンなので常にキーボード表示
    g_ShowKeyboard = true;

    g_Controller.Start();
}

void NameInput_Finalize()
{
}

void NameInput_Update(double elapsed_time)
{
    g_Controller.Update();

    if (g_ShowKeyboard)
    {
        // --------- カーソル移動（IJKL） ---------
        if (KeyLogger_IsTrigger(KK_I)) cursorRow = std::max(0, cursorRow - 1);
        if (KeyLogger_IsTrigger(KK_K)) cursorRow = std::min(4, cursorRow + 1);
        if (KeyLogger_IsTrigger(KK_J)) cursorCol = std::max(0, cursorCol - 1);
        if (KeyLogger_IsTrigger(KK_L)) cursorCol = std::min(9, cursorCol + 1);

        if (g_Controller.WasPressed(Controller::BUTTON_DPAD_UP))
            cursorRow = std::max(0, cursorRow - 1);

        if (g_Controller.WasPressed(Controller::BUTTON_DPAD_DOWN))
            cursorRow = std::min(4, cursorRow + 1);

        if (g_Controller.WasPressed(Controller::BUTTON_DPAD_LEFT))
            cursorCol = std::max(0, cursorCol - 1);

        if (g_Controller.WasPressed(Controller::BUTTON_DPAD_RIGHT))
            cursorCol = std::min(9, cursorCol + 1);

        // --------- Enter：文字追加（最大５文字まで） ---------
        if (KeyLogger_IsTrigger(KK_SPACE) ||
            g_Controller.WasPressed(Controller::BUTTON_A))
        {
            if (g_SelectedKeys.size() < 5)
            {
                if (HIRAGANA_MAP[cursorRow][cursorCol][0] != '\0')
                {
                    g_SelectedKeys.push_back({ cursorRow, cursorCol });
                }
            }
        }

        // --------- Backspace：削除 ---------
        if (KeyLogger_IsTrigger(KK_BACK) ||
            g_Controller.WasPressed(Controller::BUTTON_B))
        {
            if (!g_SelectedKeys.empty())
                g_SelectedKeys.pop_back();
        }
    }

    if (KeyLogger_IsTrigger(KK_ENTER) ||
        g_Controller.WasPressed(Controller::BUTTON_START))
    {
        Scene_SetNextScene(SCENE_GAME);
    }
}

void NameInput_Draw()
{
    Direct3D_SetDepthTest(false);
    Direct3D_SetAlphaBlend(BLEND_TRANSPARENT);
    SetViewport(0);

    float screenW = (float)Direct3D_GetBackBufferWidth();
    float screenH = (float)Direct3D_GetBackBufferHeight();

    if (g_ShowKeyboard && g_TexKeyboardId >= 0)
    {
        int kbW = Texture_GetWidth(g_TexKeyboardId);
        int kbH = Texture_GetHeight(g_TexKeyboardId);

        float zoom = 2.0f;
        float drawW = kbW * zoom;
        float drawH = kbH * zoom;

        // キーボード位置（画面下）
        float kbX = (screenW - drawW) * 0.5f;
        float kbY = screenH - drawH - 60.0f;

        // キーボード画像の描画
        Sprite_DrawZ(
            g_TexKeyboardId,
            kbX, kbY,
            drawW, drawH,
            0, 0, kbW, kbH,
            0.0f,
            0.2f,
            XMFLOAT4(1, 1, 1, 1)
        );

        float cellW = drawW / 10.0f;
        float cellH = drawH / 5.0f;

        // -------- 選択文字の描画 --------
        float selStartX = kbX + 250.0f;
        float selY = kbY - cellH - 50.0f;

        int idx = 0;
        for (auto& k : g_SelectedKeys)
        {
            float srcX = (kbW / 10.0f) * k.col;
            float srcY = (kbH / 5.0f) * k.row;

            Sprite_DrawZ(
                g_TexKeyboardId,
                selStartX + idx * (cellW + 4),
                selY,
                cellW, cellH,
                srcX, srcY, kbW / 10.0f, kbH / 5.0f,
                0,
                0.15f,
                XMFLOAT4(1, 1, 1, 1)
            );

            idx++;
        }

        // -------- カーソル枠の描画 --------
        if (g_TexFrameId >= 0)
        {
            Sprite_DrawZ(
                g_TexFrameId,
                kbX + cursorCol * cellW,
                kbY + cursorRow * cellH,
                cellW, cellH,
                0, 0,
                Texture_GetWidth(g_TexFrameId),
                Texture_GetHeight(g_TexFrameId),
                0,
                0.1f,
                XMFLOAT4(1, 1, 1, 1)
            );
        }
    }

    Direct3D_SetDepthTest(true);
}
