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
#include "hackbox_pokemon.h"

#define FLD_MBGFRM_FONT 0
#define MENU_WIN_PAL 13

typedef struct
{
    Pokemon *PokeMakeData;
    u32 PMD[64];
    u32 data_bak;

    u16 str[0x100];

    u8 page_p;
    u8 cursor_p;
    u8 value_p;
} PokeMakeWork;

typedef struct
{
    const u8 *page;
    u8 listcount;
} PmakePageTable;

typedef struct {
	u32	min;
	u32	max;
	u8	mode;
	u16	count;
}PmakeCont;

typedef struct {
	u32 label;
	const PmakeCont * cont;
}PmakeParamData;

typedef struct
{
    Window win;
    Window win_title;
    SaveData *saveData;
    HackBoxTool *hackBoxTool;
    MessageFormat *wset;
    struct ListMenuCursor *cursor;
    PokeMakeWork pmw;
    u32 seq;
    u32 mode;
} D_POKEMONMAKE;

static const PmakeCont cont00 = {1, 1024, PMC_INCDEC, 4};
static const PmakeCont cont01 = {1, 100, PMC_INCDEC, 3};
static const PmakeCont cont02 = {1, 2000000, PMC_INCDEC, 7};
static const PmakeCont cont03 = {0, 0xffffffff, PMC_INCDEC, 10};
static const PmakeCont cont04 = {0, 0xffffffff, PMC_INCDEC, 10};
static const PmakeCont cont05 = {0, 2, PMC_INCDEC, 0xff};
static const PmakeCont cont06 = {0, 24, PMC_INCDEC, 0xff};
static const PmakeCont cont07 = {0, MOVENO_MAX, PMC_INCDEC, 3};
static const PmakeCont cont08 = {0, MOVENO_MAX, PMC_INCDEC, 3};
static const PmakeCont cont09 = {0, MOVENO_MAX, PMC_INCDEC, 3};
static const PmakeCont cont10 = {0, MOVENO_MAX, PMC_INCDEC, 3};
static const PmakeCont cont11 = {0, ITEM_DATA_MAX, PMC_INCDEC, 3};
static const PmakeCont cont12 = {0, ABILITY_DATA_MAX, PMC_INCDEC, 3};
static const PmakeCont cont13 = {0, 31, PMC_INCDEC, 2};
static const PmakeCont cont14 = {0, 255, PMC_INCDEC, 3};
static const PmakeCont cont15 = {0, 31, PMC_INCDEC, 2};
static const PmakeCont cont16 = {0, 255, PMC_INCDEC, 3};
static const PmakeCont cont17 = {0, 31, PMC_INCDEC, 2};
static const PmakeCont cont18 = {0, 255, PMC_INCDEC, 3};
static const PmakeCont cont19 = {0, 31, PMC_INCDEC, 2};
static const PmakeCont cont20 = {0, 255, PMC_INCDEC, 3};
static const PmakeCont cont21 = {0, 31, PMC_INCDEC, 2};
static const PmakeCont cont22 = {0, 255, PMC_INCDEC, 3};
static const PmakeCont cont23 = {0, 31, PMC_INCDEC, 2};
static const PmakeCont cont24 = {0, 255, PMC_INCDEC, 3};
static const PmakeCont cont47 = {0, 255, PMC_INCDEC, 3};

static const PmakeParamData PMakelabelTable[] =
{
    {msg_pmlabel_00, &cont00},
    {msg_pmlabel_01, &cont01},
    {msg_pmlabel_02, &cont02},
    {msg_pmlabel_03, &cont03},
    {msg_pmlabel_04, &cont04},
    {msg_pmlabel_05, &cont05},
    {msg_pmlabel_06, &cont06},
    {msg_pmlabel_07, &cont07},
    {msg_pmlabel_08, &cont08},
    {msg_pmlabel_09, &cont09},
    {msg_pmlabel_10, &cont10},
    {msg_pmlabel_11, &cont11},
    {msg_pmlabel_12, &cont12},
    {msg_pmlabel_13, &cont13},
    {msg_pmlabel_14, &cont14},
    {msg_pmlabel_15, &cont15},
    {msg_pmlabel_16, &cont16},
    {msg_pmlabel_17, &cont17},
    {msg_pmlabel_18, &cont18},
    {msg_pmlabel_19, &cont19},
    {msg_pmlabel_20, &cont20},
    {msg_pmlabel_21, &cont21},
    {msg_pmlabel_22, &cont22},
    {msg_pmlabel_23, &cont23},
    {msg_pmlabel_24, &cont24},
    [PMAKE_FORM_NO] = {msg_pmlabel_47, &cont47},
};

static const u8 Page1[] = {
	PMAKE_NAME, PMAKE_LEVEL, PMAKE_EXP, PMAKE_ID,
	PMAKE_PERRND, PMAKE_SEX, PMAKE_PERSONAL, 0xff
};
static const u8 Page3[] = {
	PMAKE_WAZA1, PMAKE_WAZA2, PMAKE_WAZA3,
	PMAKE_WAZA4, PMAKE_ITEM, PMAKE_SPABI, PMAKE_FORM_NO, 0xff
};
static const u8 Page4[] = {
	PMAKE_HP_RND, PMAKE_HP_EXP, PMAKE_POW_RND, PMAKE_POW_EXP,
	PMAKE_DEF_RND, PMAKE_DEF_EXP, PMAKE_BATTLEPARAM1, 0xff
};
static const u8 Page5[] = {
	PMAKE_AGI_RND, PMAKE_AGI_EXP, PMAKE_EXPOW_RND, PMAKE_EXPOW_EXP,
	PMAKE_EXDEF_RND, PMAKE_EXDEF_EXP, PMAKE_BATTLEPARAM2, 0xff
};

#define PMAKE_PAGE_MAX (4)
static const PmakePageTable PageTable[] =
{
    {Page1, 5},
    {Page3, 7},
    {Page4, 6},
    {Page5, 6}
};

static void PokeMakePokeParaCalcInit(PokeMakeWork *dpw);
static void PokeMakeWorkInit(HackBoxTool * hackbox, PokeMakeWork *dpw);
static void PokeMakePokeParaWorkGetAll(PokeMakeWork *dpw);
static void D_PokemonMakeMain(SysTask *_tcb, void *work);
static u8 PutProcString(D_POKEMONMAKE *wk, u8 id, u32 pal, u8 y);
static void PokeMakeSeq_PagePut(D_POKEMONMAKE *wk);
static void CursorPut( D_POKEMONMAKE * wk, u8 mode );
static void PokeMakeSeq_ParamSelect(D_POKEMONMAKE *wk);
static void PokeMake_NumPrint(Window *win, MessageFormat *wset, PokeMakeWork *dpw, u32 num, u32 keta, u32 x, u32 y, u32 wait, u32 col);
static void PokeMake_StrPrintExp(Window *win, MessageFormat *wset, u32 id, u32 x, u32 y, u32 wait, u32 col);
static void PokeMakeSeq_ParamChange(D_POKEMONMAKE *wk);
static void ValueControl(PokeMakeWork *dpw, u8 mode);
static void PokeMakeSeq_ButtonWait(D_POKEMONMAKE *wk);
static void PmakeExitTaskSeq(D_POKEMONMAKE *wk);
static void D_PokeWazaSetPos(Pokemon *pokeparam, u16 waza, u16 pos);

