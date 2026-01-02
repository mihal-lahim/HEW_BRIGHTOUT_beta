#include "player_state.h"


void PlayerStateMachine::ChangeState(Player& player, PlayerState* newState)
{
	if (m_CurrentState)
	{
		m_CurrentState->Exit(player);
	}
	m_CurrentState = newState;
	if (m_CurrentState)
	{
		m_CurrentState->Enter(player);
	}
}

void PlayerStateMachine::Update(Player& player, double elapsedTime)
{
	if (m_CurrentState)
	{
		m_CurrentState->Update(player, elapsedTime);
	}
}

void PlayerStateMachine::Draw(const Player& player) const
{
	if (m_CurrentState)
	{
		m_CurrentState->Draw(player);
	}
}