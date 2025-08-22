#pragma once

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "types.h"
#include "heap.h"

#define OS_IE_V_BLANK              (1UL << 0)
#define OS_IE_H_BLANK              (1UL << 1)

typedef void (*GFIntrCB)(void *);

void Main_SetVBlankIntrCB(GFIntrCB cb, void *arg);
void HBlankInterruptDisable(void);
BOOL Main_SetHBlankIntrCB(GFIntrCB cb, void *arg);