static void PokeMakeInit(D_POKEMONMAKE *wk)
{
    FillWindowPixelBuffer(&wk->win_title, 15);
    PokeMake_StrPrint(&wk->win_title, msg_pminfo_02, 0, 0, FALSE, COLOR_W_BLACK);

    wk->pmw.PokeMakeData = AllocMonZeroed(HEAP_ID_HACK_BOX);

    if (wk->mode == POKEMAKE_MODE_DEBUG) {
        PokeMakeWorkInit(wk->hackBoxTool, &wk->pmw);
    }
    else
    {
        Party *temoti = SaveArray_Party_Get(wk->saveData);
        Pokemon *change_poke = Party_GetMonByIndex(temoti, 0);

        MI_CpuCopy8(change_poke, wk->pmw.PokeMakeData, SizeOfStructPokemon());
        PokeMakePokeParaWorkGetAll(&wk->pmw);
    }

    wk->pmw.page_p   = 0;
	wk->pmw.cursor_p = 0;
}

// 生成宝可梦的函数
// 先清除主页面的窗口和oam
void DebugPokemonMakeInit(HackBoxTool *hackBox, u8 mode)
{
    D_POKEMONMAKE *wk;
    BgConfig *bgl = hackBox->bgConfig;

    // 新页面数据
    wk = SysTask_GetData(PMDS_taskAdd(D_PokemonMakeMain, sizeof(D_POKEMONMAKE), 0, HEAP_ID_HACK_BOX));

    wk->seq = 0;
    wk->mode = mode;
	wk->wset = MessageFormat_New( HEAP_ID_HACK_BOX );
	wk->cursor = ListMenuCursorNew( HEAP_ID_HACK_BOX );

    wk->saveData = hackBox->saveData;
    wk->hackBoxTool = hackBox;

    AddWindowParameterized(bgl, &wk->win_title, GF_BG_LYR_SUB_0, 1, 1, 30, 4, 11, 1);
    AddWindowParameterized(bgl, &wk->win, GF_BG_LYR_SUB_0, 1, 7, 30, 16, 11, 1 + 30 * 4);
    FillWindowPixelBuffer(&wk->win, 15);
    CopyWindowToVram(&wk->win);

    LoadUserFrameGfx2(bgl, GF_BG_LYR_SUB_0, 1 + 30 * 4 + 30 * 18, MENU_WIN_PAL, 1, HEAP_ID_HACK_BOX);
    DrawFrameAndWindow2(&wk->win_title, TRUE, 1 + 30 * 4 + 30 * 18, MENU_WIN_PAL);
    DrawFrameAndWindow2(&wk->win, TRUE, 1 + 30 * 4 + 30 * 18, MENU_WIN_PAL);

    PokeMakeInit(wk);
}

static void PokeMakeWorkInit(HackBoxTool * hackbox, PokeMakeWork *dpw)
{
    u16 i;

    for (i = 0; i < 64; i++)
    {
        dpw->PMD[i] = 0;
    }
    dpw->PMD[PMAKE_NAME] = 1;

    dpw->PMD[PMAKE_PERRND] = (LCRandom() << 16) | (LCRandom());
    PlayerProfile *player = Save_PlayerData_GetProfile(hackbox->saveData);
    dpw->PMD[PMAKE_ID] = PlayerProfile_GetTrainerID(player);
    dpw->PMD[PMAKE_LEVEL] = 1;
    dpw->PMD[PMAKE_EXP] = 1;
    dpw->PMD[PMAKE_GETLEVEL] = 1;
    dpw->PMD[PMAKE_GETCASSETTE] = LANGUAGE_JAPANESE;
    dpw->PMD[PMAKE_GETBALL] = 1;
    dpw->PMD[PMAKE_CONTRY_CODE] = 1;
    dpw->PMD[PMAKE_FORM_NO] = 0;

    dpw->PMD[PMAKE_GETPLACE_Y] = 6;
    dpw->PMD[PMAKE_GETPLACE_M] = 9;
    dpw->PMD[PMAKE_GETPLACE_D] = 28;

    dpw->PMD[PMAKE_GETPLACE2_Y] = 6;
    dpw->PMD[PMAKE_GETPLACE2_M] = 9;
    dpw->PMD[PMAKE_GETPLACE2_D] = 28;

    PokeMakePokeParaCalcInit(dpw);
}

static void PokeMakePokeParaCalcInit(PokeMakeWork *dpw)
{
    u32 pow_rnd;

    pow_rnd = ((dpw->PMD[PMAKE_HP_RND] & 0x0000001f) << 0) | ((dpw->PMD[PMAKE_POW_RND] & 0x0000001f) << 5) | ((dpw->PMD[PMAKE_DEF_RND] & 0x0000001f) << 10) | ((dpw->PMD[PMAKE_AGI_RND] & 0x0000001f) << 15) | ((dpw->PMD[PMAKE_EXPOW_RND] & 0x0000001f) << 20) | ((dpw->PMD[PMAKE_EXDEF_RND] & 0x0000001f) << 25);

    CreateMon(
        dpw->PokeMakeData,
        dpw->PMD[PMAKE_NAME],
        dpw->PMD[PMAKE_LEVEL],
        pow_rnd,
        1, dpw->PMD[PMAKE_PERRND],
        1, dpw->PMD[PMAKE_ID]);

    dpw->PMD[PMAKE_SEX] = GetMonGender(dpw->PokeMakeData);
    dpw->PMD[PMAKE_PERSONAL] = GetMonNature(dpw->PokeMakeData);
    dpw->PMD[PMAKE_WAZA1] = GetMonData(dpw->PokeMakeData, MON_DATA_MOVE1, NULL);
    dpw->PMD[PMAKE_WAZA2] = GetMonData(dpw->PokeMakeData, MON_DATA_MOVE2, NULL);
    dpw->PMD[PMAKE_WAZA3] = GetMonData(dpw->PokeMakeData, MON_DATA_MOVE3, NULL);
    dpw->PMD[PMAKE_WAZA4] = GetMonData(dpw->PokeMakeData, MON_DATA_MOVE4, NULL);
    dpw->PMD[PMAKE_EXP] = GetMonData(dpw->PokeMakeData, MON_DATA_EXPERIENCE, NULL);

    dpw->PMD[PMAKE_HP] = GetMonData(dpw->PokeMakeData, MON_DATA_MAXHP, NULL);
    dpw->PMD[PMAKE_POW] = GetMonData(dpw->PokeMakeData, MON_DATA_ATK, NULL);
    dpw->PMD[PMAKE_DEF] = GetMonData(dpw->PokeMakeData, MON_DATA_DEF, NULL);
    dpw->PMD[PMAKE_AGI] = GetMonData(dpw->PokeMakeData, MON_DATA_SPEED, NULL);
    dpw->PMD[PMAKE_EXPOW] = GetMonData(dpw->PokeMakeData, MON_DATA_SPATK, NULL);
    dpw->PMD[PMAKE_EXDEF] = GetMonData(dpw->PokeMakeData, MON_DATA_SPDEF, NULL);
}

