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

typedef struct
{
    HackBoxTool *hackBoxTool;
    Window win;
    Window winTitle;
    u16 itemId;
    u16 itemNo;
    u32 seq;
} D_ITEMMAKE;

static void D_ItemStrPut(D_ITEMMAKE *wk);
static u8 D_ItemIDSel(D_ITEMMAKE *wk);
static void D_ItemMakeMain(SysTask *_tcb, void *work);
static u8 D_ItemIDSel(D_ITEMMAKE *wk);
static void D_ItemMakeBmpExit( D_ITEMMAKE * wk );
static u8 D_ItemNumSel( D_ITEMMAKE * wk );

void DebugItemMakeInit(HackBoxTool *hackBox)
{
    BgConfig *bgl = hackBox->bgConfig;

    D_ITEMMAKE *wk = (D_ITEMMAKE *)SysTask_GetData(PMDS_taskAdd(D_ItemMakeMain, sizeof(D_ITEMMAKE), 0, HEAP_ID_HACK_BOX));

    wk->seq = 0;
    wk->itemId = 1;
    wk->itemNo = 1;
    wk->hackBoxTool = hackBox;

    FillBgTilemapRect(wk->hackBoxTool->bgConfig, GF_BG_LYR_SUB_0, 0, 0, 0, 32, 24, 0);
    BgCommitTilemapBufferToVram(wk->hackBoxTool->bgConfig, GF_BG_LYR_SUB_0);

    AddWindowParameterized(bgl, &wk->winTitle, GF_BG_LYR_SUB_0, 1, 3, 30, 8, 11, 1);
    AddWindowParameterized(bgl, &wk->win, GF_BG_LYR_SUB_0, 1, 13, 30, 4, 11, 1 + 30 * 8);
    FillWindowPixelBuffer(&wk->winTitle, 15);
    PokeMake_StrPrint(&wk->winTitle, msg_tips_0, 1, 1, FALSE, COLOR_W_BLACK);
    CopyWindowToVram(&wk->winTitle);

    LoadUserFrameGfx2(bgl, GF_BG_LYR_SUB_0, 985, 13, 0, HEAP_ID_HACK_BOX);
    DrawFrameAndWindow2(&wk->win, TRUE, 985, 13);
    DrawFrameAndWindow2(&wk->winTitle, TRUE, 985, 13);

    D_ItemStrPut(wk);
    CopyWindowToVram(&wk->win);
}

// --------------------------------------------------
// 文本内容
// --------------------------------------------------
static void D_ItemStrPut(D_ITEMMAKE *wk)
{
    Window *win;
    MessageFormat *wset;
    String *expb;
    String *strb;

    win = &wk->win;
    FillWindowPixelBuffer(win, 15);

    wset = MessageFormat_New(HEAP_ID_HACK_BOX);
    expb = String_New(64, HEAP_ID_HACK_BOX);
    strb = String_New(128, HEAP_ID_HACK_BOX);

    HackBox_LoadStringByID(msg_itemmake_02, strb);
    BufferIntegerAsString(wset, 0, wk->itemId, 3, PRINTING_MODE_LEADING_ZEROS, 0);
    StringExpandPlaceholders(wset, expb, strb);
    if (wk->seq == 0)
    {
        AddTextPrinterParameterizedWithColor(win, 0, expb, 32, 8, MSG_NO_PUT, COLOR_W_RED, NULL);
    }
    else
    {
        AddTextPrinterParameterizedWithColor(win, 0, expb, 32, 8, MSG_NO_PUT, COLOR_W_BLACK, NULL);
    }

    // 道具名字
    HackBox_LoadStringByID(msg_itemmake_01, strb);
    BufferItemName(wset, 0, wk->itemId);
    StringExpandPlaceholders(wset, expb, strb);
    if (wk->seq == 0)
    {
        AddTextPrinterParameterizedWithColor(win, 0, expb, 64, 8, MSG_NO_PUT, COLOR_W_RED, NULL);
    }
    else
    {
        AddTextPrinterParameterizedWithColor(win, 0, expb, 64, 8, MSG_NO_PUT, COLOR_W_BLACK, NULL);
    }

    HackBox_LoadStringByID(msg_itemmake_00, strb);
    AddTextPrinterParameterizedWithColor(win, 0, strb, 176, 8, MSG_NO_PUT, COLOR_W_BLACK, NULL);

    HackBox_LoadStringByID(msg_itemmake_02, strb);
    BufferIntegerAsString(wset, 0, wk->itemNo, 3, PRINTING_MODE_LEADING_ZEROS, 0);
    StringExpandPlaceholders(wset, expb, strb);
    if (wk->seq == 1)
    {
        AddTextPrinterParameterizedWithColor(win, 0, expb, 188, 8, 0, COLOR_W_RED, NULL);
    }
    else
    {
        AddTextPrinterParameterizedWithColor(win, 0, expb, 188, 8, 0, COLOR_W_BLACK, NULL);
    }
    String_Delete(strb);

    String_Delete(expb);
    MessageFormat_Delete(wset);
}

