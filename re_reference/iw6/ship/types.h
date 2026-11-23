enum __bitmask __TI_flags : __int32
{
TI_IsConst = 0x1,
TI_IsVolatile = 0x2,
TI_IsUnaligned = 0x4,
TI_IsPure = 0x8,
TI_IsWinRT = 0x10,
}
;

enum __bitmask __CT_flags : __int32
{
CT_IsSimpleType = 0x1,
CT_ByReferenceOnly = 0x2,
CT_HasVirtualBase = 0x4,
CT_IsWinRTHandle = 0x8,
CT_IsStdBadAlloc = 0x10,
}
;

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
_UNKNOWN *__ptr32 FunctionStart __offset(OFF64|RVAOFF);
_UNKNOWN *__ptr32 FunctionEnd __offset(OFF64|RVAOFF|PASTEND);
_UNKNOWN *__ptr32 UnwindInfo __offset(OFF64|RVAOFF);
}
;

struct UNWIND_INFO_HDR
{
char Ver3_Flags __hex;
char PrologSize __hex;
char CntUnwindCodes __hex;
char FrReg_FrRegOff __hex;
}
;

struct UNWIND_CODE
{
char PrologOff __hex;
char OpCode_OpInfo __hex;
}
;

struct FuncInfo
{
int magicNumber __hex;
int maxState __udec;
_UNKNOWN *__ptr32 pUnwindMap __offset(OFF64|RVAOFF);
int nTryBlocks __udec;
_UNKNOWN *__ptr32 pTryBlockMap __offset(OFF64|RVAOFF);
int nIPMapEntries __udec;
_UNKNOWN *__ptr32 pIPtoStateMap __offset(OFF64|RVAOFF);
int dispUnwindHelp __udec;
_UNKNOWN *__ptr32 pESTypeList __offset(OFF64|RVAOFF);
int EHFlags __hex;
}
;

struct UnwindMapEntry
{
int toState __dec __tabform(NODUPS,1);
_UNKNOWN *__ptr32 action __offset(OFF64|RVAOFF);
}
;

struct TryBlockMapEntry
{
int tryLow __dec __tabform(NODUPS,1);
int tryHigh __dec __tabform(NODUPS,1);
int catchHigh __dec __tabform(NODUPS,1);
int nCatches __dec __tabform(NODUPS,1);
_UNKNOWN *__ptr32 pHandlerArray __offset(OFF64|RVAOFF);
}
;

struct HandlerType
{
int adjectives __hex;
_UNKNOWN *__ptr32 pType __offset(OFF64|RVAOFF);
int dispCatchObj __dec __tabform(NODUPS,1);
_UNKNOWN *__ptr32 addressOfHandler __offset(OFF64|RVAOFF);
int dispFrame __dec __tabform(NODUPS,1);
}
;

struct IPtoStateMap
{
_UNKNOWN *__ptr32 pc __offset(OFF64|RVAOFF);
int state __dec __tabform(NODUPS,1);
}
;

struct _SYSTEM_INFO
{
union
{
DWORD dwOemId;
struct
{
WORD wProcessorArchitecture;
WORD wReserved;
};
};
DWORD dwPageSize;
LPVOID lpMinimumApplicationAddress;
LPVOID lpMaximumApplicationAddress;
DWORD_PTR dwActiveProcessorMask;
DWORD dwNumberOfProcessors;
DWORD dwProcessorType;
DWORD dwAllocationGranularity;
WORD wProcessorLevel;
WORD wProcessorRevision;
}
;

union _SYSTEM_INFO::$A707B71C060B6D10F73A71917EA8473F
{
DWORD dwOemId;
struct
{
WORD wProcessorArchitecture;
WORD wReserved;
};
}
;

typedef unsigned int DWORD
;

struct _SYSTEM_INFO::$A707B71C060B6D10F73A71917EA8473F::$AA04DEB0C6383F89F13D312A174572A9
{
WORD wProcessorArchitecture;
WORD wReserved;
}
;

typedef unsigned __int16 WORD
;

typedef void *LPVOID
;

typedef ULONG_PTR DWORD_PTR
;

typedef unsigned __int64 ULONG_PTR
;

struct tagRECT
{
LONG left;
LONG top;
LONG right;
LONG bottom;
}
;

typedef int LONG
;

struct tagPOINT
{
LONG x;
LONG y;
}
;

struct _OVERLAPPED
{
ULONG_PTR Internal;
ULONG_PTR InternalHigh;
union
{
struct
{
DWORD Offset;
DWORD OffsetHigh;
};
PVOID Pointer;
};
HANDLE hEvent;
}
;

union _OVERLAPPED::$742A73540840F318F86F9CEE3D494648
{
struct
{
DWORD Offset;
DWORD OffsetHigh;
};
PVOID Pointer;
}
;

struct _OVERLAPPED::$742A73540840F318F86F9CEE3D494648::$9BFE693EDA487769FDABADE5E43394F7
{
DWORD Offset;
DWORD OffsetHigh;
}
;

typedef void *PVOID
;

typedef void *HANDLE
;

typedef GUID IID
;

typedef _LARGE_INTEGER LARGE_INTEGER
;

union _LARGE_INTEGER
{
struct
{
DWORD LowPart;
LONG HighPart;
};
_LARGE_INTEGER::$837407842DC9087486FDFA5FEB63B74E u;
LONGLONG QuadPart;
}
;

struct _LARGE_INTEGER::$837407842DC9087486FDFA5FEB63B74E
{
DWORD LowPart;
LONG HighPart;
}
;

typedef __int64 LONGLONG
;

struct _RTL_CRITICAL_SECTION
{
PRTL_CRITICAL_SECTION_DEBUG DebugInfo;
LONG LockCount;
LONG RecursionCount;
HANDLE OwningThread;
HANDLE LockSemaphore;
ULONG_PTR SpinCount;
}
;

typedef _RTL_CRITICAL_SECTION_DEBUG *PRTL_CRITICAL_SECTION_DEBUG
;

struct _RTL_CRITICAL_SECTION_DEBUG
{
WORD Type;
WORD CreatorBackTraceIndex;
_RTL_CRITICAL_SECTION *CriticalSection;
LIST_ENTRY ProcessLocksList;
DWORD EntryCount;
DWORD ContentionCount;
DWORD Flags;
WORD CreatorBackTraceIndexHigh;
WORD SpareWORD;
}
;

typedef _LIST_ENTRY LIST_ENTRY
;

struct _LIST_ENTRY
{
_LIST_ENTRY *Flink;
_LIST_ENTRY *Blink;
}
;

struct _OSVERSIONINFOA
{
DWORD dwOSVersionInfoSize;
DWORD dwMajorVersion;
DWORD dwMinorVersion;
DWORD dwBuildNumber;
DWORD dwPlatformId;
CHAR szCSDVersion[128];
}
;

typedef char CHAR
;

struct _MEMORYSTATUS
{
DWORD dwLength;
DWORD dwMemoryLoad;
SIZE_T dwTotalPhys;
SIZE_T dwAvailPhys;
SIZE_T dwTotalPageFile;
SIZE_T dwAvailPageFile;
SIZE_T dwTotalVirtual;
SIZE_T dwAvailVirtual;
}
;