static void PokeMakePokeParaCalcGetExp(PokeMakeWork *dpw)
{
    u32 nulldata = 0;

    SetMonData(dpw->PokeMakeData, MON_DATA_EXPERIENCE, &dpw->PMD[PMAKE_EXP]);
    // 技クリア
    SetMonData(dpw->PokeMakeData, MON_DATA_MOVE1, &nulldata);
    SetMonData(dpw->PokeMakeData, MON_DATA_MOVE2, &nulldata);
    SetMonData(dpw->PokeMakeData, MON_DATA_MOVE3, &nulldata);
    SetMonData(dpw->PokeMakeData, MON_DATA_MOVE4, &nulldata);

    CalcMonLevelAndStats(dpw->PokeMakeData);
    InitBoxMonMoveset(Mon_GetBoxMon(dpw->PokeMakeData));

    dpw->PMD[PMAKE_LEVEL] = GetMonData(dpw->PokeMakeData, MON_DATA_LEVEL, NULL);
    dpw->PMD[PMAKE_WAZA1] = GetMonData(dpw->PokeMakeData, MON_DATA_MOVE1, NULL);
    dpw->PMD[PMAKE_WAZA2] = GetMonData(dpw->PokeMakeData, MON_DATA_MOVE2, NULL);
    dpw->PMD[PMAKE_WAZA3] = GetMonData(dpw->PokeMakeData, MON_DATA_MOVE3, NULL);
    dpw->PMD[PMAKE_WAZA4] = GetMonData(dpw->PokeMakeData, MON_DATA_MOVE4, NULL);

    dpw->PMD[PMAKE_HP] = GetMonData(dpw->PokeMakeData, MON_DATA_MAXHP, NULL);
    dpw->PMD[PMAKE_POW] = GetMonData(dpw->PokeMakeData, MON_DATA_ATK, NULL);
    dpw->PMD[PMAKE_DEF] = GetMonData(dpw->PokeMakeData, MON_DATA_DEF, NULL);
    dpw->PMD[PMAKE_AGI] = GetMonData(dpw->PokeMakeData, MON_DATA_SPEED, NULL);
    dpw->PMD[PMAKE_EXPOW] = GetMonData(dpw->PokeMakeData, MON_DATA_SPATK, NULL);
    dpw->PMD[PMAKE_EXDEF] = GetMonData(dpw->PokeMakeData, MON_DATA_SPDEF, NULL);
}

static void PokeMakePokeParaCalcGetBattleParam( PokeMakeWork * dpw )
{
    SetMonData(dpw->PokeMakeData, MON_DATA_HP_IV, &dpw->PMD[PMAKE_HP_RND]);
    SetMonData(dpw->PokeMakeData, MON_DATA_HP_EV, &dpw->PMD[PMAKE_HP_EXP]);
    SetMonData(dpw->PokeMakeData, MON_DATA_ATK_IV, &dpw->PMD[PMAKE_POW_RND]);
    SetMonData(dpw->PokeMakeData, MON_DATA_ATK_EV, &dpw->PMD[PMAKE_POW_EXP]);
    SetMonData(dpw->PokeMakeData, MON_DATA_DEF_IV, &dpw->PMD[PMAKE_DEF_RND]);
    SetMonData(dpw->PokeMakeData, MON_DATA_DEF_EV, &dpw->PMD[PMAKE_DEF_EXP]);
    SetMonData(dpw->PokeMakeData, MON_DATA_SPEED_IV, &dpw->PMD[PMAKE_AGI_RND]);
    SetMonData(dpw->PokeMakeData, MON_DATA_SPEED_EV, &dpw->PMD[PMAKE_AGI_EXP]);
    SetMonData(dpw->PokeMakeData, MON_DATA_SPATK_IV, &dpw->PMD[PMAKE_EXPOW_RND]);
    SetMonData(dpw->PokeMakeData, MON_DATA_SPATK_EV, &dpw->PMD[PMAKE_EXPOW_EXP]);
    SetMonData(dpw->PokeMakeData, MON_DATA_SPDEF_IV, &dpw->PMD[PMAKE_EXDEF_RND]);
    SetMonData(dpw->PokeMakeData, MON_DATA_SPDEF_EV, &dpw->PMD[PMAKE_EXDEF_EXP]);

    CalcMonLevelAndStats(dpw->PokeMakeData);

    dpw->PMD[PMAKE_HP] = GetMonData(dpw->PokeMakeData, MON_DATA_MAXHP, NULL);
    dpw->PMD[PMAKE_POW] = GetMonData(dpw->PokeMakeData, MON_DATA_ATK, NULL);
    dpw->PMD[PMAKE_DEF] = GetMonData(dpw->PokeMakeData, MON_DATA_DEF, NULL);
    dpw->PMD[PMAKE_AGI] = GetMonData(dpw->PokeMakeData, MON_DATA_SPEED, NULL);
    dpw->PMD[PMAKE_EXPOW] = GetMonData(dpw->PokeMakeData, MON_DATA_SPATK, NULL);
    dpw->PMD[PMAKE_EXDEF] = GetMonData(dpw->PokeMakeData, MON_DATA_SPDEF, NULL);
}

