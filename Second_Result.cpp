#include "Second_Result.h"
#include "texture.h"
#include "sprite.h"
#include "key_logger.h"
#include "scene.h"

static int g_TexSecResId = -1;

void Second_Result_Initialize()
{
	g_TexSecResId = Texture_Load(L"texture/second_result.png");
}

void Second_Result_Finalize()
{
}

void Second_Result_Update(double elapsed_time)
{
	if (KeyLogger_IsTrigger(KK_ENTER))
	{
		//Scene_SetNextScene(SCENE_ROUND3);   //ç≈èIÉâÉEÉìÉh
	}
}

void Second_Result_Draw()
{
	Sprite_Draw(g_TexSecResId, 0.0f, 0.0f, 1.0f);
}
