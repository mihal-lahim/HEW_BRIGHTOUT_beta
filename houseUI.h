/////////////////////////
// 家状況UI
// Author : hiroshi kasiwagi
// Date :2025/12/05
///////////////////////////
#ifndef HOUSE_UI_H
#define HOUSE_UI_H

#include <DirectXMath.h>
#include <vector>
#include <memory>
#include "debug_text.h"

class House; // forward declaration

/**
 * @brief 家の状況を表示するUI
 * デバッグテキストを使用して画面に家の情報を表示
 */
class HouseUI
{
public:
    HouseUI(ID3D11Device* device, ID3D11DeviceContext* context, UINT screenWidth, UINT screenHeight);
    ~HouseUI();

    // UI初期化
    void Initialize(float posX = 20.0f, float posY = 100.0f, float fontSize = 20.0f);

    // 家リストを設定
    void SetHouses(const std::vector<class House*>& houses) { m_houses = houses; }

    // 毎フレーム更新
    void Update(double elapsedTime);

    // UI描画
    void Draw();

    // 表示位置設定
    void SetPosition(float x, float y) { m_posX = x; m_posY = y; }

    // フォントサイズ設定
    void SetFontSize(float size) { m_fontSize = size; }

private:
    std::unique_ptr<hal::DebugText> m_debugText;
    std::vector<class House*> m_houses;
    
    float m_posX = 20.0f;
    float m_posY = 100.0f;
    float m_fontSize = 20.0f;
    
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_context = nullptr;
    UINT m_screenWidth = 0;
    UINT m_screenHeight = 0;

    // 更新タイマー（0.5秒ごとに表示更新）
    float m_updateTimer = 0.0f;
    static constexpr float UPDATE_INTERVAL = 0.5f;

    // テキスト生成ヘルパー
    void BuildHouseDisplayText(std::string& outText);

};

#endif // !HOUSE_UI_H