static void PokeMakePokeParaCalcEnd(D_POKEMONMAKE *wk, PokeMakeWork *dpw)
{
	u32	pow_rnd;
	u32	tmp;

	pow_rnd = ( (dpw->PMD[PMAKE_HP_RND]&0x0000001f) << 0 )
			| ( (dpw->PMD[PMAKE_POW_RND]&0x0000001f) << 5 )
			| ( (dpw->PMD[PMAKE_DEF_RND]&0x0000001f) << 10 )
			| ( (dpw->PMD[PMAKE_AGI_RND]&0x0000001f) << 15 )
			| ( (dpw->PMD[PMAKE_EXPOW_RND]&0x0000001f) << 20 )
			| ( (dpw->PMD[PMAKE_EXDEF_RND]&0x0000001f) << 25 );

    CreateMon(
        dpw->PokeMakeData,
        dpw->PMD[PMAKE_NAME],
        dpw->PMD[PMAKE_LEVEL],
        pow_rnd,
        1, dpw->PMD[PMAKE_PERRND],
        1, dpw->PMD[PMAKE_ID]);

    if (dpw->PMD[PMAKE_WAZA1])
    {
        D_PokeWazaSetPos(dpw->PokeMakeData, dpw->PMD[PMAKE_WAZA1], 0);
    }
    if (dpw->PMD[PMAKE_WAZA2])
    {
        D_PokeWazaSetPos(dpw->PokeMakeData, dpw->PMD[PMAKE_WAZA2], 1);
    }
    if (dpw->PMD[PMAKE_WAZA3])
    {
        D_PokeWazaSetPos(dpw->PokeMakeData, dpw->PMD[PMAKE_WAZA3], 2);
    }
    if (dpw->PMD[PMAKE_WAZA4])
    {
        D_PokeWazaSetPos(dpw->PokeMakeData, dpw->PMD[PMAKE_WAZA4], 3);
    }
    SetMonData(dpw->PokeMakeData, MON_DATA_EXPERIENCE, &dpw->PMD[PMAKE_EXP]);
    SetMonData(dpw->PokeMakeData, MON_DATA_HP_IV, &dpw->PMD[PMAKE_HP_RND]);
    SetMonData(dpw->PokeMakeData, MON_DATA_HP_EV, &dpw->PMD[PMAKE_HP_EXP]);
    SetMonData(dpw->PokeMakeData, MON_DATA_ATK_IV, &dpw->PMD[PMAKE_POW_RND]);
    SetMonData(dpw->PokeMakeData, MON_DATA_ATK_EV, &dpw->PMD[PMAKE_POW_EXP]);
    SetMonData(dpw->PokeMakeData, MON_DATA_DEF_IV, &dpw->PMD[PMAKE_DEF_RND]);
    SetMonData(dpw->PokeMakeData, MON_DATA_DEF_EV, &dpw->PMD[PMAKE_DEF_EXP]);
    SetMonData(dpw->PokeMakeData, MON_DATA_SPEED_IV, &dpw->PMD[PMAKE_AGI_RND]);
    SetMonData(dpw->PokeMakeData, MON_DATA_SPEED_EV, &dpw->PMD[PMAKE_AGI_EXP]);
    SetMonData(dpw->PokeMakeData, MON_DATA_SPATK_IV, &dpw->PMD[PMAKE_EXPOW_RND]);
    SetMonData(dpw->PokeMakeData, MON_DATA_SPATK_EV, &dpw->PMD[PMAKE_EXPOW_EXP]);
    SetMonData(dpw->PokeMakeData, MON_DATA_SPDEF_IV, &dpw->PMD[PMAKE_EXDEF_RND]);
    SetMonData(dpw->PokeMakeData, MON_DATA_SPDEF_EV, &dpw->PMD[PMAKE_EXDEF_EXP]);

    SetMonData(dpw->PokeMakeData, MON_DATA_FRIENDSHIP, &dpw->PMD[PMAKE_FRIEND]);
    SetMonData(dpw->PokeMakeData, MON_DATA_POKERUS, &dpw->PMD[PMAKE_POKERUS]);
    SetMonData(dpw->PokeMakeData, MON_DATA_HELD_ITEM, &dpw->PMD[PMAKE_ITEM]);

    SetMonData(dpw->PokeMakeData, MON_DATA_ABILITY, &dpw->PMD[PMAKE_SPABI]);
    SetMonData(dpw->PokeMakeData, MON_DATA_FORM, &dpw->PMD[PMAKE_FORM_NO]);

    CalcMonLevelAndStats(dpw->PokeMakeData);
}

static void D_PokeWazaSetPos(Pokemon *pokeparam, u16 waza, u16 pos)
{
    u8 pp;

    pp = GetMoveAttr(waza, 5);
    SetMonData(pokeparam, MON_DATA_MOVE1 + pos, &waza);
    SetMonData(pokeparam, MON_DATA_MOVE1PP + pos, &pp);
}

/********************************************************************/
#define PARAMGET( index, ID ) { dpw->PMD[index] = GetMonData( dpw->PokeMakeData, ID, NULL ); }

static void PokeMakePokeParaWorkGetAll(PokeMakeWork *dpw)
{
	PARAMGET( PMAKE_NAME, MON_DATA_SPECIES )
	PARAMGET( PMAKE_LEVEL, MON_DATA_LEVEL )
	PARAMGET( PMAKE_EXP, MON_DATA_EXPERIENCE )
	PARAMGET( PMAKE_ID, MON_DATA_OTID )
	PARAMGET( PMAKE_PERRND, MON_DATA_PERSONALITY )
	PARAMGET( PMAKE_SEX, MON_DATA_GENDER )
    dpw->PMD[PMAKE_PERSONAL] = GetMonNature(dpw->PokeMakeData);
    PARAMGET( PMAKE_WAZA1, MON_DATA_MOVE1 )
	PARAMGET( PMAKE_WAZA2, MON_DATA_MOVE2 )
	PARAMGET( PMAKE_WAZA3, MON_DATA_MOVE3 )
	PARAMGET( PMAKE_WAZA4, MON_DATA_MOVE4 )
	PARAMGET( PMAKE_ITEM, MON_DATA_HELD_ITEM )

	PARAMGET( PMAKE_SPABI, MON_DATA_ABILITY )
	PARAMGET( PMAKE_HP_RND, MON_DATA_HP_IV )
	PARAMGET( PMAKE_HP_EXP, MON_DATA_HP_EV )
	PARAMGET( PMAKE_POW_RND, MON_DATA_ATK_IV )
	PARAMGET( PMAKE_POW_EXP, MON_DATA_ATK_EV )
	PARAMGET( PMAKE_DEF_RND, MON_DATA_DEF_IV )
	PARAMGET( PMAKE_DEF_EXP, MON_DATA_DEF_EV )
	PARAMGET( PMAKE_AGI_RND, MON_DATA_SPEED_IV )
	PARAMGET( PMAKE_AGI_EXP, MON_DATA_SPEED_EV )
	PARAMGET( PMAKE_EXPOW_RND, MON_DATA_SPATK_IV )
	PARAMGET( PMAKE_EXPOW_EXP, MON_DATA_SPATK_EV )
	PARAMGET( PMAKE_EXDEF_RND, MON_DATA_SPDEF_IV )
	PARAMGET( PMAKE_EXDEF_EXP, MON_DATA_SPDEF_EV )
	PARAMGET( PMAKE_STYLE, MON_DATA_COOL )
	PARAMGET( PMAKE_BEAUTIFUL, MON_DATA_BEAUTY )
	PARAMGET( PMAKE_CUTE, MON_DATA_CUTE )
	PARAMGET( PMAKE_CLEVER, MON_DATA_SMART )
	PARAMGET( PMAKE_STRONG, MON_DATA_TOUGH )
	PARAMGET( PMAKE_FUR, MON_DATA_SHEEN )
	PARAMGET( PMAKE_FRIEND, MON_DATA_FRIENDSHIP )
	PARAMGET( PMAKE_POKERUS, MON_DATA_POKERUS )
	PARAMGET( PMAKE_TAMAGO, MON_DATA_IS_EGG )


	PARAMGET( PMAKE_HP, MON_DATA_MAXHP )
	PARAMGET( PMAKE_POW, MON_DATA_ATK )
	PARAMGET( PMAKE_DEF, MON_DATA_DEF )
	PARAMGET( PMAKE_AGI, MON_DATA_SPEED )
	PARAMGET( PMAKE_EXPOW, MON_DATA_SPATK )
	PARAMGET( PMAKE_EXDEF, MON_DATA_SPDEF )
	PARAMGET( PMAKE_EVGET, MON_DATA_FATEFUL_ENCOUNTER )
	PARAMGET( PMAKE_NICKNAME, MON_DATA_HAS_NICKNAME )

	PARAMGET( PMAKE_GETLEVEL, MON_DATA_MET_LEVEL )
	PARAMGET( PMAKE_GETCASSETTE, MON_DATA_GAME_VERSION )
	PARAMGET( PMAKE_GETBALL, MON_DATA_POKEBALL )
	PARAMGET( PMAKE_CONTRY_CODE, MON_DATA_GAME_LANGUAGE )
	PARAMGET( PMAKE_FORM_NO, MON_DATA_FORM )

	PARAMGET( PMAKE_GETPLACE, MON_DATA_EGG_MET_LOCATION )
	PARAMGET( PMAKE_GETPLACE_Y, MON_DATA_EGG_MET_YEAR )
	PARAMGET( PMAKE_GETPLACE_M, MON_DATA_EGG_MET_MONTH )
	PARAMGET( PMAKE_GETPLACE_D, MON_DATA_EGG_MET_DAY )
	PARAMGET( PMAKE_GETPLACE2, MON_DATA_MET_LOCATION )
	PARAMGET( PMAKE_GETPLACE2_Y, MON_DATA_MET_YEAR )
	PARAMGET( PMAKE_GETPLACE2_M, MON_DATA_MET_MONTH )
	PARAMGET( PMAKE_GETPLACE2_D, MON_DATA_MET_DAY )
}


