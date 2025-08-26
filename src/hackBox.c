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
#include "font.h"
#include "font_data.h"
#include "unk_0200B150.h"
#include "unk_0200FA24.h"
#include "unk_02013534.h"
#include "touchscreen_list_menu.h"
#include "unk_0200ACF0.h"
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
	SpriteResource *gfxResObjs[4];
	NNSG2dRenderSurface surface;
	Window titleWindow;
	Window infoWindow;
	Window mainButtonWindow[4];
	String *textString;
	TouchscreenListMenuSpawner *menuSpawner;
	int needReloadFont[5];
	int cursor;
	SpriteResourcesHeader spriteHeader;
	Sprite *cursorSprite;
	void *charDataRaw;
    NNSG2dCharacterData *charData;
    void *monIconCharDaraRaw;
    NNSG2dCharacterData *monIconCharData;
    void *plttDataRaw;
    NNSG2dPaletteData *plttData;
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
void HackBoxTool_DrawSprite(HackBoxTool *hackBox);
static void HackBoxTool_DrawWindow(HackBoxTool *hackBox);
static void HackBox_LoadString(u16 *stringPtr, String *outString);
static void HackBox_Load4BPPScreen(HackBoxTool *hackBox, int fileIndex, u8 bgLayout, int size);
static void HackBoxTool_DrawSelectButton(HackBoxTool *hackBox);
static void HackBox_LoadFont(HackBoxTool *hackBox, int fontID, int newFiles);
static void HackBoxTool_ChangeSelectButton(HackBoxTool *hackBox);
static void HackBoxTool_ChangeCursor(HackBoxTool *hackBox);

extern u16 gText_titleName[];
extern u16 gText_InfoText[];
extern u16 gText_PokemonEdit[];
extern u16 gText_ItemEdit[];
extern u16 gText_MoreEdit[];
extern u16 gText_ExitBox[];

static u16* sChooseText[] = 
{
	gText_PokemonEdit,
	gText_ItemEdit,
	gText_MoreEdit,
	gText_ExitBox,
};

// 初始化
BOOL HackBoxTool_Init(OverlayManager *ovyMan, int *pState)
{
    HackBoxTool *data;
    Main_SetVBlankIntrCB(NULL, NULL);

    CreateHeap(HEAPID_BASE_APP, HEAP_ID_HACK_BOX, 0x30000);

    data = OverlayManager_CreateAndGetData(ovyMan, sizeof(HackBoxTool), HEAP_ID_HACK_BOX);
    memset(data, 0, sizeof(HackBoxTool));
    data->fileHandle = NARC_New(85, HEAP_ID_HACK_BOX);
    data->bgConfig = BgConfig_Alloc(HEAP_ID_HACK_BOX);

    HackBoxTool_DrawScreen(data);
	HackBoxTool_DrawSprite(data);
	HackBoxTool_DrawWindow(data);
	HackBoxTool_DrawSelectButton(data);

	GfGfx_EngineATogglePlanes(GX_PLANEMASK_OBJ, TRUE);
    Main_SetVBlankIntrCB(HackBox_VBlankCB, data);
	HBlankInterruptDisable();

	BeginNormalPaletteFade(0, 1, 1, RGB_BLACK, 16, 1, HEAP_ID_HACK_BOX);
    return TRUE;
}

