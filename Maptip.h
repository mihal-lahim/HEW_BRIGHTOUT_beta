//Maptip.h
#pragma once
#include <string>
#include <DirectXMath.h>

struct MODEL;

enum class MapTipType
{
    None = 0,
    Ground = 1,
    Karaoke = 2,
    House = 3,
    Convenience = 4,
    PowerPlant = 5,
    PowerPlantBroken = 6,
    Mansion01 = 7,
    Mansion02 = 8,
    Wacdonald = 9,
    Apartment = 10,
	ClockTower = 11,
    Dentyuu = 12,
    SpecialDentyuu = 13,
	SpecialDensen = 14,
};

struct MapTipInstance
{
    MapTipType type = MapTipType::None;
    MODEL* model = nullptr;
    DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();
    std::string sourcePath;
};