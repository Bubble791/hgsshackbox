// overlay43 朋友手册的overlay
// 理论上可以同时占用wifi_2dmapsys （overlay42）的位置，朋友手册加载的时候会先加载wifi_2dmapsys
// 起始地址02227060, 大小11904 + 21248
// 为了保留overlay43 把代码写入到一个空的debug overlay里去

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
#include "unk_02005D10.h"
#include "unk_0200ACF0.h"
#include "unk_0200B150.h"
#include "unk_0200FA24.h"
#include "unk_02013534.h"
#include "unk_020163E0.h"
#include "vram_transfer_manager.h"

#define HEAPID_BASE_APP 3
#define HEAP_ID_HACK_BOX 51

typedef struct HackBoxTool
{
    BgConfig *bgConfig;
    NARC *fileHandle;
    void *msgFormat;
} HackBoxTool;

static const BgTemplate sHackBoxBgTemplate[] =
{
	{	// DFRM_BACK
		0, 0, 0x800, 0, GF_BG_SCR_SIZE_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xc000, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
		3, 0, 0, FALSE
	},
	{	// DFRM_MSG
		0, 0, 0x800, 0, GF_BG_SCR_SIZE_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xc800, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01,
		0, 0, 0, FALSE
	},
	{	// DFRM_SCROLL
		0, 0, 0x1000, 0, GF_BG_SCR_SIZE_512x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xd000, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
		2, 0, 0, FALSE
	},
	{	// DFRM_SCRMSG
		0, 0, 0x1000, 0, GF_BG_SCR_SIZE_512x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x20000, GX_BG_EXTPLTT_01,
		1, 0, 0, FALSE
	},
	{	// UFRM_MSG
		0, 0, 0x800, 0, GF_BG_SCR_SIZE_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
		0, 0, 0, FALSE
	},
	{	// UFRM_BASE
		0, 0, 0x800, 0, GF_BG_SCR_SIZE_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
		1, 0, 0, FALSE
	},
	{	// UFRM_BACK
		0, 0, 0x800, 0, GF_BG_SCR_SIZE_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
		3, 0, 0, FALSE
	},
};

static void HackBoxTool_DrawScreen(HackBoxTool *hackBox);
static void HackBox_VBlankCB(HackBoxTool *param);

// 初始化
BOOL HackBoxTool_Init(OverlayManager *ovyMan, int *pState)
{
    HackBoxTool *data;
    Main_SetVBlankIntrCB(NULL, NULL);

    CreateHeap(HEAPID_BASE_APP, HEAP_ID_HACK_BOX, 0x20000);

    data = OverlayManager_CreateAndGetData(ovyMan, sizeof(HackBoxTool), HEAP_ID_HACK_BOX);
    memset(data, 0, sizeof(HackBoxTool));

    HackBoxTool_DrawScreen(data);
    Main_SetVBlankIntrCB(HackBox_VBlankCB, NULL);
	HBlankInterruptDisable();

    return TRUE;
}

const GraphicsBanks graphicsBanks = {
	GX_VRAM_BG_256_AB,
	GX_VRAM_BGEXTPLTT_NONE,

	GX_VRAM_SUB_BG_128_C,
	GX_VRAM_SUB_BGEXTPLTT_NONE,

	GX_VRAM_OBJ_80_EF,
	GX_VRAM_OBJEXTPLTT_NONE,

	GX_VRAM_SUB_OBJ_16_I,
	GX_VRAM_SUB_OBJEXTPLTT_NONE,

	GX_VRAM_TEX_NONE,
	GX_VRAM_TEXPLTT_NONE
};

const GraphicsModes graphicsModes = {
	GX_DISPMODE_GRAPHICS,
	GX_BGMODE_0,
	GX_BGMODE_0,
	GX_BG0_AS_2D,
};

static void HackBoxTool_DrawScreen(HackBoxTool *hackBox)
{
    hackBox->fileHandle = NARC_New(85, HEAP_ID_HACK_BOX);

    GfGfx_SetBanks(&graphicsBanks);

    hackBox->bgConfig = BgConfig_Alloc(HEAP_ID_HACK_BOX);

    SetBothScreensModesAndDisable(&graphicsModes);

    for (int i = 0; i < NELEMS(sHackBoxBgTemplate); i++)
    {
        InitBgFromTemplate(hackBox->bgConfig, i, &sHackBoxBgTemplate[i], GF_BG_TYPE_TEXT);
        BG_ClearCharDataRange(i, 32, 0, HEAP_ID_HACK_BOX);
        BgClearTilemapBufferAndCommit(hackBox->bgConfig, i);
    }

    GfGfxLoader_GXLoadPalFromOpenNarc(hackBox->fileHandle, 5, GF_PAL_LOCATION_MAIN_BG, (enum GFPalSlotOffset)0, 11 * 32, HEAP_ID_HACK_BOX);
    GfGfxLoader_GXLoadPalFromOpenNarc(hackBox->fileHandle, 4, GF_PAL_LOCATION_SUB_BG, (enum GFPalSlotOffset)0, 11 * 32, HEAP_ID_HACK_BOX);

    GfGfxLoader_LoadCharDataFromOpenNarc(hackBox->fileHandle, 7, hackBox->bgConfig, 0, 0, 0, TRUE, HEAP_ID_HACK_BOX);
	GfGfxLoader_LoadCharDataFromOpenNarc(hackBox->fileHandle, 7, hackBox->bgConfig, 2, 0, 0, TRUE, HEAP_ID_HACK_BOX);
	GfGfxLoader_LoadCharDataFromOpenNarc(hackBox->fileHandle, 6, hackBox->bgConfig, 6, 0, 0, TRUE, HEAP_ID_HACK_BOX);

    GfGfxLoader_LoadScrnDataFromOpenNarc(hackBox->fileHandle, 10, hackBox->bgConfig, 0, 0, 0, TRUE, HEAP_ID_HACK_BOX);
	GfGfxLoader_LoadScrnDataFromOpenNarc(hackBox->fileHandle, 8, hackBox->bgConfig, 6, 0, 0, TRUE, HEAP_ID_HACK_BOX);

	NARC_Delete(hackBox->fileHandle);
	BeginNormalPaletteFade(0, 1, 1, RGB_BLACK, 16, 1, HEAP_ID_HACK_BOX);
}

static void HackBox_VBlankCB(HackBoxTool *param)
{
	DoScheduledBgGpuUpdates(param->bgConfig);
    GF_RunVramTransferTasks();
    OamManager_ApplyAndResetBuffers();
}

BOOL HackBoxTool_Main(OverlayManager *ovyMan, int *pState)
{
    return FALSE;
}

BOOL HackBoxTool_Exit(OverlayManager *ovyMan, int *pState)
{
    return TRUE;
}