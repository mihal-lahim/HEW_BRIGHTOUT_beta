#include "PowerLine.h"
#include "Pole.h"
#include "cube.h"


using namespace DirectX;

PowerLine::PowerLine(PoleID first, PoleID second)
	: m_ConnectedPoles({ first, second }), m_Length(0.0f)
{}

DirectX::XMVECTOR PowerLine::GetLineVector() const
{
	Pole* pole1 = m_PoleManager->GetPole(m_ConnectedPoles.first);
	Pole* pole2 = m_PoleManager->GetPole(m_ConnectedPoles.second);

	XMVECTOR vec1 = XMLoadFloat3(&pole1->GetTopPos());
	XMVECTOR vec2 = XMLoadFloat3(&pole2->GetTopPos());

	return vec2 - vec1;
}