static void HackBoxTool_DrawScreen(HackBoxTool *hackBox)
{
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

static NNSG2dViewRect DATA_ScrollSurfaceRect = 
{
	{ 0, 256 },
	{ 255 << 12, 192 << 12}
};

void HackBoxTool_DrawSprite(HackBoxTool *hackBox)
{
	NNS_G2dInitOamManagerModule();
	GF_CreateVramTransferManager(16, HEAP_ID_HACK_BOX);
	OamManager_Create(0, 128, 0, 32, 0, 128, 0, 32, HEAP_ID_HACK_BOX);

	ObjCharTransferTemplate tmplate = {
        .maxTasks = 64,
        .sizeMain = 0x800,
        .sizeSub = 0x800,
        .heapId = HEAP_ID_HACK_BOX,
    };
	ObjCharTransfer_InitEx(&tmplate, GX_OBJVRAMMODE_CHAR_1D_32K, GX_OBJVRAMMODE_CHAR_1D_32K);
	ObjPlttTransfer_Init(32, HEAP_ID_HACK_BOX);
	ObjCharTransfer_ClearBuffers();
    ObjPlttTransfer_Reset();

	hackBox->spriteList = G2dRenderer_Init(32, &hackBox->g2dRender, HEAP_ID_HACK_BOX);
	sub_0200B27C(&hackBox->surface, &DATA_ScrollSurfaceRect, NNS_G2D_VRAM_TYPE_2DSUB, &hackBox->g2dRender.rendererInstance);

	for (int i = 0; i < 4; ++i)
        hackBox->gfxResMen[i] = Create2DGfxResObjMan(32, (GfGfxResType)i, HEAP_ID_HACK_BOX);

	hackBox->gfxResObjs[0] = AddCharResObjFromOpenNarc(hackBox->gfxResMen[0], hackBox->fileHandle, 1, TRUE, 100, NNS_G2D_VRAM_TYPE_2DSUB, HEAP_ID_HACK_BOX);
	hackBox->gfxResObjs[1] = AddPlttResObjFromOpenNarc(hackBox->gfxResMen[1], hackBox->fileHandle, 0, FALSE, 100, NNS_G2D_VRAM_TYPE_2DSUB, 5, HEAP_ID_HACK_BOX);
	hackBox->gfxResObjs[2] = AddCellOrAnimResObjFromOpenNarc(hackBox->gfxResMen[2], hackBox->fileHandle, 2, TRUE, 100, GF_GFX_RES_TYPE_CELL, HEAP_ID_HACK_BOX);
	hackBox->gfxResObjs[3] = AddCellOrAnimResObjFromOpenNarc(hackBox->gfxResMen[3], hackBox->fileHandle, 3, TRUE, 100, GF_GFX_RES_TYPE_ANIM, HEAP_ID_HACK_BOX);
	// sub_0200ADA4(hackBox->gfxResObjs[0]);
	// sub_0200B00C(hackBox->gfxResObjs[1]);
	// sub_0200A740(hackBox->gfxResObjs[0]);
	// sub_0200A740(hackBox->gfxResObjs[1]);
	sub_0200ACF0(hackBox->gfxResObjs[0]);
	sub_0200AF94(hackBox->gfxResObjs[1]);

	CreateSpriteResourcesHeader(&hackBox->spriteHeader, 100, 100, 100, 100, -1, -1, FALSE, 0, 
		hackBox->gfxResMen[0], hackBox->gfxResMen[1], hackBox->gfxResMen[2], hackBox->gfxResMen[3], NULL, NULL);

	SpriteTemplate spriteTemplate;
	spriteTemplate.spriteList = hackBox->spriteList;
	spriteTemplate.header = &hackBox->spriteHeader;
	spriteTemplate.position.x = 0;
	spriteTemplate.position.y = 0;
	spriteTemplate.position.z = 0;
	spriteTemplate.scale.x = FX32_ONE;
	spriteTemplate.scale.y = FX32_ONE;
	spriteTemplate.scale.z = FX32_ONE;
	spriteTemplate.rotation = 0;
	spriteTemplate.priority = 1;
	spriteTemplate.whichScreen = NNS_G2D_VRAM_TYPE_2DSUB;
	spriteTemplate.heapId = HEAP_ID_HACK_BOX;
	
	hackBox->cursorSprite = Sprite_CreateAffine(&spriteTemplate);

	Sprite_SetDrawFlag(hackBox->cursorSprite, TRUE);
	Sprite_SetAnimCtrlSeq(hackBox->cursorSprite, 0);

	GfGfx_EngineATogglePlanes(GX_PLANEMASK_OBJ, GF_PLANE_TOGGLE_ON);
    GfGfx_EngineBTogglePlanes(GX_PLANEMASK_OBJ, GF_PLANE_TOGGLE_ON);
}

static void HackBoxTool_DrawWindow(HackBoxTool *hackBox)
{
	hackBox->msgFormat = MessageFormat_New(HEAP_ID_HACK_BOX);
	hackBox->msgData = NewMsgDataFromNarc(MSGDATA_LOAD_DIRECT, NARC_msgdata_msg, 249, HEAP_ID_HACK_BOX);
	HackBox_LoadFont(hackBox, 4, 10);
	HackBox_LoadFont(hackBox, 0, 11);

    LoadUserFrameGfx2(hackBox->bgConfig, GF_BG_LYR_SUB_0, 0x100, 10, 0, HEAP_ID_HACK_BOX);
    LoadFontPal1(GF_PAL_LOCATION_MAIN_BG, 11 * 32, HEAP_ID_HACK_BOX);
    LoadFontPal1(GF_PAL_LOCATION_SUB_BG, 11 * 32, HEAP_ID_HACK_BOX);

	hackBox->menuSpawner = TouchscreenListMenuSpawner_Create(HEAP_ID_HACK_BOX, 0);
	// 标题部分
	AddWindowParameterized(hackBox->bgConfig, &hackBox->titleWindow, GF_BG_LYR_MAIN_0, 2, 0, 8, 3, 11, 20);
	// hackBox->textString = NewString_ReadMsgData(hackBox->msgData, 7);
	hackBox->textString = String_New(128, HEAP_ID_HACK_BOX);
	HackBox_LoadString(gText_titleName, hackBox->textString);

	FillWindowPixelBuffer(&hackBox->titleWindow, 0);
    AddTextPrinterParameterizedWithColor(&hackBox->titleWindow, 0, hackBox->textString, 2, 7, TEXT_SPEED_NOTRANSFER, MAKE_TEXT_COLOR(11, 2, 0), NULL);
    CopyWindowToVram(&hackBox->titleWindow);

	// 介绍部分
	AddWindowParameterized(hackBox->bgConfig, &hackBox->infoWindow, GF_BG_LYR_MAIN_0, 2, 7, 24, 10, 11, 20 + 24);
	HackBox_LoadString(gText_InfoText, hackBox->textString);

	FillWindowPixelBuffer(&hackBox->infoWindow, 0);
    AddTextPrinterParameterizedWithColor(&hackBox->infoWindow, 0, hackBox->textString, 7, 4, TEXT_SPEED_NOTRANSFER, MAKE_TEXT_COLOR(1, 2, 0), NULL);
    CopyWindowToVram(&hackBox->infoWindow);
}

static void HackBoxTool_DrawSelectButton(HackBoxTool *hackBox)
{
	u16 startTiles = 20 + 24 + 240;
	for (int i = 0; i < NELEMS(hackBox->mainButtonWindow); i++)
	{
		InitWindow(&hackBox->mainButtonWindow[i]);
		AddWindowParameterized(hackBox->bgConfig, &hackBox->mainButtonWindow[i], GF_BG_LYR_SUB_0, 10, 3 + i * 5, 12, 3, 11, startTiles + i * 36);
		FillWindowPixelBuffer(&hackBox->mainButtonWindow[i], 0);
		HackBox_LoadString(sChooseText[i], hackBox->textString);
		u32 stringWidth = FontID_String_GetWidth(4, hackBox->textString, 0);
		AddTextPrinterParameterizedWithColor(&hackBox->mainButtonWindow[i], 4, hackBox->textString, 2 + (96 - stringWidth) / 2, 0, TEXT_SPEED_NOTRANSFER, MAKE_TEXT_COLOR(1, 15, 0), NULL);
		CopyWindowToVram(&hackBox->mainButtonWindow[i]);
	}
	// HackBoxTool_ChangeSelectButton(hackBox);
}

static void HackBoxTool_ChangeSelectButton(HackBoxTool *hackBox)
{
	int i;
	u8 pal;

	for (i = 0; i < 4; i++)
	{
		if (hackBox->cursor == i)
			pal = 9;
		else
			pal = 2;

		BgTilemapRectChangePalette(hackBox->bgConfig, GF_BG_LYR_SUB_2, 2, 2 + 5 * i, 28, 4, pal);
	}
	ScheduleBgTilemapBufferTransfer(hackBox->bgConfig, GF_BG_LYR_SUB_2);
}

static void HackBoxTool_ChangeCursor(HackBoxTool *hackBox)
{
	Sprite_SetPositionXY(hackBox->cursorSprite, 128, 40 * hackBox->cursor + 32);
}

static void HackBox_VBlankCB(void *param)
{
	HackBoxTool *hackBox = param;
    GF_RunVramTransferTasks();
	DoScheduledBgGpuUpdates(hackBox->bgConfig);
    OamManager_ApplyAndResetBuffers();
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
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
	outString->size = index - 1;
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

static void HackBox_LoadFont(HackBoxTool *hackBox, int fontID, int newFiles)
{
	// 重新加载字体图片
	// 默认ROM使用的xzonn的码表，为了保证acg汉化版本能显示正确的字，这里把字体读取成xzonn版本的
	// 如果字体正在被共用，程序结束后需要重新加载原字库
	// 注意：新增的字体文件日版和美版不同
	if (sFontWork->fontDataRefCount[fontID] > 0)
	{
		FontData_Delete(sFontWork->fontDataMan[fontID]);
		hackBox->needReloadFont[fontID] = TRUE;
	}

	sFontWork->fontDataMan[fontID] = FontData_New(NARC_graphic_font, newFiles, FONTARC_MODE_LAZY, FALSE, HEAP_ID_HACK_BOX);
	sFontWork->fontDataRefCount[fontID]++;
}

BOOL HackBoxTool_Main(OverlayManager *ovyMan, int *pState)
{
	HackBoxTool *hackBox = OverlayManager_GetData(ovyMan);
	// Sprite_SetPositionXY(hackBox->cursorSprite, 128, 40 * hackBox->cursor);
	SpriteList_RenderAndAnimateSprites(hackBox->spriteList);
	HackBoxTool_ChangeCursor(hackBox);
    return FALSE;
}

BOOL HackBoxTool_Exit(OverlayManager *ovyMan, int *pState)
{
    return TRUE;
}