typedef ULONG_PTR SIZE_T
;

typedef _XINPUT_VIBRATION XINPUT_VIBRATION
;

struct _XINPUT_VIBRATION
{
WORD wLeftMotorSpeed;
WORD wRightMotorSpeed;
}
;

typedef _XINPUT_CAPABILITIES XINPUT_CAPABILITIES
;

struct _XINPUT_CAPABILITIES
{
BYTE Type;
BYTE SubType;
WORD Flags;
XINPUT_GAMEPAD Gamepad;
XINPUT_VIBRATION Vibration;
}
;

typedef unsigned __int8 BYTE
;

typedef _XINPUT_GAMEPAD XINPUT_GAMEPAD
;

struct _XINPUT_GAMEPAD
{
WORD wButtons;
BYTE bLeftTrigger;
BYTE bRightTrigger;
SHORT sThumbLX;
SHORT sThumbLY;
SHORT sThumbRX;
SHORT sThumbRY;
}
;

typedef __int16 SHORT
;

typedef _XINPUT_STATE XINPUT_STATE
;

struct _XINPUT_STATE
{
DWORD dwPacketNumber;
XINPUT_GAMEPAD Gamepad;
}
;

struct tagMSG
{
HWND hwnd;
UINT message;
WPARAM wParam;
LPARAM lParam;
DWORD time;
POINT pt;
}
;

typedef HWND__ *HWND
;

struct HWND__
{
int unused;
}
;

typedef unsigned int UINT
;

typedef UINT_PTR WPARAM
;

typedef unsigned __int64 UINT_PTR
;

typedef LONG_PTR LPARAM
;

typedef __int64 LONG_PTR
;

typedef tagPOINT POINT
;

typedef tagMODULEENTRY32 MODULEENTRY32
;

struct tagMODULEENTRY32
{
DWORD dwSize;
DWORD th32ModuleID;
DWORD th32ProcessID;
DWORD GlblcntUsage;
DWORD ProccntUsage;
BYTE *modBaseAddr;
DWORD modBaseSize;
HMODULE hModule;
char szModule[256];
char szExePath[260];
}
;

typedef HINSTANCE HMODULE
;

typedef HINSTANCE__ *HINSTANCE
;

struct HINSTANCE__
{
int unused;
}
;

typedef tagWNDCLASSEXA WNDCLASSEXA
;

struct tagWNDCLASSEXA
{
UINT cbSize;
UINT style;
WNDPROC lpfnWndProc;
int cbClsExtra;
int cbWndExtra;
HINSTANCE hInstance;
HICON hIcon;
HCURSOR hCursor;
HBRUSH hbrBackground;
LPCSTR lpszMenuName;
LPCSTR lpszClassName;
HICON hIconSm;
}
;

typedef LRESULT (__stdcall *WNDPROC)(HWND, UINT, WPARAM, LPARAM)
;

typedef LONG_PTR LRESULT
;

typedef HICON__ *HICON
;

struct HICON__
{
int unused;
}
;

typedef HICON HCURSOR
;

typedef HBRUSH__ *HBRUSH
;

struct HBRUSH__
{
int unused;
}
;

typedef const CHAR *LPCSTR
;

struct sockaddr
{
ADDRESS_FAMILY sa_family;
CHAR sa_data[14];
}
;

typedef USHORT ADDRESS_FAMILY
;

typedef unsigned __int16 USHORT
;

struct WSAData
{
WORD wVersion;
WORD wHighVersion;
unsigned __int16 iMaxSockets;
unsigned __int16 iMaxUdpDg;
char *lpVendorInfo;
char szDescription[257];
char szSystemStatus[129];
}
;

typedef _SYSTEMTIME SYSTEMTIME
;

struct _SYSTEMTIME
{
WORD wYear;
WORD wMonth;
WORD wDayOfWeek;
WORD wDay;
WORD wHour;
WORD wMinute;
WORD wSecond;
WORD wMilliseconds;
}
;

struct _FILETIME
{
DWORD dwLowDateTime;
DWORD dwHighDateTime;
}
;

typedef tagWNDCLASSA WNDCLASSA
;

struct tagWNDCLASSA
{
UINT style;
WNDPROC lpfnWndProc;
int cbClsExtra;
int cbWndExtra;
HINSTANCE hInstance;
HICON hIcon;
HCURSOR hCursor;
HBRUSH hbrBackground;
LPCSTR lpszMenuName;
LPCSTR lpszClassName;
}
;

struct tagMIXERLINEA
{
DWORD cbStruct;
DWORD dwDestination;
DWORD dwSource;
DWORD dwLineID;
DWORD fdwLine;
DWORD_PTR dwUser;
DWORD dwComponentType;
DWORD cChannels;
DWORD cConnections;
DWORD cControls;
CHAR szShortName[16];
CHAR szName[64];
tagMIXERLINEA::$3F59E0B102B49F482C4A8B1440A7891E Target;
}
;

struct tagMIXERLINEA::$3F59E0B102B49F482C4A8B1440A7891E
{
DWORD dwType;
DWORD dwDeviceID;
WORD wMid;
WORD wPid;
MMVERSION vDriverVersion;
CHAR szPname[32];
}
;

typedef UINT MMVERSION
;

struct tagMIXERLINECONTROLSA
{
DWORD cbStruct;
DWORD dwLineID;
union
{
DWORD dwControlID;
DWORD dwControlType;
};
DWORD cControls;
DWORD cbmxctrl;
LPMIXERCONTROLA pamxctrl;
}
;

union tagMIXERLINECONTROLSA::$DC43C2837E8C20184C3137B308FBE3E1
{
DWORD dwControlID;
DWORD dwControlType;
}
;

typedef tagMIXERCONTROLA *LPMIXERCONTROLA
;

struct tagMIXERCONTROLA
{
DWORD cbStruct;
DWORD dwControlID;
DWORD dwControlType;
DWORD fdwControl;
DWORD cMultipleItems;
CHAR szShortName[16];
CHAR szName[64];
tagMIXERCONTROLA::$86CBF7128C7FD2AC13C8612745AD13D8 Bounds;
tagMIXERCONTROLA::$AFA1D3B62FF5E28583F9FD43B125895E Metrics;
}
;

union tagMIXERCONTROLA::$86CBF7128C7FD2AC13C8612745AD13D8
{
struct
{
LONG lMinimum;
LONG lMaximum;
};
struct
{
DWORD dwMinimum;
DWORD dwMaximum;
};
DWORD dwReserved[6];
}
;

struct tagMIXERCONTROLA::$86CBF7128C7FD2AC13C8612745AD13D8::$A9226EEFE738F03A3F7D52221D0941CC
{
LONG lMinimum;
LONG lMaximum;
}
;

struct tagMIXERCONTROLA::$86CBF7128C7FD2AC13C8612745AD13D8::$A8262751543F154F80C41ED2519FD13B
{
DWORD dwMinimum;
DWORD dwMaximum;
}
;

union tagMIXERCONTROLA::$AFA1D3B62FF5E28583F9FD43B125895E
{
DWORD cSteps;
DWORD cbCustomData;
DWORD dwReserved[6];
}
;

