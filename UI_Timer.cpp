//////////////////////////////
// UI_Timer.cpp
// タイマーUI: ゲーム開始時に設定した時間をカウントダウン表示
// Author: hiroshi kasiwagi
// Date: 2025/11/27
//////////////////////////////

#include "UI_Timer.h"
#include "texture.h"
#include "sprite.h"
#include "direct3d.h"
#include <cmath>
#include <algorithm>

// DrawNumber()関数の宣言（UI_Charge.cppに定義されている）
namespace {
    void DrawNumber(int texId, int number, float x, float y, float h)
    {
        if (texId < 0) return;
        
        // digit sizing
        float digitH = h;
        float digitW = digitH * 0.7f;

        // source tile size in sprite
        int srcW = (int)(digitH * 0.7f);
        int srcH = (int)digitH;
        int srcY = 100;

        // 負数の場合は0として扱う
        if (number < 0) number = 0;

        // 0の場合は0を描画して終了（テクスチャは「1234567890」なので0は9番目）
        if (number == 0) {
            int srcX = 9 * srcW;  // 0は9番目の位置
            Sprite_Draw(texId, x, y, digitW, digitH, srcX, srcY, srcW, srcH);
            return;
        }

        // 数値を文字列に変換
        char buf[16];
        int len = 0;
        int n = number;
        
        // 数字を逆順で配列に格納
        while (n > 0 && len < 15) {
            buf[len++] = '0' + (n % 10);
            n /= 10;
        }
        
        if (len == 0) return;

        // 逆順を正順に描画
        float px = x;
        for (int i = len - 1; i >= 0; --i) {
            int digit = buf[i] - '0';
            // テクスチャが「1234567890」の順なため、0は9番目、1は0番目...9は8番目
            int srcX;
            if (digit == 0) {
                srcX = 9 * srcW;  // 0は9番目
            } else {
                srcX = (digit - 1) * srcW;  // 1～9は0～8番目
            }
            Sprite_Draw(texId, px, y, digitW, digitH, srcX, srcY, srcW, srcH);
            px += digitW;
        }
    }

    // コロン「:」を描画する関数
    void DrawColon(int texId, float x, float y, float h)
    {
        if (texId < 0) return;
        
        float digitH = h;
        float digitW = digitH * 0.7f;
        
        // コロンのテクスチャ座標（10番目の位置）
        int srcW = (int)(digitH * 0.7f);
        int srcH = (int)digitH;
        int srcY = 100;
        int colonSrcX = 10 * srcW;  // コロンのX座標
        
        Sprite_Draw(texId, x, y, digitW, digitH, colonSrcX, srcY, srcW, srcH);
    }
}

UI_Timer::UI_Timer()
    : remainingTime_(0.0f),
    totalTime_(0.0f),
    numberTexture_(-1),
    posX_(1920.0f / 2.0f - 80.0f),
    posY_(50.0f),
    digitHeight_(60.0f),
    frameTexture_(-1),
    frameX_(0.0f),
    frameY_(0.0f),
    frameWidth_(180.0f),
    frameHeight_(60.0f)
{
}

void UI_Timer::Initialize()
{
    if (numberTexture_ < 0)
        numberTexture_ = Texture_Load(L"texture/brightout_number_k.png");

    if (frameTexture_ < 0)
        frameTexture_ = Texture_Load(L"texture/BRIGHTOUT_UI_TAIMA-kl.png");
}

void UI_Timer::Update(double elapsedTime)
{
    // カウントダウン
    if (remainingTime_ > 0.0f) {
        remainingTime_ -= static_cast<float>(elapsedTime);
        if (remainingTime_ < 0.0f) {
            remainingTime_ = 0.0f;
        }
    }
}

