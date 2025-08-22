#pragma once

#include "heap.h"

typedef struct String {
    u16 maxsize;
    u16 size;
    u32 magic;
    u16 data[0];
} String;

typedef enum PrintingMode {
    PRINTING_MODE_LEFT_ALIGN,
    PRINTING_MODE_RIGHT_ALIGN,
    PRINTING_MODE_LEADING_ZEROS,
} PrintingMode;

u16 *CopyU16StringArray(u16 *dest, const u16 *src);
BOOL StringNotEqual(const u16 *a, const u16 *b);
u16 *StringFill(u16 *str, u16 val, u32 n);
u16 *StringFillEOS(u16 *s, u32 n);
u16 *ConvertUIntToDecimalString(u16 *dest, u32 num, PrintingMode strconvmode, u32 ndigits);
u16 *CopyU16StringArrayN(u16 *dest, const u16 *src, u32 n);
int StringLength(const u16 *str);
BOOL StringNotEqualN(const u16 *a, const u16 *b, u32 n);
String *String_New(u32 maxsize, HeapID heapId);
void String_Delete(String *string);
void CopyStringToU16Array(const String *string, u16 *dest, u32 n);
void CopyU16ArrayToString(String *string, const u16 *src);
void CopyU16ArrayToStringN(String *string, const u16 *src, u32 n);
int String_Compare(String *a, String *b);
void String_Copy(String *dest, const String *src);
void String_SetEmpty(String *str);
String *String_Dup(const String *src, HeapID heapId);
void String_AddChar(String *str, u16 _char);
void CopyU16ArrayToString(String *dest, const u16 *src);
void CopyU16ArrayToStringN(String *dest, const u16 *src, u32 n);
u16 *String_cstr(String *string);
void String16_FormatInteger(String *str, int num, u32 ndigits, PrintingMode strConvMode, BOOL whichCharset);
void String_UpperCharN(String *str, int n);
void String_Cat_HandleTrainerName(String *dest, String *src);
void String16_FormatUnsignedLongLong(String *str, u64 num, u32 ndigits, PrintingMode strConvMode, BOOL whichCharset);
s64 String_atoi(String *str, BOOL *flag);
u16 String_GetLength(String *str);
int String_CountLines(volatile String *str);
void String_GetLineN(String *dest, volatile String *src, u32 n);
void String_RadioAddStatic(String *string, u8 level);
u16 *String_cstr(String *str);
void String_Cat(String *dest, String *src);
BOOL String_IsTrainerName(String *string);