struct tMIXERCONTROLDETAILS
{
DWORD cbStruct;
DWORD dwControlID;
DWORD cChannels;
union
{
HWND hwndOwner;
DWORD cMultipleItems;
};
DWORD cbDetails;
LPVOID paDetails;
}
;

union tMIXERCONTROLDETAILS::$6A354DB64043DD4C59D209C2C03EE69F
{
HWND hwndOwner;
DWORD cMultipleItems;
}
;

struct tagMIXERCAPSA
{
WORD wMid;
WORD wPid;
MMVERSION vDriverVersion;
CHAR szPname[32];
DWORD fdwSupport;
DWORD cDestinations;
}
;

struct tagMONITORINFO
{
DWORD cbSize;
RECT rcMonitor;
RECT rcWork;
DWORD dwFlags;
}
;

typedef tagRECT RECT
;

struct _MEMORYSTATUSEX
{
DWORD dwLength;
DWORD dwMemoryLoad;
DWORDLONG ullTotalPhys;
DWORDLONG ullAvailPhys;
DWORDLONG ullTotalPageFile;
DWORDLONG ullAvailPageFile;
DWORDLONG ullTotalVirtual;
DWORDLONG ullAvailVirtual;
DWORDLONG ullAvailExtendedVirtual;
}
;

typedef ULONGLONG DWORDLONG
;

typedef unsigned __int64 ULONGLONG
;

enum D3D_FEATURE_LEVEL : __int32
{
D3D_FEATURE_LEVEL_9_1 = 0x9100,
D3D_FEATURE_LEVEL_9_2 = 0x9200,
D3D_FEATURE_LEVEL_9_3 = 0x9300,
D3D_FEATURE_LEVEL_10_0 = 0xA000,
D3D_FEATURE_LEVEL_10_1 = 0xA100,
D3D_FEATURE_LEVEL_11_0 = 0xB000,
D3D_FEATURE_LEVEL_11_1 = 0xB100,
D3D_FEATURE_LEVEL_12_0 = 0xC000,
D3D_FEATURE_LEVEL_12_1 = 0xC100,
}
;

struct _DISPLAY_DEVICEA
{
DWORD cb;
CHAR DeviceName[32];
CHAR DeviceString[128];
DWORD StateFlags;
CHAR DeviceID[128];
CHAR DeviceKey[128];
}
;

typedef _devicemodeA DEVMODEA
;

struct _devicemodeA
{
BYTE dmDeviceName[32];
WORD dmSpecVersion;
WORD dmDriverVersion;
WORD dmSize;
WORD dmDriverExtra;
DWORD dmFields;
union
{
struct
{
__int16 dmOrientation;
__int16 dmPaperSize;
__int16 dmPaperLength;
__int16 dmPaperWidth;
__int16 dmScale;
__int16 dmCopies;
__int16 dmDefaultSource;
__int16 dmPrintQuality;
};
struct
{
POINTL dmPosition;
DWORD dmDisplayOrientation;
DWORD dmDisplayFixedOutput;
};
};
__int16 dmColor;
__int16 dmDuplex;
__int16 dmYResolution;
__int16 dmTTOption;
__int16 dmCollate;
BYTE dmFormName[32];
WORD dmLogPixels;
DWORD dmBitsPerPel;
DWORD dmPelsWidth;
DWORD dmPelsHeight;
union
{
DWORD dmDisplayFlags;
DWORD dmNup;
};
DWORD dmDisplayFrequency;
DWORD dmICMMethod;
DWORD dmICMIntent;
DWORD dmMediaType;
DWORD dmDitherType;
DWORD dmReserved1;
DWORD dmReserved2;
DWORD dmPanningWidth;
DWORD dmPanningHeight;
}
;

union _devicemodeA::$CB1E1A2E30D5738ACA9AADF670A603C7
{
struct
{
__int16 dmOrientation;
__int16 dmPaperSize;
__int16 dmPaperLength;
__int16 dmPaperWidth;
__int16 dmScale;
__int16 dmCopies;
__int16 dmDefaultSource;
__int16 dmPrintQuality;
};
struct
{
POINTL dmPosition;
DWORD dmDisplayOrientation;
DWORD dmDisplayFixedOutput;
};
}
;

struct _devicemodeA::$CB1E1A2E30D5738ACA9AADF670A603C7::$A4EE175A7532F35C00DA5CAD042934DF
{
__int16 dmOrientation;
__int16 dmPaperSize;
__int16 dmPaperLength;
__int16 dmPaperWidth;
__int16 dmScale;
__int16 dmCopies;
__int16 dmDefaultSource;
__int16 dmPrintQuality;
}
;

struct _devicemodeA::$CB1E1A2E30D5738ACA9AADF670A603C7::$604F2BC6924E02F5722A52FC79BC44F9
{
POINTL dmPosition;
DWORD dmDisplayOrientation;
DWORD dmDisplayFixedOutput;
}
;

typedef _POINTL POINTL
;

struct _POINTL
{
LONG x;
LONG y;
}
;

union _devicemodeA::$4FBEBCCE69364E072421C9DF045BB77C
{
DWORD dmDisplayFlags;
DWORD dmNup;
}
;

typedef struct _EXCEPTION_RECORD EXCEPTION_RECORD
;

struct _EXCEPTION_RECORD
{
DWORD ExceptionCode;
DWORD ExceptionFlags;
struct _EXCEPTION_RECORD *ExceptionRecord;
PVOID ExceptionAddress;
DWORD NumberParameters;
ULONG_PTR ExceptionInformation[15];
}
;

struct fd_set
{
u_int fd_count;
SOCKET fd_array[64];
}
;

typedef unsigned int u_int
;

typedef UINT_PTR SOCKET
;

struct _OSVERSIONINFOW
{
DWORD dwOSVersionInfoSize;
DWORD dwMajorVersion;
DWORD dwMinorVersion;
DWORD dwBuildNumber;
DWORD dwPlatformId;
WCHAR szCSDVersion[128];
}
;

typedef wchar_t WCHAR
;

typedef unsigned __int16 wchar_t
;

struct timeval
{
int tv_sec;
int tv_usec;
}
;

struct __crt_locale_pointers
{
struct __crt_locale_data *locinfo;
struct __crt_multibyte_data *mbcinfo;
}
;

struct tm
{
int tm_sec;
int tm_min;
int tm_hour;
int tm_mday;
int tm_mon;
int tm_year;
int tm_wday;
int tm_yday;
int tm_isdst;
}
;

struct _finddata64i32_t
{
unsigned int attrib;
__time64_t time_create;
__time64_t time_access;
__time64_t time_write;
_fsize_t size;
char name[260];
}
;

typedef __int64 __time64_t
;

typedef unsigned int _fsize_t
;

typedef ThrowInfo _ThrowInfo
;

typedef const _s_ThrowInfo ThrowInfo
;

struct _s_ThrowInfo
{
unsigned int attributes;
PMFN pmfnUnwind;
int pForwardCompat;
int pCatchableTypeArray;
}
;

typedef int PMFN
;

typedef _iobuf FILE
;

struct _iobuf
{
char *_ptr;
int _cnt;
char *_base;
int _flag;
int _file;
int _charbuf;
int _bufsiz;
char *_tmpfname;
}
;