void UI_Timer::Draw() const
{
    if (numberTexture_ < 0) return;


    //フレーム背景(未実装)
   /* if (frameTexture_ >= 0)
    {
        Sprite_Draw(
            frameTexture_,
            frameX_, frameY_,
            frameWidth_, frameHeight_,
            0, 0, 1024, 1024
        );
    }*/


    // 残り時間（切り上げ）
    int totalSeconds = static_cast<int>(std::ceil(remainingTime_));

    // 分：秒に分割
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    // 元画像の仕様
    const int totalTiles = 10;               // 1..9,0 の10タイル
    const int textureWidth = 3702;           // 画像幅（px）
    const int textureHeight = 349;           // 画像高さ（px）
    const int srcW = textureWidth / totalTiles;
    const int srcH = textureHeight;
    const int srcY = 0;

    // 描画サイズ（縦は digitHeight_、横はアスペクト比維持）
    float digitH = digitHeight_;
    float digitW = digitH * (static_cast<float>(srcW) / static_cast<float>(srcH));

    // 開始 X（中央に寄せたいなら posX_ を調整済み）
    float x = posX_;

    // 内部：1桁を描くラムダ（0..9 の数値を1桁で描画）
    auto drawSingleDigit = [&](int num, float& drawX) {
        int digit = num;
        // テクスチャは "1 2 3 4 5 6 7 8 9 0" の順と仮定
        int texIndex = (digit == 0) ? 9 : (digit - 1);
        int texX = texIndex * srcW;
        Sprite_Draw(numberTexture_, drawX, posY_, digitW, digitH,
            texX, srcY, srcW, srcH);
        drawX += digitW;
        };

    // 分（2桁で描画）
    drawSingleDigit((minutes / 10) % 10, x);
    drawSingleDigit(minutes % 10, x);

    // コロンを自前で描画（点を2つ縦に並べる）
    // 方法: 元テクスチャの中から小領域を切り出して点として使う（透明背景なら点だけが描画される）
    auto drawColonDots = [&](float centerX, float centerY, float overallH) {
        // 点サイズ（全体高さに対する比率で可変）
        float dotW = overallH * 0.12f;   // 点の横幅
        float dotH = overallH * 0.12f;   // 点の縦幅
        float gap = overallH * 0.12f;   // 上下の点の間隔

        // 切り出す元画像の src 範囲（数字のどれかの中央付近を借用）
        // ここでは '0'（テクスチャインデックス9）の中央部分を使う
        int sourceTexIndex = 9;
        int dotSrcW = srcW / 8;   // 小さい切り出し幅
        int dotSrcH = srcH / 8;   // 小さい切り出し高さ
        int dotSrcX = sourceTexIndex * srcW + (srcW - dotSrcW) / 2;
        int dotSrcY = srcY + (srcH - dotSrcH) / 2;

        // 上の点
        float yTop = centerY - (gap * 0.5f) - (dotH * 0.5f);
        Sprite_Draw(numberTexture_, centerX - dotW * 0.5f, yTop, dotW, dotH,
            dotSrcX, dotSrcY, dotSrcW, dotSrcH);

        // 下の点
        float yBot = centerY + (gap * 0.5f) - (dotH * 0.5f);
        Sprite_Draw(numberTexture_, centerX - dotW * 0.5f, yBot, dotW, dotH,
            dotSrcX, dotSrcY, dotSrcW, dotSrcH);
        };

    // コロンの中心位置（現在の x が分の描画終了位置）
    float colonCenterX = x + digitW * 0.2f; // 少し間を空ける
    float colonCenterY = posY_ + digitH * 0.5f;

    // コロンを描く（dotサイズは digitH を基準）
    drawColonDots(colonCenterX, colonCenterY, digitH);

    // コロン分の横空け（見た目調整）
    x = colonCenterX + digitW * 0.6f;

    // 秒（2桁）
    drawSingleDigit((seconds / 10) % 10, x);
    drawSingleDigit(seconds % 10, x);
}

void UI_Timer::SetTime(float timeSeconds)
{
    remainingTime_ = std::max(0.0f, timeSeconds);
    totalTime_ = remainingTime_;
}

void UI_Timer::SetPosition(float x, float y)
{
    posX_ = x;
    posY_ = y;
}

void UI_Timer::SetDigitHeight(float digitHeight)
{
    digitHeight_ = std::max(1.0f, digitHeight);
}

void UI_Timer::SetFramePosition(float x, float y)
{
    frameX_ = x;
    frameY_ = y;
}

void UI_Timer::SetFrameSize(float w, float h)
{
    frameWidth_ = w;
    frameHeight_ = h;
}