#define PARAMPUT( index, ID ) {	SetMonData( dpw->PokeMakeData, ID, &dpw->PMD[index] ); }

static void PokeMakePokeParaWorkPutAll( PokeMakeWork * dpw )
{
	int	val;

	PARAMPUT( PMAKE_NAME, MON_DATA_SPECIES )
	PARAMPUT( PMAKE_LEVEL, MON_DATA_LEVEL )
	PARAMPUT( PMAKE_EXP, MON_DATA_EXPERIENCE )
	PARAMPUT( PMAKE_ID, MON_DATA_OTID )
	// PARAMPUT( PMAKE_PERRND, MON_DATA_PERSONALITY )
	PARAMPUT( PMAKE_SEX, MON_DATA_GENDER )

	if( dpw->PMD[PMAKE_WAZA1] ){
		D_PokeWazaSetPos( dpw->PokeMakeData, dpw->PMD[PMAKE_WAZA1], 0 );
	}
	if( dpw->PMD[PMAKE_WAZA2] ){
		D_PokeWazaSetPos( dpw->PokeMakeData, dpw->PMD[PMAKE_WAZA2], 1 );
	}
	if( dpw->PMD[PMAKE_WAZA3] ){
		D_PokeWazaSetPos( dpw->PokeMakeData, dpw->PMD[PMAKE_WAZA3], 2 );
	}
	if( dpw->PMD[PMAKE_WAZA4] ){
		D_PokeWazaSetPos( dpw->PokeMakeData, dpw->PMD[PMAKE_WAZA4], 3 );
	}

    PARAMPUT(PMAKE_ITEM, MON_DATA_HELD_ITEM)
    PARAMPUT(PMAKE_SPABI, MON_DATA_ABILITY)
    PARAMPUT(PMAKE_HP_RND, MON_DATA_HP_IV)
    PARAMPUT(PMAKE_HP_EXP, MON_DATA_HP_EV)
    PARAMPUT(PMAKE_POW_RND, MON_DATA_ATK_IV)
    PARAMPUT(PMAKE_POW_EXP, MON_DATA_ATK_EV)
    PARAMPUT(PMAKE_DEF_RND, MON_DATA_DEF_IV)
    PARAMPUT(PMAKE_DEF_EXP, MON_DATA_DEF_EV)
    PARAMPUT(PMAKE_AGI_RND, MON_DATA_SPEED_IV)
    PARAMPUT(PMAKE_AGI_EXP, MON_DATA_SPEED_EV)
    PARAMPUT(PMAKE_EXPOW_RND, MON_DATA_SPATK_IV)
    PARAMPUT(PMAKE_EXPOW_EXP, MON_DATA_SPATK_EV)
    PARAMPUT(PMAKE_EXDEF_RND, MON_DATA_SPDEF_IV)
    PARAMPUT(PMAKE_EXDEF_EXP, MON_DATA_SPDEF_EV)

    PARAMPUT(PMAKE_FRIEND, MON_DATA_FRIENDSHIP)
    PARAMPUT(PMAKE_POKERUS, MON_DATA_POKERUS)

    PARAMPUT(PMAKE_HP, MON_DATA_MAXHP)
    PARAMPUT(PMAKE_POW, MON_DATA_ATK)
    PARAMPUT(PMAKE_DEF, MON_DATA_DEF)
    PARAMPUT(PMAKE_AGI, MON_DATA_SPEED)
    PARAMPUT(PMAKE_EXPOW, MON_DATA_SPATK)
    PARAMPUT(PMAKE_EXDEF, MON_DATA_SPDEF)

    PARAMPUT(PMAKE_FORM_NO, MON_DATA_FORM)

    SetMonPersonality(dpw->PokeMakeData, dpw->PMD[PMAKE_PERRND]);
    CalcMonLevelAndStats(dpw->PokeMakeData);
}

// --------------------------------------------------
// 加入到内存里
// --------------------------------------------------
static u8 PokeMakeCopy(D_POKEMONMAKE *wk)
{
    if (wk->mode == POKEMAKE_MODE_DEBUG)
    {
        Party *party = SaveArray_Party_Get(wk->saveData);

        if (Party_AddMon(party, wk->pmw.PokeMakeData) == TRUE)
        {
            return 0;
        }

        PCStorage_PlaceMonInFirstEmptySlotInAnyBox(
            SaveArray_PCStorage_Get(wk->saveData), 
            Mon_GetBoxMon(wk->pmw.PokeMakeData)
        );
        return 1;
    }
    else if (wk->mode == POKEMAKE_MODE_CHANGE)
    {
        Party *temoti = SaveArray_Party_Get(wk->saveData);

        Pokemon *pp;
        String *str;
        u8 sex;

        pp = Party_GetMonByIndex(temoti, 0);

        str = String_New(16, HEAP_ID_HACK_BOX);
        GetMonData(pp, MON_DATA_OT_NAME_2, str);
        SetMonData(wk->pmw.PokeMakeData, MON_DATA_OT_NAME_2, str);
        String_Delete(str);

        sex = GetMonData(pp, MON_DATA_MET_GENDER, NULL);
        SetMonData(wk->pmw.PokeMakeData, MON_DATA_MET_GENDER, &sex);

        Party_SafeCopyMonToSlot_ResetUnkSub(temoti, 0, wk->pmw.PokeMakeData);
        return 0;
    }

    return 0;
}

