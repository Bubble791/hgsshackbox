#pragma once

// TODO: 修改数量
// 原版技能有468个
#define	MOVENO_MAX	(468)
#define ITEM_DATA_MAX 1000

#define HEAPID_BASE_APP 3
#define HEAP_ID_HACK_BOX 51

#define MSG_NO_PUT 0xFF

enum
{
	msg_pminfo_02,
	msg_pmstr_00,
	msg_pmstr_01,
	msg_pmstr_02,
	msg_pmstr_08,
	msg_pmstr_09,
	msg_pmstr_10,
	msg_pmstr_11,
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

	msg_pmlabel_35,
	msg_pmlabel_36,
	msg_pmlabel_37,
	msg_pmlabel_38,
	msg_pmlabel_39,
	msg_pmlabel_40,
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
} WIFINOTE_PROC_PARAM;

typedef struct HackBoxTool
{
	WIFINOTE_PROC_PARAM *procParam;
	SaveData *saveData;
    BgConfig *bgConfig;
    NARC *fileHandle;
    MessageFormat *msgFormat;
	MsgData *msgData;
	SpriteList *spriteList;
    G2dRenderer g2dRender;
	GF_2DGfxResMan *gfxResMen[4];
	SpriteResource *gfxResObjs[4];
	SpriteResource *iconResObjs[4];
	NNSG2dRenderSurface surface;
	Window titleWindow;
	Window infoWindow;
	Window mainButtonWindow[4];
	String *textString;
	TouchscreenListMenuSpawner *menuSpawner;
	int needReloadFont[5];
	int cursor;
	SpriteResourcesHeader spriteHeader;
	SpriteResourcesHeader iconSpriteHeader;
	Sprite *cursorSprite;
	int pageMode;
} HackBoxTool;

BOOL HackBoxTool_PokemonPageUI(HackBoxTool *hackBox);
void HackBox_LoadString(u16 *stringPtr, String *outString);
void HackBox_LoadStringByID(u32 id, String *outString);
SysTask *PMDS_taskAdd(SysTaskFunc func, int work_size, u32 pri, const u32 heap_id);
void PMDS_taskDel(SysTask *tcb);
