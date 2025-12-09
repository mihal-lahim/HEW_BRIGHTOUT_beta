#include "UI_HouseCount.h"
#include "house.h"
#include "sprite.h"
#include "texture.h"
#include <cmath>
#include "game_score.h"

// コンストラクタ
UI_HouseCount::UI_HouseCount(ID3D11Device* device, ID3D11DeviceContext* context,
    UINT screenWidth, UINT screenHeight)
    : m_device(device), m_context(context),
    m_screenWidth(screenWidth), m_screenHeight(screenHeight)
{
}

// デストラクタ
UI_HouseCount::~UI_HouseCount() = default;

// 初期化
void UI_HouseCount::Initialize(float posX, float posY, float digitSize)
{
    m_posX = posX;         // 描画開始位置X
    m_posY = posY;         // 描画開始位置Y
    m_digitSize = digitSize; // 1文字あたりの描画サイズ

    // スプライトシート読み込み（数字とスラッシュ）
    m_texidSheet = Texture_Load(L"texture/consolab_ascii_512.png");

    // 1セルあたりの幅と高さを計算
    m_cellWidth = m_texWidth / m_cols;   // 512 / 16 = 32
    m_cellHeight = m_texHeight / m_rows; // 512 / 16 = 32
}

// 描画対象の家リストを設定
void UI_HouseCount::SetHouses(const std::vector<House*>& houses)
{
    m_houses = houses;
}

// 毎フレーム更新（今回は未使用）
void UI_HouseCount::Update(double elapsedTime)
{
}

// 単一の文字（数字やスラッシュ）を描画
void UI_HouseCount::DrawChar(int index, float x, float y, float size)
{
    int col, row;

    // 数字 0~9 はスプライトシートの2行目（row=1）に並んでいる
    if (index >= 0 && index <= 9)
    {
        row = 1; // 2行目
        col = index; // 左から順番
    }
    // スラッシュは1行目16列目（右端）
    else if (index == 10)
    {
        row = 0; // 1行目
        col = 15; // 16列目
    }
    else
        return; // 対象外の文字は描画しない

    // スプライト描画
    // x, y : 画面上の位置
    // size : 表示サイズ（幅・高さ）
    // col*cellWidth, row*cellHeight : 画像上の切り取り座標
    // cellWidth, cellHeight : 切り取るサイズ
    Sprite_Draw(m_texidSheet, x, y, size, size,
        col * m_cellWidth, row * m_cellHeight,
        m_cellWidth, m_cellHeight);
}

// 複数桁の数字を描画
void UI_HouseCount::DrawNumber(int value, float& x, float y, float size)
{
    // 0 の場合は特別扱い
    if (value == 0)
    {
        DrawChar(0, x, y, size);
        x += size; // 次の文字位置をずらす
        return;
    }

    // 各桁を配列に格納（逆順）
    int digits[10]; // 最大10桁まで対応
    int count = 0;
    while (value > 0)
    {
        digits[count++] = value % 10;
        value /= 10;
    }

    // 配列の逆順で描画（左から表示）
    for (int i = count - 1; i >= 0; i--)
    {
        DrawChar(digits[i], x, y, size);
        x += size; // 次の文字位置にずらす
    }
}

// UI全体の描画（復旧数 / 総数）
void UI_HouseCount::Draw()
{
    int total = 0;    // 家の総数
    int repaired = 0; // 復旧済みの家の数

    // 家リストを走査
    for (auto* house : m_houses)
    {
        if (!house) continue;
        total++;
        if (house->IsRepaired())
        {
            repaired++;
			Score_AddScore(100); // 復旧済みの家1軒ごとにスコアを100点追加
        }
    }

    float x = m_posX; // 描画開始X
    float y = m_posY; // 描画開始Y

    // 復旧数を描画
    DrawNumber(repaired, x, y, m_digitSize);

    // スラッシュを描画
    DrawChar(10, x, y, m_digitSize);
    x += m_digitSize; // 次の文字位置をずらす

    // 総数を描画
    DrawNumber(total, x, y, m_digitSize);
}