// --------------------------------------------------
// 主逻辑
// --------------------------------------------------
static void D_PokemonMakeMain(SysTask *_tcb, void *work)
{
    D_POKEMONMAKE *wk = (D_POKEMONMAKE *)work;
    switch (wk->seq)
    {
        case 0:
            PokeMakeSeq_PagePut(wk);
            break;
        case 1:
            PokeMakeSeq_ParamSelect( wk );
            break;
        case 2:
            PokeMakeSeq_ParamChange(wk);
            break;
        case 3:
            PokeMakeSeq_ButtonWait( wk );
            break;
        case 4:
            wk->hackBoxTool->pageMode = HACKBOX_PAGE_MAIN;
            PmakeExitTaskSeq(wk);
            PMDS_taskDel( _tcb );
            break;
    }
}

static void PagePut(D_POKEMONMAKE *wk)
{
    PokeMakeWork *dpw = &wk->pmw;
    u16 i;

    FillWindowPixelBuffer(&wk->win, 15);
    i = 0;
    while (PageTable[dpw->page_p].page[i] != 0xFF)
    {
        PutProcString(wk, PageTable[dpw->page_p].page[i], COLOR_W_BLACK, i * 16);
        i++;
    }
    CopyWindowToVram(&wk->win);
}

static void PokeMakeSeq_PagePut(D_POKEMONMAKE *wk)
{
    PagePut(wk);
    CursorPut(wk, PMC_INIT);
    wk->seq = 1;
}

// --------------------------------------------------
// 退出
// --------------------------------------------------
static void PmakeExitTaskSeq(D_POKEMONMAKE *wk)
{
    Heap_Free(wk->pmw.PokeMakeData);

    ClearFrameAndWindow2(&wk->win_title, FALSE);
    ClearFrameAndWindow2(&wk->win, FALSE);

    RemoveWindow(&wk->win_title);
    RemoveWindow(&wk->win);

    MessageFormat_Delete(wk->wset);
    DestroyListMenuCursorObj(wk->cursor);

    FillBgTilemapRect(wk->hackBoxTool->bgConfig, GF_BG_LYR_SUB_0, 0, 0, 0, 32, 24, 0);
    BgCommitTilemapBufferToVram(wk->hackBoxTool->bgConfig, GF_BG_LYR_SUB_0);
}

// --------------------------------------------------
// 指针
// --------------------------------------------------
static void CursorPut( D_POKEMONMAKE * wk, u8 mode )
{
	PokeMakeWork * dpw = &wk->pmw;

	switch( mode )
    {
        case PMC_INIT:
            ListMenuUpdateCursorObj(wk->cursor, &wk->win, 0, dpw->cursor_p * 16);
            CopyWindowToVram(&wk->win);
            break;
        case PMC_INC:
            if (dpw->cursor_p < PageTable[dpw->page_p].listcount - 1)
            {
                FillWindowPixelRect(&wk->win, 15, 0, dpw->cursor_p * 16, 12, 16);
                dpw->cursor_p++;
                ListMenuUpdateCursorObj(wk->cursor, &wk->win, 0, dpw->cursor_p * 16);
                CopyWindowToVram(&wk->win);
            }
            break;
        case PMC_DEC:
            if (dpw->cursor_p > 0)
            {
                FillWindowPixelRect(&wk->win, 15, 0, dpw->cursor_p * 16, 12, 16);
                dpw->cursor_p--;
                ListMenuUpdateCursorObj(wk->cursor, &wk->win, 0, dpw->cursor_p * 16);
                CopyWindowToVram(&wk->win);
            }
            break;
    }
}

// --------------------------------------------------
// 数值
// --------------------------------------------------
static void PageValuePut(D_POKEMONMAKE *wk)
{
    PokeMakeWork *dpw = &wk->pmw;
    u16 i;

    FillWindowPixelBuffer(&wk->win, 15);
    i = 0;
    while (PageTable[dpw->page_p].page[i] != 0xff)
    {
        if (i == dpw->cursor_p)
        {
            PutProcString(wk, PageTable[dpw->page_p].page[i], COLOR_W_RED, i * 16);
        }
        else
        {
            PutProcString(wk, PageTable[dpw->page_p].page[i], COLOR_W_BLACK, i * 16);
        }
        i++;
    }
    CopyWindowToVram(&wk->win);
}

// 数值变更按键
static void PokeMakeSeq_ParamChange(D_POKEMONMAKE *wk)
{
    u8	data_id;

    data_id = PageTable[wk->pmw.page_p].page[wk->pmw.cursor_p];

    if (gSystem.newKeys & PAD_BUTTON_A)
    {
        wk->seq = 0;
        return;
    }

    if (gSystem.newKeys & PAD_BUTTON_B)
    {
        wk->pmw.PMD[data_id] = wk->pmw.data_bak;
        wk->seq = 0;
        return;
    }

    if (gSystem.newKeys & PAD_KEY_UP)
    {
        ValueControl(&wk->pmw, PMC_INC);
        PageValuePut(wk);
        return;
    }
    if (gSystem.newKeys & PAD_KEY_DOWN)
    {
        ValueControl(&wk->pmw, PMC_DEC);
        PageValuePut(wk);
        return;
    }

    if (PMakelabelTable[data_id].cont->count != 0xff)
    {
        if ((gSystem.newKeys & PAD_KEY_LEFT) &&
            wk->pmw.value_p < PMakelabelTable[data_id].cont->count - 1)
        {

            wk->pmw.value_p++;
            PageValuePut(wk);
            return;
        }
        if ((gSystem.newKeys & PAD_KEY_RIGHT) && wk->pmw.value_p > 0)
        {
            wk->pmw.value_p--;
            PageValuePut(wk);
            return;
        }
    }
}
// --------------------------------------------------
// 按键
// --------------------------------------------------
BOOL IsMonShiny(u32 id, u32 rnd)
{
    return ((((id & 0xffff0000) >> 16) ^ (id & 0xffff) ^ ((rnd & 0xffff0000) >> 16) ^ (rnd & 0xffff)) < 16);
}

