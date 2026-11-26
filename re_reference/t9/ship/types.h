struct _GUID
{
unsigned int Data1;
unsigned __int16 Data2;
unsigned __int16 Data3;
unsigned __int8 Data4[8];
}
;

typedef struct _GUID GUID
;

struct RUNTIME_FUNCTION
{
void *__ptr32 FunctionStart __offset(OFF64|RVAOFF);
void *__ptr32 FunctionEnd __offset(OFF64|RVAOFF|PASTEND);
void *__ptr32 UnwindInfo __offset(OFF64|RVAOFF);
}
;

struct UNWIND_INFO_HDR
{
unsigned __int8 Version : 3 __hex;
unsigned __int8 Flags : 5 __hex;
unsigned __int8 PrologSize __hex;
unsigned __int8 CntUnwindCodes __hex;
unsigned __int8 FrameRegister : 4 __hex;
unsigned __int8 FrameOffset : 4 __hex;
}
;

struct UNWIND_CODE
{
char PrologOff;
unsigned __int8 UnwindOp : 4 __udec;
unsigned __int8 OpInfo : 4 __udec;
}
;

enum __bitmask __TI_flags
{
TI_IsConst = 0x1,
TI_IsVolatile = 0x2,
TI_IsUnaligned = 0x4,
TI_IsPure = 0x8,
TI_IsWinRT = 0x10,
}
;

enum __bitmask __CT_flags
{
CT_IsSimpleType = 0x1,
CT_ByReferenceOnly = 0x2,
CT_HasVirtualBase = 0x4,
CT_IsWinRTHandle = 0x8,
CT_IsStdBadAlloc = 0x10,
}
;

union __declspec(align(32)) __m256
{
float m256_f32[8];
}
;

