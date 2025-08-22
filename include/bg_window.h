#pragma once

typedef struct BgTemplate {
    u32 x;
    u32 y;
    u32 bufferSize;
    u32 baseTile;
    u8 size;
    u8 colorMode;
    u8 screenBase;
    u8 charBase;
    u8 bgExtPltt;
    u8 priority;
    u8 areaOver;
    u8 dummy;
    u32 mosaic;
} BgTemplate;

enum GFBgScreenSize {
    GF_BG_SCR_SIZE_128x128 = 0,
    GF_BG_SCR_SIZE_256x256,
    GF_BG_SCR_SIZE_256x512,
    GF_BG_SCR_SIZE_512x256,
    GF_BG_SCR_SIZE_512x512,
    GF_BG_SCR_SIZE_1024x1024,
};

enum GFBgBufferSize {
    GF_BG_BUF_SIZE_128x128_4BPP = 128 * 128 / 32,
    GF_BG_BUF_SIZE_256x256_4BPP = 256 * 256 / 32,
    GF_BG_BUF_SIZE_256x512_4BPP = 256 * 512 / 32,
    GF_BG_BUF_SIZE_512x256_4BPP = 512 * 256 / 32,
    GF_BG_BUF_SIZE_512x512_4BPP = 512 * 512 / 32,
    GF_BG_BUF_SIZE_1024x1024_4BPP = 1024 * 1024 / 32,
    GF_BG_BUF_SIZE_128x128_8BPP = 128 * 128 / 64,
    GF_BG_BUF_SIZE_256x256_8BPP = 256 * 256 / 64,
    GF_BG_BUF_SIZE_256x512_8BPP = 256 * 512 / 64,
    GF_BG_BUF_SIZE_512x256_8BPP = 512 * 256 / 64,
    GF_BG_BUF_SIZE_512x512_8BPP = 512 * 512 / 64,
    GF_BG_BUF_SIZE_1024x1024_8BPP = 1024 * 1024 / 64,
};

enum GFBgType {
    GF_BG_TYPE_TEXT = 0,
    GF_BG_TYPE_AFFINE,
    GF_BG_TYPE_256x16PLTT,
    GF_BG_TYPE_MAX,
};

typedef struct GraphicsModes {
    GXDispMode dispMode;
    GXBGMode bgMode;
    GXBGMode subMode;
    GXBG0As _2d3dMode;
} GraphicsModes;