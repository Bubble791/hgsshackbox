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

#define COLOR_W_BLUE (MAKE_TEXT_COLOR(3, 4, 15))
#define COLOR_W_RED (MAKE_TEXT_COLOR(5, 6, 15))
#define COLOR_W_PINK (MAKE_TEXT_COLOR(7, 8, 15))
#define COLOR_W_GREEN (MAKE_TEXT_COLOR(9, 10, 15))
#define COLOR_W_BLACK (MAKE_TEXT_COLOR(1, 2, 0))

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
    MessageFormat *wset;
    struct ListMenuCursor *cursor;
    PokeMakeWork pmw;
    u32 seq;
    u32 mode;
} D_POKEMONMAKE;

static const PmakeCont cont00 = {1, 493, PMC_INCDEC, 4};
static const PmakeCont cont01 = {1, 100, PMC_INCDEC, 3};
static const PmakeCont cont02 = {1, 2000000, PMC_INCDEC, 7};
static const PmakeCont cont03 = {0, 0xffffffff, PMC_INCDEC, 10};
static const PmakeCont cont04 = {0, 0xffffffff, PMC_INCDEC, 10};
static const PmakeCont cont05 = {0, 2, PMC_INCDEC, 0xff};
static const PmakeCont cont06 = {0, 24, PMC_INCDEC, 0xff};

static const PmakeParamData PMakelabelTable[] =
{
    {msg_pmlabel_00, &cont00},
    {msg_pmlabel_01, &cont01},
    {msg_pmlabel_02, &cont02},
    {msg_pmlabel_03, &cont03},
    {msg_pmlabel_04, &cont04},
    {msg_pmlabel_05, &cont05},
    {msg_pmlabel_06, &cont06},
};

static const u8 Page1[] = {
	PMAKE_NAME, PMAKE_LEVEL, PMAKE_EXP, PMAKE_ID,
	PMAKE_PERRND, PMAKE_SEX, PMAKE_PERSONAL, 0xff
};
static const u8 Page2[] = {
	PMAKE_CONDITION, PMAKE_FRIEND, PMAKE_POKERUS,
	PMAKE_TAMAGO, PMAKE_EVGET, PMAKE_NICKNAME, 0xff
};
static const u8 Page3[] = {
	PMAKE_WAZA1, PMAKE_WAZA2, PMAKE_WAZA3,
	PMAKE_WAZA4, PMAKE_ITEM, PMAKE_SPABI, 0xff
};
static const u8 Page4[] = {
	PMAKE_HP_RND, PMAKE_HP_EXP, PMAKE_POW_RND, PMAKE_POW_EXP,
	PMAKE_DEF_RND, PMAKE_DEF_EXP, PMAKE_BATTLEPARAM1, 0xff
};
static const u8 Page5[] = {
	PMAKE_AGI_RND, PMAKE_AGI_EXP, PMAKE_EXPOW_RND, PMAKE_EXPOW_EXP,
	PMAKE_EXDEF_RND, PMAKE_EXDEF_EXP, PMAKE_BATTLEPARAM2, 0xff
};
static const u8 Page6[] = {
	PMAKE_STYLE, PMAKE_BEAUTIFUL, PMAKE_CUTE,
	PMAKE_CLEVER, PMAKE_STRONG, PMAKE_FUR, 0xff
};
static const u8 Page7[] = {
	PMAKE_GETLEVEL, PMAKE_GETCASSETTE,
	PMAKE_GETBALL, PMAKE_CONTRY_CODE, PMAKE_FORM_NO, 0xff
};
static const u8 Page8[] = {
	PMAKE_GETPLACE, PMAKE_GETPLACE_Y, PMAKE_GETPLACE_M, PMAKE_GETPLACE_D,
	PMAKE_GETPLACE2, PMAKE_GETPLACE2_Y, PMAKE_GETPLACE2_M, PMAKE_GETPLACE2_D, 0xff
};

#define	PMAKE_PAGE_MAX	(8)
static const PmakePageTable PageTable[] =
{
	{ Page1,5}, {Page2,6},
	{ Page3,6}, {Page4,6},
	{ Page5,6}, {Page6,6},
	{ Page7,5}, {Page8,8}
};

static void PokeMake_StrPrint(Window *win, u32 id, u32 x, u32 y, u32 wait, u32 color);
static void PokeMakePokeParaCalcInit(PokeMakeWork *dpw);
static void PokeMakeWorkInit(PokeMakeWork *dpw);
static void PokeMakePokeParaWorkGetAll(PokeMakeWork *dpw);
static void D_PokemonMakeMain(SysTask *_tcb, void *work);
static u8 PutProcString(D_POKEMONMAKE *wk, u8 id, u32 pal, u8 y);
static void PokeMakeSeq_PagePut(D_POKEMONMAKE *wk);
static void CursorPut( D_POKEMONMAKE * wk, u8 mode );
static void PokeMakeSeq_ParamSelect(D_POKEMONMAKE *wk);
static void PokeMake_NumPrint(Window *win, MessageFormat *wset, PokeMakeWork *dpw, u32 num, u32 keta, u32 x, u32 y, u32 wait, u32 col);
static void PokeMake_MonsNamePut(Window *win, u32 mons, u32 x, u32 y, u32 wait, u32 col);
static void PokeMake_StrPrintExp(Window *win, MessageFormat *wset, u32 id, u32 x, u32 y, u32 wait, u32 col);