// --------------------------------------------------
// 按键控制
// --------------------------------------------------
static void D_ItemMakeMain(SysTask *_tcb, void *work)
{
    D_ITEMMAKE *wk = (D_ITEMMAKE *)work;

    switch (wk->seq)
    {
        case 0:
            if (D_ItemIDSel(wk) == TRUE)
            {
                wk->hackBoxTool->pageMode = HACKBOX_PAGE_MAIN;
                D_ItemMakeBmpExit(wk);
                PMDS_taskDel(_tcb);
            }
            break;
        case 1:
            if (D_ItemNumSel(wk) == TRUE)
            {
                wk->seq = 0;
            }
            break;
    }
}

static void D_ItemIDSet(D_ITEMMAKE *wk, s8 plus)
{
    s16 id = wk->itemId;

    id += plus;

    if (id <= 0)
    {
        wk->itemId = ITEM_DATA_MAX;
    }
    else if (id > ITEM_DATA_MAX)
    {
        wk->itemId = 1;
    }
    else
    {
        wk->itemId = id;
    }
}

static u8 D_ItemIDSel(D_ITEMMAKE *wk)
{
    if (gSystem.newKeys & PAD_KEY_UP)
    {
        D_ItemIDSet(wk, 1);
        D_ItemStrPut(wk);
        return FALSE;
    }
    if (gSystem.newKeys & PAD_KEY_DOWN)
    {
        D_ItemIDSet(wk, -1);
        D_ItemStrPut(wk);
        return FALSE;
    }
    if (gSystem.newKeys & PAD_KEY_LEFT)
    {
        D_ItemIDSet(wk, -10);
        D_ItemStrPut(wk);
        return FALSE;
    }
    if (gSystem.newKeys & PAD_KEY_RIGHT)
    {
        D_ItemIDSet(wk, 10);
        D_ItemStrPut(wk);
        return FALSE;
    }

    if (gSystem.newKeys & PAD_BUTTON_A)
    {
        wk->seq = 1;
        D_ItemStrPut(wk);
        return FALSE;
    }

    if (gSystem.newKeys & PAD_BUTTON_B)
    {
        return TRUE;
    }

    return FALSE;
}

// 数量控制
static void D_ItemNumSet(D_ITEMMAKE *wk, s8 plus)
{
    s16 num = wk->itemNo;

    num += plus;

    if (num <= 0)
    {
        wk->itemNo = 999;
    }
    else if (num >= 1000)
    {
        wk->itemNo = 1;
    }
    else
    {
        wk->itemNo = num;
    }
}

// 個数変更
static u8 D_ItemNumSel( D_ITEMMAKE * wk )
{
    if (gSystem.newKeys & PAD_KEY_UP)
    {
        D_ItemNumSet(wk, 1);
        D_ItemStrPut(wk);
        return FALSE;
    }
    if (gSystem.newKeys & PAD_KEY_DOWN)
    {
        D_ItemNumSet(wk, -1);
        D_ItemStrPut(wk);
        return FALSE;
    }
    if (gSystem.newKeys & PAD_KEY_LEFT)
    {
        D_ItemNumSet(wk, -10);
        D_ItemStrPut(wk);
        return FALSE;
    }
    if (gSystem.newKeys & PAD_KEY_RIGHT)
    {
        D_ItemNumSet(wk, 10);
        D_ItemStrPut(wk);
        return FALSE;
    }

    if (gSystem.newKeys & PAD_BUTTON_A)
    {
        wk->seq = 0;
        Bag_AddItem(Save_Bag_Get(wk->hackBoxTool->saveData), wk->itemId, wk->itemNo, HEAP_ID_HACK_BOX);
        D_ItemStrPut(wk);
        return FALSE;
    }

    if (gSystem.newKeys & PAD_BUTTON_B)
    {
        wk->seq = 0;
        D_ItemStrPut(wk);
        return TRUE;
    }

    return FALSE;
}

static void D_ItemMakeBmpExit( D_ITEMMAKE * wk )
{
    ClearFrameAndWindow2(&wk->win, FALSE);
    RemoveWindow(&wk->win);
    ClearFrameAndWindow2(&wk->winTitle, FALSE);
    RemoveWindow(&wk->winTitle);

    FillBgTilemapRect(wk->hackBoxTool->bgConfig, GF_BG_LYR_SUB_0, 0, 0, 0, 32, 24, 0);
    BgCommitTilemapBufferToVram(wk->hackBoxTool->bgConfig, GF_BG_LYR_SUB_0);
}