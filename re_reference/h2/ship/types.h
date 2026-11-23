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

struct IPtoStateMap
{
_UNKNOWN *__ptr32 pc __offset(OFF64|RVAOFF);
int state __dec __tabform(NODUPS,1);
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

struct TypeDescriptor
{
_UNKNOWN *pVFTable;
_UNKNOWN *spare;
char name[] __strlit(C,"windows-1252");
}
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

typedef unsigned __int16 WORD
;

typedef _LIST_ENTRY LIST_ENTRY
;

struct _LIST_ENTRY
{
_LIST_ENTRY *Flink;
_LIST_ENTRY *Blink;
}
;

typedef unsigned int DWORD
;

typedef int LONG
;

typedef void *HANDLE
;

typedef unsigned __int64 ULONG_PTR
;

struct _OSVERSIONINFOEXW
{
DWORD dwOSVersionInfoSize;
DWORD dwMajorVersion;
DWORD dwMinorVersion;
DWORD dwBuildNumber;
DWORD dwPlatformId;
WCHAR szCSDVersion[128];
WORD wServicePackMajor;
WORD wServicePackMinor;
WORD wSuiteMask;
BYTE wProductType;
BYTE wReserved;
}
;

typedef wchar_t WCHAR
;

typedef unsigned __int16 wchar_t
;

typedef unsigned __int8 BYTE
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

typedef _CRYPTOAPI_BLOB DATA_BLOB
;

struct _CRYPTOAPI_BLOB
{
DWORD cbData;
BYTE *pbData;
}
;

struct _TIME_ZONE_INFORMATION
{
LONG Bias;
WCHAR StandardName[32];
SYSTEMTIME StandardDate;
LONG StandardBias;
WCHAR DaylightName[32];
SYSTEMTIME DaylightDate;
LONG DaylightBias;
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

struct _SYSTEM_INFO::$A707B71C060B6D10F73A71917EA8473F::$AA04DEB0C6383F89F13D312A174572A9
{
WORD wProcessorArchitecture;
WORD wReserved;
}
;

typedef void *LPVOID
;

typedef ULONG_PTR DWORD_PTR
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

typedef unsigned __int64 UINT_PTR
;

struct timeval
{
int tv_sec;
int tv_usec;
}
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

struct $BC2FB811D417144E831EE3AEA4A279C8
{
DWORD dwStructSize;
LPSTR lpszScheme;
DWORD dwSchemeLength;
INTERNET_SCHEME nScheme;
LPSTR lpszHostName;
DWORD dwHostNameLength;
INTERNET_PORT nPort;
LPSTR lpszUserName;
DWORD dwUserNameLength;
LPSTR lpszPassword;
DWORD dwPasswordLength;
LPSTR lpszUrlPath;
DWORD dwUrlPathLength;
LPSTR lpszExtraInfo;
DWORD dwExtraInfoLength;
}
;

typedef CHAR *LPSTR
;

typedef char CHAR
;

enum INTERNET_SCHEME : __int32
{
INTERNET_SCHEME_DEFAULT = 0x0,
INTERNET_SCHEME_FTP = 0x1,
INTERNET_SCHEME_FIRST = 0x1,
INTERNET_SCHEME_GOPHER = 0x2,
INTERNET_SCHEME_HTTP = 0x3,
INTERNET_SCHEME_HTTPS = 0x4,
INTERNET_SCHEME_FILE = 0x5,
INTERNET_SCHEME_NEWS = 0x6,
INTERNET_SCHEME_MAILTO = 0x7,
INTERNET_SCHEME_SOCKS = 0x8,
INTERNET_SCHEME_JAVASCRIPT = 0x9,
INTERNET_SCHEME_VBSCRIPT = 0xA,
INTERNET_SCHEME_RES = 0xB,
INTERNET_SCHEME_LAST = 0xB,
INTERNET_SCHEME_PARTIAL = 0xFFFFFFFE,
INTERNET_SCHEME_UNKNOWN = 0xFFFFFFFF,
}
;

typedef INTERNET_SCHEME $5BDA056A888DC5F03790D737B7575A6D
;

typedef WORD INTERNET_PORT
;

typedef $D8DE69AA0595B56AAEA2F23F58CB7637 WINHTTP_CURRENT_USER_IE_PROXY_CONFIG
;

struct $D8DE69AA0595B56AAEA2F23F58CB7637
{
BOOL fAutoDetect;
LPWSTR lpszAutoConfigUrl;
LPWSTR lpszProxy;
LPWSTR lpszProxyBypass;
}
;

typedef int BOOL
;

typedef WCHAR *LPWSTR
;

typedef $639EB13322B7E2DE7AC9BF644D7D7E12 WINHTTP_AUTOPROXY_OPTIONS
;

struct $639EB13322B7E2DE7AC9BF644D7D7E12
{
DWORD dwFlags;
DWORD dwAutoDetectFlags;
LPCWSTR lpszAutoConfigUrl;
LPVOID lpvReserved;
DWORD dwReserved;
BOOL fAutoLogonIfChallenged;
}
;

typedef const WCHAR *LPCWSTR
;

typedef $CAAF0F958FD5F3F6E8525632E8D5528E WINHTTP_PROXY_INFO
;

struct $CAAF0F958FD5F3F6E8525632E8D5528E
{
DWORD dwAccessType;
LPWSTR lpszProxy;
LPWSTR lpszProxyBypass;
}
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

typedef addrinfo ADDRINFOA
;

struct addrinfo
{
int ai_flags;
int ai_family;
int ai_socktype;
int ai_protocol;
size_t ai_addrlen;
char *ai_canonname;
sockaddr *ai_addr;
addrinfo *ai_next;
}
;

typedef unsigned __int64 size_t
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

struct _FILETIME
{
DWORD dwLowDateTime;
DWORD dwHighDateTime;
}
;

struct _CERT_CHAIN_PARA
{
DWORD cbSize;
CERT_USAGE_MATCH RequestedUsage;
}
;

typedef _CERT_USAGE_MATCH CERT_USAGE_MATCH
;

struct _CERT_USAGE_MATCH
{
DWORD dwType;
CERT_ENHKEY_USAGE Usage;
}
;

typedef _CTL_USAGE CERT_ENHKEY_USAGE
;

struct _CTL_USAGE
{
DWORD cUsageIdentifier;
LPSTR *rgpszUsageIdentifier;
}
;

struct _CERT_CHAIN_POLICY_PARA
{
DWORD cbSize;
DWORD dwFlags;
void *pvExtraPolicyPara;
}
;

struct _CERT_CHAIN_POLICY_STATUS
{
DWORD cbSize;
DWORD dwError;
LONG lChainIndex;
LONG lElementIndex;
void *pvExtraPolicyStatus;
}
;

struct _OSVERSIONINFOEXA
{
DWORD dwOSVersionInfoSize;
DWORD dwMajorVersion;
DWORD dwMinorVersion;
DWORD dwBuildNumber;
DWORD dwPlatformId;
CHAR szCSDVersion[128];
WORD wServicePackMajor;
WORD wServicePackMinor;
WORD wSuiteMask;
BYTE wProductType;
BYTE wReserved;
}
;

struct _CERT_CHAIN_ENGINE_CONFIG
{
DWORD cbSize;
HCERTSTORE hRestrictedRoot;
HCERTSTORE hRestrictedTrust;
HCERTSTORE hRestrictedOther;
DWORD cAdditionalStore;
HCERTSTORE *rghAdditionalStore;
DWORD dwFlags;
DWORD dwUrlRetrievalTimeout;
DWORD MaximumCachedCertificates;
DWORD CycleDetectionModulus;
HCERTSTORE hExclusiveRoot;
HCERTSTORE hExclusiveTrustedPeople;
}
;

typedef void *HCERTSTORE
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

typedef _RTL_CONDITION_VARIABLE RTL_CONDITION_VARIABLE
;

struct _RTL_CONDITION_VARIABLE
{
PVOID Ptr;
}
;

typedef void *PVOID
;

struct _SECURITY_ATTRIBUTES
{
DWORD nLength;
LPVOID lpSecurityDescriptor;
BOOL bInheritHandle;
}
;

struct DISPLAYCONFIG_DEVICE_INFO_HEADER
{
DISPLAYCONFIG_DEVICE_INFO_TYPE type;
UINT32 size;
LUID adapterId;
UINT32 id;
}
;

enum DISPLAYCONFIG_DEVICE_INFO_TYPE : __int32
{
DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME = 0x1,
DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME = 0x2,
DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_PREFERRED_MODE = 0x3,
DISPLAYCONFIG_DEVICE_INFO_GET_ADAPTER_NAME = 0x4,
DISPLAYCONFIG_DEVICE_INFO_SET_TARGET_PERSISTENCE = 0x5,
DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_BASE_TYPE = 0x6,
DISPLAYCONFIG_DEVICE_INFO_GET_SUPPORT_VIRTUAL_RESOLUTION = 0x7,
DISPLAYCONFIG_DEVICE_INFO_SET_SUPPORT_VIRTUAL_RESOLUTION = 0x8,
DISPLAYCONFIG_DEVICE_INFO_GET_ADVANCED_COLOR_INFO = 0x9,
DISPLAYCONFIG_DEVICE_INFO_SET_ADVANCED_COLOR_STATE = 0xA,
DISPLAYCONFIG_DEVICE_INFO_GET_SDR_WHITE_LEVEL = 0xB,
DISPLAYCONFIG_DEVICE_INFO_FORCE_UINT32 = 0xFFFFFFFF,
}
;

typedef DISPLAYCONFIG_DEVICE_INFO_TYPE $1A43D328328C3E5918F37A985A65BBFE
;

typedef unsigned int UINT32
;

typedef _LUID LUID
;

struct _LUID
{
DWORD LowPart;
LONG HighPart;
}
;

typedef GUID IID
;

typedef _XINPUT_STATE XINPUT_STATE
;

struct _XINPUT_STATE
{
DWORD dwPacketNumber;
XINPUT_GAMEPAD Gamepad;
}
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

typedef _XINPUT_VIBRATION XINPUT_VIBRATION
;

struct _XINPUT_VIBRATION
{
WORD wLeftMotorSpeed;
WORD wRightMotorSpeed;
}
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

struct tagRECT
{
LONG left;
LONG top;
LONG right;
LONG bottom;
}
;

struct tagPOINT
{
LONG x;
LONG y;
}
;

struct _STARTUPINFOA
{
DWORD cb;
LPSTR lpReserved;
LPSTR lpDesktop;
LPSTR lpTitle;
DWORD dwX;
DWORD dwY;
DWORD dwXSize;
DWORD dwYSize;
DWORD dwXCountChars;
DWORD dwYCountChars;
DWORD dwFillAttribute;
DWORD dwFlags;
WORD wShowWindow;
WORD cbReserved2;
LPBYTE lpReserved2;
HANDLE hStdInput;
HANDLE hStdOutput;
HANDLE hStdError;
}
;

typedef BYTE *LPBYTE
;

struct _PROCESS_INFORMATION
{
HANDLE hProcess;
HANDLE hThread;
DWORD dwProcessId;
DWORD dwThreadId;
}
;

typedef tagWNDCLASSEXW WNDCLASSEXW
;

struct tagWNDCLASSEXW
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
LPCWSTR lpszMenuName;
LPCWSTR lpszClassName;
HICON hIconSm;
}
;

typedef unsigned int UINT
;

typedef LRESULT (__stdcall *WNDPROC)(HWND, UINT, WPARAM, LPARAM)
;

typedef HWND__ *HWND
;

struct HWND__
{
int unused;
}
;

typedef UINT_PTR WPARAM
;

typedef LONG_PTR LPARAM
;

typedef __int64 LONG_PTR
;

typedef LONG_PTR LRESULT
;

typedef HINSTANCE__ *HINSTANCE
;

struct HINSTANCE__
{
int unused;
}
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

typedef const CHAR *LPCSTR
;

typedef tagRAWINPUTDEVICE RAWINPUTDEVICE
;

struct tagRAWINPUTDEVICE
{
USHORT usUsagePage;
USHORT usUsage;
DWORD dwFlags;
HWND hwndTarget;
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

typedef tagPOINT POINT
;

typedef tagWINDOWPLACEMENT WINDOWPLACEMENT
;

struct tagWINDOWPLACEMENT
{
UINT length;
UINT flags;
UINT showCmd;
POINT ptMinPosition;
POINT ptMaxPosition;
RECT rcNormalPosition;
}
;

typedef tagRECT RECT
;

struct tagPAINTSTRUCT
{
HDC hdc;
BOOL fErase;
RECT rcPaint;
BOOL fRestore;
BOOL fIncUpdate;
BYTE rgbReserved[32];
}
;

typedef HDC__ *HDC
;

struct HDC__
{
int unused;
}
;

struct tagRAWINPUT
{
RAWINPUTHEADER header;
tagRAWINPUT::$B145025D4CA0B4EFAFBBD6E93F901CD9 data;
}
;

typedef tagRAWINPUTHEADER RAWINPUTHEADER
;

struct tagRAWINPUTHEADER
{
DWORD dwType;
DWORD dwSize;
HANDLE hDevice;
WPARAM wParam;
}
;

union tagRAWINPUT::$B145025D4CA0B4EFAFBBD6E93F901CD9
{
RAWMOUSE mouse;
RAWKEYBOARD keyboard;
RAWHID hid;
}
;

typedef tagRAWMOUSE RAWMOUSE
;

struct tagRAWMOUSE
{
USHORT usFlags;
union
{
ULONG ulButtons;
struct
{
USHORT usButtonFlags;
USHORT usButtonData;
};
};
ULONG ulRawButtons;
LONG lLastX;
LONG lLastY;
ULONG ulExtraInformation;
}
;

union tagRAWMOUSE::$BAD619BF849CA7670BF9DAFAE180B654
{
ULONG ulButtons;
struct
{
USHORT usButtonFlags;
USHORT usButtonData;
};
}
;

typedef unsigned int ULONG
;

struct tagRAWMOUSE::$BAD619BF849CA7670BF9DAFAE180B654::$3E7396A53A7735AC40BFDACB443E8DAA
{
USHORT usButtonFlags;
USHORT usButtonData;
}
;

typedef tagRAWKEYBOARD RAWKEYBOARD
;

struct tagRAWKEYBOARD
{
USHORT MakeCode;
USHORT Flags;
USHORT Reserved;
USHORT VKey;
UINT Message;
ULONG ExtraInformation;
}
;

typedef tagRAWHID RAWHID
;

struct tagRAWHID
{
DWORD dwSizeHid;
DWORD dwCount;
BYTE bRawData[1];
}
;

struct _HIDP_CAPS
{
USAGE Usage;
USAGE UsagePage;
USHORT InputReportByteLength;
USHORT OutputReportByteLength;
USHORT FeatureReportByteLength;
USHORT Reserved[17];
USHORT NumberLinkCollectionNodes;
USHORT NumberInputButtonCaps;
USHORT NumberInputValueCaps;
USHORT NumberInputDataIndices;
USHORT NumberOutputButtonCaps;
USHORT NumberOutputValueCaps;
USHORT NumberOutputDataIndices;
USHORT NumberFeatureButtonCaps;
USHORT NumberFeatureValueCaps;
USHORT NumberFeatureDataIndices;
}
;

typedef USHORT USAGE
;

struct _SP_DEVICE_INTERFACE_DATA
{
DWORD cbSize;
GUID InterfaceClassGuid;
DWORD Flags;
ULONG_PTR Reserved;
}
;

struct _HIDD_ATTRIBUTES
{
ULONG Size;
USHORT VendorID;
USHORT ProductID;
USHORT VersionNumber;
}
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
union
{
XMM_SAVE_AREA32 FltSave;
struct
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
};
};
M128A VectorRegister[26];
DWORD64 VectorControl;
DWORD64 DebugControl;
DWORD64 LastBranchToRip;
DWORD64 LastBranchFromRip;
DWORD64 LastExceptionToRip;
DWORD64 LastExceptionFromRip;
}
;