static void PokeMakeInit(D_POKEMONMAKE *wk)
{
    FillWindowPixelBuffer(&wk->win_title, 15);
    PokeMake_StrPrint(&wk->win_title, msg_pminfo_02, 0, 0, FALSE, COLOR_W_BLACK);

    wk->pmw.PokeMakeData = AllocMonZeroed(HEAP_ID_HACK_BOX);

    if (wk->mode == POKEMAKE_MODE_DEBUG) {
        PokeMakeWorkInit(&wk->pmw);
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
void DebugPokemonMakeInit(HackBoxTool *hackBox)
{
    D_POKEMONMAKE *wk;
    BgConfig *bgl = hackBox->bgConfig;

    wk = SysTask_GetData(PMDS_taskAdd(D_PokemonMakeMain, sizeof(D_POKEMONMAKE), 0, HEAP_ID_HACK_BOX));

    wk->seq = 0;
    wk->mode = POKEMAKE_MODE_DEBUG;
	wk->wset = MessageFormat_New( HEAP_ID_HACK_BOX );
	wk->cursor = ListMenuCursorNew( HEAP_ID_HACK_BOX );

    wk->saveData = hackBox->saveData;

    AddWindowParameterized(bgl, &wk->win_title, GF_BG_LYR_SUB_0, 1, 1, 30, 4, 11, 1);
    AddWindowParameterized(bgl, &wk->win, GF_BG_LYR_SUB_0, 1, 7, 30, 16, 11, 1 + 30 * 4);

    LoadUserFrameGfx2(bgl, GF_BG_LYR_SUB_0, 1 + 30 * 4 + 30 * 18, MENU_WIN_PAL, 1, HEAP_ID_HACK_BOX);
    DrawFrameAndWindow2(&wk->win_title, TRUE, 1 + 30 * 4 + 30 * 18, MENU_WIN_PAL);
    DrawFrameAndWindow2(&wk->win, TRUE, 1 + 30 * 4 + 30 * 18, MENU_WIN_PAL);

    PokeMakeInit(wk);
}

static void PokeMakeWorkInit(PokeMakeWork *dpw)
{
    u16 i;

    for (i = 0; i < 64; i++)
    {
        dpw->PMD[i] = 0;
    }
    dpw->PMD[PMAKE_NAME] = 1;

    dpw->PMD[PMAKE_PERRND] = (LCRandom() << 16) | (LCRandom());

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

	//加载当前修改的PM时读取的数据
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

/********************************************************************/
// 指针
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

static void PokeMakeSeq_ParamSelect(D_POKEMONMAKE *wk)
{
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
/********************************************************************/
/*				各項目表示制御										*/
/*				各項目設定制御										*/
/********************************************************************/
//表示データ作成
static u8 PutProcString(D_POKEMONMAKE *wk, u8 id, u32 pal, u8 y)
{
    PokeMakeWork *dpw;
    u16 i;
    u8 vp, vpl;

    dpw = &wk->pmw;

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
    else if ((id >= PMAKE_LEVEL && id <= PMAKE_PERRND) ||
             (id >= PMAKE_HP_RND && id <= PMAKE_POKERUS) ||
             (id >= PMAKE_GETPLACE && id <= PMAKE_GETPLACE2_D))
    {
        PokeMake_NumPrint(&wk->win, wk->wset, dpw, dpw->PMD[id], vp, 12 + 72 + 24, y, 0xFF, pal);
    }
    else if (id == PMAKE_PERSONAL)
    {
        BufferNatureName(wk->wset, 0, dpw->PMD[id]);
        PokeMake_StrPrintExp(&wk->win, wk->wset, msg_pmstr_11, 12 + 72 + 24, y, MSG_NO_PUT, pal);
    }
    return 0;
}

static void PokeMake_StrPrint(Window *win, u32 id, u32 x, u32 y, u32 wait, u32 color)
{
    String *strb = String_New(128, HEAP_ID_HACK_BOX);

    HackBox_LoadStringByID(id, strb);
    AddTextPrinterParameterizedWithColor(win, FLD_MBGFRM_FONT, strb, x, y, wait, color, NULL);
    String_Delete(strb);
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

static void PokeMake_MonsNamePut(Window *win, u32 mons, u32 x, u32 y, u32 wait, u32 col)
{
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