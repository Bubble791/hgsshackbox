#pragma once

typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t    s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

typedef volatile  u8  vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile u64 vu64;
typedef volatile  s8  vs8;
typedef volatile s16 vs16;
typedef volatile s32 vs32;
typedef volatile s64 vs64;

#define NULL                ((void *)0)
#define TRUE 1
#define FALSE 0

#define NELEMS(array) (sizeof(array) / sizeof(array[0]))

typedef u8  bool8;
typedef int  BOOL;
typedef u16 bool16;
typedef u32 bool32;


typedef s32 fx32;
#define FX32_SHIFT          12
#define FX32_INT_SIZE       19
#define FX32_DEC_SIZE       12

#define FX32_INT_MASK       0x7ffff000
#define FX32_DEC_MASK       0x00000fff
#define FX32_SIGN_MASK      0x80000000

typedef u32 FSOverlayID;

typedef struct
{
    fx32 x;
    fx32 y;
    fx32 z;
}
VecFx32;

#define	ALIGN4 __attribute__((aligned(4)))
#define MOVE_TABLES_TERMIN 0xFEFE
#define THUMB_FUNC __attribute__((target("thumb")))
#define LONG_CALL __attribute__((long_call))
#define UNUSED __attribute__((unused))
#define FALLTHROUGH __attribute__ ((fallthrough))
#define PACKED __attribute__((packed))

#define RGB(r, g, b) (((b & 0x1F) << 10) | ((g & 0x1F) << 5) | (r & 0x1F))