typedef unsigned __int64 DWORD64
;

union _CONTEXT::$D2ECA93702C646ACAFACD524BE9E8FEB
{
XMM_SAVE_AREA32 FltSave;
struct
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
};
}
;

typedef XSAVE_FORMAT XMM_SAVE_AREA32
;

typedef _XSAVE_FORMAT XSAVE_FORMAT
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

typedef _M128A M128A
;

struct __declspec(align(16)) _M128A
{
ULONGLONG Low;
LONGLONG High;
}
;

struct _CONTEXT::$D2ECA93702C646ACAFACD524BE9E8FEB::$897D11C01F73F7E79A06B0B9ED9B9414
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

struct _EXCEPTION_POINTERS
{
PEXCEPTION_RECORD ExceptionRecord;
PCONTEXT ContextRecord;
}
;

typedef EXCEPTION_RECORD *PEXCEPTION_RECORD
;

typedef _EXCEPTION_RECORD EXCEPTION_RECORD
;

struct _EXCEPTION_RECORD
{
DWORD ExceptionCode;
DWORD ExceptionFlags;
_EXCEPTION_RECORD *ExceptionRecord;
PVOID ExceptionAddress;
DWORD NumberParameters;
ULONG_PTR ExceptionInformation[15];
}
;

typedef _CONTEXT *PCONTEXT
;

