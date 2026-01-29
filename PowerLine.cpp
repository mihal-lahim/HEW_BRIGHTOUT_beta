#include "PowerLine.h"
#include "Pole.h"


using namespace DirectX;

PowerLine::PowerLine(PoleID first, PoleID second)
	: m_ConnectedPoles({ first, second }), m_Length(0.0f)
{}

Vector3 PowerLine::GetLineVector() const
{
	Pole* pole1 = m_PoleManager->GetPole(m_ConnectedPoles.first);
	Pole* pole2 = m_PoleManager->GetPole(m_ConnectedPoles.second);

	Vector3 vec1 = pole1->GetTopPos();
	Vector3 vec2 = pole2->GetTopPos();

	return vec2 - vec1;
}