struct _stat64i32
{
_dev_t st_dev;
_ino_t st_ino;
unsigned __int16 st_mode;
__int16 st_nlink;
__int16 st_uid;
__int16 st_gid;
_dev_t st_rdev;
_off_t st_size;
__time64_t st_atime;
__time64_t st_mtime;
__time64_t st_ctime;
}
;

typedef unsigned int _dev_t
;

typedef unsigned __int16 _ino_t
;

typedef int _off_t
;

struct _stat64
{
_dev_t st_dev;
_ino_t st_ino;
unsigned __int16 st_mode;
__int16 st_nlink;
__int16 st_uid;
__int16 st_gid;
_dev_t st_rdev;
__int64 st_size;
__time64_t st_atime;
__time64_t st_mtime;
__time64_t st_ctime;
}
;

struct C_SCOPE_TABLE
{
_UNKNOWN *__ptr32 Begin __offset(OFF64|RVAOFF);
_UNKNOWN *__ptr32 End __offset(OFF64|RVAOFF|PASTEND);
_UNKNOWN *__ptr32 Handler __offset(OFF64|RVAOFF);
_UNKNOWN *__ptr32 Target __offset(OFF64|RVAOFF);
}
;

struct std::ios_base
;

struct Concurrency::details::_RefCounter
;

struct Concurrency::details::ContextBase
;

struct __ExceptionPtr
;

struct Concurrency::details::_NonReentrantLock
;

struct Concurrency::details::SchedulerProxy
;

struct Concurrency::details::ThreadScheduler
;

struct Concurrency::details::VirtualProcessor
;

struct DNameNode
;

struct Concurrency::details::_Scheduler
;

struct std::exception
;

struct Concurrency::details::UMSFreeVirtualProcessorRoot
;

struct Concurrency::details::WorkQueue
;

struct Concurrency::details::_TaskCollectionBase
;

struct Concurrency::details::UMSBackgroundPoller
;

struct Concurrency::details::_Concurrent_queue_base_v4
;

struct Concurrency::details::UMSFreeThreadProxyFactory
;

struct std::bad_weak_ptr
;

struct Concurrency::details::_AsyncTaskCollection
;

struct Concurrency::details::ExternalContextBase
;

struct _LocaleUpdate
;

typedef void **va_list
;

typedef unsigned __int64 *PULONG_PTR
;

typedef EXCEPTION_RECORD *PEXCEPTION_RECORD
;

struct __declspec(align(16)) _CONTEXT
{
DWORD64 P1Home;
DWORD64 P2Home;
DWORD64 P3Home;
DWORD64 P4Home;
DWORD64 P5Home;
DWORD64 P6Home;
DWORD ContextFlags;
DWORD MxCsr;
WORD SegCs;
WORD SegDs;
WORD SegEs;
WORD SegFs;
WORD SegGs;
WORD SegSs;
DWORD EFlags;
DWORD64 Dr0;
DWORD64 Dr1;
DWORD64 Dr2;
DWORD64 Dr3;
DWORD64 Dr6;
DWORD64 Dr7;
DWORD64 Rax;
DWORD64 Rcx;
DWORD64 Rdx;
DWORD64 Rbx;
DWORD64 Rsp;
DWORD64 Rbp;
DWORD64 Rsi;
DWORD64 Rdi;
DWORD64 R8;
DWORD64 R9;
DWORD64 R10;
DWORD64 R11;
DWORD64 R12;
DWORD64 R13;
DWORD64 R14;
DWORD64 R15;
DWORD64 Rip;
union _CONTEXT::$BCD05C38A0A011FAB68B98156F5D4292 DUMMYUNIONNAME;
M128A VectorRegister[26];
DWORD64 VectorControl;
DWORD64 DebugControl;
DWORD64 LastBranchToRip;
DWORD64 LastBranchFromRip;
DWORD64 LastExceptionToRip;
DWORD64 LastExceptionFromRip;
}
;

typedef unsigned __int64 ULONG64
;

typedef unsigned __int64 *PULONG64
;

typedef unsigned __int64 DWORD64
;

typedef unsigned __int64 *PDWORD64
;

struct __declspec(align(16)) _M128A
{
ULONGLONG Low;
LONGLONG High;
}
;

typedef struct _M128A M128A
;

typedef struct _M128A *PM128A
;

struct __declspec(align(16)) _XSAVE_FORMAT
{
WORD ControlWord;
WORD StatusWord;
BYTE TagWord;
BYTE Reserved1;
WORD ErrorOpcode;
DWORD ErrorOffset;
WORD ErrorSelector;
WORD Reserved2;
DWORD DataOffset;
WORD DataSelector;
WORD Reserved3;
DWORD MxCsr;
DWORD MxCsr_Mask;
M128A FloatRegisters[8];
M128A XmmRegisters[16];
BYTE Reserved4[96];
}
;

typedef struct _XSAVE_FORMAT XSAVE_FORMAT
;

typedef struct _XSAVE_FORMAT *PXSAVE_FORMAT
;

typedef XSAVE_FORMAT XMM_SAVE_AREA32
;

typedef XSAVE_FORMAT *PXMM_SAVE_AREA32
;

struct _CONTEXT::$BCD05C38A0A011FAB68B98156F5D4292::$0A4EAB0903F2AB73148FDDD396F7836E
{
M128A Header[2];
M128A Legacy[8];
M128A Xmm0;
M128A Xmm1;
M128A Xmm2;
M128A Xmm3;
M128A Xmm4;
M128A Xmm5;
M128A Xmm6;
M128A Xmm7;
M128A Xmm8;
M128A Xmm9;
M128A Xmm10;
M128A Xmm11;
M128A Xmm12;
M128A Xmm13;
M128A Xmm14;
M128A Xmm15;
}
;

union _CONTEXT::$BCD05C38A0A011FAB68B98156F5D4292
{
XMM_SAVE_AREA32 FltSave;
struct _CONTEXT::$BCD05C38A0A011FAB68B98156F5D4292::$0A4EAB0903F2AB73148FDDD396F7836E DUMMYSTRUCTNAME;
}
;

typedef struct _CONTEXT CONTEXT
;

typedef struct _CONTEXT *PCONTEXT
;

struct _EXCEPTION_POINTERS
{
PEXCEPTION_RECORD ExceptionRecord;
PCONTEXT ContextRecord;
}
;

typedef struct _EXCEPTION_POINTERS EXCEPTION_POINTERS
;

typedef struct _EXCEPTION_POINTERS *PEXCEPTION_POINTERS
;

union __declspec(align(8)) __m64
{
unsigned __int64 m64_u64;
float m64_f32[2];
__int8 m64_i8[8];
__int16 m64_i16[4];
__int32 m64_i32[2];
__int64 m64_i64;
unsigned __int8 m64_u8[8];
unsigned __int16 m64_u16[4];
unsigned __int32 m64_u32[2];
}
;

union __declspec(align(16)) __m128
{
float m128_f32[4];
unsigned __int64 m128_u64[2];
__int8 m128_i8[16];
__int16 m128_i16[8];
__int32 m128_i32[4];
__int64 m128_i64[2];
unsigned __int8 m128_u8[16];
unsigned __int16 m128_u16[8];
unsigned __int32 m128_u32[4];
}
;

