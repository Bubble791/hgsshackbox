#include "global.h"

#include "bg_window.h"
#include "filesystem.h"
#include "font.h"
#include "gf_gfx_loader.h"
#include "gf_gfx_planes.h"
#include "launch_application.h"
#include "math_util.h"
#include "menu_input_state.h"
#include "message_format.h"
#include "msgdata.h"
#include "obj_char_transfer.h"
#include "obj_pltt_transfer.h"
#include "pokemon_icon_idx.h"
#include "sound_02004A44.h"
#include "systask_environment.h"
#include "system.h"
#include "text.h"
#include "font.h"
#include "font_data.h"
#include "unk_0200B150.h"
#include "unk_0200FA24.h"
#include "unk_02013534.h"
#include "touchscreen_list_menu.h"
#include "unk_0200ACF0.h"
#include "unk_02005D10.h"
#include "vram_transfer_manager.h"
#include "hackbox.h"

#define COLOR_W_GREEN (MAKE_TEXT_COLOR(9, 10, 15))
#define COLOR_W_BLACK (MAKE_TEXT_COLOR(5, 2, 1))

enum
{
    HACK_BOX_SPECIES,
    HACK_BOX_LEVEL,
    HACK_BOX_EXP,
    HACK_BOX_ID,
    HACK_BOX_PID,
    HACK_BOX_SEX,
    HACK_BOX_PERSONAL,
};

static const u8 sPokeEditPage1[] = 
{
    HACK_BOX_SPECIES, 0xff
};

static const struct HackBoxPokemonTable
{
    const u8 *page;
    u8 listcount; // 可以编辑的最大值
} sPokemonPageTable[] = 
{
    {sPokeEditPage1, 1}
};

extern u16 gText_PokemonEdit_Species[];

static u16* sPokeEditText[] = 
{
	[HACK_BOX_SPECIES] = gText_PokemonEdit_Species,
};

static void HackBoxTool_PokePagePut(HackBoxTool *hackBox);
static u8 HackBoxTool_PokePutParam(HackBoxTool *wk, u8 id, u32 pal, u8 y);

void HackBoxTool_PokemonPageUI(HackBoxTool *hackBox)
{
    HackBoxToolPokemon *hackBoxPoke = hackBox->hackBoxPoke;

    hackBoxPoke->listCursor = ListMenuCursorNew(HEAP_ID_HACK_BOX);
    InitWindow(&hackBoxPoke->pokemonEditWindow);
	AddWindowParameterized(hackBox->bgConfig, &hackBoxPoke->pokemonEditWindow, GF_BG_LYR_SUB_3, 2, 7, 28, 15, 15, 340);
    AddWindowParameterized(hackBox->bgConfig, &hackBoxPoke->partyIconWindow, GF_BG_LYR_SUB_3, 2, 1, 28, 4, 15, 340 + 28 * 15);

	DrawFrameAndWindow1(&hackBoxPoke->pokemonEditWindow, FALSE, 1, 15);
	FillWindowPixelBuffer(&hackBoxPoke->pokemonEditWindow, 0x11);
	CopyWindowToVram(&hackBoxPoke->pokemonEditWindow);

    DrawFrameAndWindow1(&hackBoxPoke->partyIconWindow, FALSE, 1, 15);
	FillWindowPixelBuffer(&hackBoxPoke->partyIconWindow, 0x11);
	CopyWindowToVram(&hackBoxPoke->partyIconWindow);

	Sprite_SetDrawFlag(hackBox->cursorSprite, FALSE);
	GfGfx_EngineBTogglePlanes(GX_PLANEMASK_BG0, GF_PLANE_TOGGLE_OFF);

    HackBoxTool_PokePagePut(hackBox);
}

static void HackBoxTool_PokePagePut(HackBoxTool *hackBox)
{
    HackBoxToolPokemon *hackBoxPoke = hackBox->hackBoxPoke;
    u16 i = 0;

    FillWindowPixelBuffer(&hackBoxPoke->pokemonEditWindow, 0x11);
    while (sPokemonPageTable[hackBoxPoke->pageNow].page[i] != 0xff)
    {
        HackBoxTool_PokePutParam(hackBox, sPokemonPageTable[hackBoxPoke->pageNow].page[i], COLOR_W_BLACK, i * 16);
        i++;
    }
    CopyWindowToVram(&hackBoxPoke->pokemonEditWindow);
}

static void CursorPut(HackBoxTool *hackBox, u8 mode)
{
    HackBoxToolPokemon *hackBoxPoke = hackBox->hackBoxPoke;

    switch (mode)
    {
        case 0:
            ListMenuUpdateCursorObj(hackBoxPoke->listCursor, &hackBoxPoke->pokemonEditWindow, 0, hackBoxPoke->selectRow * 16);
            CopyWindowToVram(&hackBoxPoke->pokemonEditWindow);
            break;
        case 1:
            if (hackBoxPoke->selectRow < sPokemonPageTable[hackBoxPoke->pageNow].listcount - 1)
            {
                FillWindowPixelRect(&hackBoxPoke->pokemonEditWindow, 15, 0, hackBoxPoke->selectRow * 16, 12, 16);
                hackBoxPoke->selectRow++;
                ListMenuUpdateCursorObj(hackBoxPoke->listCursor, &hackBoxPoke->pokemonEditWindow, 0, hackBoxPoke->selectRow * 16);
                CopyWindowToVram(&hackBoxPoke->pokemonEditWindow);
            }
            break;
        case 2:
            if (hackBoxPoke->selectRow > 0)
            {
                FillWindowPixelRect(&hackBoxPoke->pokemonEditWindow, 15, 0, hackBoxPoke->selectRow * 16, 12, 16);
                hackBoxPoke->selectRow--;
                ListMenuUpdateCursorObj(hackBoxPoke->listCursor, &hackBoxPoke->pokemonEditWindow, 0, hackBoxPoke->selectRow * 16);
                CopyWindowToVram(&hackBoxPoke->pokemonEditWindow);
            }
            break;
    }
}

static void PokeMake_PrintString(HackBoxTool *hackBox, Window *win, u16 id, u32 x, u32 y, u32 color)
{
    HackBox_LoadString(sPokeEditText[id], hackBox->textString);
    AddTextPrinterParameterizedWithColor(win, 0, hackBox->textString, x, y, 0xFF, color, NULL);
}

static u8 HackBoxTool_PokePutParam(HackBoxTool *hackBox, u8 id, u32 pal, u8 y)
{
    HackBoxToolPokemon *hackBoxPoke = hackBox->hackBoxPoke;

    if (id == HACK_BOX_SPECIES)
    {
        PokeMake_PrintString(hackBox, &hackBoxPoke->pokemonEditWindow, id, 12, y, COLOR_W_BLACK);
    }
}