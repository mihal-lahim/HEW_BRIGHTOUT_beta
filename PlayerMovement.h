#ifndef PLAYER_MOVEMENT_H
#define PLAYER_MOVEMENT_H

#include <DirectXMath.h>
#include "Movement.h"
#include "PoleManager.h"
#include "Ray.h"


class Player;
class Camera;

// ƒvƒŒƒCƒ„[ˆÚ“®İ’è’l\‘¢‘Ì
struct PlayerMoveCtx
{
	// •às‘¬“x
	float WalkSpeed = 10.0f;
	// ‹ó’†ˆÚ“®‘¬“x
	float AirMoveSpeed = 3.0f;
	// ‹ó’†Å¬‘¬“x‚Ì”{—¦
	float AirMinSpeedFactor = 0.2f;

	// ƒWƒƒƒ“ƒv—Í
	float JumpForce = 20.0f;
	// “d‹CƒWƒƒƒ“ƒv—Í
	float ElectricJumpForce = 20.0f;

	// “düãˆÚ“®‘¬“x
	float LineMoveSpeed = 0.0f;
	// “düãˆÚ“®‘¬“x‚ÌÅ¬’l
	float LineMoveSpeedMin = 1.0f;

	// ƒŒƒCƒLƒƒƒXƒgƒIƒtƒZƒbƒg‹——£
	float RayCastOffset = 0.0f;
	// ’n–ÊŒŸoƒIƒtƒZƒbƒg‹——£
	float GroundDetectOffset = 0.5f;
	// ƒŒƒC‚Ì’·‚³
	float RayLength = 10.0f;

	// d—Í‰Á‘¬“x
	float Gravity = -30.0f;
};


// ƒvƒŒƒCƒ„[ˆÚ“®ƒNƒ‰ƒX
class PlayerMovement : public Movement
{
public:
	// ’n–Ê”»’èˆ—
	bool IsOnGround() const;

	// ƒŒƒCƒLƒƒƒXƒgXVˆ—
	void UpdateRayCast();

	// d—Í“K—pˆ—
	void ApplyGravity();

	// ’nãˆÚ“®ˆ—
	void GroundMove(float inputX, float inputZ, float velocity);
	// •àsˆ—
	void Walk(float inputX, float inputZ);
	// ‘–sˆ—
	void Run(float inputX, float inputZ);
	// ƒWƒƒƒ“ƒvŒÄ‚Ño‚µˆ—
	void Jump(float inputX, float inputZ, float force);
	// ’nãƒWƒƒƒ“ƒvˆ—
	void GroundJump(float inputX, float inputZ);
	// “d‹CƒWƒƒƒ“ƒvˆ—
	void ElectricJump(float inputX, float inputZ);


	// “düã‚ÉˆÚ“®‚·‚éˆ—
	void SnapToPowerLine(PowerLineID lineID);
	// “düã•ûŒüw’èˆ—
	void Turn(float inputX, float inputZ);
	// “düãˆÚ“®ˆ—
	void LineMove();
	// “düËoˆ—
	void Eject(float inputX, float inputZ);

	// PoleManagerd
	void SetPoleManager(PoleManager* poleManager) { m_PoleManager = poleManager; }

	// ˆÚ“®İ’è’læ“¾Eİ’è
	void SetMoveCtx(const PlayerMoveCtx& moveCtx) { m_Ctx = moveCtx; }
	const PlayerMoveCtx& GetMoveCtx() const { return m_Ctx; }

	// ƒRƒ“ƒXƒgƒ‰ƒNƒ^
	PlayerMovement(Camera* camera, PlayerMoveCtx moveCtx = {})
		: m_Camera(camera), m_Ctx(moveCtx)
	{
	}

	// XVˆ—
	void PostUpdate() override;
private:
	// ˆÚ“®İ’è’l
	PlayerMoveCtx m_Ctx;

	// ƒJƒƒ‰
	Camera* m_Camera = nullptr;

	// PoleManager
	PoleManager* m_PoleManager = nullptr;

	// ’n–Ê”»’è—pƒŒƒCƒLƒƒƒXƒg
	Ray m_GroundRay{ {},{} };


	// “d’Œ
	PoleID m_StartPole = -1;
	PoleID m_DestPole = -1;

	// “düID
	PowerLineID m_LineID = -1;

	// “düã‚ÌˆÊ’uƒpƒ‰ƒ[ƒ^
	float m_LineParam = 0.0f;

	// ÅŒã‚Ì“ü—Í•ûŒü
	Vector3 m_LastInputDir{};



	// “ü—Í•ûŒü‚ğ•Û‘¶
	Vector3 SetInputDir(float inputX, float inputZ);
};

// “ü—Í•ûŒü‚ğƒ[ƒ‹ƒhÀ•WŒn‚É•ÏŠ·‚·‚éŠÖ”
Vector3 ConvertToWorldFromInput(const Vector3& inputDir, const Camera* camera);

// ƒ[ƒ‹ƒhÀ•WŒn‚Ì•ûŒüƒxƒNƒgƒ‹‚ğXZ•½–Êã‚ÌƒxƒNƒgƒ‹‚É•ÏŠ·‚·‚éŠÖ”
Vector3 ConvertToXZPlane(const Vector3& worldDir);

#endif