struct __m128d
{
double m128d_f64[2];
}
;

union __declspec(align(16)) __m128i
{
__int8 m128i_i8[16];
__int16 m128i_i16[8];
__int32 m128i_i32[4];
__int64 m128i_i64[2];
unsigned __int8 m128i_u8[16];
unsigned __int16 m128i_u16[8];
unsigned __int32 m128i_u32[4];
unsigned __int64 m128i_u64[2];
}
;

union __declspec(align(32)) __m256
{
float m256_f32[8];
}
;

union __declspec(align(32)) __m256d
{
double m256d_f64[4];
}
;

union __declspec(align(32)) __m256i
{
__int8 m256i_i8[32];
__int16 m256i_i16[16];
__int32 m256i_i32[8];
__int64 m256i_i64[4];
unsigned __int8 m256i_u8[32];
unsigned __int16 m256i_u16[16];
unsigned __int32 m256i_u32[8];
unsigned __int64 m256i_u64[4];
}
;

struct LargeLocal
;

typedef int BOOL
;

typedef _JBTYPE jmp_buf[16]
;

typedef SETJMP_FLOAT128 _JBTYPE
;

typedef _SETJMP_FLOAT128 SETJMP_FLOAT128
;

struct __declspec(align(16)) _SETJMP_FLOAT128
{
unsigned __int64 Part[2];
}
;

typedef $BA519187DB4FB038EDB1959BB4D2DF27 _CRT_DOUBLE
;

struct $BA519187DB4FB038EDB1959BB4D2DF27
{
double x;
}
;

typedef $E6A349E6C594207448DC3F8A57F3A91C _CRT_FLOAT
;

struct $E6A349E6C594207448DC3F8A57F3A91C
{
float f;
}
;

struct bdBandwidthTestClient
;

struct bdBandwidthTestResults
;

struct bdLobbyService
;

struct EntHandle
;

struct bdTag
;

struct _TEB
{
_NT_TIB NtTib;
void *EnvironmentPointer;
_CLIENT_ID ClientId;
void *ActiveRpcHandle;
void *ThreadLocalStoragePointer;
_PEB *ProcessEnvironmentBlock;
unsigned int LastErrorValue;
unsigned int CountOfOwnedCriticalSections;
void *CsrClientThread;
void *Win32ThreadInfo;
unsigned int User32Reserved[26];
unsigned int UserReserved[5];
void *WOW32Reserved;
unsigned int CurrentLocale;
unsigned int FpSoftwareStatusRegister;
void *SystemReserved1[54];
int ExceptionCode;
_ACTIVATION_CONTEXT_STACK *ActivationContextStackPointer;
uint8_t SpareBytes[24];
unsigned int TxFsContext;
_GDI_TEB_BATCH GdiTebBatch;
_CLIENT_ID RealClientId;
void *GdiCachedProcessHandle;
unsigned int GdiClientPID;
unsigned int GdiClientTID;
void *GdiThreadLocalInfo;
uint64_t Win32ClientInfo[62];
void *glDispatchTable[233];
uint64_t glReserved1[29];
void *glReserved2;
void *glSectionInfo;
void *glSection;
void *glTable;
void *glCurrentRC;
void *glContext;
unsigned int LastStatusValue;
_UNICODE_STRING StaticUnicodeString;
wchar_t StaticUnicodeBuffer[261];
void *DeallocationStack;
void *TlsSlots[64];
_LIST_ENTRY TlsLinks;
void *Vdm;
void *ReservedForNtRpc;
void *DbgSsReserved[2];
unsigned int HardErrorMode;
void *Instrumentation[11];
_GUID ActivityId;
void *SubProcessTag;
void *EtwLocalData;
void *EtwTraceData;
void *WinSockData;
unsigned int GdiBatchCount;
union
{
_PROCESSOR_NUMBER CurrentIdealProcessor;
unsigned int IdealProcessorValue;
uint8_t ReservedPad0;
};
uint8_t ReservedPad1;
uint8_t ReservedPad2;
uint8_t IdealProcessor;
unsigned int GuaranteedStackBytes;
void *ReservedForPerf;
void *ReservedForOle;
unsigned int WaitingOnLoaderLock;
void *SavedPriorityState;
uint64_t SoftPatchPtr1;
void *ThreadPoolData;
void **TlsExpansionSlots;
void *DeallocationBStore;
void *BStoreLimit;
unsigned int MuiGeneration;
unsigned int IsImpersonating;
void *NlsCache;
void *pShimData;
unsigned int HeapVirtualAffinity;
void *CurrentTransactionHandle;
struct _TEB_ACTIVE_FRAME *ActiveFrame;
void *FlsData;
void *PreferredLanguages;
void *UserPrefLanguages;
void *MergedPrefLanguages;
unsigned int MuiImpersonation;
volatile uint16_t CrossTebFlags;
unsigned __int16 SpareCrossTebBits : 16;
uint16_t SameTebFlags;
unsigned __int16 SafeThunkCall : 1;
unsigned __int16 InDebugPrint : 1;
unsigned __int16 HasFiberData : 1;
unsigned __int16 SkipThreadAttach : 1;
unsigned __int16 WerInShipAssertCode : 1;
unsigned __int16 RanProcessInit : 1;
unsigned __int16 ClonedThread : 1;
unsigned __int16 SuppressDebugMsg : 1;
unsigned __int16 DisableUserStackWalk : 1;
unsigned __int16 RtlExceptionAttached : 1;
unsigned __int16 InitialThread : 1;
unsigned __int16 SpareSameTebBits : 5;
void *TxnScopeEnterCallback;
void *TxnScopeExitCallback;
void *TxnScopeContext;
unsigned int LockCount;
unsigned int SpareUlong0;
void *ResourceRetValue;
}
;

struct _NT_TIB
{
_EXCEPTION_REGISTRATION_RECORD *ExceptionList;
PVOID StackBase;
PVOID StackLimit;
PVOID SubSystemTib;
union
{
PVOID FiberData;
DWORD Version;
};
PVOID ArbitraryUserPointer;
_NT_TIB *Self;
}
;

struct _EXCEPTION_REGISTRATION_RECORD
{
_EXCEPTION_REGISTRATION_RECORD *Next;
PEXCEPTION_ROUTINE Handler;
}
;

typedef EXCEPTION_ROUTINE *PEXCEPTION_ROUTINE
;

typedef EXCEPTION_DISPOSITION __stdcall EXCEPTION_ROUTINE(_EXCEPTION_RECORD *ExceptionRecord, PVOID EstablisherFrame, _CONTEXT *ContextRecord, PVOID DispatcherContext)
;

typedef _EXCEPTION_DISPOSITION EXCEPTION_DISPOSITION
;

enum _EXCEPTION_DISPOSITION
{
ExceptionContinueExecution = 0x0,
ExceptionContinueSearch = 0x1,
ExceptionNestedException = 0x2,
ExceptionCollidedUnwind = 0x3,
}
;

union _NT_TIB::$0349ADB4452EC09BEC08E2292695FBBA
{
PVOID FiberData;
DWORD Version;
}
;

struct _CLIENT_ID
{
HANDLE UniqueProcess;
HANDLE UniqueThread;
}
;

