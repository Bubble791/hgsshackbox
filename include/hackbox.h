#pragma once

#define HEAPID_BASE_APP 3
#define HEAP_ID_HACK_BOX 51

enum
{
	HACKBOX_PAGE_MAIN,
	HACKBOX_PAGE_POKEMON,
	HACKBOX_PAGE_ITEM,
	HACKBOX_PAGE_MORE,
};

typedef struct HackBoxToolPokemon
{
    u8 selectRow;
	u8 pageNow;
	Window pokemonEditWindow;
	Window partyIconWindow;
	struct ListMenuCursor *listCursor;
} HackBoxToolPokemon;

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
	int pageMode;
	// pokemon
	HackBoxToolPokemon *hackBoxPoke;
} HackBoxTool;

void HackBoxTool_PokemonPageUI(HackBoxTool *hackBox);
void HackBox_LoadString(u16 *stringPtr, String *outString);