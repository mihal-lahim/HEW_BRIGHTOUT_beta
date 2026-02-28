#include "PlayerMorphsystem.h"
#include "Player.h"
#include "Pole.h"
#include "PowerLine.h"
#include "SceneSystem.h"
#include <limits>

void PlayerMorphSystem::Awake()
{
	auto poleManagers = scene().currentScene().GetComponents<PoleManager>();
	if (!poleManagers.empty())
	{
		m_PoleManager = poleManagers.front();
	}

	if (m_PoleManager)
	{
		if (auto* player = gameObject().GetComponent<Player>())
		{
			if (player->movement)
			{
				player->movement->SetPoleManager(m_PoleManager);
			}
		}
	}
}

bool PlayerMorphSystem::CanMorph()
{
	if (!m_PoleManager)
		return false;

	Vector3 playerPos = gameObject().transform().position();
	float radiusSq = m_MorphRadius * m_MorphRadius;
	Pole* nearestPole = nullptr;
	float bestDistSq = radiusSq;

	for (auto* pole : m_PoleManager->GetPoles())
	{
		if (!pole)
			continue;

		Vector3 polePos = pole->gameObject().transform().position();
		Vector3 diff = playerPos - polePos;
		float distSq = diff.x * diff.x + diff.z * diff.z;
		if (distSq <= bestDistSq)
		{
			bestDistSq = distSq;
			nearestPole = pole;
		}
	}

	return nearestPole && !nearestPole->GetLines().empty();
}

PowerLineID PlayerMorphSystem::GetNearestPowerLineID()
{
	if (!m_PoleManager)
		return -1;

	Vector3 playerPos = gameObject().transform().position();
	float radiusSq = m_MorphRadius * m_MorphRadius;
	Pole* nearestPole = nullptr;
	float bestDistSq = radiusSq;

	for (auto* pole : m_PoleManager->GetPoles())
	{
		if (!pole)
			continue;

		Vector3 polePos = pole->gameObject().transform().position();
		Vector3 diff = playerPos - polePos;
		float distSq = diff.x * diff.x + diff.z * diff.z;
		if (distSq <= bestDistSq)
		{
			bestDistSq = distSq;
			nearestPole = pole;
		}
	}

	if (!nearestPole)
		return -1;

	PowerLineID nearestLine = -1;
	float bestLineDistSq = std::numeric_limits<float>::max();

	for (PowerLineID lineID : nearestPole->GetLines())
	{
		PowerLine* line = m_PoleManager->GetPowerLine(lineID);
		if (!line)
			continue;

		Vector3 linePos = line->gameObject().transform().position();
		Vector3 diff = playerPos - linePos;
		float distSq = diff.x * diff.x + diff.z * diff.z;
		if (distSq < bestLineDistSq)
		{
			bestLineDistSq = distSq;
			nearestLine = lineID;
		}
	}

	return nearestLine;
}