struct _PEB
{
uint8_t InheritedAddressSpace;
uint8_t ReadImageFileExecOptions;
uint8_t BeingDebugged;
uint8_t BitField;
unsigned __int8 ImageUsesLargePages : 1;
unsigned __int8 IsProtectedProcess : 1;
unsigned __int8 IsLegacyProcess : 1;
unsigned __int8 IsImageDynamicallyRelocated : 1;
unsigned __int8 SkipPatchingUser32Forwarders : 1;
unsigned __int8 SpareBits : 3;
void *Mutant;
void *ImageBaseAddress;
_PEB_LDR_DATA *Ldr;
_RTL_USER_PROCESS_PARAMETERS *ProcessParameters;
void *SubSystemData;
void *ProcessHeap;
_RTL_CRITICAL_SECTION *FastPebLock;
void *AtlThunkSListPtr;
void *IFEOKey;
unsigned int CrossProcessFlags;
unsigned __int32 ProcessInJob : 1;
unsigned __int32 ProcessInitializing : 1;
unsigned __int32 ProcessUsingVEH : 1;
unsigned __int32 ProcessUsingVCH : 1;
unsigned __int32 ProcessUsingFTH : 1;
unsigned __int32 ReservedBits0 : 27;
union
{
void *KernelCallbackTable;
void *UserSharedInfoPtr;
};
unsigned int SystemReserved[1];
unsigned int AtlThunkSListPtr32;
void *ApiSetMap;
unsigned int TlsExpansionCounter;
void *TlsBitmap;
unsigned int TlsBitmapBits[2];
void *ReadOnlySharedMemoryBase;
void *HotpatchInformation;
void **ReadOnlyStaticServerData;
void *AnsiCodePageData;
void *OemCodePageData;
void *UnicodeCaseTableData;
unsigned int NumberOfProcessors;
unsigned int NtGlobalFlag;
_LARGE_INTEGER CriticalSectionTimeout;
uint64_t HeapSegmentReserve;
uint64_t HeapSegmentCommit;
uint64_t HeapDeCommitTotalFreeThreshold;
uint64_t HeapDeCommitFreeBlockThreshold;
unsigned int NumberOfHeaps;
unsigned int MaximumNumberOfHeaps;
void **ProcessHeaps;
void *GdiSharedHandleTable;
void *ProcessStarterHelper;
unsigned int GdiDCAttributeList;
_RTL_CRITICAL_SECTION *LoaderLock;
unsigned int OSMajorVersion;
unsigned int OSMinorVersion;
uint16_t OSBuildNumber;
uint16_t OSCSDVersion;
unsigned int OSPlatformId;
unsigned int ImageSubsystem;
unsigned int ImageSubsystemMajorVersion;
unsigned int ImageSubsystemMinorVersion;
uint64_t ActiveProcessAffinityMask;
unsigned int GdiHandleBuffer[60];
void *PostProcessInitRoutine;
void *TlsExpansionBitmap;
unsigned int TlsExpansionBitmapBits[32];
unsigned int SessionId;
_ULARGE_INTEGER AppCompatFlags;
_ULARGE_INTEGER AppCompatFlagsUser;
void *pShimData;
void *AppCompatInfo;
_UNICODE_STRING CSDVersion;
const struct _ACTIVATION_CONTEXT_DATA *ActivationContextData;
struct _ASSEMBLY_STORAGE_MAP *ProcessAssemblyStorageMap;
const struct _ACTIVATION_CONTEXT_DATA *SystemDefaultActivationContextData;
struct _ASSEMBLY_STORAGE_MAP *SystemAssemblyStorageMap;
uint64_t MinimumStackCommit;
struct _FLS_CALLBACK_INFO *FlsCallback;
_LIST_ENTRY FlsListHead;
void *FlsBitmap;
unsigned int FlsBitmapBits[4];
unsigned int FlsHighIndex;
void *WerRegistrationData;
void *WerShipAssertPtr;
void *pContextData;
void *pImageHeaderHash;
unsigned int TracingFlags;
unsigned __int32 HeapTracingEnabled : 1;
unsigned __int32 CritSecTracingEnabled : 1;
unsigned __int32 SpareTracingBits : 30;
}
;

typedef unsigned __int8 uint8_t
;

struct _PEB_LDR_DATA
{
BYTE Reserved1[8];
PVOID Reserved2[3];
LIST_ENTRY InMemoryOrderModuleList;
}
;

struct _RTL_USER_PROCESS_PARAMETERS
{
BYTE Reserved1[16];
PVOID Reserved2[10];
UNICODE_STRING ImagePathName;
UNICODE_STRING CommandLine;
}
;

typedef _UNICODE_STRING UNICODE_STRING
;

struct _UNICODE_STRING
{
unsigned __int16 Length;
unsigned __int16 MaximumLength;
wchar_t *Buffer;
}
;

union _PEB::$6F1CA9A36B21C857AE5467E073440320
{
void *KernelCallbackTable;
void *UserSharedInfoPtr;
}
;

typedef unsigned __int64 uint64_t
;

typedef unsigned __int16 uint16_t
;

union _ULARGE_INTEGER
{
struct
{
DWORD LowPart;
DWORD HighPart;
};
_ULARGE_INTEGER::$0354AA9C204208F00D0965D07BBE7FAC u;
ULONGLONG QuadPart;
}
;

struct _ULARGE_INTEGER::$0354AA9C204208F00D0965D07BBE7FAC
{
DWORD LowPart;
DWORD HighPart;
}
;

struct _ACTIVATION_CONTEXT_STACK
{
struct _RTL_ACTIVATION_CONTEXT_STACK_FRAME *ActiveFrame;
_LIST_ENTRY FrameListCache;
unsigned int Flags;
unsigned int NextCookieSequenceNumber;
unsigned int StackId;
}
;

struct _GDI_TEB_BATCH
{
unsigned int Offset;
uint64_t HDC;
unsigned int Buffer[310];
}
;

union _TEB::$A3D02A70492DFE9D91413B66511C1D96
{
_PROCESSOR_NUMBER CurrentIdealProcessor;
unsigned int IdealProcessorValue;
uint8_t ReservedPad0;
}
;

struct _PROCESSOR_NUMBER
{
WORD Group;
BYTE Number;
BYTE Reserved;
}
;

struct bdByteBuffer
;

struct bdSessionInvite
;

struct bdChannelMember
;

struct bdChannelInfo
;

struct bdSessionID
;

struct bdQoSProbeListener
;

struct bdAntiCheatChallenges
;

struct bdUserAccountID
;

struct bdFeatureBanInfo
;

struct hks::GarbageCollector
;

struct bdNetImpl
;

struct bdNetStartParams
;

struct bdSockAddr
;

struct bdInetAddr
;

struct bdString
;

struct bdCypherAES
;

struct bdAntiCheatResponses
;

struct bdCypher3Des
;

struct bdAuthService
;

struct bdConnection
;

struct bdAddr
;

struct MatchMakingInfo
;

struct hks
;

struct hks::CodeGenerator
;

struct hks::CodeGenerator::FunctionGenerationState::ExpListEntry
;

struct hks::CodeGenerator::FunctionGenerationState
;

