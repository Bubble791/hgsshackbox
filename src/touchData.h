
#define TP_MODESEL_BTN_X (2 * 8 + 2)
#define TP_MODESEL_BTN_W (28 * 8 - 4)
#define TP_MODESEL_BTN_H (4 * 8 - 4)
#define TP_MODESEL_BTN_OY (5 * 8)
#define TP_MODESEL_BTN_Y01 (3 * 8 + 2)
#define TP_MODESEL_BTN_Y02 (TP_MODESEL_BTN_Y01 + TP_MODESEL_BTN_OY)
#define TP_MODESEL_BTN_Y03 (TP_MODESEL_BTN_Y02 + TP_MODESEL_BTN_OY)
#define TP_MODESEL_BTN_Y04 (TP_MODESEL_BTN_Y03 + TP_MODESEL_BTN_OY)

static const TouchscreenHitbox sModeSelectTouch[] = 
{
	{
		.rect = 
		{
			.top = TP_MODESEL_BTN_Y01, .bottom = TP_MODESEL_BTN_Y01 + TP_MODESEL_BTN_H, 
			.left = TP_MODESEL_BTN_X, .right = TP_MODESEL_BTN_X + TP_MODESEL_BTN_W
		}
	},
	{
		.rect = 
		{
			.top = TP_MODESEL_BTN_Y02, .bottom = TP_MODESEL_BTN_Y02 + TP_MODESEL_BTN_H, 
			.left = TP_MODESEL_BTN_X, .right = TP_MODESEL_BTN_X + TP_MODESEL_BTN_W
		}
	},
	{
		.rect = 
		{
			.top = TP_MODESEL_BTN_Y03, .bottom = TP_MODESEL_BTN_Y03 + TP_MODESEL_BTN_H, 
			.left = TP_MODESEL_BTN_X, .right = TP_MODESEL_BTN_X + TP_MODESEL_BTN_W
		}
	},
	{
		.rect = 
		{
			.top = TP_MODESEL_BTN_Y04, .bottom = TP_MODESEL_BTN_Y04 + TP_MODESEL_BTN_H, 
			.left = TP_MODESEL_BTN_X, .right = TP_MODESEL_BTN_X + TP_MODESEL_BTN_W
		}
	},
	{TOUCHSCREEN_RECTLIST_END, 0, 0, 0},
};