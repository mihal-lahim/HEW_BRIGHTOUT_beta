
#include "player.h"

Player::Player()
{
}

void Player::Update(double elapsedSec)
{
	m_StateMachine->Update(*this, elapsedSec);
}

void Player::Draw() const
{
	m_StateMachine->Draw(*this);
}
