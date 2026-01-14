

#ifndef HEALTH_H
#define HEALTH_H

#include "Component.h"
#include "GameObject.h"

class Health : public Component
{
private:
	// Å‘å‘Ì—Í‚ÆŒ»Ý‘Ì—Í
	float m_MaxHealth = 0.0f;
	float m_CurrentHealth = 0.0f;

public:

	Health(float maxHealth)
		: m_MaxHealth(maxHealth), m_CurrentHealth(maxHealth) 
	{}
	~Health() = default;

	// Å‘å‘Ì—Í‚ÌÝ’è‚ÆŽæ“¾
	void SetMaxHealth(float maxHealth);
	float GetMaxHealth() const { return m_MaxHealth; }

	// Œ»Ý‘Ì—Í‚ÌÝ’è‚ÆŽæ“¾
	void SetCurrentHealth(float currentHealth);
	float GetCurrentHealth() const { return m_CurrentHealth; }

	// ƒ_ƒ[ƒW‚ðŽó‚¯‚é
	void TakeDamage(float damage) { SetCurrentHealth(m_CurrentHealth - damage); }
	// ‰ñ•œ‚·‚é
	void Heal(float amount) { SetCurrentHealth(m_CurrentHealth + amount); }

	// ¶‘¶ó‘Ô‚ÌŽæ“¾
	bool IsAlive() const { return m_CurrentHealth > 0.0f; }
};


#endif
