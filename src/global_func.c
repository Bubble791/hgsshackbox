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

#ifdef DEBUG_PRINT_SYS
__attribute__((section(".ewram"))) char DebugTextBuf[0xAC];
#endif

SysTask *PMDS_taskAdd(SysTaskFunc func, int work_size, u32 pri, const u32 heap_id)
{
    void *work_memory;

    if (work_size)
    {
        work_memory = AllocFromHeap(heap_id, work_size);
        if (work_memory == NULL)
        {
            return NULL;
        }
        memset(work_memory, 0, work_size);
    }
    else
    {
        work_memory = NULL;
    }
    return SysTask_CreateOnMainQueue(func, work_memory, pri);
}

void PMDS_taskDel(SysTask *tcb)
{
    void *work_memory;

    work_memory = SysTask_GetData(tcb);
    if (work_memory != NULL)
    {
        Heap_Free(work_memory);
    }
    SysTask_Destroy(tcb);
}

// 功能性函数
void HackBox_LoadString(u16 *stringPtr, String *outString)
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

void PokeMake_StrPrint(Window *win, u32 id, u32 x, u32 y, u32 wait, u32 color)
{
    String *strb = String_New(128, HEAP_ID_HACK_BOX);

    HackBox_LoadStringByID(id, strb);
    AddTextPrinterParameterizedWithColor(win, 0, strb, x, y, wait, color, NULL);
    String_Delete(strb);
}

extern u16 gText_msg_pmlabel_00[];
extern u16 gText_msg_pmlabel_01[];
extern u16 gText_msg_pmlabel_02[];
extern u16 gText_msg_pmlabel_03[];
extern u16 gText_msg_pmlabel_04[];
extern u16 gText_msg_pmlabel_05[];
extern u16 gText_msg_pmlabel_06[];
extern u16 gText_msg_pmlabel_07[];
extern u16 gText_msg_pmlabel_08[];
extern u16 gText_msg_pmlabel_09[];
extern u16 gText_msg_pmlabel_10[];
extern u16 gText_msg_pmlabel_11[];
extern u16 gText_msg_pmlabel_12[];
extern u16 gText_msg_pmlabel_13[];
extern u16 gText_msg_pmlabel_14[];
extern u16 gText_msg_pmlabel_15[];
extern u16 gText_msg_pmlabel_16[];
extern u16 gText_msg_pmlabel_17[];
extern u16 gText_msg_pmlabel_18[];
extern u16 gText_msg_pmlabel_19[];
extern u16 gText_msg_pmlabel_20[];
extern u16 gText_msg_pmlabel_21[];
extern u16 gText_msg_pmlabel_22[];
extern u16 gText_msg_pmlabel_23[];
extern u16 gText_msg_pmlabel_24[];

extern u16 gText_msg_pmlabel_35[];
extern u16 gText_msg_pmlabel_36[];
extern u16 gText_msg_pmlabel_37[];
extern u16 gText_msg_pmlabel_38[];
extern u16 gText_msg_pmlabel_39[];
extern u16 gText_msg_pmlabel_40[];
extern u16 gText_msg_pmlabel_47[];

extern u16 gText_msg_pminfo_00[];
extern u16 gText_msg_pminfo_01[];
extern u16 gText_msg_pminfo_02[];
extern u16 gText_msg_pmstr_00[];
extern u16 gText_msg_pmstr_01[];
extern u16 gText_msg_pmstr_02[];
extern u16 gText_msg_pmstr_08[];
extern u16 gText_msg_pmstr_09[];
extern u16 gText_msg_pmstr_10[];
extern u16 gText_msg_pmstr_11[];
extern u16 gText_msg_itemmake_00[];
extern u16 gText_msg_itemmake_01[];
extern u16 gText_msg_itemmake_02[];
extern u16 gText_msg_tips_0[];

static const u16 *sGlobalString[] = 
{
    [msg_tips_0] = gText_msg_tips_0,
    [msg_pmstr_00] = gText_msg_pmstr_00,
    [msg_pmstr_01] = gText_msg_pmstr_01,
    [msg_pmstr_02] = gText_msg_pmstr_02,
    [msg_pmstr_08] = gText_msg_pmstr_08,
    [msg_pmstr_09] = gText_msg_pmstr_09,
    [msg_pmstr_10] = gText_msg_pmstr_10,
    [msg_pmstr_11] = gText_msg_pmstr_11,


    [msg_pmlabel_00] = gText_msg_pmlabel_00,
    [msg_pmlabel_01] = gText_msg_pmlabel_01,
    [msg_pmlabel_02] = gText_msg_pmlabel_02,
    [msg_pmlabel_03] = gText_msg_pmlabel_03,
    [msg_pmlabel_04] = gText_msg_pmlabel_04,
    [msg_pmlabel_05] = gText_msg_pmlabel_05,
    [msg_pmlabel_06] = gText_msg_pmlabel_06,
    [msg_pmlabel_07] = gText_msg_pmlabel_07,
    [msg_pmlabel_08] = gText_msg_pmlabel_08,
    [msg_pmlabel_09] = gText_msg_pmlabel_09,
    [msg_pmlabel_10] = gText_msg_pmlabel_10,
    [msg_pmlabel_11] = gText_msg_pmlabel_11,
    [msg_pmlabel_12] = gText_msg_pmlabel_12,
    [msg_pmlabel_13] = gText_msg_pmlabel_13,
    [msg_pmlabel_14] = gText_msg_pmlabel_14,
    [msg_pmlabel_15] = gText_msg_pmlabel_15,
    [msg_pmlabel_16] = gText_msg_pmlabel_16,
    [msg_pmlabel_17] = gText_msg_pmlabel_17,
    [msg_pmlabel_18] = gText_msg_pmlabel_18,
    [msg_pmlabel_19] = gText_msg_pmlabel_19,
    [msg_pmlabel_20] = gText_msg_pmlabel_20,
    [msg_pmlabel_21] = gText_msg_pmlabel_21,
    [msg_pmlabel_22] = gText_msg_pmlabel_22,
    [msg_pmlabel_23] = gText_msg_pmlabel_23,
    [msg_pmlabel_24] = gText_msg_pmlabel_24,

    [msg_pmlabel_35] = gText_msg_pmlabel_35,
    [msg_pmlabel_36] = gText_msg_pmlabel_36,
    [msg_pmlabel_37] = gText_msg_pmlabel_37,
    [msg_pmlabel_38] = gText_msg_pmlabel_38,
    [msg_pmlabel_39] = gText_msg_pmlabel_39,
    [msg_pmlabel_40] = gText_msg_pmlabel_40,
    [msg_pmlabel_47] = gText_msg_pmlabel_47,

    [msg_pminfo_00] = gText_msg_pminfo_00,
    [msg_pminfo_01] = gText_msg_pminfo_01,
    [msg_pminfo_02] = gText_msg_pminfo_02,

    [msg_itemmake_00] = gText_msg_itemmake_00,
    [msg_itemmake_01] = gText_msg_itemmake_01,
    [msg_itemmake_02] = gText_msg_itemmake_02,
};

void HackBox_LoadStringByID(u32 id, String *outString)
{
    if ( id >= MSG_MAX)
    {
        id = 0;
    }
    
    HackBox_LoadString((u16 *)sGlobalString[id], outString);
}