struct hks::HeapVisualizer
;

struct hks::Preprocessor
;

struct hks::StringReader
;

struct hks::StructProto
;

struct hks::StringPinner
;

struct hks::Visitor
;

struct hks::CompilerReader
;

struct hks::CompilerLexer
;

struct hks::HashTable
;

struct hks::Parser
;

struct hks::Visitor::Node
;

struct hks::BytecodeReader
;

struct hks::CallStack
;

struct hks::Serializer
;

struct hks::MemoryManager
;

struct hks::StringTable
;

struct hks::Preprocessor::Action
;

struct hks::BytecodeWriter
;

struct hks::StructInst
;

struct hks::StructUtil
;

struct hks::MemoryAllocationsByType
;

struct hks::SimpleCompilerState
;

struct hks::BytecodeReader::Reader
;

struct hks::HksCharacterBuffer
;

struct hks::Preprocessor::PreprocessorStateProxy
;

struct hks::GarbageInspector
;

struct hks::MemoryBuffer
;

struct hks::LoggingVisitor
;

struct CustomSearchInfo_FindPath
;

struct CustomSearchInfo_FindPathAlien
;

struct CustomSearchInfo_FindPathWithinRadius
;

struct SentientHandle
;

struct pathnode_t
;

struct bdSecurityKeyMapListener
;

struct hks::CompilerParseAcceptor
;

struct std::error_category
;

struct bdQoSProbeInfo
;

struct dwQoSMultiProbeListener
;

struct bdMessage
;

struct bdDTLSHeader
;

struct dxGeom
;

struct dxUserGeom
;

struct dxSimpleSpace
;

struct dxSpace
;

struct dxBox
;

struct dxGeomTransform
;

struct dxJointLimitMotor
;

struct bdSocketRouter
;

struct Leaderboard
;

struct PublicProfileInfo
;

struct MemoryHashInfo
;

struct bdSecurityKey
;

struct bdSecurityID
;

struct bdHTTPWrapper
;

struct GfxCmdBufState
;

struct bdCommonAddr
;

struct bdDTLSCookieAck
;

struct bdTrulyRandomImpl
;

struct bdEndpoint
;

struct bdEndpointHashingClass
;

struct bdHashTiger192
;

struct bdBitBuffer
;

struct bdMutex
;

struct bdRemoteTask
;

struct bdInitAckChunk
;

struct bdMatchMakingInfo
;

struct bdReferencable
;

struct bdTitleUtilities
;

struct bdFileMetaData
;

struct bdFileInfo
;

struct bdFileData
;

struct bdGroupCount
;

struct bdEventLogID
;

struct bdFileID
;

struct bdContentStreamingBase
;

struct bdUCD
;

struct bdDMLInfo
;

struct bdVerifyString
;

struct bdStatsInfo
;

struct bdStorage
;

struct bdTimeStamp
;

struct bdLegacyStatsInfo
;

struct bdCounterValue
;

struct bdLobbyEventHandler
;

struct bdTags
;

struct bdUserInfo
;

struct bdPerformanceValue
;

struct bdStats
;

struct bdContentStreaming
;

struct bdRemoteTaskManager
;

struct bdContextUserStorageFileInfo
;

struct bdProfileInfo
;

struct bdStatsInfoEx
;

struct bdVoteRank
;

struct bdKeyValuePair
;

struct bdAntiCheatChallengeParam
;

struct bdAntiCheatChallenge
;

struct bdTaskByteBuffer
;

struct bdLobbyConnectionListener
;

struct bdPendingBufferTransfer
;

struct bdLobbyConnection
;

struct bdHTTPWrapperBase
;

struct bdSummaryMetaHandler
;

struct bdURL
;

struct bdPreCopyResult
;

struct bdUCDPublicKeyInfo
;

struct bdAuthTicket
;

struct std::_Iostream_error_category
;

struct bdUPnPConfig
;

struct bdGetHostByNameConfig
;

struct bdGetHostByName
;

struct bdUPnP
;

struct bdUPnPDevice
;

struct bdQoSProbe
;

struct bdQoSProbe::bdQoSProbeEntryWrapper
;

struct bdAddrHandle
;

struct bdQoSReplyPacket
;

struct bdPacketInterceptor
;

struct bdSecurityKeyMap
;

struct bdNATTypeDiscoveryConfig
;

struct bdNATTypeDiscoveryClient
;

struct bdIPDiscoveryConfig
;

struct bdIPDiscoveryClient
;

struct bdSocketRouterConfig
;

struct bdNATTravListener
;

struct bdQoSRequestPacket
;

struct bdServiceBandwidthArbitrator
;

struct bdNATTravClient
;

struct bdNATTravClientData
;

struct bdDTLSError
;

struct bdDTLSInitAck
;

struct bdDTLSAssociation
;

struct bdPacketBuffer
;

struct bdPacketBufferInternal
;

struct bdDTLSInit
;

struct bdDTLSCookieEcho
;

struct bdNATTypeDiscoveryPacket
;

struct bdNATTypeDiscoveryPacketReply
;

struct bdIPDiscoveryPacket
;

struct bdIPDiscoveryPacketReply
;

struct bdNATTraversalPacket
;

struct bdDTLSData
;

struct bdDTLSXorCursor
;

struct std::bad_alloc
;

struct std::out_of_range
;

struct std::bad_exception
;

struct bdConnectionStoreConfig
;

struct bdConnectionStore
;

struct bdDispatcher
;

struct bdLoopbackConnection
;

struct bdUnicastConnection
;

struct bdConnectionStatistics
;

struct bdPacket
;

struct bdUnreliableSendWindow
;

struct bdUnreliableReceiveWindow
;

struct bdChunk
;

struct bdDataChunk
;

struct bdReliableSendWindow
;

struct bdReliableReceiveWindow
;

struct bdInitChunk
;

struct bdCookie
;

struct bdCookieEchoChunk
;

struct bdCookieAckChunk
;

struct bdHeartbeatChunk
;

struct bdHeartbeatAckChunk
;

struct bdShutdownChunk
;

struct bdShutdownAckChunk
;

struct bdShutdownCompleteChunk
;

struct bdSAckChunk
;

struct bdStopwatch
;

struct bdHashMD5
;

struct bdRSAKey
;

struct bdSocket
;

struct bdRandom
;

struct bdHMacSHA1
;

struct bdRunnable
;

struct bdStreamSocket
;

struct bdThread
;

struct bdSemaphore
;

struct bdSequenceNumber
;

struct bdSequenceNumberStore
;

struct bdECCKey
;

struct bdAddrHandleRefWrapper
;

struct bdCypher
;

struct IDirectSoundCaptureVtbl
{
HRESULT (__stdcall *QueryInterface)(IDirectSoundCapture *This, const IID *const, LPVOID *);
ULONG (__stdcall *AddRef)(IDirectSoundCapture *This);
ULONG (__stdcall *Release)(IDirectSoundCapture *This);
HRESULT (__stdcall *CreateCaptureBuffer)(IDirectSoundCapture *This, LPCDSCBUFFERDESC pcDSCBufferDesc, LPDIRECTSOUNDCAPTUREBUFFER *ppDSCBuffer, LPUNKNOWN pUnkOuter);
HRESULT (__stdcall *GetCaps)(IDirectSoundCapture *This, LPDSCCAPS pDSCCaps);
HRESULT (__stdcall *Initialize)(IDirectSoundCapture *This, LPCGUID pcGuidDevice);
}
;

