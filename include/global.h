#ifndef POKEHEARTGOLD_GLOBAL_H
#define POKEHEARTGOLD_GLOBAL_H

#include <MSL.h>
#include <nitro.h>
#include <nitro/code16.h>
#include <nnsys.h>
#include <stddef.h>
#include <string.h>

#include "constants/global.h"

#include "assert.h"
#include "config.h" // MUST COME BEFORE ANY OTHER GAMEFREAK HEADERS
#include "pm_version.h"

#define NELEMS(a) (sizeof(a) / sizeof(*(a)))

typedef struct {
    int x;
    int y;
    int z;
} Vec32;

#define ARRAY_ASSIGN_EX(dest, src, as_type, count)            \
    {                                                         \
        struct tmp {                                          \
            as_type _[count];                                 \
        };                                                    \
        *(struct tmp *)&(dest) = *(const struct tmp *)&(src); \
    }

#define ARRAY_ASSIGN(dest, src) ARRAY_ASSIGN_EX(dest, src, typeof(*(dest)), NELEMS(dest))
#define DEBUG_PRINT_SYS

#ifdef DEBUG_PRINT_SYS
void sprintf(char *buf, char *str, ...);
extern char DebugTextBuf[0xAC];
extern void debugsyscall(char* msg);
#define NOCASHGBAPRINTADDR1 0x04FFFA14 // does not automatically add the newline

#define DebugPrintf(...) { sprintf(DebugTextBuf, __VA_ARGS__); debugsyscall(DebugTextBuf); *(volatile u32 *)NOCASHGBAPRINTADDR1 = (u32)DebugTextBuf; }

#define DebugVar(var) DebugPrintf(#var ": %d\n", (var))
#define DebugHex(var) DebugPrintf(#var ": 0x%x\n", (var))

#endif

#endif // POKEHEARTGOLD_GLOBAL_H
