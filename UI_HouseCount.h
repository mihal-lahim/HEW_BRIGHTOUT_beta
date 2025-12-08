#pragma once
#include <vector>
#include <DirectXMath.h>
#include <d3d11.h>  

class House; 

/**
 * @brief 家の数を UI で表示するクラス
 *
 * 画面上に「復旧した家の数 / 家の総数」を描画します。
 * 描画は数字スプライトとスラッシュをスプライトシートから切り取って行います。
 */
class UI_HouseCount
{
public:
    // コンストラクタ：デバイス・コンテキスト・画面サイズを受け取る
    UI_HouseCount(ID3D11Device* device, ID3D11DeviceContext* context,
        UINT screenWidth, UINT screenHeight);

    // デストラクタ
    ~UI_HouseCount();

    // 初期化：描画位置と文字サイズを設定
    void Initialize(float posX = 30.0f, float posY = 30.0f, float digitSize = 32.0f);

    // 描画対象の家リストを設定
    void SetHouses(const std::vector<House*>& houses);

    // 毎フレーム更新（必要であればタイマーやアニメーション処理に使用）
    void Update(double elapsedTime);

    // UI の描画
    void Draw();

private:
    // Direct3D デバイスとコンテキスト
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_context = nullptr;

    // 画面サイズ
    UINT m_screenWidth = 0;
    UINT m_screenHeight = 0;

    // 描画位置
    float m_posX = 0.0f;
    float m_posY = 0.0f;

    // 数字の表示サイズ
    float m_digitSize = 32.0f;

    // 描画対象の家リスト
    std::vector<House*> m_houses;

    // 数字・スラッシュのスプライトシート情報
    int m_texidSheet = -1;    // スプライトシートのテクスチャID
    int m_texWidth = 512;     // スプライトシート幅
    int m_texHeight = 512;    // スプライトシート高さ
    int m_cols = 16;          // スプライトシートの列数
    int m_rows = 16;          // スプライトシートの行数（8行以降は空白）
    int m_cellWidth = 0;      // 1セルの幅（512/16=32）
    int m_cellHeight = 0;     // 1セルの高さ（512/16=32）

    /**
     * @brief 単一の文字（数字またはスラッシュ）を描画
     * @param index 0~9:数字, 10:スラッシュ
     * @param x 画面上のX座標
     * @param y 画面上のY座標
     * @param size 描画サイズ（幅・高さ）
     */
    void DrawChar(int index, float x, float y, float size);

    /**
     * @brief 複数桁の整数を描画
     * @param value 描画する数値
     * @param x 画面上のX座標（描画後に次の文字位置に更新される）
     * @param y 画面上のY座標
     * @param size 1文字のサイズ
     */
    void DrawNumber(int value, float& x, float y, float size);
};