static void PokeMakeSeq_ParamSelect(D_POKEMONMAKE *wk)
{
    if (gSystem.newKeys & PAD_BUTTON_A)
    {
        wk->pmw.data_bak = wk->pmw.PMD[PageTable[wk->pmw.page_p].page[wk->pmw.cursor_p]];
        wk->pmw.value_p = 0;
        PageValuePut(wk);
        wk->seq = 2;
        return;
    }

    if( gSystem.newKeys & PAD_BUTTON_B ){
		wk->seq = 4;
		return;
	}

    if (gSystem.newKeys & PAD_BUTTON_START)
    {
        if (wk->mode != POKEMAKE_MODE_CHANGE)
        {
            PokeMakePokeParaCalcEnd(wk, &wk->pmw);
        }
        else
        {
            PokeMakePokeParaWorkPutAll(&wk->pmw);
        }
        BOOL ret = PokeMakeCopy(wk);
        FillWindowPixelRect(&wk->win, 15, 24, 64, 30 * 8 - 24, 32);
        if (ret == 1)
        {
            PokeMake_StrPrint(&wk->win, msg_pminfo_01, 32, 72, 0, COLOR_W_BLACK);
        }
        else
        {
            PokeMake_StrPrint(&wk->win, msg_pminfo_00, 32, 72, 0, COLOR_W_BLACK);
        }
        wk->seq = 3;
        return;
    }

    if (gSystem.newKeys & PAD_BUTTON_SELECT)
    {
        u16 species = wk->pmw.PMD[PMAKE_NAME];
        u32 newPID = 0;
        u32 id = wk->pmw.PMD[PMAKE_ID];
        u16 gender = wk->pmw.PMD[PMAKE_SEX];
        u16 nature = wk->pmw.PMD[PMAKE_PERSONAL];
        u16 sid = HIHALF(id);
        u16 tid = LOHALF(id);

        for (;;)
        {
            newPID = (LCRandom() | (LCRandom() << 16));
            u8 shinyRange = 1;
            newPID = (((shinyRange ^ (sid ^ tid)) ^ LOHALF(newPID)) << 16) | LOHALF(newPID);

            u8 new_nature = newPID % 25;
            if ((new_nature == nature) &&
                (gender == GetGenderBySpeciesAndPersonality(species, newPID)))
            {
                if (IsMonShiny(id, newPID))
                    break;
            }
        }

        wk->pmw.PMD[PMAKE_PERRND] = newPID;
        return;
    } 

    if (gSystem.newKeys & PAD_KEY_UP)
    {
        CursorPut(wk, PMC_DEC);
        return;
    }

    if (gSystem.newKeys & PAD_KEY_DOWN)
    {
        CursorPut(wk, PMC_INC);
        return;
    }

    if ((gSystem.newKeys & PAD_BUTTON_L) && wk->pmw.page_p > 0)
    {
        wk->pmw.page_p--;
        wk->pmw.cursor_p = 0;
        wk->seq = 0;
        return;
    }

    if ((gSystem.newKeys & PAD_BUTTON_R) && wk->pmw.page_p < PMAKE_PAGE_MAX - 1)
    {
        wk->pmw.page_p++;
        wk->pmw.cursor_p = 0;
        wk->seq = 0;
        return;
    }
}

static void PokeMakeSeq_ButtonWait(D_POKEMONMAKE *wk)
{
    if (gSystem.newKeys & PAD_BUTTON_A)
    {
        wk->seq = 0;
    }
}
/********************************************************************/
/*				各項目表示制御										*/
/*				各項目設定制御										*/
/********************************************************************/
//表示データ作成
static u8 PutProcString(D_POKEMONMAKE *wk, u8 id, u32 pal, u8 y)
{
    PokeMakeWork *dpw;
    u8 vp, vpl;

    dpw = &wk->pmw;

    if (id == PMAKE_BATTLEPARAM1)
    {
        PokeMake_StrPrint(&wk->win, msg_pmlabel_35, 12, y, MSG_NO_PUT, COLOR_W_BLUE);
        PokeMake_NumPrint(&wk->win, wk->wset, dpw, dpw->PMD[PMAKE_HP], 3, 12 + 24, y + 16, MSG_NO_PUT, COLOR_W_BLACK);

        PokeMake_StrPrint(&wk->win, msg_pmlabel_36, 12 + 72, y, MSG_NO_PUT, COLOR_W_BLUE);
        PokeMake_NumPrint(&wk->win, wk->wset, dpw, dpw->PMD[PMAKE_POW], 3, 12 + 72 + 24, y + 16, MSG_NO_PUT, COLOR_W_BLACK);

        PokeMake_StrPrint(&wk->win, msg_pmlabel_37, 12 + 72 + 72, y, MSG_NO_PUT, COLOR_W_BLUE);
        PokeMake_NumPrint(&wk->win, wk->wset, dpw, dpw->PMD[PMAKE_DEF], 3, 12 + 72 + 72 + 24, y + 16, MSG_NO_PUT, COLOR_W_BLACK);
        return 0;
    }

    if (id == PMAKE_BATTLEPARAM2)
    {
        PokeMake_StrPrint(&wk->win, msg_pmlabel_38, 12, y, MSG_NO_PUT, COLOR_W_BLUE);
        PokeMake_NumPrint(&wk->win, wk->wset, dpw, dpw->PMD[PMAKE_AGI], 3, 12 + 24, y + 16, MSG_NO_PUT, COLOR_W_BLACK);

        PokeMake_StrPrint(&wk->win, msg_pmlabel_39, 12 + 72, y, MSG_NO_PUT, COLOR_W_BLUE);
        PokeMake_NumPrint(&wk->win, wk->wset, dpw, dpw->PMD[PMAKE_EXPOW], 3, 12 + 72 + 24, y + 16, MSG_NO_PUT, COLOR_W_BLACK);

        PokeMake_StrPrint(&wk->win, msg_pmlabel_40, 12 + 72 + 72, y, MSG_NO_PUT, COLOR_W_BLUE);
        PokeMake_NumPrint(&wk->win, wk->wset, dpw, dpw->PMD[PMAKE_EXDEF], 3, 12 + 72 + 72 + 24, y + 16, MSG_NO_PUT, COLOR_W_BLACK);
        return 0;
    }

    PokeMake_StrPrint(&wk->win, PMakelabelTable[id].label, 12, y, 0xFF, pal);
    vp = PMakelabelTable[id].cont->count;
    vpl = vp * 4;

    if (id == PMAKE_NAME)
    {
        PokeMake_NumPrint(&wk->win, wk->wset, dpw, dpw->PMD[id], vp, 12 + 72 + 24, y, 0xFF, pal);
        BufferSpeciesName(wk->wset, 0, dpw->PMD[id]);
        PokeMake_StrPrintExp(&wk->win, wk->wset, msg_pmstr_11, 12 + 72 + 24 + 32, y, MSG_NO_PUT, pal);
    }
    else if (id == PMAKE_SEX)
    {
        switch (dpw->PMD[id])
        {
            case 0: // オス
                PokeMake_StrPrint(&wk->win, msg_pmstr_01, 12 + 72 + 24, y, MSG_NO_PUT, COLOR_W_BLACK);
                break;
            case 1: // メス
                PokeMake_StrPrint(&wk->win, msg_pmstr_02, 12 + 72 + 24, y, MSG_NO_PUT, COLOR_W_BLACK);
                break;
            case 2: // なし
                PokeMake_StrPrint(&wk->win, msg_pmstr_00, 12 + 72 + 24, y, MSG_NO_PUT, COLOR_W_BLACK);
                break;
        }
    }
    else if( id >= PMAKE_WAZA1 && id <= PMAKE_WAZA4 ){
		PokeMake_NumPrint(&wk->win, wk->wset, dpw, dpw->PMD[id], vp, 12+72+24, y, MSG_NO_PUT, pal );
		BufferMoveName( wk->wset, 0, dpw->PMD[id] );
		PokeMake_StrPrintExp(&wk->win, wk->wset, msg_pmstr_08, 12+72+24+32, y, MSG_NO_PUT, pal );
	}
    else if ((id >= PMAKE_LEVEL && id <= PMAKE_PERRND) ||
             (id >= PMAKE_HP_RND && id <= PMAKE_POKERUS) ||
             (id >= PMAKE_GETPLACE && id <= PMAKE_GETPLACE2_D))
    {
        PokeMake_NumPrint(&wk->win, wk->wset, dpw, dpw->PMD[id], vp, 12 + 72 + 24, y, 0xFF, pal);
    }
    else if (id == PMAKE_ITEM)
    {
        PokeMake_NumPrint(&wk->win, wk->wset, dpw, dpw->PMD[id], vp, 12 + 72 + 24, y, MSG_NO_PUT, pal);
        BufferItemName(wk->wset, 0, dpw->PMD[id]);
        PokeMake_StrPrintExp(&wk->win, wk->wset, msg_pmstr_09, 12 + 72 + 24 + 32, y, MSG_NO_PUT, pal);
    }
    else if (id == PMAKE_SPABI)
    {
        int ability = dpw->PMD[PMAKE_SPABI];

        PokeMake_NumPrint(&wk->win, wk->wset, dpw, dpw->PMD[id], vp, 12 + 72 + 24, y, MSG_NO_PUT, pal);
        BufferAbilityName(wk->wset, 0, ability);
        PokeMake_StrPrintExp(&wk->win, wk->wset, msg_pmstr_10, 12 + 72 + 24 + 32, y, MSG_NO_PUT, pal);
    }
    else if (id == PMAKE_PERSONAL)
    {
        BufferNatureName(wk->wset, 0, dpw->PMD[id]);
        PokeMake_StrPrintExp(&wk->win, wk->wset, msg_pmstr_11, 12 + 72 + 24, y, MSG_NO_PUT, pal);
    }
    return 0;
}

