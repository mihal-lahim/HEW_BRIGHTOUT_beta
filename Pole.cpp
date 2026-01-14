#include "Pole.h"
#include "cube.h"
#include "model.h"
#include "PowerLine.h"

using namespace DirectX;

Pole::Pole(const XMFLOAT3& pos, float height)
	: m_Height(height),
	m_TopPos(XMFLOAT3(pos.x, pos.y + height, pos.z))
{
	Transform.Position = pos;
}