struct _STARTUPINFOW
{
DWORD cb;
LPWSTR lpReserved;
LPWSTR lpDesktop;
LPWSTR lpTitle;
DWORD dwX;
DWORD dwY;
DWORD dwXSize;
DWORD dwYSize;
DWORD dwXCountChars;
DWORD dwYCountChars;
DWORD dwFillAttribute;
DWORD dwFlags;
WORD wShowWindow;
WORD cbReserved2;
LPBYTE lpReserved2;
HANDLE hStdInput;
HANDLE hStdOutput;
HANDLE hStdError;
}
;

typedef _FILETIME FILETIME
;

struct _WIN32_FIND_DATAW
{
DWORD dwFileAttributes;
FILETIME ftCreationTime;
FILETIME ftLastAccessTime;
FILETIME ftLastWriteTime;
DWORD nFileSizeHigh;
DWORD nFileSizeLow;
DWORD dwReserved0;
DWORD dwReserved1;
WCHAR cFileName[260];
WCHAR cAlternateFileName[14];
}
;

struct _cpinfo
{
UINT MaxCharSize;
BYTE DefaultChar[2];
BYTE LeadByte[12];
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

struct __crt_locale_pointers
{
struct __crt_locale_data *locinfo;
struct __crt_multibyte_data *mbcinfo;
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

struct hks::BytecodeWriter
;

struct bdNetImpl
;

struct bdPendingBufferTransfer
;

struct bdUCD
;

struct hks::SimpleCompilerState
;

struct bdDataChunk
;

struct dwQoSMultiProbeListener
;

struct hks::MemoryAllocationsByType
;

struct bdQoSReplyPacket
;

struct hks
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

struct hks::Visitor
;

struct hks::CompilerReader
;

struct hks::CompilerLexer
;

struct hks::CodeGenerator
;

struct hks::HashTable
;

struct hks::GarbageCollector
;

struct hks::Visitor::Node
;

struct hks::BytecodeReader
;

struct hks::CallStack
;

struct hks::Serializer
;

struct hks::StructInst
;

struct hks::StructUtil
;

struct hks::BytecodeReader::Reader
;

struct hks::Preprocessor::PreprocessorStateProxy
;

struct hks::StringTable
;

struct hks::Parser
;

struct hks::StringPinner
;

struct hks::GarbageInspector
;

struct hks::MemoryBuffer
;

struct bdSocketRouter
;

struct bdDTLSAssociation
;

struct bdDTLSCookieAck
;

struct CustomSearchInfo_FindPathAlien
;

struct LargeLocal
;

struct SentientHandle
;

struct bdHashSHA256
;

struct bdReferencable
;

struct bdLobbyService
;

struct bdFileInfo
;

struct bdContextUserStorageFileInfo
;

struct pathnode_t
;

struct bdUnicastConnection
;

struct bdStorage
;

struct bdInetAddr
;

struct bdAddr
;

struct bdVerifyString
;

struct overlappedTask
;

struct bdTimeStamp
;

struct bdContentStreaming
;

struct bdHTTPWrapper
;

struct bdMessage
;

struct bdDTLSCookieEcho
;

struct bdRemoteTask
;

struct MatchMakingInfo
;

struct bdSecurityKeyMap
;

struct CacheLbTaskData
;

struct bdFileData
;

struct bdUserAccountID
;

struct MemoryHashInfo
;

struct bdAntiCheatResponses
;

struct bdStopwatch
;

struct bdGetHostByName
;

struct bdIPDiscoveryPacketReply
;

struct bdQoSRequestPacket
;

struct bdAddrHandle
;

struct bdCypher3Des
;

struct bdUserInfo
;

struct bdByteBuffer
;

struct bdGroupCount
;

struct bdTag
;

struct bdSecurityKey
;

struct bdCommonAddr
;

struct bdSecurityID
;

struct bdEndpoint
;

struct bdEndpointHashingClass
;

struct bdRandom
;

struct bdBitBuffer
;

struct bdHashTiger192
;

struct bdNetStartParams
;

struct bdUPnPConfig
;

struct bdChannelInfo
;

struct bdGetHostByNameConfig
;

struct bdUPnP
;

struct bdUPnPDevice
;

struct bdSockAddr
;

struct bdDispatcher
;

struct bdQoSProbe
;

struct bdQoSProbe::bdQoSProbeEntryWrapper
;

struct bdQoSRemoteAddr
;

struct bdNATTypeDiscoveryConfig
;

struct bdNATTypeDiscoveryClient
;

struct bdProfileInfo
;

struct bdIPDiscoveryConfig
;

struct bdIPDiscoveryClient
;

struct bdDTLSError
;

struct bdDTLSInitAck
;

struct bdSAckChunk
;

struct bdNATTraversalPacket
;

struct bdNATTravListener
;

struct bdRunnable
;

struct bdServiceBandwidthArbitrator
;

struct bdNATTravClient::CachedTraversal
;

struct bdNATTravClient
;

struct bdNATTravClientData
;

struct bdPacketBuffer
;

struct bdPacketBufferInternal
;

struct bdDTLSInit
;

struct bdBandwidthTestClient
;

struct bdPacketInterceptor
;

struct bdDTLSHeader
;

struct bdTaskByteBuffer
;

struct bdCookie
;

struct bdNATTypeDiscoveryPacket
;

struct bdNATTypeDiscoveryPacketReply
;

struct bdIPDiscoveryPacket
;

struct bdDTLSData
;

struct PublicProfileInfo
;

struct bdStatsInfo
;

struct bdBandwidthTestResults
;

struct bdMatchMakingInfo
;

struct bdAntiCheatChallenges
;

struct bdSessionID
;

struct bdFileMetaData
;

struct bdPreCopyResult
;

struct bdLobbyEventHandler
;

struct bdStats
;

struct bdHTTPWrapperBase
;

struct bdLobbyConnectionListener
;

struct bdLobbyConnection
;

struct bdDMLInfo
;

struct bdSessionInvite
;

struct bdSummaryMetaHandler
;

struct bdCypherAES
;

struct bdAntiCheatChallenge
;

struct bdAntiCheatChallengeParam
;

struct bdString
;

struct bdSocket
;

struct bdVoteRank
;

struct GfxCmdBufState
;

struct bdTrulyRandomImpl
;

struct bdThread
;

struct bdStreamSocket
;

struct bdECCKey
;

struct bdSequenceNumber
;

struct bdSequenceNumberStore
;

struct bdHeartbeatAckChunk
;

struct bdHashMD5
;

struct bdHMacSHA256
;

struct bdSemaphore
;

struct bdHMacSHA1
;

struct std::exception
;

struct dxGeom
;

struct bdURL
;

struct bdUCDPublicKeyInfo
;

struct bdConnection
;

struct bdConnectionStore
;

struct bdPacket
;

struct bdSocketRouterConfig
;

struct bdConnectionStoreConfig
;

struct bdLoopbackConnection
;

struct bdConnectionStatistics
;

struct bdUnreliableSendWindow
;

struct bdUnreliableReceiveWindow
;

struct bdChunk
;

struct bdReliableSendWindow
;

struct bdReliableReceiveWindow
;

struct bdInitChunk
;

struct bdInitAckChunk
;

struct bdCookieEchoChunk
;

struct bdCookieAckChunk
;

struct bdHeartbeatChunk
;

struct bdShutdownChunk
;

struct bdShutdownAckChunk
;

struct bdShutdownCompleteChunk
;

struct bdAddrHandleRefWrapper
;

struct bdMutex
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

struct std::bad_alloc
;

struct type_info
;

struct std::bad_exception
;

struct EntHandle
;

struct dxGeomTransform
;

struct bdLegacyStatsInfo
;

struct bdQoSProbeListener
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

struct bdFeatureBanInfo
;

enum PMem_Source
{
PMEM_SOURCE_EXTERNAL = 0x0,
PMEM_SOURCE_DATABASE = 0x1,
PMEM_SOURCE_DEFAULT_LOW = 0x2,
PMEM_SOURCE_DEFAULT_HIGH = 0x3,
PMEM_SOURCE_MOVIE = 0x4,
PMEM_SOURCE_SCRIPT = 0x5,
PMEM_SOURCE_UNK5 = 0x5,
PMEM_SOURCE_UNK6 = 0x6,
PMEM_SOURCE_UNK7 = 0x7,
PMEM_SOURCE_UNK8 = 0x8,
PMEM_SOURCE_CUSTOMIZATION = 0x9,
}
;

struct PhysicalMemoryAllocation
{
const char *name;
char __pad0[16];
unsigned __int64 pos;
char __pad1[8];
}
;

struct PhysicalMemoryPrim
{
const char *name;
unsigned int allocListCount;
char __pad0[4];
unsigned __int8 *buf;
char __pad1[8];
int unk1;
char __pad2[4];
unsigned __int64 pos;
PhysicalMemoryAllocation allocList[32];
}
;

struct PhysicalMemory
{
PhysicalMemoryPrim prim[2];
}
;

enum PMem_Direction
{
PHYS_ALLOC_LOW = 0x0,
PHYS_ALLOC_HIGH = 0x1,
PHYS_ALLOC_COUNT = 0x2,
}
;

typedef struct _MEMORY_BASIC_INFORMATION *PMEMORY_BASIC_INFORMATION
;

struct _MEMORY_BASIC_INFORMATION
{
PVOID BaseAddress;
PVOID AllocationBase;
DWORD AllocationProtect;
SIZE_T RegionSize;
DWORD State;
DWORD Protect;
DWORD Type;
}
;

typedef const void *LPCVOID
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

typedef DWORD *LPDWORD
;

typedef struct _FILETIME *LPFILETIME
;

typedef unsigned __int64 uintptr_t
;

typedef size_t rsize_t
;

typedef DWORD LCID
;

typedef WORD *LPWORD
;

typedef const CHAR *LPCCH
;

typedef BOOL *LPBOOL
;

typedef struct _STARTUPINFOA *LPSTARTUPINFOA
;

typedef struct _PROCESS_INFORMATION *LPPROCESS_INFORMATION
;

typedef __crt_locale_pointers *_locale_t
;

typedef const WCHAR *PCNZWCH
;

typedef struct _CONSOLE_READCONSOLE_CONTROL *PCONSOLE_READCONSOLE_CONTROL
;

struct _CONSOLE_READCONSOLE_CONTROL
{
ULONG nLength;
ULONG nInitialChars;
ULONG dwCtrlWakeupMask;
ULONG dwControlKeyState;
}
;

typedef unsigned __int64 *PULONG64
;

typedef struct _OVERLAPPED *LPOVERLAPPED
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

typedef HINSTANCE HMODULE
;

typedef struct addrinfo *PADDRINFOA
;

typedef int *LPINT
;

typedef struct _SP_DEVICE_INTERFACE_DATA *PSP_DEVICE_INTERFACE_DATA
;

typedef struct _SP_DEVINFO_DATA *PSP_DEVINFO_DATA
;

struct _SP_DEVINFO_DATA
{
DWORD cbSize;
GUID ClassGuid;
DWORD DevInst;
ULONG_PTR Reserved;
}
;

typedef struct _HIDP_PREPARSED_DATA *PHIDP_PREPARSED_DATA
;

typedef struct HMENU__ *HMENU
;

struct HMENU__
{
int unused;
}
;

typedef int INT
;

typedef const WCHAR *LPCWCH
;

typedef PRTL_CRITICAL_SECTION LPCRITICAL_SECTION
;

typedef struct _RTL_CRITICAL_SECTION *PRTL_CRITICAL_SECTION
;

typedef PVOID PSECURITY_DESCRIPTOR
;

typedef RTL_CONDITION_VARIABLE *PCONDITION_VARIABLE
;

typedef const CHAR *PCSTR
;

typedef ULONG_PTR HCRYPTPROV
;

typedef ULONG_PTR HCRYPTHASH
;

typedef struct _CERT_CHAIN_PARA *PCERT_CHAIN_PARA
;

typedef const CERT_CONTEXT *PCCERT_CONTEXT
;

typedef struct _CERT_CONTEXT CERT_CONTEXT
;

struct _CERT_CONTEXT
{
DWORD dwCertEncodingType;
BYTE *pbCertEncoded;
DWORD cbCertEncoded;
PCERT_INFO pCertInfo;
HCERTSTORE hCertStore;
}
;

typedef struct _CERT_INFO *PCERT_INFO
;

struct _CERT_INFO
{
DWORD dwVersion;
CRYPT_INTEGER_BLOB SerialNumber;
CRYPT_ALGORITHM_IDENTIFIER SignatureAlgorithm;
CERT_NAME_BLOB Issuer;
FILETIME NotBefore;
FILETIME NotAfter;
CERT_NAME_BLOB Subject;
CERT_PUBLIC_KEY_INFO SubjectPublicKeyInfo;
CRYPT_BIT_BLOB IssuerUniqueId;
CRYPT_BIT_BLOB SubjectUniqueId;
DWORD cExtension;
PCERT_EXTENSION rgExtension;
}
;

typedef struct _CRYPTOAPI_BLOB CRYPT_INTEGER_BLOB
;

typedef struct _CRYPT_ALGORITHM_IDENTIFIER CRYPT_ALGORITHM_IDENTIFIER
;

struct _CRYPT_ALGORITHM_IDENTIFIER
{
LPSTR pszObjId;
CRYPT_OBJID_BLOB Parameters;
}
;

typedef struct _CRYPTOAPI_BLOB CRYPT_OBJID_BLOB
;

typedef struct _CRYPTOAPI_BLOB CERT_NAME_BLOB
;

typedef struct _CERT_PUBLIC_KEY_INFO CERT_PUBLIC_KEY_INFO
;

struct _CERT_PUBLIC_KEY_INFO
{
CRYPT_ALGORITHM_IDENTIFIER Algorithm;
CRYPT_BIT_BLOB PublicKey;
}
;

typedef struct _CRYPT_BIT_BLOB CRYPT_BIT_BLOB
;

struct _CRYPT_BIT_BLOB
{
DWORD cbData;
BYTE *pbData;
DWORD cUnusedBits;
}
;

typedef struct _CERT_EXTENSION *PCERT_EXTENSION
;

struct _CERT_EXTENSION
{
LPSTR pszObjId;
BOOL fCritical;
CRYPT_OBJID_BLOB Value;
}
;

typedef const CERT_CHAIN_CONTEXT *PCCERT_CHAIN_CONTEXT
;

typedef struct _CERT_CHAIN_CONTEXT CERT_CHAIN_CONTEXT
;

struct _CERT_CHAIN_CONTEXT
{
DWORD cbSize;
CERT_TRUST_STATUS TrustStatus;
DWORD cChain;
PCERT_SIMPLE_CHAIN *rgpChain;
DWORD cLowerQualityChainContext;
PCCERT_CHAIN_CONTEXT *rgpLowerQualityChainContext;
BOOL fHasRevocationFreshnessTime;
DWORD dwRevocationFreshnessTime;
DWORD dwCreateFlags;
GUID ChainId;
}
;

typedef struct _CERT_TRUST_STATUS CERT_TRUST_STATUS
;

struct _CERT_TRUST_STATUS
{
DWORD dwErrorStatus;
DWORD dwInfoStatus;
}
;

typedef struct _CERT_SIMPLE_CHAIN *PCERT_SIMPLE_CHAIN
;

struct _CERT_SIMPLE_CHAIN
{
DWORD cbSize;
CERT_TRUST_STATUS TrustStatus;
DWORD cElement;
PCERT_CHAIN_ELEMENT *rgpElement;
PCERT_TRUST_LIST_INFO pTrustListInfo;
BOOL fHasRevocationFreshnessTime;
DWORD dwRevocationFreshnessTime;
}
;

typedef struct _CERT_CHAIN_ELEMENT *PCERT_CHAIN_ELEMENT
;

struct _CERT_CHAIN_ELEMENT
{
DWORD cbSize;
PCCERT_CONTEXT pCertContext;
CERT_TRUST_STATUS TrustStatus;
PCERT_REVOCATION_INFO pRevocationInfo;
PCERT_ENHKEY_USAGE pIssuanceUsage;
PCERT_ENHKEY_USAGE pApplicationUsage;
LPCWSTR pwszExtendedErrorInfo;
}
;

typedef struct _CERT_REVOCATION_INFO *PCERT_REVOCATION_INFO
;

struct _CERT_REVOCATION_INFO
{
DWORD cbSize;
DWORD dwRevocationResult;
LPCSTR pszRevocationOid;
LPVOID pvOidSpecificInfo;
BOOL fHasFreshnessTime;
DWORD dwFreshnessTime;
PCERT_REVOCATION_CRL_INFO pCrlInfo;
}
;

typedef struct _CERT_REVOCATION_CRL_INFO *PCERT_REVOCATION_CRL_INFO
;

struct _CERT_REVOCATION_CRL_INFO
{
DWORD cbSize;
PCCRL_CONTEXT pBaseCrlContext;
PCCRL_CONTEXT pDeltaCrlContext;
PCRL_ENTRY pCrlEntry;
BOOL fDeltaCrlEntry;
}
;

typedef const CRL_CONTEXT *PCCRL_CONTEXT
;

typedef struct _CRL_CONTEXT CRL_CONTEXT
;

struct _CRL_CONTEXT
{
DWORD dwCertEncodingType;
BYTE *pbCrlEncoded;
DWORD cbCrlEncoded;
PCRL_INFO pCrlInfo;
HCERTSTORE hCertStore;
}
;

typedef struct _CRL_INFO *PCRL_INFO
;

struct _CRL_INFO
{
DWORD dwVersion;
CRYPT_ALGORITHM_IDENTIFIER SignatureAlgorithm;
CERT_NAME_BLOB Issuer;
FILETIME ThisUpdate;
FILETIME NextUpdate;
DWORD cCRLEntry;
PCRL_ENTRY rgCRLEntry;
DWORD cExtension;
PCERT_EXTENSION rgExtension;
}
;

typedef struct _CRL_ENTRY *PCRL_ENTRY
;

struct _CRL_ENTRY
{
CRYPT_INTEGER_BLOB SerialNumber;
FILETIME RevocationDate;
DWORD cExtension;
PCERT_EXTENSION rgExtension;
}
;

typedef struct _CTL_USAGE *PCERT_ENHKEY_USAGE
;

typedef struct _CERT_TRUST_LIST_INFO *PCERT_TRUST_LIST_INFO
;

struct _CERT_TRUST_LIST_INFO
{
DWORD cbSize;
PCTL_ENTRY pCtlEntry;
PCCTL_CONTEXT pCtlContext;
}
;

typedef struct _CTL_ENTRY *PCTL_ENTRY
;

struct _CTL_ENTRY
{
CRYPT_DATA_BLOB SubjectIdentifier;
DWORD cAttribute;
PCRYPT_ATTRIBUTE rgAttribute;
}
;

typedef struct _CRYPTOAPI_BLOB CRYPT_DATA_BLOB
;

typedef struct _CRYPT_ATTRIBUTE *PCRYPT_ATTRIBUTE
;

struct _CRYPT_ATTRIBUTE
{
LPSTR pszObjId;
DWORD cValue;
PCRYPT_ATTR_BLOB rgValue;
}
;

typedef struct _CRYPTOAPI_BLOB *PCRYPT_ATTR_BLOB
;

typedef const CTL_CONTEXT *PCCTL_CONTEXT
;

typedef struct _CTL_CONTEXT CTL_CONTEXT
;

struct _CTL_CONTEXT
{
DWORD dwMsgAndCertEncodingType;
BYTE *pbCtlEncoded;
DWORD cbCtlEncoded;
PCTL_INFO pCtlInfo;
HCERTSTORE hCertStore;
HCRYPTMSG hCryptMsg;
BYTE *pbCtlContent;
DWORD cbCtlContent;
}
;

typedef struct _CTL_INFO *PCTL_INFO
;

struct _CTL_INFO
{
DWORD dwVersion;
CTL_USAGE SubjectUsage;
CRYPT_DATA_BLOB ListIdentifier;
CRYPT_INTEGER_BLOB SequenceNumber;
FILETIME ThisUpdate;
FILETIME NextUpdate;
CRYPT_ALGORITHM_IDENTIFIER SubjectAlgorithm;
DWORD cCTLEntry;
PCTL_ENTRY rgCTLEntry;
DWORD cExtension;
PCERT_EXTENSION rgExtension;
}
;

typedef struct _CTL_USAGE CTL_USAGE
;

typedef void *HCRYPTMSG
;

typedef HANDLE HCERTCHAINENGINE
;

typedef ULONG_PTR HCRYPTKEY
;

typedef unsigned int ALG_ID
;

typedef struct _CRYPT_DECODE_PARA *PCRYPT_DECODE_PARA
;

struct _CRYPT_DECODE_PARA
{
DWORD cbSize;
PFN_CRYPT_ALLOC pfnAlloc;
PFN_CRYPT_FREE pfnFree;
}
;

typedef LPVOID (__stdcall *PFN_CRYPT_ALLOC)(size_t cbSize)
;

typedef void (__stdcall *PFN_CRYPT_FREE)(LPVOID pv)
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

typedef ACCESS_MASK REGSAM
;

typedef DWORD ACCESS_MASK
;

typedef struct _SECURITY_ATTRIBUTES *LPSECURITY_ATTRIBUTES
;

typedef HANDLE HLOCAL
;