static void PokeMake_StrPrintExp(Window *win, MessageFormat *wset, u32 id, u32 x, u32 y, u32 wait, u32 col)
{
    String *expb;
    String *strb;

    expb = String_New(128, HEAP_ID_HACK_BOX);
    strb = String_New(128, HEAP_ID_HACK_BOX);
    HackBox_LoadStringByID(id, strb);

    StringExpandPlaceholders(wset, expb, strb);

    AddTextPrinterParameterizedWithColor(win, FLD_MBGFRM_FONT, expb, x, y, wait, col, NULL);
    String_Delete(strb);
    String_Delete(expb);
}

static u32 NumPutColorGet(PokeMakeWork *dpw, u8 cp, u32 col)
{
    if (col == COLOR_W_RED)
    {
        if (cp == dpw->value_p)
        {
            return COLOR_W_BLUE;
        }
        else
        {
            return COLOR_W_RED;
        }
    }
    return COLOR_W_BLACK;
}

extern u16 gText_PokemonEdit_SpeciesNum[];
// 数字文本打印
static void PokeMake_NumPrint(Window *win, MessageFormat *wset, PokeMakeWork *dpw, u32 num, u32 keta, u32 x, u32 y, u32 wait, u32 col)
{
    String *expb;
    u32 i, tmp, siz;

    String *strb = String_New(128, HEAP_ID_HACK_BOX);
    expb = String_New(32, HEAP_ID_HACK_BOX);
    HackBox_LoadString(gText_PokemonEdit_SpeciesNum, strb);

    tmp = 1;
    for (i = 0; i < keta - 1; i++)
    {
        tmp *= 10;
    }

    siz = 0;
    for (i = keta; i >= 1; i--)
    {
        BufferIntegerAsString(wset, 0, num / tmp, 1, PRINTING_MODE_LEADING_ZEROS, 0);
        StringExpandPlaceholders(wset, expb, strb);
        AddTextPrinterParameterizedWithColor(win, FLD_MBGFRM_FONT, expb, x + siz, y, 0xFF, NumPutColorGet(dpw, i - 1, col), NULL);
        num %= tmp;
        tmp /= 10;
        siz += FontID_String_GetWidth(FLD_MBGFRM_FONT, expb, 0);
    }

    if (wait == 0)
    {
        CopyWindowToVram(win);
    }

    String_Delete(strb);
    String_Delete(expb);
}

/********************************************************************/
// 数值修改
/********************************************************************/
static const u32 PmakeValueTable[] = {
	1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000
};

static u32 getproc(PokeMakeWork *dpw, u8 id)
{
    u32 dat;

    switch (id)
    {
        default:
            dat = dpw->PMD[id];
            break;
    }
    return dat;
}

static void setproc(PokeMakeWork *dpw, u32 dat, u8 id)
{
    if (id == PMAKE_NAME || id == PMAKE_PERRND || id == PMAKE_ID)
    {
        dpw->PMD[id] = dat;
        PokeMakePokeParaCalcInit(dpw);
    }
    else if (id == PMAKE_LEVEL)
    {
        dpw->PMD[id] = dat;
        dpw->PMD[PMAKE_EXP] = GetMonExpBySpeciesAndLevel(dpw->PMD[PMAKE_NAME], dat);
        PokeMakePokeParaCalcInit(dpw);
    }
    else if (id == PMAKE_EXP)
    {
        dpw->PMD[id] = dat;
        PokeMakePokeParaCalcGetExp(dpw);
    }
    else if (id >= PMAKE_HP_RND && id <= PMAKE_EXDEF_EXP)
    {
        dpw->PMD[id] = dat;
        PokeMakePokeParaCalcGetBattleParam(dpw);
    }
    else
    {
        dpw->PMD[id] = dat;
    }
}

static void ValueControl(PokeMakeWork *dpw, u8 mode)
{
    u32 data, datamin, datamax, value;

    data = getproc(dpw, PageTable[dpw->page_p].page[dpw->cursor_p]);
    datamin = PMakelabelTable[PageTable[dpw->page_p].page[dpw->cursor_p]].cont->min;
    datamax = PMakelabelTable[PageTable[dpw->page_p].page[dpw->cursor_p]].cont->max;
    value = PmakeValueTable[dpw->value_p];

    switch (PMakelabelTable[PageTable[dpw->page_p].page[dpw->cursor_p]].cont->mode)
    {
        case PMC_INCDEC:
            switch (mode)
            {
            case PMC_INC:
                if (data < datamax - value)
                {
                    data += value;
                }
                else
                {
                    if (data < datamax)
                    {
                        data = datamax;
                    }
                    else
                    {
                        data = datamin;
                    }
                }
                break;

            case PMC_DEC:
                if (data > datamin + value)
                {
                    data -= value;
                }
                else
                {
                    if (data > datamin)
                    {
                        data = datamin;
                    }
                    else
                    {
                        data = datamax;
                    }
                }
                break;
            }
            break;
    }
    setproc(dpw, data, PageTable[dpw->page_p].page[dpw->cursor_p]);
}
