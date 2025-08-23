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

void debugsyscall(u8 *buf);
void sprintf(u8 *buf, char *str, ...);
#define debug_printf(...) { u8 buf_assumeunuasedfasdf[128]; sprintf(buf_assumeunuasedfasdf, __VA_ARGS__); debugsyscall(buf_assumeunuasedfasdf); }

#endif // POKEHEARTGOLD_GLOBAL_H
