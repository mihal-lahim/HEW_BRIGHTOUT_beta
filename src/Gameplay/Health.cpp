#include "health.h"

void Health::SetMaxHealth(float maxHealth)
{
	m_MaxHealth = maxHealth;
	if (m_CurrentHealth > m_MaxHealth)
	{
		m_CurrentHealth = m_MaxHealth;
	}
}

void Health::SetCurrentHealth(float currentHealth)
{
	m_CurrentHealth = currentHealth;
	if (m_CurrentHealth > m_MaxHealth)
	{
		m_CurrentHealth = m_MaxHealth;
	}
	else if (m_CurrentHealth < 0.0f)
	{
		m_CurrentHealth = 0.0f;
	}
}
