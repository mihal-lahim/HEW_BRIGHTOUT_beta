#include "Pole.h"
#include "cube.h"
#include "model.h"
#include "PowerLine.h"

using namespace DirectX;

Pole::Pole(const XMFLOAT3& pos, MODEL* model, float height)
	: GameObject(pos, model, -1),
      m_Height(height),
	m_TopPos(XMFLOAT3(pos.x, pos.y + height, pos.z))
{}

void Pole::Draw() const
{
    // モデルがある場合はモデルを描画
    if (m_pModel != nullptr) {
        XMMATRIX rotation = XMMatrixRotationY(m_Rotation.y);
        XMMATRIX translation = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
        XMMATRIX world = rotation * translation;
        ModelDraw(m_pModel, world);
    } else {
        // モデルがない場合は従来のキューブで描画
        XMMATRIX scale = XMMatrixScaling(1.0f, m_Height, 1.0f);
        XMMATRIX rotation = XMMatrixRotationY(m_Rotation.y);
        XMMATRIX translation = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
        XMMATRIX world = scale * rotation * translation;
        Cube_Draw(0, world);
    }
}
