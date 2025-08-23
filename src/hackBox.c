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
    },
    {
        .bufferSize = GF_BG_BUF_SIZE_512x256_4BPP,
        .baseTile = 0x00000000,
        .size = GF_BG_SCR_SIZE_512x256,
        .colorMode = GX_BG_COLORMODE_16,
        .screenBase = GX_BG_SCRBASE_0xe000,
        .charBase = GX_BG_CHARBASE_0x10000,
        .bgExtPltt = GX_BG_EXTPLTT_01,
        .priority = 2,
        .areaOver = GX_BG_AREAOVER_XLU,
    },
    {
        .bufferSize = GF_BG_BUF_SIZE_256x256_4BPP,
        .baseTile = 0x00000000,
        .size = GF_BG_SCR_SIZE_256x256,
        .colorMode = GX_BG_COLORMODE_16,
        .screenBase = GX_BG_SCRBASE_0xd000,
        .charBase = GX_BG_CHARBASE_0x00000,
        .bgExtPltt = GX_BG_EXTPLTT_01,
        .priority = 3,
        .areaOver = GX_BG_AREAOVER_XLU,
        .mosaic = 0x00000000,
    },
    {
        .bufferSize = 0xFFFF,
    },
    {
        .bufferSize = GF_BG_BUF_SIZE_256x256_4BPP,
        .size = GF_BG_SCR_SIZE_256x256,
        .colorMode = GX_BG_COLORMODE_16,
        .screenBase = GX_BG_SCRBASE_0xf800,
        .charBase = GX_BG_CHARBASE_0x10000,
        .bgExtPltt = GX_BG_EXTPLTT_01,
        .priority = 0,
        .areaOver = GX_BG_AREAOVER_XLU,
    }
};

static void HackBox_InitObjCharPlttTransfer(void);
static void HackBox_ToggleGfxPlanes(BOOL enable);
static void HackBox_VBlankCB(void *param);
static void HackBox_LoadBgResource(BgConfig *bgConfig);

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
    CreateHeap(HEAPID_BASE_APP, HEAP_ID_HACK_BOX, 0x20000);

    data = OverlayManager_CreateAndGetData(ovyMan, sizeof(HackBoxTool), HEAP_ID_HACK_BOX);
    memset(data, 0, sizeof(HackBoxTool));

    data->bgConfig = BgConfig_Alloc(HEAP_ID_HACK_BOX);
    data->msgFormat = MessageFormat_New(HEAP_ID_HACK_BOX);
    // 加载文本文件
    // data->msgData_249 = NewMsgDataFromNarc(MSGDATA_LOAD_DIRECT, NARC_msgdata_msg, NARC_msg_msg_0249_bin, HEAP_ID_NAMING_SCREEN);

    SetKeyRepeatTimers(4, 8);

    // gfx初始化
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

    // 图层模式管理
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
        if (bgTemplate[i].bufferSize != 0xFFFF)
        {
            InitBgFromTemplate(data->bgConfig, i, &bgTemplate[i], GF_BG_TYPE_TEXT);
            BgClearTilemapBufferAndCommit(data->bgConfig, i);
        }
    }
    HackBox_ToggleGfxPlanes(FALSE);
    BG_ClearCharDataRange(GF_BG_LYR_MAIN_0, 0x20, 0, HEAP_ID_HACK_BOX);
    BG_ClearCharDataRange(GF_BG_LYR_SUB_0, 0x20, 0, HEAP_ID_HACK_BOX);
    G2S_BlendNone();

    // 字体初始化
    FontID_Alloc(2, HEAP_ID_HACK_BOX);

    // Vblank回调
    Main_SetVBlankIntrCB(HackBox_VBlankCB, NULL);
    // obj资源管理器初始化
    HackBox_InitObjCharPlttTransfer();

    // 加载图形资源
    HackBox_LoadBgResource(data->bgConfig);

    BeginNormalPaletteFade(0, 1, 1, RGB_BLACK, 16, 1, HEAP_ID_HACK_BOX);
    HackBox_ToggleGfxPlanes(GF_PLANE_TOGGLE_ON);
    GfGfx_BothDispOn();

    return TRUE;
}

static void HackBox_ToggleGfxPlanes(BOOL enable)
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

static void HackBox_VBlankCB(void *param)
{
    GF_RunVramTransferTasks();
    OamManager_ApplyAndResetBuffers();
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

static void HackBox_InitObjCharPlttTransfer(void)
{
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
}

static void HackBox_LoadBgResource(BgConfig *bgConfig)
{
    NARC *narc = NARC_New(NARC_data_namein, HEAP_ID_HACK_BOX);

    GfGfxLoader_GXLoadPalFromOpenNarc(narc, 0, GF_PAL_LOCATION_MAIN_BG, (enum GFPalSlotOffset)0, 0x60, HEAP_ID_HACK_BOX);
    GfGfxLoader_GXLoadPal(NARC_a_0_1_4, 71, GF_PAL_LOCATION_SUB_BG, (enum GFPalSlotOffset)0, 0x20, HEAP_ID_HACK_BOX);
    BG_SetMaskColor(GF_BG_LYR_SUB_0, RGB_BLACK);

    GfGfxLoader_LoadCharDataFromOpenNarc(narc, 2, bgConfig, GF_BG_LYR_MAIN_2, 0, 0, TRUE, HEAP_ID_HACK_BOX);
    GfGfxLoader_LoadScrnDataFromOpenNarc(narc, 4, bgConfig, GF_BG_LYR_MAIN_2, 0, 0x600, TRUE, HEAP_ID_HACK_BOX);

    GfGfxLoader_LoadCharDataFromOpenNarc(narc, 2, bgConfig, GF_BG_LYR_MAIN_1, 0, 0x2000, TRUE, HEAP_ID_HACK_BOX);
    GfGfxLoader_LoadScrnDataFromOpenNarc(narc, 6, bgConfig, GF_BG_LYR_MAIN_1, 0, 0x380, TRUE, HEAP_ID_HACK_BOX);

    GfGfxLoader_LoadScrnDataFromOpenNarc(narc, 7, bgConfig, GF_BG_LYR_MAIN_0, 0, 0x380, TRUE, HEAP_ID_HACK_BOX);

    NARC_Delete(narc);
}

BOOL HackBoxTool_Main(OverlayManager *ovyMan, int *pState)
{
    IsPaletteFadeFinished();
    return FALSE;
}