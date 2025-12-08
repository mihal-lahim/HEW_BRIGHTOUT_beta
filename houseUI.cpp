/////////////////////////
// 家状況UI実装
// Author : hiroshi kasiwagi
// Date :2025/12/05
///////////////////////////
#include "houseUI.h"
#include "house.h"
#include <cstdio>
#include <sstream>

HouseUI::HouseUI(ID3D11Device* device, ID3D11DeviceContext* context, UINT screenWidth, UINT screenHeight)
    : m_device(device), m_context(context), m_screenWidth(screenWidth), m_screenHeight(screenHeight)
{
}

HouseUI::~HouseUI() = default;

void HouseUI::Initialize(float posX, float posY, float fontSize)
{
    m_posX = posX;
    m_posY = posY;
    m_fontSize = fontSize;

    // DebugTextを初期化
    m_debugText = std::make_unique<hal::DebugText>(
        m_device,
        m_context,
        L"texture/consolab_ascii_512.png",
        m_screenWidth,
        m_screenHeight,
        m_posX,
        m_posY,
        1,
        static_cast<int>(m_fontSize)
    );
}

void HouseUI::Update(double elapsedTime)
{
    m_updateTimer += static_cast<float>(elapsedTime);
}

void HouseUI::Draw()
{
    if (!m_debugText || m_houses.empty())
        return;

    // テキスト内容を構築
    std::string displayText;
    BuildHouseDisplayText(displayText);

    // DebugTextに設定して描画
    m_debugText->SetText(displayText.c_str(), { 1.0f, 1.0f, 1.0f, 1.0f });
    m_debugText->Draw();
    m_debugText->Clear();
}

void HouseUI::BuildHouseDisplayText(std::string& outText)
{
    std::stringstream ss;

    // タイトル
    ss << "=== HOUSE STATUS ===\n";

    // 家の総数と復旧状況
    int totalHouses = static_cast<int>(m_houses.size());
    int repairedCount = 0;

    for (const auto& house : m_houses) {
        if (house && house->IsRepaired()) {
            repairedCount++;
        }
    }

    ss << "Total: " << totalHouses << " | Repaired: " << repairedCount << "/" << totalHouses << "\n";
    ss << "Progress: ";
    
    // プログレスバー表示（簡易版）
    int barLength = 10;
    int filledLength = (totalHouses > 0) ? (repairedCount * barLength) / totalHouses : 0;
    for (int i = 0; i < barLength; ++i) {
        ss << (i < filledLength ? "[#]" : "[ ]");
    }
    ss << "\n\n";

    // 個別の家の情報
    ss << "--- DETAILS ---\n";
    for (int i = 0; i < static_cast<int>(m_houses.size()); ++i) {
        const auto& house = m_houses[i];
        if (!house) continue;

        // 家の名前と状態
        const char* statusStr = house->IsRepaired() ? "[OK]" : "[NG]";
        float electricity = house->GetElectricity();
        float maxElectricity = house->GetMaxElectricity();
        float percentage = (maxElectricity > 0.0f) ? (electricity / maxElectricity) * 100.0f : 0.0f;

        DirectX::XMFLOAT3 pos = house->GetPosition();

        ss << "H" << (i + 1) << " " << statusStr << " ";
        ss << "Power: " << static_cast<int>(electricity) << "/" << static_cast<int>(maxElectricity);
        ss << " (" << static_cast<int>(percentage) << "%)\n";
    }

    outText = ss.str();
}
