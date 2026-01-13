#pragma once

#define HIHALF(n) (((n) & 0xFFFF0000) >> 16)

// Extracts the lower 16 bits of a 32-bit number
#define LOHALF(n) ((n) & 0xFFFF)

// TODO: 修改数量
// 原版技能有468个
#define	MOVENO_MAX	(468)
#define ITEM_DATA_MAX 1000
#define ABILITY_DATA_MAX 255

#define HEAPID_BASE_APP 3
#define HEAP_ID_HACK_BOX 51

#define MSG_NO_PUT 0xFF

enum
{
	msg_tips_0,
	msg_pminfo_02,
	msg_pmstr_00,
	msg_pmstr_01,
	msg_pmstr_02,
	msg_pmstr_08,
	msg_pmstr_09,
	msg_pmstr_10,
	msg_pmstr_11,
	msg_pminfo_00,
	msg_pminfo_01,
	msg_pmlabel_00,
	msg_pmlabel_01,
	msg_pmlabel_02,
	msg_pmlabel_03,
	msg_pmlabel_04,
	msg_pmlabel_05,
	msg_pmlabel_06,
	msg_pmlabel_07,
	msg_pmlabel_08,
	msg_pmlabel_09,
	msg_pmlabel_10,
	msg_pmlabel_11,
	msg_pmlabel_12,
	msg_pmlabel_13,
	msg_pmlabel_14,
	msg_pmlabel_15,
	msg_pmlabel_16,
	msg_pmlabel_17,
	msg_pmlabel_18,
	msg_pmlabel_19,
	msg_pmlabel_20,
	msg_pmlabel_21,
	msg_pmlabel_22,
	msg_pmlabel_23,
	msg_pmlabel_24,
	msg_pmlabel_47,

	msg_pmlabel_35,
	msg_pmlabel_36,
	msg_pmlabel_37,
	msg_pmlabel_38,
	msg_pmlabel_39,
	msg_pmlabel_40,

	msg_itemmake_00,
	msg_itemmake_01,
	msg_itemmake_02,
	MSG_MAX
};

enum
{
	HACKBOX_PAGE_MAIN,
	HACKBOX_PAGE_ADD_POKEMON,
	HACKBOX_PAGE_CHANGE_POKEMON,
	HACKBOX_PAGE_ITEM,
};

enum
{
	POKEMAKE_MODE_DEBUG = 0, // 标准生成
	POKEMAKE_MODE_CHANGE,	 // 改写模式
};

typedef struct
{
	SaveData *saveData;
	MenuInputStateMgr* stateMgr;
} WIFINOTE_PROC_PARAM;

typedef struct HackBoxTool
{
	WIFINOTE_PROC_PARAM *procParam;
	SaveData *saveData;
    BgConfig *bgConfig;
    NARC *fileHandle;
    MessageFormat *msgFormat;
	SpriteList *spriteList;
    G2dRenderer g2dRender;
	GF_2DGfxResMan *gfxResMen[4];
	SpriteResource *gfxResObjs[4];
	NNSG2dRenderSurface surface;
	Window titleWindow;
	Window infoWindow;
	Window mainButtonWindow[4];
	String *textString;
	struct FontWork backupFontWork;
	int cursor;
	SpriteResourcesHeader spriteHeader;
	Sprite *cursorSprite;
	int pageMode;
} HackBoxTool;

BOOL HackBoxTool_PokemonPageUI(HackBoxTool *hackBox);
void HackBox_LoadString(u16 *stringPtr, String *outString);
void HackBox_LoadStringByID(u32 id, String *outString);
SysTask *PMDS_taskAdd(SysTaskFunc func, int work_size, u32 pri, const u32 heap_id);
void PMDS_taskDel(SysTask *tcb);
void DebugItemMakeInit(HackBoxTool *hackBox);
void PokeMake_StrPrint(Window *win, u32 id, u32 x, u32 y, u32 wait, u32 color);
