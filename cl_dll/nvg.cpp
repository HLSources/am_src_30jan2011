#include "hud.h"
#include "cl_util.h"
#include "const.h"
#include "entity_state.h"
#include "cl_entity.h"
#include <string.h>
#include <stdio.h>
#include "parsemsg.h"
#include "event_api.h"
#include "vgui_TeamFortressViewport.h"

#include "triangleapi.h"

bool g_NVGOn;

DECLARE_MESSAGE(m_NVG, NVGActivate )
//DECLARE_MESSAGE(m_NVG, NVG )

int CHudNVG::Init(void)
{
	HOOK_MESSAGE( NVGActivate );
//	HOOK_MESSAGE( NVG );

//	m_iNVG = 0;
	m_iOn = 0;
	g_NVGOn = false;
//	m_flBattery = 0;

	m_iFlags = 0;
//	m_iFlags |= HUD_ACTIVE;

	gHUD.AddHudElem(this);

	//srand( (unsigned)time( NULL ) );

	return 1;
}


int CHudNVG::VidInit(void)
{
// this is the NVG effect sprite
	m_hFlicker = LoadSprite("sprites/nvg.spr");
//	m_hFlicker2 = LoadSprite("sprites/nvg_noise.spr");

	return 1;
}

int CHudNVG::Draw(float fTime)
{
	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT)) ))
	return 1;

//	int iHiRes = CVAR_GET_FLOAT("cl_nvghires");

	if (m_iOn)
	{
		g_NVGOn = true;

		int x, y, w, h;
		int frame;

		SPR_Set(m_hFlicker, 100, 100, 100 );// COLOR

//		SPR_Set(m_hFlicker, 255, 255, 255 );// COLOR

		// play at 15fps
		frame = (int)(fTime * 15) % SPR_Frames(m_hFlicker);

		w = SPR_Width(m_hFlicker,0);
		h = SPR_Height(m_hFlicker,0);

		for(y = -(rand() % h); y < ScreenHeight; y += h) 
		{
			for(x = -(rand() % w); x < ScreenWidth; x += w) 
			{
				SPR_DrawAdditive( frame, x, y, NULL );
			}
		}
/*
		gEngfuncs.pTriAPI->RenderMode(kRenderTransAdd); //additive
		gEngfuncs.pTriAPI->SpriteTexture( (struct model_s *) gEngfuncs.GetSpritePointer(SPR_Load("sprites/nvg.spr")) , 0);//use hotglow, or any other sprite for the texture
		gEngfuncs.pTriAPI->CullFace( TRI_NONE ); //no culling
		gEngfuncs.pTriAPI->Color4f(1.0, 1.0 , 1.0, 1.0);
		gEngfuncs.pTriAPI->Brightness(1.0);
		gEngfuncs.pTriAPI->Begin(TRI_QUADS); //start our quad
		gEngfuncs.pTriAPI->TexCoord2f(0.0f, 1.0f);gEngfuncs.pTriAPI->Vertex3f(0, 0, 0); //top left
		gEngfuncs.pTriAPI->TexCoord2f(0.0f, 0.0f);gEngfuncs.pTriAPI->Vertex3f(0, ScreenHeight, 0); //bottom left
		gEngfuncs.pTriAPI->TexCoord2f(1.0f, 0.0f);gEngfuncs.pTriAPI->Vertex3f(ScreenWidth, ScreenHeight, 0); //bottom right
		gEngfuncs.pTriAPI->TexCoord2f(1.0f, 1.0f);gEngfuncs.pTriAPI->Vertex3f(ScreenWidth, 0, 0); //top right
		gEngfuncs.pTriAPI->End(); //end our list of vertexes
		gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
*/
////////////2
		/*
		int x2, y2, w2, h2;
		int frame2;

		if ( iHiRes != 0 )
		{
//			SPR_Set(m_hFlicker2, 100, 200, 100 );// COLOR
			SPR_Set(m_hFlicker2, 50, 50, 50 );// COLOR

			// play at 15fps
			frame2 = (int)(fTime * 5) % SPR_Frames(m_hFlicker2);//15

			w2 = SPR_Width(m_hFlicker2,0);
			h2 = SPR_Height(m_hFlicker2,0);

			for(y2 = -(rand() % h2); y2 < ScreenHeight; y2 += h2) 
			{
				for(x2 = -(rand() % w2); x2 < ScreenWidth; x2 += w2) 
				{
					SPR_DrawAdditive( frame2, x2, y2, NULL );
				}
			}
		}
*/
////////////2			
	//	 gViewPort->ShowScope();
//	}
//	else
//	{
//		gViewPort->HideScope();
	}
	else
	g_NVGOn = false;

	return 1;
}

int CHudNVG::MsgFunc_NVGActivate(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );

	// update NVG data
	m_iOn = READ_BYTE();
//	m_flBattery = (float)READ_BYTE();
	
	if (m_iOn==1)
	{
		m_iFlags |= HUD_ACTIVE;
//		gViewPort->ShowScope();
		g_NVGOn = true;
	}
	else
	{
		m_iFlags &= ~HUD_ACTIVE;
//		gViewPort->HideScope();
		g_NVGOn = false;
	}

	return 1;
}
void CHudNVG::Reset(void)
{
	m_iOn = 0;
	g_NVGOn = false;
//	gViewPort->HideScope();
}
/*
int CHudNVG::MsgFunc_NVG(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );

	// update NVG data
	m_iNVG = READ_BYTE();
	m_flBattery = (float)READ_BYTE();

	if (m_iNVG)
	m_iFlags |= HUD_ACTIVE;
	else
	m_iFlags &= ~HUD_ACTIVE;

	m_iOn = 0;

	return 1;
}
*/

