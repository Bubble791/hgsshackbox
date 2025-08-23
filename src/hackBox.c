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
#include "touchscreen_list_menu.h"
#include "vram_transfer_manager.h"

#define HEAPID_BASE_APP 3
#define HEAP_ID_HACK_BOX 51

typedef struct HackBoxTool
{
    BgConfig *bgConfig;
    NARC *fileHandle;
    MessageFormat *msgFormat;
	MsgData *msgData;
	SpriteList *spriteList;
    G2dRenderer g2dRender;
	GF_2DGfxResMan *gfxResMen[4];
	SpriteResource *gfxResObjs[2][4];
	Window titleWindow;
	Window infoWindow;
	String *textString;
	TouchscreenListMenuSpawner *menuSpawner;
} HackBoxTool;

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

static const BgTemplate sHackBoxBgTemplate[] =
{
	{	// DFRM_BACK
		0, 0, 0x800, 0, GF_BG_SCR_SIZE_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xc000, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
		0, 0, 0, FALSE
	},
	{	// DFRM_MSG
		0, 0, 0x800, 0, GF_BG_SCR_SIZE_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xc800, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01,
		3, 0, 0, FALSE
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
static void HackBox_VBlankCB(void *param);
static void HackBoxTool_DrawSprite(HackBoxTool *hackBox);
static void HackBoxTool_DrawWindow(HackBoxTool *hackBox);
static void HackBox_LoadString(u16 *stringPtr, String *outString);
static void HackBox_Load4BPPScreen(HackBoxTool *hackBox, int fileIndex, u8 bgLayout, int size);

extern u16 gText_titleName[];
extern u16 gText_InfoText[];

// 初始化
BOOL HackBoxTool_Init(OverlayManager *ovyMan, int *pState)
{
    HackBoxTool *data;
    Main_SetVBlankIntrCB(NULL, NULL);

    CreateHeap(HEAPID_BASE_APP, HEAP_ID_HACK_BOX, 0x30000);

    data = OverlayManager_CreateAndGetData(ovyMan, sizeof(HackBoxTool), HEAP_ID_HACK_BOX);
    memset(data, 0, sizeof(HackBoxTool));

    HackBoxTool_DrawScreen(data);
	HackBoxTool_DrawSprite(data);
	HackBoxTool_DrawWindow(data);

	GfGfx_EngineATogglePlanes(GX_PLANEMASK_OBJ, TRUE);
    Main_SetVBlankIntrCB(HackBox_VBlankCB, NULL);
	HBlankInterruptDisable();

	BeginNormalPaletteFade(0, 1, 1, RGB_BLACK, 16, 1, HEAP_ID_HACK_BOX);
    return TRUE;
}

static void HackBoxTool_DrawScreen(HackBoxTool *hackBox)
{
    hackBox->fileHandle = NARC_New(85, HEAP_ID_HACK_BOX);
    hackBox->bgConfig = BgConfig_Alloc(HEAP_ID_HACK_BOX);

    GfGfx_SetBanks(&graphicsBanks);
    SetBothScreensModesAndDisable(&graphicsModes);

    for (int i = 0; i < NELEMS(sHackBoxBgTemplate); i++)
    {
        InitBgFromTemplate(hackBox->bgConfig, i, &sHackBoxBgTemplate[i], GF_BG_TYPE_TEXT);
        BG_ClearCharDataRange(i, 32, 0, HEAP_ID_HACK_BOX);
        BgClearTilemapBufferAndCommit(hackBox->bgConfig, i);
    }

    GfGfxLoader_GXLoadPalFromOpenNarc(hackBox->fileHandle, 5, GF_PAL_LOCATION_MAIN_BG, (enum GFPalSlotOffset)0, 11 * 32, HEAP_ID_HACK_BOX);
    GfGfxLoader_GXLoadPalFromOpenNarc(hackBox->fileHandle, 5, GF_PAL_LOCATION_SUB_BG, (enum GFPalSlotOffset)0, 11 * 32, HEAP_ID_HACK_BOX);

    GfGfxLoader_LoadCharDataFromOpenNarc(hackBox->fileHandle, 7, hackBox->bgConfig, GF_BG_LYR_MAIN_1, 0, 0, TRUE, HEAP_ID_HACK_BOX);
	GfGfxLoader_LoadCharDataFromOpenNarc(hackBox->fileHandle, 7, hackBox->bgConfig, 2, 0, 0, TRUE, HEAP_ID_HACK_BOX);
	GfGfxLoader_LoadCharDataFromOpenNarc(hackBox->fileHandle, 7, hackBox->bgConfig, 6, 0, 0, TRUE, HEAP_ID_HACK_BOX);

	HackBox_Load4BPPScreen(hackBox, 14, GF_BG_LYR_MAIN_1, 2048);
	HackBox_Load4BPPScreen(hackBox, 11, GF_BG_LYR_SUB_2, 2048);

	// NARC_Delete(hackBox->fileHandle);
}

static void HackBoxTool_DrawSprite(HackBoxTool *hackBox)
{
	NNS_G2dInitOamManagerModule();
	GF_CreateVramTransferManager(16, HEAP_ID_HACK_BOX);
	OamManager_Create(0, 128, 0, 32, 0, 128, 0, 32, HEAP_ID_HACK_BOX);

	ObjCharTransferTemplate tmplate = {
        .maxTasks = 20,
        .sizeMain = 0x800,
        .sizeSub = 0x800,
        .heapId = HEAP_ID_HACK_BOX,
    };
    ObjCharTransfer_Init(&tmplate);
	ObjPlttTransfer_Init(20, HEAP_ID_HACK_BOX);
	ObjCharTransfer_ClearBuffers();
    ObjPlttTransfer_Reset();

	hackBox->spriteList = G2dRenderer_Init(44, &hackBox->g2dRender, HEAP_ID_HACK_BOX);
	G2dRenderer_SetSubSurfaceCoords(&hackBox->g2dRender, 0, FX32_CONST(256));

	for (int i = 0; i < 4; ++i)
        hackBox->gfxResMen[i] = Create2DGfxResObjMan(2, (GfGfxResType)i, HEAP_ID_HACK_BOX);

}

static void HackBoxTool_DrawWindow(HackBoxTool *hackBox)
{
	hackBox->msgFormat = MessageFormat_New(HEAP_ID_HACK_BOX);
	hackBox->msgData = NewMsgDataFromNarc(MSGDATA_LOAD_DIRECT, NARC_msgdata_msg, 249, HEAP_ID_HACK_BOX);
	FontID_Alloc(0, HEAP_ID_HACK_BOX);

    LoadFontPal1(GF_PAL_LOCATION_MAIN_BG, (enum GFPalSlotOffset)0x180, HEAP_ID_HACK_BOX);
    LoadUserFrameGfx2(hackBox->bgConfig, GF_BG_LYR_SUB_0, 0x100, 10, 0, HEAP_ID_HACK_BOX);
    LoadFontPal1(GF_PAL_LOCATION_SUB_BG, (enum GFPalSlotOffset)0x180, HEAP_ID_HACK_BOX);

	hackBox->menuSpawner = TouchscreenListMenuSpawner_Create(HEAP_ID_HACK_BOX, 0);
	// 标题部分
	AddWindowParameterized(hackBox->bgConfig, &hackBox->titleWindow, GF_BG_LYR_MAIN_0, 1, 0, 8, 3, 11, 20);
	// hackBox->textString = NewString_ReadMsgData(hackBox->msgData, 7);
	hackBox->textString = String_New(128, HEAP_ID_HACK_BOX);
	HackBox_LoadString(gText_titleName, hackBox->textString);

	FillWindowPixelBuffer(&hackBox->titleWindow, 0);
    AddTextPrinterParameterizedWithColor(&hackBox->titleWindow, 0, hackBox->textString, 7, 4, TEXT_SPEED_NOTRANSFER, MAKE_TEXT_COLOR(1, 2, 0), NULL);
    CopyWindowToVram(&hackBox->titleWindow);

	// 介绍部分
	AddWindowParameterized(hackBox->bgConfig, &hackBox->infoWindow, GF_BG_LYR_MAIN_0, 6, 7, 20, 10, 11, 20 + 24);
	HackBox_LoadString(gText_InfoText, hackBox->textString);

	FillWindowPixelBuffer(&hackBox->infoWindow, 0);
    AddTextPrinterParameterizedWithColor(&hackBox->infoWindow, 0, hackBox->textString, 7, 4, TEXT_SPEED_NOTRANSFER, MAKE_TEXT_COLOR(1, 2, 0), NULL);
    CopyWindowToVram(&hackBox->infoWindow);
}

static void HackBox_VBlankCB(void *param)
{
	HackBoxTool *hackBox = param;
	DoScheduledBgGpuUpdates(hackBox->bgConfig);
    GF_RunVramTransferTasks();
    OamManager_ApplyAndResetBuffers();
}

// 功能性函数
static void HackBox_LoadString(u16 *stringPtr, String *outString)
{
	int index;

	for (index = 0;; index++, stringPtr++)
	{
		outString->data[index] = *stringPtr;
		if (*stringPtr == 0xFFFF)
			break;
	}
	outString->maxsize = index;
	outString->size = index;
}

static void HackBox_Load4BPPScreen(HackBoxTool *hackBox, int fileIndex, u8 bgLayout, int size)
{
	void *rawData = GfGfxLoader_LoadFromOpenNarc(hackBox->fileHandle, fileIndex, FALSE, HEAP_ID_HACK_BOX, FALSE);
	void *bgTilemapBuffer = GetBgTilemapBuffer(hackBox->bgConfig, bgLayout);

	if (bgTilemapBuffer != NULL) {
		BG_LoadScreenTilemapData(hackBox->bgConfig, (u8)bgLayout, rawData, size);
		BgCopyOrUncompressTilemapBufferRangeToVram(hackBox->bgConfig, (u8)bgLayout, rawData, size, 0);
	}
	Heap_Free(rawData);
}

BOOL HackBoxTool_Main(OverlayManager *ovyMan, int *pState)
{
    return FALSE;
}

BOOL HackBoxTool_Exit(OverlayManager *ovyMan, int *pState)
{
    return TRUE;
}