struct IDirectSoundCapture
{
IDirectSoundCaptureVtbl *lpVtbl;
}
;

typedef int HRESULT
;

typedef unsigned int ULONG
;

typedef const DSCBUFFERDESC *LPCDSCBUFFERDESC
;

typedef _DSCBUFFERDESC DSCBUFFERDESC
;

struct _DSCBUFFERDESC
{
DWORD dwSize;
DWORD dwFlags;
DWORD dwBufferBytes;
DWORD dwReserved;
LPWAVEFORMATEX lpwfxFormat;
DWORD dwFXCount;
LPDSCEFFECTDESC lpDSCFXDesc;
}
;

typedef tWAVEFORMATEX *LPWAVEFORMATEX
;

struct tWAVEFORMATEX
{
WORD wFormatTag;
WORD nChannels;
DWORD nSamplesPerSec;
DWORD nAvgBytesPerSec;
WORD nBlockAlign;
WORD wBitsPerSample;
WORD cbSize;
}
;

typedef _DSCEFFECTDESC *LPDSCEFFECTDESC
;

struct _DSCEFFECTDESC
{
DWORD dwSize;
DWORD dwFlags;
GUID guidDSCFXClass;
GUID guidDSCFXInstance;
DWORD dwReserved1;
DWORD dwReserved2;
}
;

typedef IDirectSoundCaptureBuffer *LPDIRECTSOUNDCAPTUREBUFFER
;

struct IDirectSoundCaptureBuffer
{
IDirectSoundCaptureBufferVtbl *lpVtbl;
}
;

struct IDirectSoundCaptureBufferVtbl
{
HRESULT (__stdcall *QueryInterface)(IDirectSoundCaptureBuffer *This, const IID *const, LPVOID *);
ULONG (__stdcall *AddRef)(IDirectSoundCaptureBuffer *This);
ULONG (__stdcall *Release)(IDirectSoundCaptureBuffer *This);
HRESULT (__stdcall *GetCaps)(IDirectSoundCaptureBuffer *This, LPDSCBCAPS pDSCBCaps);
HRESULT (__stdcall *GetCurrentPosition)(IDirectSoundCaptureBuffer *This, LPDWORD pdwCapturePosition, LPDWORD pdwReadPosition);
HRESULT (__stdcall *GetFormat)(IDirectSoundCaptureBuffer *This, LPWAVEFORMATEX pwfxFormat, DWORD dwSizeAllocated, LPDWORD pdwSizeWritten);
HRESULT (__stdcall *GetStatus)(IDirectSoundCaptureBuffer *This, LPDWORD pdwStatus);
HRESULT (__stdcall *Initialize)(IDirectSoundCaptureBuffer *This, LPDIRECTSOUNDCAPTURE pDirectSoundCapture, LPCDSCBUFFERDESC pcDSCBufferDesc);
HRESULT (__stdcall *Lock)(IDirectSoundCaptureBuffer *This, DWORD dwOffset, DWORD dwBytes, LPVOID *ppvAudioPtr1, LPDWORD pdwAudioBytes1, LPVOID *ppvAudioPtr2, LPDWORD pdwAudioBytes2, DWORD dwFlags);
HRESULT (__stdcall *Start)(IDirectSoundCaptureBuffer *This, DWORD dwFlags);
HRESULT (__stdcall *Stop)(IDirectSoundCaptureBuffer *This);
HRESULT (__stdcall *Unlock)(IDirectSoundCaptureBuffer *This, LPVOID pvAudioPtr1, DWORD dwAudioBytes1, LPVOID pvAudioPtr2, DWORD dwAudioBytes2);
}
;

typedef _DSCBCAPS *LPDSCBCAPS
;

struct _DSCBCAPS
{
DWORD dwSize;
DWORD dwFlags;
DWORD dwBufferBytes;
DWORD dwReserved;
}
;

typedef DWORD *LPDWORD
;

typedef IDirectSoundCapture *LPDIRECTSOUNDCAPTURE
;

typedef IUnknown *LPUNKNOWN
;

struct IUnknown
{
IUnknownVtbl *lpVtbl;
}
;

struct IUnknownVtbl
{
HRESULT (__stdcall *QueryInterface)(IUnknown *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(IUnknown *This);
ULONG (__stdcall *Release)(IUnknown *This);
}
;

typedef _DSCCAPS *LPDSCCAPS
;

struct _DSCCAPS
{
DWORD dwSize;
DWORD dwFlags;
DWORD dwFormats;
DWORD dwChannels;
}
;

typedef const GUID *LPCGUID
;

typedef unsigned __int64 size_t
;

typedef WCHAR *LPWSTR
;

typedef struct HDC__ *HDC
;

struct HDC__
{
int unused;
}
;

typedef unsigned __int64 uintptr_t
;

typedef size_t rsize_t
;

typedef __crt_locale_pointers *_locale_t
;

typedef struct _UNWIND_HISTORY_TABLE *PUNWIND_HISTORY_TABLE
;

struct _UNWIND_HISTORY_TABLE
{
DWORD Count;
BYTE LocalHint;
BYTE GlobalHint;
BYTE Search;
BYTE Once;
DWORD64 LowAddress;
DWORD64 HighAddress;
UNWIND_HISTORY_TABLE_ENTRY Entry[12];
}
;

typedef struct _UNWIND_HISTORY_TABLE_ENTRY UNWIND_HISTORY_TABLE_ENTRY
;

struct _UNWIND_HISTORY_TABLE_ENTRY
{
DWORD64 ImageBase;
PRUNTIME_FUNCTION FunctionEntry;
}
;

typedef struct _IMAGE_RUNTIME_FUNCTION_ENTRY *PRUNTIME_FUNCTION
;

struct _IMAGE_RUNTIME_FUNCTION_ENTRY
{
DWORD BeginAddress;
DWORD EndAddress;
union
{
DWORD UnwindInfoAddress;
DWORD UnwindData;
};
}
;

union _IMAGE_RUNTIME_FUNCTION_ENTRY::$1FC7DDE9AB0EB2C3B18F286E3EAA9FDE
{
DWORD UnwindInfoAddress;
DWORD UnwindData;
}
;

typedef unsigned int u_long
;

typedef unsigned __int16 wint_t
;

typedef struct HMENU__ *HMENU
;

struct HMENU__
{
int unused;
}
;

typedef CHAR *LPSTR
;

typedef HKEY *PHKEY
;

typedef struct HKEY__ *HKEY
;

struct HKEY__
{
int unused;
}
;

typedef struct _FILETIME *PFILETIME
;

typedef int INT
;

typedef struct HMIXER__ *HMIXER
;

struct HMIXER__
{
int unused;
}
;

typedef void (__stdcall *LPOVERLAPPED_COMPLETION_ROUTINE)(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
;

typedef struct _OVERLAPPED *LPOVERLAPPED
;

typedef struct _MEMORY_BASIC_INFORMATION *PMEMORY_BASIC_INFORMATION
;

