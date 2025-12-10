//1stROUNDŒã‚ÌƒŠƒUƒ‹ƒg

#include "First_Result.h"
#include "texture.h"
#include "sprite.h"
#include "key_logger.h"
#include "scene.h"


static int g_TexFirResId = -1;

void First_Result_Initialize()
{
	g_TexFirResId = Texture_Load(L"texture/first_result.png");
}

void First_Result_Finalize()
{
}

void First_Result_Update(double elapsed_time)
{
	if (KeyLogger_IsTrigger(KK_ENTER))
	{
		//Scene_SetNextScene(SCENE_ROUND2);
	}
}

void First_Result_Draw()
{
	Sprite_Draw(g_TexFirResId, 0.0f, 0.0f, 1.0f);
}
