// overlay43 朋友手册的overlay
#include "global.h"
#include "gx_bgcnt.h"
#include "overlay_manager.h"
#include "gf_gfx_planes.h"
#include "bg_window.h"
#include "obj_char_transfer.h"

#define HEAPID_BASE_APP 3
#define HEAP_ID_HACK_BOX 51

typedef struct HackBoxTool
{
    void *bgConfig;
    void *msgFormat;
} HackBoxTool;

static BgTemplate bgTemplate[] =
{
    {
        .bufferSize = GF_BG_BUF_SIZE_512x256_4BPP,
        .baseTile = 0x00000000,
        .size = GF_BG_SCR_SIZE_512x256,
        .colorMode = GX_BG_COLORMODE_16,
        .screenBase = GX_BG_SCRBASE_0xf000,
        .charBase = GX_BG_CHARBASE_0x10000,
        .bgExtPltt = GX_BG_EXTPLTT_01,
        .priority = 1,
        .areaOver = GX_BG_AREAOVER_XLU,
    }
};

static void NamingScreen_InitObjCharPlttTransfer(void);
static void NamingScreen_ToggleGfxPlanes(BOOL enable);
static void NamingScreen_VBlankCB(void *param);

// 初始化
BOOL HackBoxTool_Init(OverlayManager *ovyMan, int *pState)
{
    HackBoxTool *data;
    Main_SetVBlankIntrCB(NULL, NULL);
    HBlankInterruptDisable();
    GfGfx_DisableEngineAPlanes();
    GfGfx_DisableEngineBPlanes();
    GX_SetVisiblePlane(0);
    GXS_SetVisiblePlane(0);
    CreateHeap(HEAPID_BASE_APP, HEAP_ID_HACK_BOX, 0x28000);

    data = OverlayManager_CreateAndGetData(ovyMan, sizeof(HackBoxTool), HEAP_ID_HACK_BOX);
    memset(data, 0, sizeof(HackBoxTool));

    data->bgConfig = BgConfig_Alloc(HEAP_ID_HACK_BOX);
    data->msgFormat = MessageFormat_New(HEAP_ID_HACK_BOX);

    SetKeyRepeatTimers(4, 8);

    GraphicsBanks graphicsBanks = {
        GX_VRAM_BG_128_A,
        GX_VRAM_BGEXTPLTT_NONE,
        GX_VRAM_SUB_BG_128_C,
        GX_VRAM_SUB_BGEXTPLTT_NONE,
        GX_VRAM_OBJ_128_B,
        GX_VRAM_OBJEXTPLTT_NONE,
        GX_VRAM_SUB_OBJ_16_I,
        GX_VRAM_SUB_OBJEXTPLTT_NONE,
        GX_VRAM_TEX_NONE,
        GX_VRAM_TEXPLTT_NONE,
    };
    GfGfx_SetBanks(&graphicsBanks);

    GraphicsModes graphicsModes = {
        GX_DISPMODE_GRAPHICS,
        GX_BGMODE_0,
        GX_BGMODE_0,
        GX_BG0_AS_2D,
    };
    SetBothScreensModesAndDisable(&graphicsModes);

    // 图层初始化
    for (int i = 0; i < NELEMS(bgTemplate); i++)
    {
        InitBgFromTemplate(data->bgConfig, i, &bgTemplate, GF_BG_TYPE_TEXT);
        BgClearTilemapBufferAndCommit(data->bgConfig, i);
    }
    NamingScreen_ToggleGfxPlanes(FALSE);
    G2S_BlendNone();

    // 字体初始化
    FontID_Alloc(2, HEAP_ID_HACK_BOX);
    Main_SetVBlankIntrCB(NamingScreen_VBlankCB, NULL);
    NamingScreen_InitObjCharPlttTransfer();

    return TRUE;
}

static void NamingScreen_ToggleGfxPlanes(BOOL enable)
{
    GfGfx_EngineATogglePlanes(GX_PLANEMASK_BG0, enable);
    GfGfx_EngineATogglePlanes(GX_PLANEMASK_BG1, enable);
    GfGfx_EngineATogglePlanes(GX_PLANEMASK_BG2, enable);
    GfGfx_EngineATogglePlanes(GX_PLANEMASK_BG3, FALSE);
    GfGfx_EngineATogglePlanes(GX_PLANEMASK_OBJ, enable);
    GfGfx_EngineBTogglePlanes(GX_PLANEMASK_BG0, enable);
    GfGfx_EngineBTogglePlanes(GX_PLANEMASK_BG1, FALSE);
    GfGfx_EngineBTogglePlanes(GX_PLANEMASK_OBJ, FALSE);
}

static void NamingScreen_VBlankCB(void *param)
{
    GF_RunVramTransferTasks();
    OamManager_ApplyAndResetBuffers();
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

static void NamingScreen_InitObjCharPlttTransfer(void)
{
    ObjCharTransferTemplate tmplate = {
        .maxTasks = 20,
        .sizeMain = 0x800,
        .sizeSub = 0x800,
        .heapId = HEAP_ID_NAMING_SCREEN,
    };
    ObjCharTransfer_Init(&tmplate);
    ObjPlttTransfer_Init(20, HEAP_ID_NAMING_SCREEN);
    ObjCharTransfer_ClearBuffers();
    ObjPlttTransfer_Reset();
}

BOOL HackBoxTool_Main(OverlayManager *ovyMan, int *pState)
{
    Main_SetVBlankIntrCB(NULL, NULL);
    HBlankInterruptDisable();
    GfGfx_DisableEngineAPlanes();
    GfGfx_DisableEngineBPlanes();
    GX_SetVisiblePlane(0);
    GXS_SetVisiblePlane(0);
    CreateHeap(HEAPID_BASE_APP, HEAP_ID_HACK_BOX, 0x28000);

    return TRUE;
}