struct _GUID
{
unsigned int Data1;
unsigned __int16 Data2;
unsigned __int16 Data3;
char Data4[8];
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

typedef wchar_t WCHAR
;

typedef unsigned __int16 wchar_t
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

typedef _CRYPTOAPI_BLOB DATA_BLOB
;

struct _CRYPTOAPI_BLOB
{
DWORD cbData;
BYTE *pbData;
}
;

typedef unsigned __int8 BYTE
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

typedef CHAR *LPSTR
;

typedef char CHAR
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

enum __shex INTERNET_SCHEME : __int32
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
INTERNET_SCHEME_PARTIAL = -0x2,
INTERNET_SCHEME_UNKNOWN = -0x1,
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

typedef _RTL_CONDITION_VARIABLE RTL_CONDITION_VARIABLE
;

struct _RTL_CONDITION_VARIABLE
{
PVOID Ptr;
}
;

typedef void *PVOID
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

struct _SECURITY_ATTRIBUTES
{
DWORD nLength;
LPVOID lpSecurityDescriptor;
BOOL bInheritHandle;
}
;

struct tagRECT
{
LONG left;
LONG top;
LONG right;
LONG bottom;
}
;

typedef _RTL_SRWLOCK RTL_SRWLOCK
;

struct _RTL_SRWLOCK
{
PVOID Ptr;
}
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

typedef _FILETIME FILETIME
;

struct _MODULEINFO
{
LPVOID lpBaseOfDll;
DWORD SizeOfImage;
LPVOID EntryPoint;
}
;

struct _PERFORMANCE_INFORMATION
{
DWORD cb;
SIZE_T CommitTotal;
SIZE_T CommitLimit;
SIZE_T CommitPeak;
SIZE_T PhysicalTotal;
SIZE_T PhysicalAvailable;
SIZE_T SystemCache;
SIZE_T KernelTotal;
SIZE_T KernelPaged;
SIZE_T KernelNonpaged;
SIZE_T PageSize;
DWORD HandleCount;
DWORD ProcessCount;
DWORD ThreadCount;
}
;

typedef ULONG_PTR SIZE_T
;

typedef _PROCESS_MEMORY_COUNTERS PROCESS_MEMORY_COUNTERS
;

struct _PROCESS_MEMORY_COUNTERS
{
DWORD cb;
DWORD PageFaultCount;
SIZE_T PeakWorkingSetSize;
SIZE_T WorkingSetSize;
SIZE_T QuotaPeakPagedPoolUsage;
SIZE_T QuotaPagedPoolUsage;
SIZE_T QuotaPeakNonPagedPoolUsage;
SIZE_T QuotaNonPagedPoolUsage;
SIZE_T PagefileUsage;
SIZE_T PeakPagefileUsage;
}
;

typedef tagPROCESSENTRY32 PROCESSENTRY32
;

struct tagPROCESSENTRY32
{
DWORD dwSize;
DWORD cntUsage;
DWORD th32ProcessID;
ULONG_PTR th32DefaultHeapID;
DWORD th32ModuleID;
DWORD cntThreads;
DWORD th32ParentProcessID;
LONG pcPriClassBase;
DWORD dwFlags;
CHAR szExeFile[260];
}
;

typedef tagTHREADENTRY32 THREADENTRY32
;

struct tagTHREADENTRY32
{
DWORD dwSize;
DWORD cntUsage;
DWORD th32ThreadID;
DWORD th32OwnerProcessID;
LONG tpBasePri;
LONG tpDeltaPri;
DWORD dwFlags;
}
;

struct _NOTIFYICONDATAA
{
DWORD cbSize;
HWND hWnd;
UINT uID;
UINT uFlags;
UINT uCallbackMessage;
HICON hIcon;
CHAR szTip[128];
DWORD dwState;
DWORD dwStateMask;
CHAR szInfo[256];
union
{
UINT uTimeout;
UINT uVersion;
};
CHAR szInfoTitle[64];
DWORD dwInfoFlags;
GUID guidItem;
HICON hBalloonIcon;
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

typedef HICON__ *HICON
;

struct HICON__
{
int unused;
}
;

union _NOTIFYICONDATAA::$2D483C7EC7880DB522C635EA0E00C909
{
UINT uTimeout;
UINT uVersion;
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

typedef unsigned __int64 ULONGLONG
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

struct _LUID
{
DWORD LowPart;
LONG HighPart;
}
;

struct _TOKEN_PRIVILEGES
{
DWORD PrivilegeCount;
LUID_AND_ATTRIBUTES Privileges[1];
}
;

typedef _LUID_AND_ATTRIBUTES LUID_AND_ATTRIBUTES
;

struct _LUID_AND_ATTRIBUTES
{
LUID Luid;
DWORD Attributes;
}
;

typedef _LUID LUID
;

struct _MINIDUMP_EXCEPTION_INFORMATION
{
DWORD ThreadId;
PEXCEPTION_POINTERS ExceptionPointers;
BOOL ClientPointers;
}
;

typedef _EXCEPTION_POINTERS *PEXCEPTION_POINTERS
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

struct _MINIDUMP_CALLBACK_INFORMATION
{
MINIDUMP_CALLBACK_ROUTINE CallbackRoutine;
PVOID CallbackParam;
}
;

typedef BOOL (__stdcall *MINIDUMP_CALLBACK_ROUTINE)(PVOID CallbackParam, PMINIDUMP_CALLBACK_INPUT CallbackInput, PMINIDUMP_CALLBACK_OUTPUT CallbackOutput)
;

typedef _MINIDUMP_CALLBACK_INPUT *PMINIDUMP_CALLBACK_INPUT
;

struct _MINIDUMP_CALLBACK_INPUT
{
ULONG ProcessId;
HANDLE ProcessHandle;
ULONG CallbackType;
union
{
HRESULT Status;
MINIDUMP_THREAD_CALLBACK Thread;
MINIDUMP_THREAD_EX_CALLBACK ThreadEx;
MINIDUMP_MODULE_CALLBACK Module;
MINIDUMP_INCLUDE_THREAD_CALLBACK IncludeThread;
MINIDUMP_INCLUDE_MODULE_CALLBACK IncludeModule;
MINIDUMP_IO_CALLBACK Io;
MINIDUMP_READ_MEMORY_FAILURE_CALLBACK ReadMemoryFailure;
ULONG SecondaryFlags;
MINIDUMP_VM_QUERY_CALLBACK VmQuery;
MINIDUMP_VM_PRE_READ_CALLBACK VmPreRead;
MINIDUMP_VM_POST_READ_CALLBACK VmPostRead;
};
}
;

typedef unsigned int ULONG
;

union _MINIDUMP_CALLBACK_INPUT::$209AEB699CFB7D95B8CC4C392D5365D9
{
HRESULT Status;
MINIDUMP_THREAD_CALLBACK Thread;
MINIDUMP_THREAD_EX_CALLBACK ThreadEx;
MINIDUMP_MODULE_CALLBACK Module;
MINIDUMP_INCLUDE_THREAD_CALLBACK IncludeThread;
MINIDUMP_INCLUDE_MODULE_CALLBACK IncludeModule;
MINIDUMP_IO_CALLBACK Io;
MINIDUMP_READ_MEMORY_FAILURE_CALLBACK ReadMemoryFailure;
ULONG SecondaryFlags;
MINIDUMP_VM_QUERY_CALLBACK VmQuery;
MINIDUMP_VM_PRE_READ_CALLBACK VmPreRead;
MINIDUMP_VM_POST_READ_CALLBACK VmPostRead;
}
;

typedef int HRESULT
;

typedef _MINIDUMP_THREAD_CALLBACK MINIDUMP_THREAD_CALLBACK
;

struct _MINIDUMP_THREAD_CALLBACK
{
ULONG ThreadId;
HANDLE ThreadHandle;
CONTEXT Context;
ULONG SizeOfContext;
ULONG64 StackBase;
ULONG64 StackEnd;
}
;

typedef _CONTEXT CONTEXT
;

typedef unsigned __int64 ULONG64
;

typedef _MINIDUMP_THREAD_EX_CALLBACK MINIDUMP_THREAD_EX_CALLBACK
;

struct _MINIDUMP_THREAD_EX_CALLBACK
{
ULONG ThreadId;
HANDLE ThreadHandle;
CONTEXT Context;
ULONG SizeOfContext;
ULONG64 StackBase;
ULONG64 StackEnd;
ULONG64 BackingStoreBase;
ULONG64 BackingStoreEnd;
}
;

typedef _MINIDUMP_MODULE_CALLBACK MINIDUMP_MODULE_CALLBACK
;

struct _MINIDUMP_MODULE_CALLBACK
{
PWCHAR FullPath;
ULONG64 BaseOfImage;
ULONG SizeOfImage;
ULONG CheckSum;
ULONG TimeDateStamp;
VS_FIXEDFILEINFO VersionInfo;
PVOID CvRecord;
ULONG SizeOfCvRecord;
PVOID MiscRecord;
ULONG SizeOfMiscRecord;
}
;

typedef WCHAR *PWCHAR
;

typedef tagVS_FIXEDFILEINFO VS_FIXEDFILEINFO
;

struct tagVS_FIXEDFILEINFO
{
DWORD dwSignature;
DWORD dwStrucVersion;
DWORD dwFileVersionMS;
DWORD dwFileVersionLS;
DWORD dwProductVersionMS;
DWORD dwProductVersionLS;
DWORD dwFileFlagsMask;
DWORD dwFileFlags;
DWORD dwFileOS;
DWORD dwFileType;
DWORD dwFileSubtype;
DWORD dwFileDateMS;
DWORD dwFileDateLS;
}
;

typedef _MINIDUMP_INCLUDE_THREAD_CALLBACK MINIDUMP_INCLUDE_THREAD_CALLBACK
;

struct _MINIDUMP_INCLUDE_THREAD_CALLBACK
{
ULONG ThreadId;
}
;

typedef _MINIDUMP_INCLUDE_MODULE_CALLBACK MINIDUMP_INCLUDE_MODULE_CALLBACK
;

struct _MINIDUMP_INCLUDE_MODULE_CALLBACK
{
ULONG64 BaseOfImage;
}
;

typedef _MINIDUMP_IO_CALLBACK MINIDUMP_IO_CALLBACK
;

struct _MINIDUMP_IO_CALLBACK
{
HANDLE Handle;
ULONG64 Offset;
PVOID Buffer;
ULONG BufferBytes;
}
;

typedef _MINIDUMP_READ_MEMORY_FAILURE_CALLBACK MINIDUMP_READ_MEMORY_FAILURE_CALLBACK
;

struct _MINIDUMP_READ_MEMORY_FAILURE_CALLBACK
{
ULONG64 Offset;
ULONG Bytes;
HRESULT FailureStatus;
}
;

typedef _MINIDUMP_VM_QUERY_CALLBACK MINIDUMP_VM_QUERY_CALLBACK
;

struct _MINIDUMP_VM_QUERY_CALLBACK
{
ULONG64 Offset;
}
;

typedef _MINIDUMP_VM_PRE_READ_CALLBACK MINIDUMP_VM_PRE_READ_CALLBACK
;

struct _MINIDUMP_VM_PRE_READ_CALLBACK
{
ULONG64 Offset;
PVOID Buffer;
ULONG Size;
}
;

typedef _MINIDUMP_VM_POST_READ_CALLBACK MINIDUMP_VM_POST_READ_CALLBACK
;

struct _MINIDUMP_VM_POST_READ_CALLBACK
{
ULONG64 Offset;
PVOID Buffer;
ULONG Size;
ULONG Completed;
HRESULT Status;
}
;

typedef _MINIDUMP_CALLBACK_OUTPUT *PMINIDUMP_CALLBACK_OUTPUT
;

struct _MINIDUMP_CALLBACK_OUTPUT
{
union
{
ULONG ModuleWriteFlags;
ULONG ThreadWriteFlags;
ULONG SecondaryFlags;
struct
{
ULONG64 MemoryBase;
ULONG MemorySize;
};
struct
{
BOOL CheckCancel;
BOOL Cancel;
};
HANDLE Handle;
struct
{
MINIDUMP_MEMORY_INFO VmRegion;
BOOL Continue;
};
struct
{
HRESULT VmQueryStatus;
MINIDUMP_MEMORY_INFO VmQueryResult;
};
struct
{
HRESULT VmReadStatus;
ULONG VmReadBytesCompleted;
};
HRESULT Status;
};
}
;

union _MINIDUMP_CALLBACK_OUTPUT::$C9A0632DE137089B71B06FD3ADD2A485
{
ULONG ModuleWriteFlags;
ULONG ThreadWriteFlags;
ULONG SecondaryFlags;
struct
{
ULONG64 MemoryBase;
ULONG MemorySize;
};
struct
{
BOOL CheckCancel;
BOOL Cancel;
};
HANDLE Handle;
struct
{
MINIDUMP_MEMORY_INFO VmRegion;
BOOL Continue;
};
struct
{
HRESULT VmQueryStatus;
MINIDUMP_MEMORY_INFO VmQueryResult;
};
struct
{
HRESULT VmReadStatus;
ULONG VmReadBytesCompleted;
};
HRESULT Status;
}
;

struct _MINIDUMP_CALLBACK_OUTPUT::$C9A0632DE137089B71B06FD3ADD2A485::$B73842D2487355742032524E6C1F616F
{
ULONG64 MemoryBase;
ULONG MemorySize;
}
;

struct _MINIDUMP_CALLBACK_OUTPUT::$C9A0632DE137089B71B06FD3ADD2A485::$6676F8A9360AC7EE7BF76CF86686A9C2
{
BOOL CheckCancel;
BOOL Cancel;
}
;

struct _MINIDUMP_CALLBACK_OUTPUT::$C9A0632DE137089B71B06FD3ADD2A485::$F8353D2A9E976112689B2133B2020A41
{
MINIDUMP_MEMORY_INFO VmRegion;
BOOL Continue;
}
;

typedef _MINIDUMP_MEMORY_INFO MINIDUMP_MEMORY_INFO
;

struct _MINIDUMP_MEMORY_INFO
{
ULONG64 BaseAddress;
ULONG64 AllocationBase;
ULONG32 AllocationProtect;
ULONG32 __alignment1;
ULONG64 RegionSize;
ULONG32 State;
ULONG32 Protect;
ULONG32 Type;
ULONG32 __alignment2;
}
;

typedef unsigned int ULONG32
;

struct _MINIDUMP_CALLBACK_OUTPUT::$C9A0632DE137089B71B06FD3ADD2A485::$3080A4B1DC62317E643182FABDCFDC6E
{
HRESULT VmQueryStatus;
MINIDUMP_MEMORY_INFO VmQueryResult;
}
;

struct _MINIDUMP_CALLBACK_OUTPUT::$C9A0632DE137089B71B06FD3ADD2A485::$AB7449E124192D7CE79544B03D16812B
{
HRESULT VmReadStatus;
ULONG VmReadBytesCompleted;
}
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

struct _SYSTEM_LOGICAL_PROCESSOR_INFORMATION
{
ULONG_PTR ProcessorMask;
LOGICAL_PROCESSOR_RELATIONSHIP Relationship;
union
{
_SYSTEM_LOGICAL_PROCESSOR_INFORMATION::$D6931DE555EC91ED0741B0AFB854A317::$9A3D5550B6DEF9CE405BB00483386E67 ProcessorCore;
_SYSTEM_LOGICAL_PROCESSOR_INFORMATION::$D6931DE555EC91ED0741B0AFB854A317::$DB4434C0CE3788EF589018B7D8F8E412 NumaNode;
CACHE_DESCRIPTOR Cache;
ULONGLONG Reserved[2];
};
}
;

enum LOGICAL_PROCESSOR_RELATIONSHIP : __int32
{
RelationProcessorCore = 0x0,
RelationNumaNode = 0x1,
RelationCache = 0x2,
RelationProcessorPackage = 0x3,
RelationGroup = 0x4,
RelationAll = 0xFFFF,
}
;

typedef LOGICAL_PROCESSOR_RELATIONSHIP _LOGICAL_PROCESSOR_RELATIONSHIP
;

union _SYSTEM_LOGICAL_PROCESSOR_INFORMATION::$D6931DE555EC91ED0741B0AFB854A317
{
_SYSTEM_LOGICAL_PROCESSOR_INFORMATION::$D6931DE555EC91ED0741B0AFB854A317::$9A3D5550B6DEF9CE405BB00483386E67 ProcessorCore;
_SYSTEM_LOGICAL_PROCESSOR_INFORMATION::$D6931DE555EC91ED0741B0AFB854A317::$DB4434C0CE3788EF589018B7D8F8E412 NumaNode;
CACHE_DESCRIPTOR Cache;
ULONGLONG Reserved[2];
}
;

struct _SYSTEM_LOGICAL_PROCESSOR_INFORMATION::$D6931DE555EC91ED0741B0AFB854A317::$9A3D5550B6DEF9CE405BB00483386E67
{
BYTE Flags;
}
;

struct _SYSTEM_LOGICAL_PROCESSOR_INFORMATION::$D6931DE555EC91ED0741B0AFB854A317::$DB4434C0CE3788EF589018B7D8F8E412
{
DWORD NodeNumber;
}
;

typedef _CACHE_DESCRIPTOR CACHE_DESCRIPTOR
;

struct _CACHE_DESCRIPTOR
{
BYTE Level;
BYTE Associativity;
WORD LineSize;
DWORD Size;
PROCESSOR_CACHE_TYPE Type;
}
;

enum PROCESSOR_CACHE_TYPE : __int32
{
CacheUnified = 0x0,
CacheInstruction = 0x1,
CacheData = 0x2,
CacheTrace = 0x3,
}
;

typedef PROCESSOR_CACHE_TYPE _PROCESSOR_CACHE_TYPE
;

typedef _IP_ADAPTER_ADDRESSES_LH IP_ADAPTER_ADDRESSES_LH
;

struct _IP_ADAPTER_ADDRESSES_LH
{
union
{
ULONGLONG Alignment;
struct
{
ULONG Length;
IF_INDEX IfIndex;
};
};
_IP_ADAPTER_ADDRESSES_LH *Next;
PCHAR AdapterName;
PIP_ADAPTER_UNICAST_ADDRESS_LH FirstUnicastAddress;
PIP_ADAPTER_ANYCAST_ADDRESS_XP FirstAnycastAddress;
PIP_ADAPTER_MULTICAST_ADDRESS_XP FirstMulticastAddress;
PIP_ADAPTER_DNS_SERVER_ADDRESS_XP FirstDnsServerAddress;
PWCHAR DnsSuffix;
PWCHAR Description;
PWCHAR FriendlyName;
BYTE PhysicalAddress[8];
ULONG PhysicalAddressLength;
union
{
ULONG Flags;
struct
{
unsigned __int32 DdnsEnabled : 1;
unsigned __int32 RegisterAdapterSuffix : 1;
unsigned __int32 Dhcpv4Enabled : 1;
unsigned __int32 ReceiveOnly : 1;
unsigned __int32 NoMulticast : 1;
unsigned __int32 Ipv6OtherStatefulConfig : 1;
unsigned __int32 NetbiosOverTcpipEnabled : 1;
unsigned __int32 Ipv4Enabled : 1;
unsigned __int32 Ipv6Enabled : 1;
unsigned __int32 Ipv6ManagedAddressConfigurationSupported : 1;
};
};
ULONG Mtu;
IFTYPE IfType;
IF_OPER_STATUS OperStatus;
IF_INDEX Ipv6IfIndex;
ULONG ZoneIndices[16];
PIP_ADAPTER_PREFIX_XP FirstPrefix;
ULONG64 TransmitLinkSpeed;
ULONG64 ReceiveLinkSpeed;
PIP_ADAPTER_WINS_SERVER_ADDRESS_LH FirstWinsServerAddress;
PIP_ADAPTER_GATEWAY_ADDRESS_LH FirstGatewayAddress;
ULONG Ipv4Metric;
ULONG Ipv6Metric;
IF_LUID Luid;
SOCKET_ADDRESS Dhcpv4Server;
NET_IF_COMPARTMENT_ID CompartmentId;
NET_IF_NETWORK_GUID NetworkGuid;
NET_IF_CONNECTION_TYPE ConnectionType;
TUNNEL_TYPE TunnelType;
SOCKET_ADDRESS Dhcpv6Server;
BYTE Dhcpv6ClientDuid[130];
ULONG Dhcpv6ClientDuidLength;
ULONG Dhcpv6Iaid;
PIP_ADAPTER_DNS_SUFFIX FirstDnsSuffix;
}
;

union _IP_ADAPTER_ADDRESSES_LH::$88EAB68B38CC03D4ADFF2CEE83F7F421
{
ULONGLONG Alignment;
struct
{
ULONG Length;
IF_INDEX IfIndex;
};
}
;

struct _IP_ADAPTER_ADDRESSES_LH::$88EAB68B38CC03D4ADFF2CEE83F7F421::$540D896092981D96F2B14DC684EC72A2
{
ULONG Length;
IF_INDEX IfIndex;
}
;

typedef NET_IFINDEX IF_INDEX
;

typedef ULONG NET_IFINDEX
;

typedef CHAR *PCHAR
;

typedef _IP_ADAPTER_UNICAST_ADDRESS_LH *PIP_ADAPTER_UNICAST_ADDRESS_LH
;

struct _IP_ADAPTER_UNICAST_ADDRESS_LH
{
union
{
ULONGLONG Alignment;
struct
{
ULONG Length;
DWORD Flags;
};
};
_IP_ADAPTER_UNICAST_ADDRESS_LH *Next;
SOCKET_ADDRESS Address;
IP_PREFIX_ORIGIN PrefixOrigin;
IP_SUFFIX_ORIGIN SuffixOrigin;
IP_DAD_STATE DadState;
ULONG ValidLifetime;
ULONG PreferredLifetime;
ULONG LeaseLifetime;
UINT8 OnLinkPrefixLength;
}
;

union _IP_ADAPTER_UNICAST_ADDRESS_LH::$E412F09F4EF1DCB2A353111A4C8EAC81
{
ULONGLONG Alignment;
struct
{
ULONG Length;
DWORD Flags;
};
}
;

struct _IP_ADAPTER_UNICAST_ADDRESS_LH::$E412F09F4EF1DCB2A353111A4C8EAC81::$558C11AAB270A8DE5636A48AC52A55A4
{
ULONG Length;
DWORD Flags;
}
;

typedef _SOCKET_ADDRESS SOCKET_ADDRESS
;

struct _SOCKET_ADDRESS
{
LPSOCKADDR lpSockaddr;
INT iSockaddrLength;
}
;

typedef sockaddr *LPSOCKADDR
;

typedef int INT
;

typedef NL_PREFIX_ORIGIN IP_PREFIX_ORIGIN
;

typedef $81C96E3F196AC83D02B4522B31857D28 NL_PREFIX_ORIGIN
;

enum $81C96E3F196AC83D02B4522B31857D28
{
IpPrefixOriginOther = 0x0,
IpPrefixOriginManual = 0x1,
IpPrefixOriginWellKnown = 0x2,
IpPrefixOriginDhcp = 0x3,
IpPrefixOriginRouterAdvertisement = 0x4,
IpPrefixOriginUnchanged = 0x10,
}
;

typedef NL_SUFFIX_ORIGIN IP_SUFFIX_ORIGIN
;

typedef $4ADD18C59427D2D4505C2F6B781D5A2B NL_SUFFIX_ORIGIN
;

enum $4ADD18C59427D2D4505C2F6B781D5A2B
{
NlsoOther = 0x0,
NlsoManual = 0x1,
NlsoWellKnown = 0x2,
NlsoDhcp = 0x3,
NlsoLinkLayerAddress = 0x4,
NlsoRandom = 0x5,
IpSuffixOriginOther = 0x0,
IpSuffixOriginManual = 0x1,
IpSuffixOriginWellKnown = 0x2,
IpSuffixOriginDhcp = 0x3,
IpSuffixOriginLinkLayerAddress = 0x4,
IpSuffixOriginRandom = 0x5,
IpSuffixOriginUnchanged = 0x10,
}
;

typedef NL_DAD_STATE IP_DAD_STATE
;

typedef $06D1ED250FD368380D92731A8809DBAC NL_DAD_STATE
;

enum $06D1ED250FD368380D92731A8809DBAC
{
NldsInvalid = 0x0,
NldsTentative = 0x1,
NldsDuplicate = 0x2,
NldsDeprecated = 0x3,
NldsPreferred = 0x4,
IpDadStateInvalid = 0x0,
IpDadStateTentative = 0x1,
IpDadStateDuplicate = 0x2,
IpDadStateDeprecated = 0x3,
IpDadStatePreferred = 0x4,
}
;

typedef unsigned __int8 UINT8
;

typedef _IP_ADAPTER_ANYCAST_ADDRESS_XP *PIP_ADAPTER_ANYCAST_ADDRESS_XP
;

struct _IP_ADAPTER_ANYCAST_ADDRESS_XP
{
union
{
ULONGLONG Alignment;
struct
{
ULONG Length;
DWORD Flags;
};
};
_IP_ADAPTER_ANYCAST_ADDRESS_XP *Next;
SOCKET_ADDRESS Address;
}
;

union _IP_ADAPTER_ANYCAST_ADDRESS_XP::$27D98D245193571B98223D43E8FDA74A
{
ULONGLONG Alignment;
struct
{
ULONG Length;
DWORD Flags;
};
}
;

struct _IP_ADAPTER_ANYCAST_ADDRESS_XP::$27D98D245193571B98223D43E8FDA74A::$558C11AAB270A8DE5636A48AC52A55A4
{
ULONG Length;
DWORD Flags;
}
;

typedef _IP_ADAPTER_MULTICAST_ADDRESS_XP *PIP_ADAPTER_MULTICAST_ADDRESS_XP
;

struct _IP_ADAPTER_MULTICAST_ADDRESS_XP
{
union
{
ULONGLONG Alignment;
struct
{
ULONG Length;
DWORD Flags;
};
};
_IP_ADAPTER_MULTICAST_ADDRESS_XP *Next;
SOCKET_ADDRESS Address;
}
;

union _IP_ADAPTER_MULTICAST_ADDRESS_XP::$C661A151FC55CDAC9C76B7690C297B03
{
ULONGLONG Alignment;
struct
{
ULONG Length;
DWORD Flags;
};
}
;

struct _IP_ADAPTER_MULTICAST_ADDRESS_XP::$C661A151FC55CDAC9C76B7690C297B03::$558C11AAB270A8DE5636A48AC52A55A4
{
ULONG Length;
DWORD Flags;
}
;

typedef _IP_ADAPTER_DNS_SERVER_ADDRESS_XP *PIP_ADAPTER_DNS_SERVER_ADDRESS_XP
;

struct _IP_ADAPTER_DNS_SERVER_ADDRESS_XP
{
union
{
ULONGLONG Alignment;
struct
{
ULONG Length;
DWORD Reserved;
};
};
_IP_ADAPTER_DNS_SERVER_ADDRESS_XP *Next;
SOCKET_ADDRESS Address;
}
;

union _IP_ADAPTER_DNS_SERVER_ADDRESS_XP::$C726E6BF6F48850843A5B30ADC20782D
{
ULONGLONG Alignment;
struct
{
ULONG Length;
DWORD Reserved;
};
}
;

struct _IP_ADAPTER_DNS_SERVER_ADDRESS_XP::$C726E6BF6F48850843A5B30ADC20782D::$F985BF779D0F5FE11994F28A81731018
{
ULONG Length;
DWORD Reserved;
}
;

union _IP_ADAPTER_ADDRESSES_LH::$25CF9B0FF74BBFD763B030897BA4FF99
{
ULONG Flags;
struct
{
unsigned __int32 DdnsEnabled : 1;
unsigned __int32 RegisterAdapterSuffix : 1;
unsigned __int32 Dhcpv4Enabled : 1;
unsigned __int32 ReceiveOnly : 1;
unsigned __int32 NoMulticast : 1;
unsigned __int32 Ipv6OtherStatefulConfig : 1;
unsigned __int32 NetbiosOverTcpipEnabled : 1;
unsigned __int32 Ipv4Enabled : 1;
unsigned __int32 Ipv6Enabled : 1;
unsigned __int32 Ipv6ManagedAddressConfigurationSupported : 1;
};
}
;

struct _IP_ADAPTER_ADDRESSES_LH::$25CF9B0FF74BBFD763B030897BA4FF99::$E43178AF9457E50D9523A864F18E103E
{
unsigned __int32 DdnsEnabled : 1;
unsigned __int32 RegisterAdapterSuffix : 1;
unsigned __int32 Dhcpv4Enabled : 1;
unsigned __int32 ReceiveOnly : 1;
unsigned __int32 NoMulticast : 1;
unsigned __int32 Ipv6OtherStatefulConfig : 1;
unsigned __int32 NetbiosOverTcpipEnabled : 1;
unsigned __int32 Ipv4Enabled : 1;
unsigned __int32 Ipv6Enabled : 1;
unsigned __int32 Ipv6ManagedAddressConfigurationSupported : 1;
}
;

typedef ULONG IFTYPE
;

enum IF_OPER_STATUS : __int32
{
IfOperStatusUp = 0x1,
IfOperStatusDown = 0x2,
IfOperStatusTesting = 0x3,
IfOperStatusUnknown = 0x4,
IfOperStatusDormant = 0x5,
IfOperStatusNotPresent = 0x6,
IfOperStatusLowerLayerDown = 0x7,
}
;

typedef IF_OPER_STATUS $1D78DD8871C05E6FD7D3820402F49BC1
;

typedef _IP_ADAPTER_PREFIX_XP *PIP_ADAPTER_PREFIX_XP
;

struct _IP_ADAPTER_PREFIX_XP
{
union
{
ULONGLONG Alignment;
struct
{
ULONG Length;
DWORD Flags;
};
};
_IP_ADAPTER_PREFIX_XP *Next;
SOCKET_ADDRESS Address;
ULONG PrefixLength;
}
;

union _IP_ADAPTER_PREFIX_XP::$062DE21AAEE92240DFF2100A002A9516
{
ULONGLONG Alignment;
struct
{
ULONG Length;
DWORD Flags;
};
}
;

struct _IP_ADAPTER_PREFIX_XP::$062DE21AAEE92240DFF2100A002A9516::$558C11AAB270A8DE5636A48AC52A55A4
{
ULONG Length;
DWORD Flags;
}
;

typedef _IP_ADAPTER_WINS_SERVER_ADDRESS_LH *PIP_ADAPTER_WINS_SERVER_ADDRESS_LH
;

struct _IP_ADAPTER_WINS_SERVER_ADDRESS_LH
{
union
{
ULONGLONG Alignment;
struct
{
ULONG Length;
DWORD Reserved;
};
};
_IP_ADAPTER_WINS_SERVER_ADDRESS_LH *Next;
SOCKET_ADDRESS Address;
}
;

union _IP_ADAPTER_WINS_SERVER_ADDRESS_LH::$ED1413B78AFF918A573A0A81B51AB7FA
{
ULONGLONG Alignment;
struct
{
ULONG Length;
DWORD Reserved;
};
}
;

struct _IP_ADAPTER_WINS_SERVER_ADDRESS_LH::$ED1413B78AFF918A573A0A81B51AB7FA::$F985BF779D0F5FE11994F28A81731018
{
ULONG Length;
DWORD Reserved;
}
;

typedef _IP_ADAPTER_GATEWAY_ADDRESS_LH *PIP_ADAPTER_GATEWAY_ADDRESS_LH
;

struct _IP_ADAPTER_GATEWAY_ADDRESS_LH
{
union
{
ULONGLONG Alignment;
struct
{
ULONG Length;
DWORD Reserved;
};
};
_IP_ADAPTER_GATEWAY_ADDRESS_LH *Next;
SOCKET_ADDRESS Address;
}
;

union _IP_ADAPTER_GATEWAY_ADDRESS_LH::$4DEA9AA2BE4A87A26E25AF13B4F5640E
{
ULONGLONG Alignment;
struct
{
ULONG Length;
DWORD Reserved;
};
}
;

struct _IP_ADAPTER_GATEWAY_ADDRESS_LH::$4DEA9AA2BE4A87A26E25AF13B4F5640E::$F985BF779D0F5FE11994F28A81731018
{
ULONG Length;
DWORD Reserved;
}
;

typedef NET_LUID IF_LUID
;

typedef NET_LUID_LH NET_LUID
;

typedef _NET_LUID_LH NET_LUID_LH
;

union _NET_LUID_LH
{
ULONG64 Value;
_NET_LUID_LH::$0227A65EDE726BD8ABDBF310B0E65A75 Info;
}
;

struct _NET_LUID_LH::$0227A65EDE726BD8ABDBF310B0E65A75
{
unsigned __int64 Reserved : 24;
unsigned __int64 NetLuidIndex : 24;
unsigned __int64 IfType : 16;
}
;

typedef UINT32 NET_IF_COMPARTMENT_ID
;

typedef unsigned int UINT32
;

typedef GUID NET_IF_NETWORK_GUID
;

enum NET_IF_CONNECTION_TYPE : __int32
{
NET_IF_CONNECTION_DEDICATED = 0x1,
NET_IF_CONNECTION_PASSIVE = 0x2,
NET_IF_CONNECTION_DEMAND = 0x3,
NET_IF_CONNECTION_MAXIMUM = 0x4,
}
;

typedef NET_IF_CONNECTION_TYPE _NET_IF_CONNECTION_TYPE
;

enum TUNNEL_TYPE : __int32
{
TUNNEL_TYPE_NONE = 0x0,
TUNNEL_TYPE_OTHER = 0x1,
TUNNEL_TYPE_DIRECT = 0x2,
TUNNEL_TYPE_6TO4 = 0xB,
TUNNEL_TYPE_ISATAP = 0xD,
TUNNEL_TYPE_TEREDO = 0xE,
TUNNEL_TYPE_IPHTTPS = 0xF,
}
;

typedef TUNNEL_TYPE $E38CE7F1AC08E248910BE7D26668C82A
;

typedef _IP_ADAPTER_DNS_SUFFIX *PIP_ADAPTER_DNS_SUFFIX
;

struct _IP_ADAPTER_DNS_SUFFIX
{
_IP_ADAPTER_DNS_SUFFIX *Next;
WCHAR String[256];
}
;

typedef GUID IID
;

typedef VARIANT VARIANTARG
;

typedef tagVARIANT VARIANT
;

struct tagVARIANT
{
union
{
struct
{
VARTYPE vt;
WORD wReserved1;
WORD wReserved2;
WORD wReserved3;
union
{
LONGLONG llVal;
LONG lVal;
BYTE bVal;
SHORT iVal;
FLOAT fltVal;
DOUBLE dblVal;
VARIANT_BOOL boolVal;
SCODE scode;
CY cyVal;
DATE date;
BSTR bstrVal;
IUnknown *punkVal;
IDispatch *pdispVal;
SAFEARRAY *parray;
BYTE *pbVal;
SHORT *piVal;
LONG *plVal;
LONGLONG *pllVal;
FLOAT *pfltVal;
DOUBLE *pdblVal;
VARIANT_BOOL *pboolVal;
SCODE *pscode;
CY *pcyVal;
DATE *pdate;
BSTR *pbstrVal;
IUnknown **ppunkVal;
IDispatch **ppdispVal;
SAFEARRAY **pparray;
VARIANT *pvarVal;
PVOID byref;
CHAR cVal;
USHORT uiVal;
ULONG ulVal;
ULONGLONG ullVal;
INT intVal;
UINT uintVal;
DECIMAL *pdecVal;
CHAR *pcVal;
USHORT *puiVal;
ULONG *pulVal;
ULONGLONG *pullVal;
INT *pintVal;
UINT *puintVal;
struct
{
PVOID pvRecord;
IRecordInfo *pRecInfo;
};
};
};
DECIMAL decVal;
};
}
;

union tagVARIANT::$E93DC971A089CC95F6C875332324C1E7
{
struct
{
VARTYPE vt;
WORD wReserved1;
WORD wReserved2;
WORD wReserved3;
union
{
LONGLONG llVal;
LONG lVal;
BYTE bVal;
SHORT iVal;
FLOAT fltVal;
DOUBLE dblVal;
VARIANT_BOOL boolVal;
SCODE scode;
CY cyVal;
DATE date;
BSTR bstrVal;
IUnknown *punkVal;
IDispatch *pdispVal;
SAFEARRAY *parray;
BYTE *pbVal;
SHORT *piVal;
LONG *plVal;
LONGLONG *pllVal;
FLOAT *pfltVal;
DOUBLE *pdblVal;
VARIANT_BOOL *pboolVal;
SCODE *pscode;
CY *pcyVal;
DATE *pdate;
BSTR *pbstrVal;
IUnknown **ppunkVal;
IDispatch **ppdispVal;
SAFEARRAY **pparray;
VARIANT *pvarVal;
PVOID byref;
CHAR cVal;
USHORT uiVal;
ULONG ulVal;
ULONGLONG ullVal;
INT intVal;
UINT uintVal;
DECIMAL *pdecVal;
CHAR *pcVal;
USHORT *puiVal;
ULONG *pulVal;
ULONGLONG *pullVal;
INT *pintVal;
UINT *puintVal;
struct
{
PVOID pvRecord;
IRecordInfo *pRecInfo;
};
};
};
DECIMAL decVal;
}
;

struct tagVARIANT::$E93DC971A089CC95F6C875332324C1E7::$65D68C826D16CA47CF95571D7BFCD657
{
VARTYPE vt;
WORD wReserved1;
WORD wReserved2;
WORD wReserved3;
union
{
LONGLONG llVal;
LONG lVal;
BYTE bVal;
SHORT iVal;
FLOAT fltVal;
DOUBLE dblVal;
VARIANT_BOOL boolVal;
SCODE scode;
CY cyVal;
DATE date;
BSTR bstrVal;
IUnknown *punkVal;
IDispatch *pdispVal;
SAFEARRAY *parray;
BYTE *pbVal;
SHORT *piVal;
LONG *plVal;
LONGLONG *pllVal;
FLOAT *pfltVal;
DOUBLE *pdblVal;
VARIANT_BOOL *pboolVal;
SCODE *pscode;
CY *pcyVal;
DATE *pdate;
BSTR *pbstrVal;
IUnknown **ppunkVal;
IDispatch **ppdispVal;
SAFEARRAY **pparray;
VARIANT *pvarVal;
PVOID byref;
CHAR cVal;
USHORT uiVal;
ULONG ulVal;
ULONGLONG ullVal;
INT intVal;
UINT uintVal;
DECIMAL *pdecVal;
CHAR *pcVal;
USHORT *puiVal;
ULONG *pulVal;
ULONGLONG *pullVal;
INT *pintVal;
UINT *puintVal;
struct
{
PVOID pvRecord;
IRecordInfo *pRecInfo;
};
};
}
;

typedef unsigned __int16 VARTYPE
;

union tagVARIANT::$::$65D68C826D16CA47CF95571D7BFCD657::$E09503A454170B491AC1C4312CE36FE6
{
LONGLONG llVal;
LONG lVal;
BYTE bVal;
SHORT iVal;
FLOAT fltVal;
DOUBLE dblVal;
VARIANT_BOOL boolVal;
SCODE scode;
CY cyVal;
DATE date;
BSTR bstrVal;
IUnknown *punkVal;
IDispatch *pdispVal;
SAFEARRAY *parray;
BYTE *pbVal;
SHORT *piVal;
LONG *plVal;
LONGLONG *pllVal;
FLOAT *pfltVal;
DOUBLE *pdblVal;
VARIANT_BOOL *pboolVal;
SCODE *pscode;
CY *pcyVal;
DATE *pdate;
BSTR *pbstrVal;
IUnknown **ppunkVal;
IDispatch **ppdispVal;
SAFEARRAY **pparray;
VARIANT *pvarVal;
PVOID byref;
CHAR cVal;
USHORT uiVal;
ULONG ulVal;
ULONGLONG ullVal;
INT intVal;
UINT uintVal;
DECIMAL *pdecVal;
CHAR *pcVal;
USHORT *puiVal;
ULONG *pulVal;
ULONGLONG *pullVal;
INT *pintVal;
UINT *puintVal;
struct
{
PVOID pvRecord;
IRecordInfo *pRecInfo;
};
}
;

typedef __int16 SHORT
;

typedef float FLOAT
;

typedef double DOUBLE
;

typedef __int16 VARIANT_BOOL
;

typedef LONG SCODE
;

typedef tagCY CY
;

union tagCY
{
struct
{
ULONG Lo;
LONG Hi;
};
LONGLONG int64;
}
;

struct tagCY::$3EA7BC8C29B528C7CA1203FC489E132F
{
ULONG Lo;
LONG Hi;
}
;

typedef double DATE
;

typedef OLECHAR *BSTR
;

typedef WCHAR OLECHAR
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

struct IDispatch
{
IDispatchVtbl *lpVtbl;
}
;

struct IDispatchVtbl
{
HRESULT (__stdcall *QueryInterface)(IDispatch *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(IDispatch *This);
ULONG (__stdcall *Release)(IDispatch *This);
HRESULT (__stdcall *GetTypeInfoCount)(IDispatch *This, UINT *pctinfo);
HRESULT (__stdcall *GetTypeInfo)(IDispatch *This, UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
HRESULT (__stdcall *GetIDsOfNames)(IDispatch *This, const IID *const riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
HRESULT (__stdcall *Invoke)(IDispatch *This, DISPID dispIdMember, const IID *const riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);
}
;

typedef DWORD LCID
;

struct ITypeInfo
{
ITypeInfoVtbl *lpVtbl;
}
;

struct ITypeInfoVtbl
{
HRESULT (__stdcall *QueryInterface)(ITypeInfo *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ITypeInfo *This);
ULONG (__stdcall *Release)(ITypeInfo *This);
HRESULT (__stdcall *GetTypeAttr)(ITypeInfo *This, TYPEATTR **ppTypeAttr);
HRESULT (__stdcall *GetTypeComp)(ITypeInfo *This, ITypeComp **ppTComp);
HRESULT (__stdcall *GetFuncDesc)(ITypeInfo *This, UINT index, FUNCDESC **ppFuncDesc);
HRESULT (__stdcall *GetVarDesc)(ITypeInfo *This, UINT index, VARDESC **ppVarDesc);
HRESULT (__stdcall *GetNames)(ITypeInfo *This, MEMBERID memid, BSTR *rgBstrNames, UINT cMaxNames, UINT *pcNames);
HRESULT (__stdcall *GetRefTypeOfImplType)(ITypeInfo *This, UINT index, HREFTYPE *pRefType);
HRESULT (__stdcall *GetImplTypeFlags)(ITypeInfo *This, UINT index, INT *pImplTypeFlags);
HRESULT (__stdcall *GetIDsOfNames)(ITypeInfo *This, LPOLESTR *rgszNames, UINT cNames, MEMBERID *pMemId);
HRESULT (__stdcall *Invoke)(ITypeInfo *This, PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);
HRESULT (__stdcall *GetDocumentation)(ITypeInfo *This, MEMBERID memid, BSTR *pBstrName, BSTR *pBstrDocString, DWORD *pdwHelpContext, BSTR *pBstrHelpFile);
HRESULT (__stdcall *GetDllEntry)(ITypeInfo *This, MEMBERID memid, INVOKEKIND invKind, BSTR *pBstrDllName, BSTR *pBstrName, WORD *pwOrdinal);
HRESULT (__stdcall *GetRefTypeInfo)(ITypeInfo *This, HREFTYPE hRefType, ITypeInfo **ppTInfo);
HRESULT (__stdcall *AddressOfMember)(ITypeInfo *This, MEMBERID memid, INVOKEKIND invKind, PVOID *ppv);
HRESULT (__stdcall *CreateInstance)(ITypeInfo *This, IUnknown *pUnkOuter, const IID *const riid, PVOID *ppvObj);
HRESULT (__stdcall *GetMops)(ITypeInfo *This, MEMBERID memid, BSTR *pBstrMops);
HRESULT (__stdcall *GetContainingTypeLib)(ITypeInfo *This, ITypeLib **ppTLib, UINT *pIndex);
void (__stdcall *ReleaseTypeAttr)(ITypeInfo *This, TYPEATTR *pTypeAttr);
void (__stdcall *ReleaseFuncDesc)(ITypeInfo *This, FUNCDESC *pFuncDesc);
void (__stdcall *ReleaseVarDesc)(ITypeInfo *This, VARDESC *pVarDesc);
}
;

typedef tagTYPEATTR TYPEATTR
;

struct tagTYPEATTR
{
GUID guid;
LCID lcid;
DWORD dwReserved;
MEMBERID memidConstructor;
MEMBERID memidDestructor;
LPOLESTR lpstrSchema;
ULONG cbSizeInstance;
TYPEKIND typekind;
WORD cFuncs;
WORD cVars;
WORD cImplTypes;
WORD cbSizeVft;
WORD cbAlignment;
WORD wTypeFlags;
WORD wMajorVerNum;
WORD wMinorVerNum;
TYPEDESC tdescAlias;
IDLDESC idldescType;
}
;

typedef DISPID MEMBERID
;

typedef LONG DISPID
;

typedef OLECHAR *LPOLESTR
;

typedef tagTYPEKIND TYPEKIND
;

enum tagTYPEKIND
{
TKIND_ENUM = 0x0,
TKIND_RECORD = 0x1,
TKIND_MODULE = 0x2,
TKIND_INTERFACE = 0x3,
TKIND_DISPATCH = 0x4,
TKIND_COCLASS = 0x5,
TKIND_ALIAS = 0x6,
TKIND_UNION = 0x7,
TKIND_MAX = 0x8,
}
;

typedef tagTYPEDESC TYPEDESC
;

struct tagTYPEDESC
{
union
{
tagTYPEDESC *lptdesc;
tagARRAYDESC *lpadesc;
HREFTYPE hreftype;
};
VARTYPE vt;
}
;

union tagTYPEDESC::$AC700B6542D8071E244CADABF8A32897
{
tagTYPEDESC *lptdesc;
tagARRAYDESC *lpadesc;
HREFTYPE hreftype;
}
;

struct tagARRAYDESC
{
TYPEDESC tdescElem;
USHORT cDims;
SAFEARRAYBOUND rgbounds[1];
}
;

typedef tagSAFEARRAYBOUND SAFEARRAYBOUND
;

struct tagSAFEARRAYBOUND
{
ULONG cElements;
LONG lLbound;
}
;

typedef DWORD HREFTYPE
;

typedef tagIDLDESC IDLDESC
;

struct tagIDLDESC
{
ULONG_PTR dwReserved;
USHORT wIDLFlags;
}
;

struct ITypeComp
{
ITypeCompVtbl *lpVtbl;
}
;

struct ITypeCompVtbl
{
HRESULT (__stdcall *QueryInterface)(ITypeComp *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ITypeComp *This);
ULONG (__stdcall *Release)(ITypeComp *This);
HRESULT (__stdcall *Bind)(ITypeComp *This, LPOLESTR szName, ULONG lHashVal, WORD wFlags, ITypeInfo **ppTInfo, DESCKIND *pDescKind, BINDPTR *pBindPtr);
HRESULT (__stdcall *BindType)(ITypeComp *This, LPOLESTR szName, ULONG lHashVal, ITypeInfo **ppTInfo, ITypeComp **ppTComp);
}
;

typedef tagDESCKIND DESCKIND
;

enum tagDESCKIND
{
DESCKIND_NONE = 0x0,
DESCKIND_FUNCDESC = 0x1,
DESCKIND_VARDESC = 0x2,
DESCKIND_TYPECOMP = 0x3,
DESCKIND_IMPLICITAPPOBJ = 0x4,
DESCKIND_MAX = 0x5,
}
;

typedef tagBINDPTR BINDPTR
;

union tagBINDPTR
{
FUNCDESC *lpfuncdesc;
VARDESC *lpvardesc;
ITypeComp *lptcomp;
}
;

typedef tagFUNCDESC FUNCDESC
;

struct tagFUNCDESC
{
MEMBERID memid;
SCODE *lprgscode;
ELEMDESC *lprgelemdescParam;
FUNCKIND funckind;
INVOKEKIND invkind;
CALLCONV callconv;
SHORT cParams;
SHORT cParamsOpt;
SHORT oVft;
SHORT cScodes;
ELEMDESC elemdescFunc;
WORD wFuncFlags;
}
;

typedef tagELEMDESC ELEMDESC
;

struct tagELEMDESC
{
TYPEDESC tdesc;
union
{
IDLDESC idldesc;
PARAMDESC paramdesc;
};
}
;

union tagELEMDESC::$7C8F4CED1424251743D09680A1A0B07D
{
IDLDESC idldesc;
PARAMDESC paramdesc;
}
;

typedef tagPARAMDESC PARAMDESC
;

struct tagPARAMDESC
{
LPPARAMDESCEX pparamdescex;
USHORT wParamFlags;
}
;

typedef tagPARAMDESCEX *LPPARAMDESCEX
;

struct tagPARAMDESCEX
{
ULONG cBytes;
VARIANTARG varDefaultValue;
}
;

typedef tagFUNCKIND FUNCKIND
;

enum tagFUNCKIND
{
FUNC_VIRTUAL = 0x0,
FUNC_PUREVIRTUAL = 0x1,
FUNC_NONVIRTUAL = 0x2,
FUNC_STATIC = 0x3,
FUNC_DISPATCH = 0x4,
}
;

typedef tagINVOKEKIND INVOKEKIND
;

enum tagINVOKEKIND
{
INVOKE_FUNC = 0x1,
INVOKE_PROPERTYGET = 0x2,
INVOKE_PROPERTYPUT = 0x4,
INVOKE_PROPERTYPUTREF = 0x8,
}
;

typedef tagCALLCONV CALLCONV
;

enum tagCALLCONV
{
CC_FASTCALL = 0x0,
CC_CDECL = 0x1,
CC_MSCPASCAL = 0x2,
CC_PASCAL = 0x2,
CC_MACPASCAL = 0x3,
CC_STDCALL = 0x4,
CC_FPFASTCALL = 0x5,
CC_SYSCALL = 0x6,
CC_MPWCDECL = 0x7,
CC_MPWPASCAL = 0x8,
CC_MAX = 0x9,
}
;

typedef tagVARDESC VARDESC
;

struct tagVARDESC
{
MEMBERID memid;
LPOLESTR lpstrSchema;
union
{
ULONG oInst;
VARIANT *lpvarValue;
};
ELEMDESC elemdescVar;
WORD wVarFlags;
VARKIND varkind;
}
;

union tagVARDESC::$E6274BD6A7149C9CC2413444FF769F0B
{
ULONG oInst;
VARIANT *lpvarValue;
}
;

typedef tagVARKIND VARKIND
;

enum tagVARKIND
{
VAR_PERINSTANCE = 0x0,
VAR_STATIC = 0x1,
VAR_CONST = 0x2,
VAR_DISPATCH = 0x3,
}
;

typedef tagDISPPARAMS DISPPARAMS
;

struct tagDISPPARAMS
{
VARIANTARG *rgvarg;
DISPID *rgdispidNamedArgs;
UINT cArgs;
UINT cNamedArgs;
}
;

typedef tagEXCEPINFO EXCEPINFO
;

struct tagEXCEPINFO
{
WORD wCode;
WORD wReserved;
BSTR bstrSource;
BSTR bstrDescription;
BSTR bstrHelpFile;
DWORD dwHelpContext;
PVOID pvReserved;
HRESULT (__stdcall *pfnDeferredFillIn)(tagEXCEPINFO *);
SCODE scode;
}
;

struct ITypeLib
{
ITypeLibVtbl *lpVtbl;
}
;

struct ITypeLibVtbl
{
HRESULT (__stdcall *QueryInterface)(ITypeLib *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ITypeLib *This);
ULONG (__stdcall *Release)(ITypeLib *This);
UINT (__stdcall *GetTypeInfoCount)(ITypeLib *This);
HRESULT (__stdcall *GetTypeInfo)(ITypeLib *This, UINT index, ITypeInfo **ppTInfo);
HRESULT (__stdcall *GetTypeInfoType)(ITypeLib *This, UINT index, TYPEKIND *pTKind);
HRESULT (__stdcall *GetTypeInfoOfGuid)(ITypeLib *This, const GUID *const guid, ITypeInfo **ppTinfo);
HRESULT (__stdcall *GetLibAttr)(ITypeLib *This, TLIBATTR **ppTLibAttr);
HRESULT (__stdcall *GetTypeComp)(ITypeLib *This, ITypeComp **ppTComp);
HRESULT (__stdcall *GetDocumentation)(ITypeLib *This, INT index, BSTR *pBstrName, BSTR *pBstrDocString, DWORD *pdwHelpContext, BSTR *pBstrHelpFile);
HRESULT (__stdcall *IsName)(ITypeLib *This, LPOLESTR szNameBuf, ULONG lHashVal, BOOL *pfName);
HRESULT (__stdcall *FindName)(ITypeLib *This, LPOLESTR szNameBuf, ULONG lHashVal, ITypeInfo **ppTInfo, MEMBERID *rgMemId, USHORT *pcFound);
void (__stdcall *ReleaseTLibAttr)(ITypeLib *This, TLIBATTR *pTLibAttr);
}
;

typedef tagTLIBATTR TLIBATTR
;

struct tagTLIBATTR
{
GUID guid;
LCID lcid;
SYSKIND syskind;
WORD wMajorVerNum;
WORD wMinorVerNum;
WORD wLibFlags;
}
;

typedef tagSYSKIND SYSKIND
;

enum tagSYSKIND
{
SYS_WIN16 = 0x0,
SYS_WIN32 = 0x1,
SYS_MAC = 0x2,
SYS_WIN64 = 0x3,
}
;

typedef tagSAFEARRAY SAFEARRAY
;

struct tagSAFEARRAY
{
USHORT cDims;
USHORT fFeatures;
ULONG cbElements;
ULONG cLocks;
PVOID pvData;
SAFEARRAYBOUND rgsabound[1];
}
;

typedef tagDEC DECIMAL
;

struct tagDEC
{
USHORT wReserved;
union
{
struct
{
BYTE scale;
BYTE sign;
};
USHORT signscale;
};
ULONG Hi32;
union
{
struct
{
ULONG Lo32;
ULONG Mid32;
};
ULONGLONG Lo64;
};
}
;

union tagDEC::$64EC678C49E7BE49873AFBFB7A849D34
{
struct
{
BYTE scale;
BYTE sign;
};
USHORT signscale;
}
;

struct tagDEC::$64EC678C49E7BE49873AFBFB7A849D34::$7F8459940C2B08BD5D82B0F27239141B
{
BYTE scale;
BYTE sign;
}
;

union tagDEC::$D28E26DEC3EC762C06C2AA9D0F7AC301
{
struct
{
ULONG Lo32;
ULONG Mid32;
};
ULONGLONG Lo64;
}
;

struct tagDEC::$D28E26DEC3EC762C06C2AA9D0F7AC301::$674876891A86A76F12C10005982BCA56
{
ULONG Lo32;
ULONG Mid32;
}
;

struct tagVARIANT::$::$::$E09503A454170B491AC1C4312CE36FE6::$0FDBD249F1AECD6A49409B6B82281578
{
PVOID pvRecord;
IRecordInfo *pRecInfo;
}
;

struct IRecordInfo
{
IRecordInfoVtbl *lpVtbl;
}
;

struct IRecordInfoVtbl
{
HRESULT (__stdcall *QueryInterface)(IRecordInfo *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(IRecordInfo *This);
ULONG (__stdcall *Release)(IRecordInfo *This);
HRESULT (__stdcall *RecordInit)(IRecordInfo *This, PVOID pvNew);
HRESULT (__stdcall *RecordClear)(IRecordInfo *This, PVOID pvExisting);
HRESULT (__stdcall *RecordCopy)(IRecordInfo *This, PVOID pvExisting, PVOID pvNew);
HRESULT (__stdcall *GetGuid)(IRecordInfo *This, GUID *pguid);
HRESULT (__stdcall *GetName)(IRecordInfo *This, BSTR *pbstrName);
HRESULT (__stdcall *GetSize)(IRecordInfo *This, ULONG *pcbSize);
HRESULT (__stdcall *GetTypeInfo)(IRecordInfo *This, ITypeInfo **ppTypeInfo);
HRESULT (__stdcall *GetField)(IRecordInfo *This, PVOID pvData, LPCOLESTR szFieldName, VARIANT *pvarField);
HRESULT (__stdcall *GetFieldNoCopy)(IRecordInfo *This, PVOID pvData, LPCOLESTR szFieldName, VARIANT *pvarField, PVOID *ppvDataCArray);
HRESULT (__stdcall *PutField)(IRecordInfo *This, ULONG wFlags, PVOID pvData, LPCOLESTR szFieldName, VARIANT *pvarField);
HRESULT (__stdcall *PutFieldNoCopy)(IRecordInfo *This, ULONG wFlags, PVOID pvData, LPCOLESTR szFieldName, VARIANT *pvarField);
HRESULT (__stdcall *GetFieldNames)(IRecordInfo *This, ULONG *pcNames, BSTR *rgBstrNames);
BOOL (__stdcall *IsMatchingType)(IRecordInfo *This, IRecordInfo *pRecordInfo);
PVOID (__stdcall *RecordCreate)(IRecordInfo *This);
HRESULT (__stdcall *RecordCreateCopy)(IRecordInfo *This, PVOID pvSource, PVOID *ppvDest);
HRESULT (__stdcall *RecordDestroy)(IRecordInfo *This, PVOID pvRecord);
}
;

typedef const OLECHAR *LPCOLESTR
;

typedef GUID KNOWNFOLDERID
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

typedef LRESULT (__stdcall *WNDPROC)(HWND, UINT, WPARAM, LPARAM)
;

typedef UINT_PTR WPARAM
;

typedef LONG_PTR LPARAM
;

typedef __int64 LONG_PTR
;

typedef LONG_PTR LRESULT
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

struct HSTRING_HEADER
{
HSTRING_HEADER::$F359371FD90076C908911EE40321603B Reserved;
}
;

union HSTRING_HEADER::$F359371FD90076C908911EE40321603B
{
PVOID Reserved1;
char Reserved2[24];
}
;

struct tagPOINT
{
LONG x;
LONG y;
}
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

typedef tagPOINT POINT
;

typedef tagRECT RECT
;

struct tagMONITORINFO
{
DWORD cbSize;
RECT rcMonitor;
RECT rcWork;
DWORD dwFlags;
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

enum __shex DISPLAYCONFIG_DEVICE_INFO_TYPE : __int32
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
DISPLAYCONFIG_DEVICE_INFO_FORCE_UINT32 = -0x1,
}
;

typedef DISPLAYCONFIG_DEVICE_INFO_TYPE $1A43D328328C3E5918F37A985A65BBFE
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

typedef _devicemodeW DEVMODEW
;

struct _devicemodeW
{
WCHAR dmDeviceName[32];
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
WCHAR dmFormName[32];
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

union _devicemodeW::$5A39A2C3BB0B1D2D5888DFEE2BA0B23C
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

struct _devicemodeW::$5A39A2C3BB0B1D2D5888DFEE2BA0B23C::$A4EE175A7532F35C00DA5CAD042934DF
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

struct _devicemodeW::$5A39A2C3BB0B1D2D5888DFEE2BA0B23C::$604F2BC6924E02F5722A52FC79BC44F9
{
POINTL dmPosition;
DWORD dmDisplayOrientation;
DWORD dmDisplayFixedOutput;
}
;

union _devicemodeW::$4FBEBCCE69364E072421C9DF045BB77C
{
DWORD dmDisplayFlags;
DWORD dmNup;
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

typedef _XINPUT_VIBRATION XINPUT_VIBRATION
;

struct _XINPUT_VIBRATION
{
WORD wLeftMotorSpeed;
WORD wRightMotorSpeed;
}
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

typedef tagWNDCLASSW WNDCLASSW
;

struct tagWNDCLASSW
{
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

struct timecaps_tag
{
UINT wPeriodMin;
UINT wPeriodMax;
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

typedef tagPROPVARIANT PROPVARIANT
;

struct tagPROPVARIANT
{
union
{
struct
{
VARTYPE vt;
PROPVAR_PAD1 wReserved1;
PROPVAR_PAD2 wReserved2;
PROPVAR_PAD3 wReserved3;
union
{
CHAR cVal;
UCHAR bVal;
SHORT iVal;
USHORT uiVal;
LONG lVal;
ULONG ulVal;
INT intVal;
UINT uintVal;
LARGE_INTEGER hVal;
ULARGE_INTEGER uhVal;
FLOAT fltVal;
DOUBLE dblVal;
VARIANT_BOOL boolVal;
SCODE scode;
CY cyVal;
DATE date;
FILETIME filetime;
CLSID *puuid;
CLIPDATA *pclipdata;
BSTR bstrVal;
BSTRBLOB bstrblobVal;
BLOB blob;
LPSTR pszVal;
LPWSTR pwszVal;
IUnknown *punkVal;
IDispatch *pdispVal;
IStream *pStream;
IStorage *pStorage;
LPVERSIONEDSTREAM pVersionedStream;
LPSAFEARRAY parray;
CAC cac;
CAUB caub;
CAI cai;
CAUI caui;
CAL cal;
CAUL caul;
CAH cah;
CAUH cauh;
CAFLT caflt;
CADBL cadbl;
CABOOL cabool;
CASCODE cascode;
CACY cacy;
CADATE cadate;
CAFILETIME cafiletime;
CACLSID cauuid;
CACLIPDATA caclipdata;
CABSTR cabstr;
CABSTRBLOB cabstrblob;
CALPSTR calpstr;
CALPWSTR calpwstr;
CAPROPVARIANT capropvar;
CHAR *pcVal;
UCHAR *pbVal;
SHORT *piVal;
USHORT *puiVal;
LONG *plVal;
ULONG *pulVal;
INT *pintVal;
UINT *puintVal;
FLOAT *pfltVal;
DOUBLE *pdblVal;
VARIANT_BOOL *pboolVal;
DECIMAL *pdecVal;
SCODE *pscode;
CY *pcyVal;
DATE *pdate;
BSTR *pbstrVal;
IUnknown **ppunkVal;
IDispatch **ppdispVal;
LPSAFEARRAY *pparray;
PROPVARIANT *pvarVal;
};
};
DECIMAL decVal;
};
}
;

union tagPROPVARIANT::$B01A803922ACEB71FC353E1ACAA1539E
{
struct
{
VARTYPE vt;
PROPVAR_PAD1 wReserved1;
PROPVAR_PAD2 wReserved2;
PROPVAR_PAD3 wReserved3;
union
{
CHAR cVal;
UCHAR bVal;
SHORT iVal;
USHORT uiVal;
LONG lVal;
ULONG ulVal;
INT intVal;
UINT uintVal;
LARGE_INTEGER hVal;
ULARGE_INTEGER uhVal;
FLOAT fltVal;
DOUBLE dblVal;
VARIANT_BOOL boolVal;
SCODE scode;
CY cyVal;
DATE date;
FILETIME filetime;
CLSID *puuid;
CLIPDATA *pclipdata;
BSTR bstrVal;
BSTRBLOB bstrblobVal;
BLOB blob;
LPSTR pszVal;
LPWSTR pwszVal;
IUnknown *punkVal;
IDispatch *pdispVal;
IStream *pStream;
IStorage *pStorage;
LPVERSIONEDSTREAM pVersionedStream;
LPSAFEARRAY parray;
CAC cac;
CAUB caub;
CAI cai;
CAUI caui;
CAL cal;
CAUL caul;
CAH cah;
CAUH cauh;
CAFLT caflt;
CADBL cadbl;
CABOOL cabool;
CASCODE cascode;
CACY cacy;
CADATE cadate;
CAFILETIME cafiletime;
CACLSID cauuid;
CACLIPDATA caclipdata;
CABSTR cabstr;
CABSTRBLOB cabstrblob;
CALPSTR calpstr;
CALPWSTR calpwstr;
CAPROPVARIANT capropvar;
CHAR *pcVal;
UCHAR *pbVal;
SHORT *piVal;
USHORT *puiVal;
LONG *plVal;
ULONG *pulVal;
INT *pintVal;
UINT *puintVal;
FLOAT *pfltVal;
DOUBLE *pdblVal;
VARIANT_BOOL *pboolVal;
DECIMAL *pdecVal;
SCODE *pscode;
CY *pcyVal;
DATE *pdate;
BSTR *pbstrVal;
IUnknown **ppunkVal;
IDispatch **ppdispVal;
LPSAFEARRAY *pparray;
PROPVARIANT *pvarVal;
};
};
DECIMAL decVal;
}
;

struct tagPROPVARIANT::$B01A803922ACEB71FC353E1ACAA1539E::$1A0FF913CB6A2700FBDFFDCC16233417
{
VARTYPE vt;
PROPVAR_PAD1 wReserved1;
PROPVAR_PAD2 wReserved2;
PROPVAR_PAD3 wReserved3;
union
{
CHAR cVal;
UCHAR bVal;
SHORT iVal;
USHORT uiVal;
LONG lVal;
ULONG ulVal;
INT intVal;
UINT uintVal;
LARGE_INTEGER hVal;
ULARGE_INTEGER uhVal;
FLOAT fltVal;
DOUBLE dblVal;
VARIANT_BOOL boolVal;
SCODE scode;
CY cyVal;
DATE date;
FILETIME filetime;
CLSID *puuid;
CLIPDATA *pclipdata;
BSTR bstrVal;
BSTRBLOB bstrblobVal;
BLOB blob;
LPSTR pszVal;
LPWSTR pwszVal;
IUnknown *punkVal;
IDispatch *pdispVal;
IStream *pStream;
IStorage *pStorage;
LPVERSIONEDSTREAM pVersionedStream;
LPSAFEARRAY parray;
CAC cac;
CAUB caub;
CAI cai;
CAUI caui;
CAL cal;
CAUL caul;
CAH cah;
CAUH cauh;
CAFLT caflt;
CADBL cadbl;
CABOOL cabool;
CASCODE cascode;
CACY cacy;
CADATE cadate;
CAFILETIME cafiletime;
CACLSID cauuid;
CACLIPDATA caclipdata;
CABSTR cabstr;
CABSTRBLOB cabstrblob;
CALPSTR calpstr;
CALPWSTR calpwstr;
CAPROPVARIANT capropvar;
CHAR *pcVal;
UCHAR *pbVal;
SHORT *piVal;
USHORT *puiVal;
LONG *plVal;
ULONG *pulVal;
INT *pintVal;
UINT *puintVal;
FLOAT *pfltVal;
DOUBLE *pdblVal;
VARIANT_BOOL *pboolVal;
DECIMAL *pdecVal;
SCODE *pscode;
CY *pcyVal;
DATE *pdate;
BSTR *pbstrVal;
IUnknown **ppunkVal;
IDispatch **ppdispVal;
LPSAFEARRAY *pparray;
PROPVARIANT *pvarVal;
};
}
;

typedef WORD PROPVAR_PAD1
;

typedef WORD PROPVAR_PAD2
;

typedef WORD PROPVAR_PAD3
;

union tagPROPVARIANT::$::$1A0FF913CB6A2700FBDFFDCC16233417::$C55E7FD68DDBD4820BEEA04B5D0E46A7
{
CHAR cVal;
UCHAR bVal;
SHORT iVal;
USHORT uiVal;
LONG lVal;
ULONG ulVal;
INT intVal;
UINT uintVal;
LARGE_INTEGER hVal;
ULARGE_INTEGER uhVal;
FLOAT fltVal;
DOUBLE dblVal;
VARIANT_BOOL boolVal;
SCODE scode;
CY cyVal;
DATE date;
FILETIME filetime;
CLSID *puuid;
CLIPDATA *pclipdata;
BSTR bstrVal;
BSTRBLOB bstrblobVal;
BLOB blob;
LPSTR pszVal;
LPWSTR pwszVal;
IUnknown *punkVal;
IDispatch *pdispVal;
IStream *pStream;
IStorage *pStorage;
LPVERSIONEDSTREAM pVersionedStream;
LPSAFEARRAY parray;
CAC cac;
CAUB caub;
CAI cai;
CAUI caui;
CAL cal;
CAUL caul;
CAH cah;
CAUH cauh;
CAFLT caflt;
CADBL cadbl;
CABOOL cabool;
CASCODE cascode;
CACY cacy;
CADATE cadate;
CAFILETIME cafiletime;
CACLSID cauuid;
CACLIPDATA caclipdata;
CABSTR cabstr;
CABSTRBLOB cabstrblob;
CALPSTR calpstr;
CALPWSTR calpwstr;
CAPROPVARIANT capropvar;
CHAR *pcVal;
UCHAR *pbVal;
SHORT *piVal;
USHORT *puiVal;
LONG *plVal;
ULONG *pulVal;
INT *pintVal;
UINT *puintVal;
FLOAT *pfltVal;
DOUBLE *pdblVal;
VARIANT_BOOL *pboolVal;
DECIMAL *pdecVal;
SCODE *pscode;
CY *pcyVal;
DATE *pdate;
BSTR *pbstrVal;
IUnknown **ppunkVal;
IDispatch **ppdispVal;
LPSAFEARRAY *pparray;
PROPVARIANT *pvarVal;
}
;

typedef unsigned __int8 UCHAR
;

typedef _ULARGE_INTEGER ULARGE_INTEGER
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

typedef GUID CLSID
;

typedef tagCLIPDATA CLIPDATA
;

struct tagCLIPDATA
{
ULONG cbSize;
LONG ulClipFmt;
BYTE *pClipData;
}
;

typedef tagBSTRBLOB BSTRBLOB
;

struct tagBSTRBLOB
{
ULONG cbSize;
BYTE *pData;
}
;

typedef tagBLOB BLOB
;

struct tagBLOB
{
ULONG cbSize;
BYTE *pBlobData;
}
;

struct IStream
{
IStreamVtbl *lpVtbl;
}
;

struct IStreamVtbl
{
HRESULT (__stdcall *QueryInterface)(IStream *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(IStream *This);
ULONG (__stdcall *Release)(IStream *This);
HRESULT (__stdcall *Read)(IStream *This, void *pv, ULONG cb, ULONG *pcbRead);
HRESULT (__stdcall *Write)(IStream *This, const void *pv, ULONG cb, ULONG *pcbWritten);
HRESULT (__stdcall *Seek)(IStream *This, LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition);
HRESULT (__stdcall *SetSize)(IStream *This, ULARGE_INTEGER libNewSize);
HRESULT (__stdcall *CopyTo)(IStream *This, IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten);
HRESULT (__stdcall *Commit)(IStream *This, DWORD grfCommitFlags);
HRESULT (__stdcall *Revert)(IStream *This);
HRESULT (__stdcall *LockRegion)(IStream *This, ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
HRESULT (__stdcall *UnlockRegion)(IStream *This, ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
HRESULT (__stdcall *Stat)(IStream *This, STATSTG *pstatstg, DWORD grfStatFlag);
HRESULT (__stdcall *Clone)(IStream *This, IStream **ppstm);
}
;

typedef tagSTATSTG STATSTG
;

struct tagSTATSTG
{
LPOLESTR pwcsName;
DWORD type;
ULARGE_INTEGER cbSize;
FILETIME mtime;
FILETIME ctime;
FILETIME atime;
DWORD grfMode;
DWORD grfLocksSupported;
CLSID clsid;
DWORD grfStateBits;
DWORD reserved;
}
;

struct IStorage
{
IStorageVtbl *lpVtbl;
}
;

struct IStorageVtbl
{
HRESULT (__stdcall *QueryInterface)(IStorage *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(IStorage *This);
ULONG (__stdcall *Release)(IStorage *This);
HRESULT (__stdcall *CreateStream)(IStorage *This, const OLECHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStream **ppstm);
HRESULT (__stdcall *OpenStream)(IStorage *This, const OLECHAR *pwcsName, void *reserved1, DWORD grfMode, DWORD reserved2, IStream **ppstm);
HRESULT (__stdcall *CreateStorage)(IStorage *This, const OLECHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStorage **ppstg);
HRESULT (__stdcall *OpenStorage)(IStorage *This, const OLECHAR *pwcsName, IStorage *pstgPriority, DWORD grfMode, SNB snbExclude, DWORD reserved, IStorage **ppstg);
HRESULT (__stdcall *CopyTo)(IStorage *This, DWORD ciidExclude, const IID *rgiidExclude, SNB snbExclude, IStorage *pstgDest);
HRESULT (__stdcall *MoveElementTo)(IStorage *This, const OLECHAR *pwcsName, IStorage *pstgDest, const OLECHAR *pwcsNewName, DWORD grfFlags);
HRESULT (__stdcall *Commit)(IStorage *This, DWORD grfCommitFlags);
HRESULT (__stdcall *Revert)(IStorage *This);
HRESULT (__stdcall *EnumElements)(IStorage *This, DWORD reserved1, void *reserved2, DWORD reserved3, IEnumSTATSTG **ppenum);
HRESULT (__stdcall *DestroyElement)(IStorage *This, const OLECHAR *pwcsName);
HRESULT (__stdcall *RenameElement)(IStorage *This, const OLECHAR *pwcsOldName, const OLECHAR *pwcsNewName);
HRESULT (__stdcall *SetElementTimes)(IStorage *This, const OLECHAR *pwcsName, const FILETIME *pctime, const FILETIME *patime, const FILETIME *pmtime);
HRESULT (__stdcall *SetClass)(IStorage *This, const IID *const clsid);
HRESULT (__stdcall *SetStateBits)(IStorage *This, DWORD grfStateBits, DWORD grfMask);
HRESULT (__stdcall *Stat)(IStorage *This, STATSTG *pstatstg, DWORD grfStatFlag);
}
;

typedef LPOLESTR *SNB
;

struct IEnumSTATSTG
{
IEnumSTATSTGVtbl *lpVtbl;
}
;

struct IEnumSTATSTGVtbl
{
HRESULT (__stdcall *QueryInterface)(IEnumSTATSTG *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(IEnumSTATSTG *This);
ULONG (__stdcall *Release)(IEnumSTATSTG *This);
HRESULT (__stdcall *Next)(IEnumSTATSTG *This, ULONG celt, STATSTG *rgelt, ULONG *pceltFetched);
HRESULT (__stdcall *Skip)(IEnumSTATSTG *This, ULONG celt);
HRESULT (__stdcall *Reset)(IEnumSTATSTG *This);
HRESULT (__stdcall *Clone)(IEnumSTATSTG *This, IEnumSTATSTG **ppenum);
}
;

typedef tagVersionedStream *LPVERSIONEDSTREAM
;

struct tagVersionedStream
{
GUID guidVersion;
IStream *pStream;
}
;

typedef SAFEARRAY *LPSAFEARRAY
;

typedef tagCAC CAC
;

struct tagCAC
{
ULONG cElems;
CHAR *pElems;
}
;

typedef tagCAUB CAUB
;

struct tagCAUB
{
ULONG cElems;
UCHAR *pElems;
}
;

typedef tagCAI CAI
;

struct tagCAI
{
ULONG cElems;
SHORT *pElems;
}
;

typedef tagCAUI CAUI
;

struct tagCAUI
{
ULONG cElems;
USHORT *pElems;
}
;

typedef tagCAL CAL
;

struct tagCAL
{
ULONG cElems;
LONG *pElems;
}
;

typedef tagCAUL CAUL
;

struct tagCAUL
{
ULONG cElems;
ULONG *pElems;
}
;

typedef tagCAH CAH
;

struct tagCAH
{
ULONG cElems;
LARGE_INTEGER *pElems;
}
;

typedef tagCAUH CAUH
;

struct tagCAUH
{
ULONG cElems;
ULARGE_INTEGER *pElems;
}
;

typedef tagCAFLT CAFLT
;

struct tagCAFLT
{
ULONG cElems;
FLOAT *pElems;
}
;

typedef tagCADBL CADBL
;

struct tagCADBL
{
ULONG cElems;
DOUBLE *pElems;
}
;

typedef tagCABOOL CABOOL
;

struct tagCABOOL
{
ULONG cElems;
VARIANT_BOOL *pElems;
}
;

typedef tagCASCODE CASCODE
;

struct tagCASCODE
{
ULONG cElems;
SCODE *pElems;
}
;

typedef tagCACY CACY
;

struct tagCACY
{
ULONG cElems;
CY *pElems;
}
;

typedef tagCADATE CADATE
;

struct tagCADATE
{
ULONG cElems;
DATE *pElems;
}
;

typedef tagCAFILETIME CAFILETIME
;

struct tagCAFILETIME
{
ULONG cElems;
FILETIME *pElems;
}
;

typedef tagCACLSID CACLSID
;

struct tagCACLSID
{
ULONG cElems;
CLSID *pElems;
}
;

typedef tagCACLIPDATA CACLIPDATA
;

struct tagCACLIPDATA
{
ULONG cElems;
CLIPDATA *pElems;
}
;

typedef tagCABSTR CABSTR
;

struct tagCABSTR
{
ULONG cElems;
BSTR *pElems;
}
;

typedef tagCABSTRBLOB CABSTRBLOB
;

struct tagCABSTRBLOB
{
ULONG cElems;
BSTRBLOB *pElems;
}
;

typedef tagCALPSTR CALPSTR
;

struct tagCALPSTR
{
ULONG cElems;
LPSTR *pElems;
}
;

typedef tagCALPWSTR CALPWSTR
;

struct tagCALPWSTR
{
ULONG cElems;
LPWSTR *pElems;
}
;

typedef tagCAPROPVARIANT CAPROPVARIANT
;

struct tagCAPROPVARIANT
{
ULONG cElems;
PROPVARIANT *pElems;
}
;

typedef GUID UUID
;

struct _SP_DEVINFO_DATA
{
DWORD cbSize;
GUID ClassGuid;
DWORD DevInst;
ULONG_PTR Reserved;
}
;

struct _DISPLAY_DEVICEW
{
DWORD cb;
WCHAR DeviceName[32];
WCHAR DeviceString[128];
DWORD StateFlags;
WCHAR DeviceID[128];
WCHAR DeviceKey[128];
}
;

struct tagWAVEINCAPSW
{
WORD wMid;
WORD wPid;
MMVERSION vDriverVersion;
WCHAR szPname[32];
DWORD dwFormats;
WORD wChannels;
WORD wReserved1;
}
;

typedef UINT MMVERSION
;

struct tagWAVEOUTCAPSW
{
WORD wMid;
WORD wPid;
MMVERSION vDriverVersion;
WCHAR szPname[32];
DWORD dwFormats;
WORD wChannels;
WORD wReserved1;
DWORD dwSupport;
}
;

struct tagMIXERLINEW
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
WCHAR szShortName[16];
WCHAR szName[64];
tagMIXERLINEW::$9A19AF7771B1B870F9962D397CED60DA Target;
}
;

struct tagMIXERLINEW::$9A19AF7771B1B870F9962D397CED60DA
{
DWORD dwType;
DWORD dwDeviceID;
WORD wMid;
WORD wPid;
MMVERSION vDriverVersion;
WCHAR szPname[32];
}
;

struct tagMIXERLINECONTROLSW
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
LPMIXERCONTROLW pamxctrl;
}
;

union tagMIXERLINECONTROLSW::$DC43C2837E8C20184C3137B308FBE3E1
{
DWORD dwControlID;
DWORD dwControlType;
}
;

typedef tagMIXERCONTROLW *LPMIXERCONTROLW
;

struct tagMIXERCONTROLW
{
DWORD cbStruct;
DWORD dwControlID;
DWORD dwControlType;
DWORD fdwControl;
DWORD cMultipleItems;
WCHAR szShortName[16];
WCHAR szName[64];
tagMIXERCONTROLW::$3D7A05C52ED08258D8A0343834E0D122 Bounds;
tagMIXERCONTROLW::$AFA1D3B62FF5E28583F9FD43B125895E Metrics;
}
;

union tagMIXERCONTROLW::$3D7A05C52ED08258D8A0343834E0D122
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

struct tagMIXERCONTROLW::$3D7A05C52ED08258D8A0343834E0D122::$A9226EEFE738F03A3F7D52221D0941CC
{
LONG lMinimum;
LONG lMaximum;
}
;

struct tagMIXERCONTROLW::$3D7A05C52ED08258D8A0343834E0D122::$A8262751543F154F80C41ED2519FD13B
{
DWORD dwMinimum;
DWORD dwMaximum;
}
;

union tagMIXERCONTROLW::$AFA1D3B62FF5E28583F9FD43B125895E
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

struct _cpinfo
{
UINT MaxCharSize;
BYTE DefaultChar[2];
BYTE LeadByte[12];
}
;

struct _BY_HANDLE_FILE_INFORMATION
{
DWORD dwFileAttributes;
FILETIME ftCreationTime;
FILETIME ftLastAccessTime;
FILETIME ftLastWriteTime;
DWORD dwVolumeSerialNumber;
DWORD nFileSizeHigh;
DWORD nFileSizeLow;
DWORD nNumberOfLinks;
DWORD nFileIndexHigh;
DWORD nFileIndexLow;
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

struct xtime
{
__time64_t sec;
int nsec;
}
;

typedef __int64 __time64_t
;

typedef _Mbstatet mbstate_t
;

struct _Mbstatet
{
unsigned int _Wchar;
unsigned __int16 _Byte;
unsigned __int16 _State;
}
;

struct _Cvtvec
{
unsigned int _Page;
unsigned int _Mbcurmax;
int _Isclocale;
unsigned __int8 _Isleadbyte[32];
}
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

typedef unsigned int _dev_t
;

typedef unsigned __int16 _ino_t
;

struct __crt_locale_pointers
{
struct __crt_locale_data *locinfo;
struct __crt_multibyte_data *mbcinfo;
}
;

struct _onexit_table_t
{
_PVFV *_first;
_PVFV *_last;
_PVFV *_end;
}
;

typedef void (__cdecl *_PVFV)()
;

struct fenv_t
{
unsigned int _Fe_ctl;
unsigned int _Fe_stat;
}
;

struct _LocaleUpdate
;

struct charNode
;

struct __crt_strtox
;

struct std::locale::facet
;

struct Concurrency::details::stl_critical_section_vista
;

struct _anonymous_namespace_
;

struct std::ios_base
;

struct __crt_strtox::big_integer
;

struct __FrameHandler3::TryBlockMap
;

struct _lambda_525d7225a58b45f091abf5754dc0c700_
;

struct _lambda_9a32fed5bf61b6b509b2d3f6003082a1_
;

struct boost::math::detail
;

struct FH4::FuncInfoHeader
;

struct _lambda_c5860995281e5c4ce005b3de8f5874ee_
;

struct _lambda_082c17da81b0962e08c0587ee0fac50c_
;

struct std::bad_function_call
;

struct std::_Lockit
;

struct __crt_stdio_output
;

struct __crt_strtox::floating_point_value
;

struct FH4
;

struct Concurrency::details::_Condition_variable
;

struct std::exception
;

struct std::_Fac_tidy_reg_t
;

struct __crt_mbstring
;

struct std::_Init_locks
;

struct std::locale::_Locimp
;

struct __ExceptionPtr
;

struct Concurrency::critical_section
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

typedef float vec_t
;

typedef vec_t vec3_t[3]
;

typedef vec_t vec4_t[4]
;

struct float4
{
float v[4];
}
;

struct PhysicsLibrary
{
const char *name;
bool isMaterialList;
bool isBodyQualityList;
bool isMotionPropertiesList;
bool isGlobalTypeCompendium;
unsigned int havokDataSize;
char *havokData;
}
;

struct PhysicsSFXEventAssetMaterialEntry
{
int surfaceType;
PhysicsSFXEventAssetRules rules;
}
;

struct PhysicsSFXEventAssetAngleEntry
{
float angle;
float hitSoundVolumeModifierMin;
float hitSoundVolumeModifierMax;
float hitSoundPitchModifierMin;
float hitSoundPitchModifierMax;
PhysicsSFXEventAssetRules rules;
}
;

struct PhysicsSFXEventAssetSpeedEntry
{
float speedThreshold;
float hitSoundVolumeModifierMin;
float hitSoundVolumeModifierMax;
float hitSoundPitchModifierMin;
float hitSoundPitchModifierMax;
PhysicsSFXEventAssetRules rules;
}
;

struct PhysicsSFXEventAssetMassEntry
{
float massThreshold;
float hitSoundVolumeModifierMin;
float hitSoundVolumeModifierMax;
float hitSoundPitchModifierMin;
float hitSoundPitchModifierMax;
PhysicsSFXEventAssetRules rules;
}
;

struct PhysicsSFXEventAssetGravityEntry
{
bool gravity;
PhysicsSFXEventAssetRules rules;
}
;

struct PhysicsSFXEventAssetAtmosphereEntry
{
bool atmosphere;
PhysicsSFXEventAssetRules rules;
}
;

struct PhysicsSFXEventAssetCameraDistanceEntry
{
float distance;
PhysicsSFXEventAssetRules rules;
}
;

struct PhysicsSFXEventAssetFrequencyEntry
{
int maxEvents;
PhysicsSFXEventAssetRules rules;
}
;

struct PhysicsSFXEventAssetRule
{
PhysicsSFXEventAssetRuleType type;
PhysicsSFXEventAssetRuleUnion u;
}
;

struct PhysicsSFXEventAsset
{
const char *name;
int priority;
PhysicsSFXEventAssetRules rules;
}
;

struct PhysicsVFXEventAssetMaterialEntry
{
int surfaceType;
PhysicsVFXEventAssetRules rules;
}
;

struct PhysicsVFXEventAssetAngleEntry
{
float angle;
PhysicsVFXEventAssetRules rules;
}
;

struct PhysicsVFXEventAssetSpeedEntry
{
float speedThreshold;
PhysicsVFXEventAssetRules rules;
}
;

struct PhysicsVFXEventAssetMassEntry
{
float massThreshold;
PhysicsVFXEventAssetRules rules;
}
;

struct PhysicsVFXEventAssetGravityEntry
{
bool gravity;
PhysicsVFXEventAssetRules rules;
}
;

struct PhysicsVFXEventAssetAtmosphereEntry
{
bool atmosphere;
PhysicsVFXEventAssetRules rules;
}
;

struct PhysicsVFXEventAssetCameraDistanceEntry
{
float distance;
PhysicsVFXEventAssetRules rules;
}
;

struct PhysicsVFXEventAssetFrequencyEntry
{
int maxEvents;
PhysicsVFXEventAssetRules rules;
}
;

struct PhysicsVFXEventAssetRule
{
PhysicsVFXEventAssetRuleType type;
PhysicsVFXEventAssetRuleUnion u;
}
;

struct PhysicsVFXEventAsset
{
const char *name;
int priority;
PhysicsVFXEventAssetRules rules;
}
;

struct PhysicsAsset
{
const char *name;
unsigned int havokDataSize;
char *havokData;
int numRigidBodies;
int numConstraints;
int numSFXEventAssets;
PhysicsSFXEventAsset **sfxEventAssets;
int numVFXEventAssets;
PhysicsVFXEventAsset **vfxEventAssets;
PhysicsAssetUsageCounter physicsUsageCounter;
char __pad0[4];
}
;

struct PhysicsFXPipeline
{
const char *name;
char __pad0[8];
unk_1453E1068 unk;
}
;

struct PhysicsFXShape
{
const char *name;
unsigned int size;
vec3_t *vecs;
}
;

struct PhysicsWorldMeshDebugDrawBuffer
{
unsigned int numVerts;
vec3_t *vertices;
unsigned int numIndices;
unsigned __int16 *indices;
unsigned int numAreas;
float *areas;
}
;

struct PhysicsDebugData
{
const char *name;
PhysicsWorldMeshDebugData worldMeshDebugData;
Bounds pmoveSampleBounds;
int pmoveSamplesX;
int pmoveSamplesY;
float pmoveSampleSizeX;
float pmoveSampleSizeY;
unsigned int pmoveNumTracesPerSample;
float pmoveTraceLength;
unsigned int pmoveNumSamples;
vec3_t *pmoveSamplePositions;
char *pmoveSampleHitCounts;
float *pmoveSampleTimes;
float pmoveAverageTraceCost;
Bounds bulletSampleBounds;
int bulletSamplesX;
int bulletSamplesY;
int bulletSamplesZ;
float bulletSampleSizeX;
float bulletSampleSizeY;
float bulletSampleSizeZ;
unsigned int bulletNumTracesPerSample;
unsigned int bulletNumSamples;
vec3_t *bulletSamplePositions;
char *bulletSampleHitCounts;
float *bulletSampleAverageTimes;
float bulletAverageTraceCost;
}
;

struct XAnimNotifyInfo
{
scr_string_t name;
float time;
}
;

struct XAnimPartTrans
{
unsigned __int16 size;
unsigned __int8 smallTrans;
XAnimPartTransData u;
}
;

struct XAnimDeltaPartQuat2
{
unsigned __int16 size;
XAnimDeltaPartQuatData2 u;
}
;

struct XAnimDeltaPartQuat
{
unsigned __int16 size;
XAnimDeltaPartQuatData u;
}
;

struct XAnimDeltaPart
{
XAnimPartTrans *trans;
XAnimDeltaPartQuat2 *quat2;
XAnimDeltaPartQuat *quat;
}
;

struct XAnimParts
{
const char *name;
scr_string_t *names;
char *dataByte;
__int16 *dataShort;
int *dataInt;
__int16 *randomDataShort;
char *randomDataByte;
int *randomDataInt;
XAnimIndices indices;
XAnimNotifyInfo *notify;
XAnimDeltaPart *deltaPart;
unsigned int randomDataShortCount;
unsigned int randomDataByteCount;
unsigned int indexCount;
float framerate;
float frequency;
unsigned __int16 dataByteCount;
unsigned __int16 dataShortCount;
unsigned __int16 dataIntCount;
unsigned __int16 randomDataIntCount;
unsigned __int16 numframes;
unsigned __int8 flags;
unsigned __int8 boneCount[10];
unsigned __int8 notifyCount;
unsigned __int8 assetType;
unsigned __int8 unk1;
unsigned __int8 unk2;
}
;

struct XSurfaceShared
{
XSurfaceSharedData data;
unsigned int dataSize;
XSurfaceSharedFlags flags;
}
;

struct PackedLmapCoords
{
unsigned int packed;
}
;

struct XRigidVertList
{
unsigned __int16 boneOffsetIndex;
unsigned __int16 vertCount;
unsigned __int16 triOffset;
unsigned __int16 triCount;
}
;

struct XSubdivRigidVertList
{
unsigned int firstFace;
unsigned int faceCount;
unsigned int firstRegularPatch;
unsigned int regularPatchCount;
}
;

struct GraphicsUnknown_Function_Table
{
HRESULT (__fastcall *QueryInterface)(IGraphicsUnknown *, const _GUID *, void **);
unsigned int (__fastcall *AddRef)(IGraphicsUnknown *);
unsigned int (__fastcall *Release)(IGraphicsUnknown *);
}
;

struct XSurfaceSubdivLevel
{
XSubdivRigidVertList *rigidVertLists;
unsigned int faceCount;
unsigned int regularPatchCount;
unsigned int regularPatchOffset;
unsigned int facePointCount;
unsigned int facePointValence4Count;
unsigned int facePointBufferSize;
unsigned int edgePointCount;
unsigned int edgePointSmoothEnd;
unsigned int edgePointUVBorderEnd;
unsigned int vertexPointCount;
unsigned int vertexPointValence4Count;
unsigned int vertexPointBufferSize;
unsigned int normalCount;
unsigned int normalBufferSize;
unsigned int transitionPointCount;
unsigned int vertCount;
unsigned int vertOffset;
unsigned __int16 *faceIndices;
unsigned __int16 *regularPatchIndices;
unsigned int *regularPatchFlags;
unsigned int *facePoints;
unsigned int *edgePoints;
unsigned int *vertexPoints;
unsigned int *normals;
unsigned int *transitionPoints;
float *regularPatchCones;
}
;

struct XSurfaceSubdivInfo
{
XSurfaceSubdivLevel *levels;
unsigned int flags;
unsigned int totalVertCount;
unsigned int totalRegularPatchCount;
GfxSubdivCache cache;
}
;

struct BlendShapesPerVert
{
unsigned __int64 *perVertBlendShapeData;
unsigned __int16 *surfBlendShapesData;
unsigned int perVertBlendShapeDataSize;
unsigned __int16 surfBlendShapeCount;
unsigned __int16 modelWeightsCount;
unsigned __int16 tangentVertCount;
unsigned __int16 noTangentVertCount;
unsigned __int16 blendMapVertCount;
unsigned __int16 surfFirstBlendMapShapeIndex;
}
;

struct BlendShapesRecalcTangentFrameData
{
unsigned int bufferSize;
unsigned __int16 *topologicalData;
}
;

struct XSurface
{
unsigned __int16 flags;
unsigned __int16 vertCount;
unsigned __int16 triCount;
unsigned __int16 blendShapeTargetCount;
char rigidVertListCount;
char subdivLevelCount;
unsigned __int16 ugbID;
unsigned int hash;
unsigned __int16 blendVertCounts[8];
unsigned int blendVertSize;
unsigned int sharedVertDataOffset;
unsigned int sharedIndexDataOffset;
unsigned int sharedTriClusterDataOffset;
unsigned int sharedColorDataOffset;
unsigned int sharedSecondUVDataOffset;
unsigned int sharedNormalTransformDataOffset;
unsigned int sharedTensionAccumTableOffset;
unsigned int sharedTensionDataOffset;
XSurfaceShared *shared;
PackedLmapCoords *lmapCoords;
XRigidVertList *rigidVertLists;
unsigned __int16 *blendVerts;
XSurfaceSubdivInfo *subdiv;
DObjPartBits partBits;
Bounds surfBounds;
Bounds *childBounds;
BlendShapesPerVert *blendShapesPerVert;
BlendShapesRecalcTangentFrameData *blendShapesRecalcTangentFrameData;
}
;

struct __declspec(align(8)) XModelSurfs
{
const char *name;
XSurface *surfs;
XPakEntryInfo xpakEntry;
XSurfaceShared *shared;
unsigned __int16 numsurfs;
char ugbState;
int partBits[8];
}
;

struct ScriptableDef
{
const char *name;
ScriptableDef *nextScriptableDef;
int flags;
unsigned int numParts;
ScriptablePartDef *parts;
unsigned int maxNumDynEntsRequired;
unsigned int partCount;
unsigned int serverInstancedPartCount;
unsigned int serverControlledPartCount;
unsigned int maxNumDynEntPartsBase;
unsigned int maxNumDynEntPartsForSpawning;
unsigned __int16 eventStreamSizeRequiredServer;
unsigned __int16 eventStreamSizeRequiredClient;
unsigned int usablePartCount;
unsigned int ffMemCost;
scr_string_t animationTreeName;
XAnim_s *animationTreeDef[2];
float viewRadius;
bool networkLODRangeOverride;
float networkLODRangeOverrideDistance;
unsigned int numXModels;
XModel **models;
char spatialActivationMode;
}
;

struct ScriptablePartDef
{
const char *name;
scr_string_t scrName;
int flags;
unsigned int flatId;
unsigned int serverInstanceFlatId;
unsigned int serverControlledFlatId;
const char *tagName;
scr_string_t scrTagName;
unsigned int numStates;
ScriptableStateDef *states;
ScriptablePartDef *parentPart;
unsigned int numChildParts;
ScriptablePartDef *childParts;
unsigned int numDamageTagOverrides;
ScriptableDamageTagOverride *damageTagOverrides;
unsigned __int16 eventStreamBufferOffsetServer;
unsigned __int16 eventStreamBufferOffsetClient;
unsigned __int16 eventStreamSize;
}
;

struct ScriptableEventDef
{
ScriptableEventBaseDef base;
Scriptable_EventType type;
ScriptableEventDefUnion data;
}
;

struct XModel
{
const char *name;
unsigned __int16 numsurfs;
char numLods;
char collLod;
unsigned __int16 mdaoVolumeCount;
char shadowCutoffLod;
char physicsUseCategory;
char characterCollBoundsType;
char numAimAssistBones;
char impactType;
char mdaoType;
char numBones;
char numRootBones;
unsigned __int16 numClientBones;
char numClothAssets;
unsigned int flags;
int contents;
float scale;
float radius;
Bounds bounds;
float edgeLength;
unsigned int lgvData;
XModelPhysicsUsageCounter physicsUsageCounter;
unsigned int noScalePartBits[8];
ScriptableDef *scriptableMoverDef;
XAnimProceduralBones *proceduralBones;
XAnimDynamicBones *dynamicBones;
scr_string_t *aimAssistBones;
scr_string_t *boneNames;
char *parentList;
__int16 *quats;
float *trans;
char *partClassification;
DObjAnimMat *baseMat;
vec3_t *ikHingeAxis;
ReactiveMotionModelInfo *reactiveMotionInfo;
Material **materialHandles;
XModelLodInfo lodInfo[6];
XBoneInfo *boneInfo;
float *himipRadiusInvSq;
PhysicsAsset *physicsAsset;
PhysicsFXShape *physicsFXShape;
XModelDetailCollision *detailCollision;
ClothAsset **clothAssets;
XModelBlendShapeInfo *blendShapeInfo;
void *unk_wut;
MdaoVolume *mdaoVolumes;
XModelDecalVolumesInfo *decalVolumesInfo;
}
;

struct XCompositeSubmodel
{
char *modelName;
vec3_t translation;
vec3_t angles;
scr_string_t parentBoneName;
int parentSubmodelSlot;
XModel *model;
}
;

struct XCompositeModelDef
{
const char *name;
XCompositeSubmodel *submodels;
XCompositeSubCompositeModel *subCompositeModels;
}
;

struct XCompositeSubCompositeModel
{
char *compositeModelName;
vec3_t translation;
vec3_t angles;
scr_string_t parentBoneName;
int parentSubmodelSlot;
XCompositeModelDef *compositeModel;
}
;

struct ScriptableNoteTrackDef
{
int flags;
const char *noteTrackName;
scr_string_t scrNoteTrackName;
unsigned int numEvents;
ScriptableEventDef *events;
}
;

struct ParticleCurveControlPointDef
{
float time;
float value;
float invTimeDelta;
unsigned int pad[1];
}
;

struct MaterialSerializedShader
{
const char *name;
const char *debugName;
char *program;
char *metaData;
unsigned int programSize;
unsigned int metaDataSize;
unsigned int microCodeCrc;
}
;

struct MaterialPipelineState
{
unsigned int packetSize;
unsigned int metaDataSize;
char *packet;
char *metaData;
MaterialSerializedShader *serializedShaders[4];
void *sourceShaders[4];
ID3D12PipelineState *derivedPso;
ID3D12PipelineState *pso;
}
;

struct MaterialShaderArgument
{
unsigned __int8 type;
unsigned __int8 shader;
unsigned __int16 dest;
MaterialArgumentDef u;
}
;

struct MaterialTechnique
{
MaterialTechniqueHeader hdr;
MaterialPass passArray[1];
}
;

struct MaterialTechniqueSet
{
const char *name;
unsigned __int16 flags;
unsigned __int8 worldVertFormat;
unsigned __int8 preDisplacementOnlyCount;
unsigned __int16 techniqueCount;
__int16 techniqueMaskStartIndex[3];
__int16 unk[2];
unsigned __int64 techniqueMask[3];
MaterialTechnique **techniques;
}
;

struct GfxImageFallback
{
char *pixels;
unsigned int size;
unsigned __int16 width;
unsigned __int16 height;
}
;

struct GfxImage
{
GfxTexture texture;
DXGI_FORMAT imageFormat;
unsigned int flags;
MapType mapType;
TextureSemantic semantic;
GfxImageCategory category;
PicMip picmip;
CardMemory cardMemory;
unsigned int dataLen1;
unsigned int dataLen2;
unsigned __int16 width;
unsigned __int16 height;
unsigned __int16 depth;
unsigned __int16 numElements;
unsigned __int8 levelCount;
unsigned __int8 streamed;
unsigned __int8 unknown1;
unsigned __int8 *pixelData;
GfxImageStreamData streams[4];
const char *name;
}
;

struct MaterialTextureDef
{
unsigned int nameHash;
char nameStart;
char nameEnd;
unsigned __int8 samplerState;
unsigned __int8 semantic;
GfxImage *image;
}
;

struct MaterialConstantDef
{
unsigned int nameHash;
char name[12];
float literal[4];
}
;

struct GfxDecalVolumeMaterial
{
const char *name;
GfxImage *channels[6];
unsigned int flags;
vec3_t colorTint;
float alphaDissolveParms;
float emissiveScale;
unsigned int packedDisplacementScaleAndBias;
float displacementCutoffDistance;
float displacementCutoffFalloff;
unsigned int packedTemperatureBaseAndScale;
char textureAtlasRowCount;
char textureAtlasColumnCount;
char padding[6];
}
;

struct MaterialConstantBufferDef
{
unsigned int vsDataSize;
unsigned int hsDataSize;
unsigned int dsDataSize;
unsigned int psDataSize;
unsigned int vsOffsetDataSize;
unsigned int hsOffsetDataSize;
unsigned int dsOffsetDataSize;
unsigned int psOffsetDataSize;
unsigned __int8 *vsData;
unsigned __int8 *hsData;
unsigned __int8 *dsData;
unsigned __int8 *psData;
unsigned __int16 *vsOffsetData;
unsigned __int16 *hsOffsetData;
unsigned __int16 *dsOffsetData;
unsigned __int16 *psOffsetData;
ID3D11Buffer *vsConstantBuffer;
ID3D11Buffer *hsConstantBuffer;
ID3D11Buffer *dsConstantBuffer;
ID3D11Buffer *psConstantBuffer;
}
;

struct Material
{
union
{
const char *name;
MaterialInfo info;
};
unsigned __int8 textureCount;
unsigned __int8 constantCount;
unsigned __int8 stateFlags;
unsigned __int8 cameraRegion;
unsigned __int8 materialType;
unsigned __int8 layerCount;
unsigned __int8 assetFlags;
char pad;
MaterialTechniqueSet *techniqueSet;
MaterialTextureDef *textureTable;
MaterialConstantDef *constantTable;
unsigned __int8 stateBitsEntry[152];
unsigned __int8 stateBitsCount;
GfxStateBits *stateBitsTable;
unsigned __int8 *constantBufferIndex;
MaterialConstantBufferDef *constantBufferTable;
unsigned __int8 constantBufferCount;
const char **subMaterials;
}
;

struct ParticleSystemDef
{
const char *name;
ParticleEmitterDef *emitterDefs;
ParticleScriptedInputNodeDef *scriptedInputNodeDefs;
int version;
int numEmitters;
int numScriptedInputNodes;
unsigned int flags;
int occlusionOverrideEmitterIndex;
ParticlePhaseOptions phaseOptions;
float drawFrustumCullRadius;
float updateFrustumCullRadius;
float sunDistance;
int preRollMSec;
float4 editorPosition;
float4 editorRotation;
float4 gameTweakPosition;
float4 gameTweakRotation;
}
;

struct FxParticleSimAnimationParticleData
{
unsigned __int16 xNormalizedPos;
unsigned __int16 yNormalizedPos;
unsigned __int16 zNormalizedPos;
unsigned __int16 xNormalizedWidth;
unsigned __int16 yNormalizedHeight;
unsigned __int16 orientation;
unsigned __int16 lifetime;
unsigned __int16 particleID;
unsigned __int16 xNormalizedPosNextFrame;
unsigned __int16 yNormalizedPosNextFrame;
unsigned __int16 zNormalizedPosNextFrame;
unsigned __int16 xNormalizedWidthNextFrame;
unsigned __int16 yNormalizedHeightNextFrame;
__int16 orientationDelta;
unsigned __int16 colorTableIndex;
unsigned __int16 nextColorTableIndex;
}
;

struct FxParticleSimAnimationParticleDataOrientation3D
{
unsigned __int16 yOrientation;
unsigned __int16 zOrientation;
__int16 yOrientationDelta;
__int16 zOrientationDelta;
}
;

struct FxParticleSimAnimationParticleDataIndex
{
char index;
char nextIndex;
}
;

struct FxParticleSimAnimationFrame
{
unsigned int particleDataOffset;
unsigned int numActiveParticles;
}
;

struct FxParticleSimAnimation
{
const char *name;
Material *material;
FxParticleSimAnimationHeader header;
FxParticleSimAnimationParticleData *particleData;
FxParticleSimAnimationParticleDataOrientation3D *particleDataOrientation3D;
FxParticleSimAnimationParticleDataIndex *particleDataIndex;
FxParticleSimAnimationFrame *frames;
vec4_t *colorTable;
}
;

struct VectorSubField
{
vec4_t *linearData;
ExtentBounds worldBounds;
vec3_t ds;
unsigned int numEntries[3];
unsigned int flags;
VectorFieldType type;
unsigned int pad[4];
}
;

struct VectorField
{
const char *name;
VectorSubField *subFields;
ExtentBounds worldBounds;
unsigned int numSubFields;
unsigned int pad;
}
;

struct GfxLightDef
{
const char *name;
const char *iesProfile;
unsigned int unk0;
unsigned int unk1;
float coordOffset;
float coordScale;
float unk2;
int pad;
}
;

union ParticleLinkedAssetDef
{
Material *material;
XModel *model;
ParticleSystemDef *particleSystem;
ParticlePhysicsFXData physicsFXData;
FxParticleSimAnimation *particleSim;
ParticleMarkVisuals decal;
const char *sound;
VectorField *vectorField;
GfxLightDef *lightDef;
char pad[32];
}
;

struct ParticleSpawnMeshAssetDef
{
unsigned int m_numVertsTotal;
unsigned int m_numTrisTotal;
unsigned int m_pad[2];
}
;

struct ParticleModuleDef
{
ParticleModuleType moduleType;
unsigned int m_pad[3];
ParticleModuleTypeDef moduleData;
}
;

struct ParticleModuleGroupDef
{
ParticleModuleDef *moduleDefs;
int numModules;
bool disabled;
}
;

struct ParticleStateDef
{
ParticleModuleGroupDef *moduleGroupDefs;
unsigned int elementType;
unsigned __int64 flags;
unsigned int pad[2];
}
;

struct ParticleEmitterDef
{
ParticleStateDef *stateDefs;
int numStates;
ParticleFloatRange particleSpawnRate;
ParticleFloatRange particleLife;
ParticleFloatRange particleDelay;
unsigned int particleCountMax;
ParticleIntRange particleBurstCount;
ParticleFloatRange emitterLife;
ParticleFloatRange emitterDelay;
int randomSeed;
ParticleFloatRange spawnRangeSq;
float fadeOutMaxDistance;
ParticleCurveDef fadeCurveDef;
float spawnFrustumCullRadius;
unsigned int flags;
ParticleGravityOptions gravityOptions;
int groupIDs[4];
float unk1;
float unk2;
ParticleFloatRange particleSpawnShapeRange;
unsigned int pad[1];
}
;

union ParticleScriptedInputNodeDef
{
ParticleScriptedInputNode nodeBase;
ParticleScriptedInputNodeKVP KVP;
ParticleScriptedInputNodeEmitterDisable emitterDisable;
}
;

struct FxCombinedDef
{
FxCombinedUnion u;
FxCombinedType type;
}
;

struct SndAlias
{
const char *aliasName;
const char *subtitle;
const char *secondaryAliasName;
const char *assetFileName;
unsigned int id;
unsigned int secondaryId;
unsigned int assetId;
unsigned int contextType;
unsigned int contextValue;
unsigned int context2Type;
unsigned int context2Value;
unsigned int duck;
unsigned __int64 surfaceFlags;
int sequence;
float volMin;
float volMax;
int volModIndex;
float pitchMin;
float pitchMax;
float donutFadeEnd;
float distMin;
float distMax;
float velocityMin;
int flags;
float probability;
float lfePercentage;
float centerPercentage;
int startDelay;
int startDelayMax;
unsigned int volumeFalloffCurve;
unsigned int lpfCurve;
unsigned int hpfCurve;
unsigned int reverbSendCurve;
float envelopMin;
float envelopMax;
float envelopPercentage;
unsigned int speakerMap;
float reverbMultiplier;
float farReverbMultiplier;
unsigned int occlusionShape;
unsigned int dopplerPreset;
float smartPanDistance2d;
float smartPanDistance3d;
float smartPanAttenuation2d;
float stereoSpreadMinDist;
float stereoSpreadMaxDist;
float stereoSpreadMidPoint;
char stereoSpreadMaxAngle;
char masterPriority;
float masterPercentage;
float masterSlavePercentage;
float masterThreshold;
__int16 maxDuration_dsec;
}
;

struct SndAliasList
{
const char *aliasName;
SndStringHash id;
SndAlias *head;
int count;
int sequence;
}
;

struct WeaponAnimPackageStateTimers
{
unsigned __int16 iFireDelay;
unsigned __int16 iMeleeDelay;
unsigned __int16 iDetonateDelay;
unsigned __int16 iFirstRaiseTime;
unsigned __int16 iAltRaiseTime;
unsigned __int16 iAltRaiseADSTime;
unsigned __int16 iAltRaiseAkimboTime;
unsigned __int16 iRechamberTime;
unsigned __int16 rechamberTimeOneHanded;
unsigned __int16 iRechamberBoltTime;
unsigned __int16 iHoldFireTime;
unsigned __int16 iHoldPrimeTime;
unsigned __int16 iDetonateTime;
unsigned __int16 iMeleeTime;
unsigned __int16 meleeChargeTime;
unsigned __int16 meleeChargeDelay;
unsigned __int16 altMeleeTime;
unsigned __int16 altMeleeChargeTime;
unsigned __int16 altMeleeChargeDelay;
unsigned __int16 iReloadTime;
unsigned __int16 iFastReloadTime;
unsigned __int16 reloadShowRocketTime;
unsigned __int16 iReloadEmptyTime;
unsigned __int16 iFastReloadEmptyTime;
unsigned __int16 iReloadAddTime;
unsigned __int16 iFastReloadAddTime;
unsigned __int16 iReloadEmptyAddTime;
unsigned __int16 iFastReloadEmptyAddTime;
unsigned __int16 iReloadStartTime;
unsigned __int16 iAltOverride3PInTime;
unsigned __int16 iAltOverride3POutTime;
unsigned __int16 iReloadStartAddTime;
unsigned __int16 iFastReloadStartAddTime;
unsigned __int16 iReloadEndTime;
unsigned __int16 iFastReloadEndTime;
unsigned __int16 iDropTime;
unsigned __int16 iRaiseTime;
unsigned __int16 iAltDropTime;
unsigned __int16 iAltDropADSTime;
unsigned __int16 iAltDropAkimboTime;
unsigned __int16 quickDropTime;
unsigned __int16 quickRaiseTime;
unsigned __int16 iBreachRaiseTime;
unsigned __int16 iEmptyRaiseTime;
unsigned __int16 iEmptyDropTime;
unsigned __int16 sprintInTime;
unsigned __int16 sprintLoopTime;
unsigned __int16 sprintOutTime;
unsigned __int16 walkTime;
unsigned __int16 gungHoTime;
unsigned __int16 crawlTime;
unsigned __int16 stunnedTimeBegin;
unsigned __int16 stunnedTimeLoop;
unsigned __int16 stunnedTimeEnd;
unsigned __int16 nightVisionWearTime;
unsigned __int16 nightVisionWearTimeFadeOutEnd;
unsigned __int16 nightVisionWearTimePowerUp;
unsigned __int16 nightVisionRemoveTime;
unsigned __int16 nightVisionRemoveTimePowerDown;
unsigned __int16 nightVisionRemoveTimeFadeInStart;
unsigned __int16 primeTime;
bool bHoldFullPrime;
bool bAltOverride3PADSOnly;
unsigned __int16 blastFrontTime;
unsigned __int16 blastRightTime;
unsigned __int16 blastBackTime;
unsigned __int16 blastLeftTime;
unsigned __int16 slideInTime;
unsigned __int16 slideLoopTime;
unsigned __int16 slideOutTime;
unsigned __int16 dodgeTime;
unsigned __int16 wallRunInTime;
unsigned __int16 wallRunOutTime;
unsigned __int16 leapInTime;
unsigned __int16 leapLoopTime;
unsigned __int16 leapOutTime;
unsigned __int16 leapCancelTime;
unsigned __int16 chargeInTime;
unsigned __int16 chargeLoopTime;
unsigned __int16 chargeOutTime;
unsigned __int16 iRegenerationTime;
unsigned __int16 iRegenerationAddTime;
unsigned __int16 iReloadInterruptTime;
unsigned __int16 iReloadEmptyInterruptTime;
unsigned __int16 iReloadEndInterruptTime;
unsigned __int16 iReloadMultiple1InterruptTime;
unsigned __int16 iReloadMultiple2InterruptTime;
unsigned __int16 iReloadMultiple3InterruptTime;
unsigned __int16 iReloadMultiple4InterruptTime;
unsigned __int16 iReloadMultiple5InterruptTime;
unsigned __int16 iReloadMultiple6InterruptTime;
unsigned __int16 iReloadMultiple7InterruptTime;
unsigned __int16 iReloadMultiple8InterruptTime;
unsigned __int16 iFastReloadInterruptTime;
unsigned __int16 iFastReloadEmptyInterruptTime;
unsigned __int16 iFastReloadEndInterruptTime;
unsigned __int16 iFastReloadMultiple1InterruptTime;
unsigned __int16 iFastReloadMultiple2InterruptTime;
unsigned __int16 iFastReloadMultiple3InterruptTime;
unsigned __int16 iFastReloadMultiple4InterruptTime;
unsigned __int16 iFastReloadMultiple5InterruptTime;
unsigned __int16 iFastReloadMultiple6InterruptTime;
unsigned __int16 iFastReloadMultiple7InterruptTime;
unsigned __int16 iFastReloadMultiple8InterruptTime;
}
;

struct BlendSpace2DAnimEntry
{
XAnimParts *anim;
vec2_t animCoords;
}
;

struct BlendSpace2DDef
{
const char *name;
vec2_t rangeX;
vec2_t rangeY;
BlendSpace2DAnimEntry *animEntries;
unsigned int numAnimEntries;
unsigned int *triangleCoords;
unsigned int numTriangleCoords;
int blendTimeMs;
bool allAnimsNotify;
}
;

struct WeaponAnimPackage
{
const char *name;
XAnimParts **anims;
WeaponAnimPackageStateTimers *timers;
int meleeAnimType;
int meleeAnimPrimaryCount;
int meleeAnimAltCount;
int fireAnimTimesMs[4];
int pad;
FootstepAnims *footstep;
}
;

struct WeaponVFXPackageDefs
{
FxCombinedDef viewFlashEffect;
FxCombinedDef worldFlashEffect;
FxCombinedDef viewFlashADSEffect;
FxCombinedDef viewFlashOverchargeEffect;
FxCombinedDef viewFlashOverchargeADSEffect;
FxCombinedDef worldFlashOverchargeEffect;
FxCombinedDef viewShellEjectEffect;
FxCombinedDef worldShellEjectEffect;
FxCombinedDef viewLastShotEjectEffect;
FxCombinedDef worldLastShotEjectEffect;
FxCombinedDef viewMagEjectEffect;
}
;

struct WeaponVFXPackage
{
const char *name;
WeaponVFXPackageDefs *vfx;
}
;

struct WeaponSFXPackageSounds
{
const char *name;
SndAliasLookup pickupSound;
SndAliasLookup pickupSoundPlayer;
SndAliasLookup ammoPickupSound;
SndAliasLookup ammoPickupSoundPlayer;
SndAliasLookup projectileSound;
SndAliasLookup pullbackSound;
SndAliasLookup pullbackSoundPlayer;
SndAliasLookup pullbackSoundQuick;
SndAliasLookup pullbackSoundQuickPlayer;
SndAliasLookup fireSound;
SndAliasLookup fireSoundPlayer;
SndAliasLookup fireSoundPlayerAkimbo;
SndAliasLookup fireSoundPlayerAkimboRight;
SndAliasLookup fireStartSound;
SndAliasLookup fireStartSoundPlayer;
SndAliasLookup fireLoopSound;
SndAliasLookup fireLoopSoundPlayer;
SndAliasLookup fireStopSound;
SndAliasLookup fireStopSoundPlayer;
SndAliasLookup fireLastSound;
SndAliasLookup fireLastSoundPlayer;
SndAliasLookup fireLastSoundPlayerAkimbo;
SndAliasLookup fireLastSoundPlayerAkimboRight;
SndAliasLookup regenerateSound;
SndAliasLookup regenerateSoundPlayer;
SndAliasLookup emptyFireSound;
SndAliasLookup emptyFireSoundPlayer;
SndAliasLookup emptyRegenFireSound;
SndAliasLookup emptyRegenFireSoundPlayer;
SndAliasLookup rechamberSound;
SndAliasLookup rechamberSoundPlayer;
SndAliasLookup reloadSound;
SndAliasLookup reloadSoundPlayer;
SndAliasLookup reloadEmptySound;
SndAliasLookup reloadEmptySoundPlayer;
SndAliasLookup reloadStartSound;
SndAliasLookup reloadStartSoundPlayer;
SndAliasLookup reloadEndSound;
SndAliasLookup reloadEndSoundPlayer;
SndAliasLookup detonateSound;
SndAliasLookup detonateSoundPlayer;
SndAliasLookup nightVisionWearSound;
SndAliasLookup nightVisionWearSoundPlayer;
SndAliasLookup nightVisionRemoveSound;
SndAliasLookup nightVisionRemoveSoundPlayer;
SndAliasLookup altSwitchSound;
SndAliasLookup altSwitchSoundPlayer;
SndAliasLookup raiseSound;
SndAliasLookup raiseSoundPlayer;
SndAliasLookup firstRaiseSound;
SndAliasLookup firstRaiseSoundPlayer;
SndAliasLookup putawaySound;
SndAliasLookup putawaySoundPlayer;
SndAliasLookup enterAdsSoundPlayer;
SndAliasLookup leaveAdsSoundPlayer;
SndAliasLookup bulletRicochetImpact;
SndAliasLookup projExplosionSound;
SndAliasLookup projDudSound;
SndAliasLookup projIgnitionSound;
SndAliasLookup bounceSound;
SndAliasLookup rollingSound;
}
;

struct WeaponSFXPackage
{
const char *name;
WeaponSFXPackageSounds *sounds;
char rattleSoundType;
const char *szAdsrBaseSetting;
const char *szWeapSndReflectionClass;
float weapSndFireVolumeShot1;
float weapSndFireVolumeShot2;
float weapSndFireVolumeShot3;
float weapSndProneFireLFEVolume;
}
;

struct SndIndexEntry
{
unsigned __int16 value;
unsigned __int16 next;
}
;

struct SndSendEffectParams
{
char name[64];
SndStringHash id;
float smoothing;
float earlyTime;
float lateTime;
float earlyGain;
float lateGain;
float lateGainProx;
float returnGain;
float earlyLpf;
float lateLpf;
float inputLpf;
float dampLpf;
float wallReflect;
float dryGain;
float earlySize;
float lateSize;
float diffusion;
float rearLevel;
}
;

struct ADSRSetting
{
const char *name;
SndStringHash id;
float attackLength;
ADSRCurve attackCurve;
float decayLength;
ADSRCurve decayCurve;
float releaseLength;
ADSRCurve releaseCurve;
float sustainLevel;
float sustainLength;
}
;

struct AmbientDef
{
const char *name;
SndStringHash id;
unsigned __int16 ambientEventIndex;
unsigned __int16 numEvents;
float timeMin;
float timeMax;
}
;

struct AmbientEvent
{
SndStringHash id;
unsigned __int16 ambientElementIndex;
float weight;
}
;

struct AmbientElement
{
SndStringHash id;
const char *unusedNameField;
const char *aliasName;
float rangeMin;
float rangeMax;
float coneMin;
float coneMax;
}
;

struct FilterDef
{
SndStringHash id;
unsigned __int8 entChannelIdx;
unsigned __int8 band;
SND_EQTYPE type;
float freq;
float gain;
float q;
}
;

struct MixDef
{
SndStringHash id;
unsigned __int16 volModIndex;
float volume;
float fade;
}
;

struct AdsrZoneEntry
{
const char *name;
SndStringHash id;
const char *weaponName;
int adsrIdx;
unsigned int weaponIdx;
}
;

struct OccludeDef
{
SndStringHash id;
unsigned __int16 entChannelIdx;
float cutoffLevel1;
float cutoffLevel2;
float cutoffLevel3;
float cutoffLevel4;
float attnLevel1;
float attnLevel2;
float attnLevel3;
float attnLevel4;
}
;

struct ReverbDef
{
SndStringHash id;
SndStringHash roomTypeId;
char roomType[64];
float dryLevel;
float wetLevel;
float fadeTime;
}
;

struct TimescaleEntry
{
SndStringHash id;
SndStringHash presetName;
int entChannelIdx;
float scale;
}
;

struct ZoneDef
{
SndStringHash id;
SndStringHash stateId;
__int16 reverbIndex;
__int16 startOcclusionIndex;
__int16 numOcclusion;
__int16 startFilterIndex;
__int16 numFilter;
__int16 startMixIndex;
__int16 numMix;
const char *ambientStream;
__int16 startAdsrZoneIndex;
__int16 numAdsrZone;
__int16 startNPCAdsrZoneIndex;
__int16 numNPCAdsrZone;
__int16 ambientDefIndex;
__int16 unkIndex;
__int16 startFullOccIndex;
__int16 numDisableFullOcc;
__int16 startPlayerBreathStateIndex;
__int16 numPlayerBreathStates;
SndStringHash weapReflId;
const char *zoneName;
const char *stateName;
const char *ambientName;
const char *mixName;
const char *reverbName;
const char *filterName;
const char *occlusionName;
const char *plrAdsrName;
const char *npcAdsrName;
const char *weapReflName;
const char *fullOccName;
const char *playerBreathStateName;
SndStringHash contextType1;
SndStringHash contextValue1;
SndStringHash contextType2;
SndStringHash contextValue2;
SndStringHash contextType3;
SndStringHash contextValue3;
SndStringHash contextType4;
SndStringHash contextValue4;
}
;

struct FullOcclusionDef
{
SndStringHash id;
SndStringHash presetName;
int entChannelIdx;
}
;

struct PlayerBreathStateDef
{
SndStringHash stateType;
const char *inhaleAlias;
const char *exhaleAlias;
int minBreaths;
}
;

struct SndDuck
{
char name[64];
SndStringHash id;
float fadeIn;
float fadeOut;
float startDelay;
float minDistance;
float distance;
float length;
SndStringHash fadeInCurve;
SndStringHash fadeOutCurve;
float *attenuation;
float *lpf;
bool updateWhilePaused;
bool trackAmplitude;
bool disableInSplitscreen;
float unused[2];
float aliasAttenuation;
float aliasLpf;
SndStringHash duckAlias;
}
;

struct SndMusicState
{
char name[64];
SndStringHash id;
SndMusicAsset intro;
SndMusicAsset exit;
unsigned int loopCount;
SndMusicAsset *loops;
int order;
int interruptPriority;
int persistPriority;
bool isRandom;
bool isSequential;
bool skipPreviousExit;
bool wallClockSync;
unsigned int refCount;
SndMusicStateStatus status;
}
;

struct SndMusicSet
{
char name[64];
SndStringHash id;
unsigned int stateCount;
SndMusicState *states;
}
;

struct WeaponReflectionDef
{
const char *name;
unsigned int id;
const char *weaponClass;
unsigned int weaponClassId;
const char *type;
WeapReflRoomType zoneType;
float zoneTypeScalar;
float closeMedThreshold;
float medFarThreshold;
float farThreshold;
float npcCloseMedThreshold;
float npcMedFarThreshold;
float npcFarThreshold;
float speedOfSound;
float maxPitchClose;
float pitchDistClose;
float maxPitchMed;
float pitchDistMed;
float maxPitchFar;
float pitchDistFar;
float npcMaxPitchMed;
float npcPitchDistMed;
float npcMaxPitchFar;
float npcPitchDistFar;
float frontVol;
float frontEq;
float sideVol;
float sideEq;
float rearVol;
float rearEq;
}
;

struct DopplerPreset
{
char name[64];
SndStringHash id;
float speedOfSound;
float playerVelocityScale;
float minPitch;
float maxPitch;
float smoothing;
}
;

struct OcclusionShape
{
char name[64];
SndStringHash id;
float unk;
float innerAngleVolume;
float outerAngleVolume;
float occludedVolume;
float innerAngleLPF;
float outerAngleLPF;
float occludedLPF;
}
;

struct FocusCone
{
char name[64];
unsigned int id;
float innerAngle;
float outerAngle;
float innerDist;
float outerDist;
float attenuation;
}
;

struct SndCurve
{
char name[64];
SndStringHash id;
int numPoints;
vec2_t points[16];
}
;

struct SpeakerMap
{
char name[64];
SndStringHash id;
float monoToStereo[2];
float stereoToStereo[4];
float monoTo51[6];
float stereoTo51[12];
float monoTo71[8];
float stereoTo71[16];
}
;

struct SndFutz
{
char name[64];
unsigned int id;
bool enable;
float bpfF;
float bpfQ;
float lsG;
float lsF;
float lsQ;
float dist;
float preG;
float postG;
float th;
float tg;
float clippre;
float clippost;
float blend;
unsigned int startAliasId;
unsigned int stopAliasId;
unsigned int loopAliasId;
}
;

struct StreamKey
{
const char *name;
XPakEntryInfo xpakInfo;
StreamKeyData data;
$073322FE7C1053F6D96A83C00F8F6D30 ___u3;
unsigned int dataSize;
char behaviorIndex[1];
char flags;
}
;

struct SndBankTransient
{
const char *name;
unsigned int elementCount;
unsigned int *elements;
}
;

struct RumbleGraph
{
const char *name;
vec2_t knots[16];
unsigned __int16 knotCount;
}
;

struct SwaySettings
{
HipSwaySettings hip;
AdsSwaySettings ads;
AdvancedHipSwaySettings adv;
float shellShockScale;
bool overrideHip;
bool overrideAds;
}
;

struct WeaponOffsetPatternDescription
{
bool active;
scr_string_t patternKey;
WeaponOffsetBlendCurveId curveType;
WeaponOffsetPatternId patternType;
WeaponOffsetTransformType transformType;
float frequency;
float blendTime;
vec3_t magnitude;
float hipScale;
float rotationOffset;
float bulletDirScale;
float fullAutoScale;
int fullAutoBullets;
float fullAutoDecay;
int referenceIndex;
int kickOrSnapDecayIndex;
}
;

struct RumbleInfo
{
const char *name;
int duration;
float range;
RumbleGraph *highRumbleGraph;
RumbleGraph *lowRumbleGraph;
bool fadeWithDistance;
bool broadcast;
}
;

struct TracerDef
{
const char *name;
Material *material;
unsigned int drawInterval;
bool fadeOverTime;
float fadeTime;
float speed;
float beamLength;
float beamWidth;
float screwRadius;
float screwDist;
vec4_t colors[5];
}
;

struct LaserDef
{
const char *name;
Material *laserMaterial;
Material *laserLightMaterial;
bool ownerOnly;
char pad1[3];
float range;
float radius;
float endOffsetViewmodel;
float flarePct;
FxCombinedDef beamEffect;
FxCombinedDef laserEndEffect;
vec4_t color;
vec4_t hdrColorScale;
bool laserLight;
char pad2[3];
float laserLightRadius;
float laserLightBeginOffset;
float laserLightEndOffset;
float laserLightBodyTweak;
vec4_t laserLightColor;
vec4_t laserLightHdrColorScale;
float range_alt;
float radius_alt;
float laserLightRadius_alt;
float flarePct_alt;
}
;

struct Gesture
{
const char *name;
GestureType type;
GesturePriority priority;
bool looping;
XAnimParts **anims;
GestureWeaponSettings weaponSettings;
GestureDirectionalSettings directionalSettings;
GestureLookAroundSettings lookAroundSettings;
}
;

struct OverrideBindingList
{
AttachmentSlot slot;
unsigned int attachments[16];
}
;

struct OverrideBinding
{
unsigned int numBindingLists;
OverrideBindingList *bindingLists;
int attachmentVarIndex;
}
;

struct AnimOverride
{
scr_string_t attachment1;
scr_string_t attachment2;
WeaponAnimPackage *overrides;
WeaponAnimPackage *overridesAlt;
}
;

struct SFXOverride
{
scr_string_t attachment1;
scr_string_t attachment2;
WeaponSFXPackage *overrides;
WeaponSFXPackage *overridesAlt;
}
;

struct VFXOverride
{
scr_string_t attachment1;
scr_string_t attachment2;
WeaponVFXPackage *overrides;
WeaponVFXPackage *overridesAlt;
}
;

struct WeaponOffsetPatternScale
{
scr_string_t patternKey;
vec3_t scale;
}
;

struct WeaponOffsetPatternScaleInfo
{
int numPatternScales;
WeaponOffsetPatternScale *patternScales;
}
;

struct BallisticInfoCalculated
{
float *distances;
float zeroingAngle;
int numDistanceEntries;
}
;

struct BallisticInfo
{
float muzzleVelocity;
float ballisticCoefficient;
float diameter;
float mass;
float gravityFactor;
float zeroingDistance;
BallisticInfoCalculated *calculated;
bool enableBallisticTrajectory;
int lifeTimeMs;
}
;

struct HyperBurstInfo
{
float fireTimeFraction;
int coolDownMs;
bool enabled;
}
;

struct WeaponEntityNotify
{
WeaponEntityNotifyFlags flags;
float radius;
float height;
float minSpeed;
}
;

struct WeaponDef
{
const char *szOverlayName;
XModel *gunXModel;
XModel *gunXModelLeftHand;
XModel *gunXModelRightHand;
XModel *defaultViewModel;
XModel *defaultWorldModelLeftHand;
XModel *defaultWorldModelRightHand;
XModel *worldModel;
XModel *worldXModelLeftHand;
XModel *worldXModelRightHand;
XModel *defaultWorldModel;
XModel *playerShadowModel;
XModel *playerShadowModelLeftHand;
XModel *playerShadowModelRightHand;
XModel *handXModel;
XModel **complexWorldModel;
XModel **complexWorldModelLeftHand;
XModel **complexWorldModelRightHand;
XModel **complexGunXModel;
XModel **complexGunXModelLeftHand;
XModel **complexGunXModelRightHand;
unsigned int numComplexModels;
int pad;
Material *camoWorldModelMaterialOverride[16];
Material *camoViewModelMaterialOverride[16];
float unk_0;
float unk_1;
float unk_2;
float unk_3;
WeaponAnimPackage *szXAnims;
WeaponAnimPackage *szXAnimsRightHanded;
WeaponAnimPackage *szXAnimsLeftHanded;
bool unk_4_0;
bool unk_4_1;
char pad17[2];
int iFireTime;
int iFireDelay;
int iFireTimeAkimbo;
int iFireDelayAkimbo;
int pad3;
const char *szModeName;
scr_string_t *notetrackRumbleMapKeys;
scr_string_t *notetrackRumbleMapValues;
scr_string_t *notetrackFXMapKeys;
FxCombinedDef *notetrackFXMapValues;
scr_string_t *notetrackFXMapTagValues;
playerAnimType_t playerAnimType;
scriptedAnimEvent_t scriptedAnimEvent;
scriptedAnimType_t scriptedAnimType;
weapType_t weapType;
unsigned int whizbyType;
weapClass_t weapClass;
PenetrateType penetrateType;
AnimDamageType deathAnimDamageType;
weapInventoryType_t inventoryType;
weapFireType_t fireType;
int burstCount;
targetAssistType_t targetAssistType;
targetAssistBehavior_t targetAssistBehavior;
float pad28;
Material *unkMaterial;
float unk_18;
int unk_19;
float unk_20;
float unk_21;
float unk_22;
float unk_23;
float unk_24;
float unk_25;
float unk_26;
float unk_27;
float unk_28;
float unk_29;
float unk_30;
float unk_31;
float unk_32;
float unk_33;
float unk_34;
float unk_35;
float unk_36;
float unk_37;
float unk_38;
float unk_39;
float unk_40;
float unk_41;
float unk_42;
float unk_43;
float unk_44;
float unk_45;
float unk_46;
float unk_47;
float unk_48;
float unk_49;
float unk_50;
float unk_51;
int unk_52;
int unk_53;
int unk_54;
int unk_55;
int unk_56;
int unk_57;
int unk_58;
int unk_59;
int unk_60;
int unk_61;
float unk_62;
float unk_63;
float unk_64;
bool unk_65_0;
bool unk_65_1;
bool unk_65_2;
bool unk_65_3;
float targetAssistRange;
float targetAssistAngle;
float targetAssistLosOffsetForward;
float targetAssistLosOffsetRight;
float targetAssistLosOffsetUp;
bool targetAssistOnlyAvailableInAds;
bool unk_71_1;
bool unk_71_2;
bool unk_71_3;
bool bSegmentedReload;
bool unk_72_1;
bool unk_72_2;
bool unk_72_3;
float burstFireCooldown;
bool burstFireAuto;
bool pad27[3];
OffhandClass offhandClass;
weapStance_t stance;
float pad18;
WeaponVFXPackage *vfxPackage;
WeaponSFXPackage *sfxPackage;
ReticleType reticleType;
float pad8;
Material *reticleCenter;
Material *reticleSide;
Material *reticleOnePiece;
int iReticleCenterSize;
int iReticleSideSize;
int iReticleMinOfs;
bool reticleCenterPulse;
char pad25[3];
activeReticleType_t activeReticleType;
float vStandMove[3];
float vStandRot[3];
float strafeMove[3];
float strafeRot[3];
float vDuckedOfs[3];
float vDuckedMove[3];
float vDuckedRot[3];
float vProneOfs[3];
float vProneMove[3];
float vProneRot[3];
float fPosMoveRate;
float fPosProneMoveRate;
float fStandMoveMinSpeed;
float fDuckedMoveMinSpeed;
float fProneMoveMinSpeed;
float fPosRotRate;
float fPosProneRotRate;
float fStandRotMinSpeed;
float fDuckedRotMinSpeed;
float fProneRotMinSpeed;
float pad4;
XModel *worldClipModel;
XModel *rocketModel;
XModel *knifeModel;
XModel *worldKnifeModel;
Material *hudIcon;
Material *pickupIcon;
Material *dangerIcon;
Material *throwBackIcon;
bool hideWarningIcons;
char pad6[3];
float warningIconsDelay;
Material *ammoCounterIcon;
ammoCounterClipType_t ammoCounterClip;
int iStartAmmo;
int iPerkStartAmmo;
int iPerkMaxAmmo;
const char *szAmmoName;
int iAmmoIndex;
int iMaxAmmo;
int shotCount;
const char *szSharedAmmoCapName;
int iSharedAmmoCapIndex;
int iSharedAmmoCap;
int iAmmoUsedPerShot;
bool requireAmmoUsedPerShot;
char pad24[3];
float lowAmmoWarningThreshold;
bool disableNoAmmoWarning;
char pad19[3];
int damage;
int playerDamage;
int meleeDamage;
int iDamageType;
float autoAimRange;
float aimAssistRange;
float aimAssistRangeAds;
float aimAssistPitchSlowdown;
float aimAssistPitchSlowdownAds;
float aimAssistYawSlowdown;
float aimAssistYawSlowdownAds;
float aimAssistLockonStrength;
float aimPadding;
float enemyCrosshairRange;
float moveSpeedScale;
float adsMoveSpeedScale;
float sprintDurationScale;
float sprintRestoreDelay;
float fAdsZoomInFrac;
float fAdsZoomOutFrac;
ADSOverlay overlay;
WeapOverlayInteface_t overlayInterface;
float fAdsBobFactor;
float fAdsViewBobMult;
float fHipSpreadStandMin;
float fHipSpreadDuckedMin;
float fHipSpreadProneMin;
float hipSpreadSprintMin;
float hipSpreadInAirMin;
float hipSpreadStandMax;
float hipSpreadDuckedMax;
float hipSpreadProneMax;
float hipSpreadSprintMax;
float hipSpreadInAirMax;
float fHipSpreadDecayRate;
float fHipSpreadFireAdd;
float fHipSpreadTurnAdd;
float fHipSpreadMoveAdd;
float fHipSpreadDuckedDecay;
float fHipSpreadProneDecay;
float hipSpreadSprintDecay;
float hipSpreadInAirDecay;
float fHipReticleSidePos;
float fAdsIdleAmount;
float fHipIdleAmount;
float adsIdleSpeed;
float hipIdleSpeed;
float fIdleCrouchFactor;
float fIdleProneFactor;
float fGunMaxPitch;
float fGunMaxYaw;
float adsIdleLerpStartTime;
float adsIdleLerpTime;
float slideSpreadMin;
float slideSpreadMax;
float slideSpreadDecayRate;
float slideSpreadFireAdd;
float slideSpreadTurnAdd;
float swayMaxAngleSteadyAim;
float swayMaxAngle;
float swayLerpSpeed;
float swayPitchScale;
float swayYawScale;
float swayHorizScale;
float swayVertScale;
float swayShellShockScale;
float adsSwayMaxAngle;
float adsSwayLerpSpeed;
float adsSway_UNKNOWN_Scale;
float adsSwayPitchScale;
float adsSwayYawScale;
float adsSwayHorizScale;
float adsSwayVertScale;
float adsFireRateScale;
float adsDamageRangeScale;
float adsFireAnimFrac;
float fireTimerLerpToAdsScale;
bool alwaysFireAtMaxRangeInAds;
char pad23[3];
float dualWieldViewModelOffset;
float fScopeDriftDelay;
float fScopeDriftLerpInTime;
float fScopeDriftSteadyTime;
float fScopeDriftLerpOutTime;
float fScopeDriftSteadyFactor;
float fScopeDriftUnsteadyFactor;
weaponIconRatioType_t killIconRatio;
int iReloadAmmoAdd;
int iReloadStartAdd;
int ammoDropStockMin;
int ammoDropClipPercentMin;
int ammoDropClipPercentMax;
int iExplosionRadius;
int iExplosionRadiusMin;
int iExplosionForceRadius;
int iExplosionInnerDamage;
int iExplosionOuterDamage;
float damageConeAngle;
float bulletExplDmgMult;
float bulletExplRadiusMult;
int iProjectileSpeed;
int iProjectileSpeedUp;
int iProjectileSpeedForward;
int iProjectileActivateDist;
int iProjectileDetonationRadius;
float projLifetime;
float projLifetimeStdDeviation;
float timeToAccelerate;
float projectileCurvature;
float pad2;
XModel *projectileModel;
weapProjExposion_t projExplosion;
FxCombinedDef projExplosionEffect;
FxCombinedDef projDudEffect;
WeapStickinessType stickiness;
float ricochetChance;
bool riotShieldEnableDamage;
int riotShieldHealth;
float riotShieldDamageMult;
float pad7;
float *parallelBounce;
float *perpendicularBounce;
FxCombinedDef projBodyEffect;
FxCombinedDef projTrailEffect;
FxCombinedDef projBeaconEffect;
vec3_t vProjectileColor;
guidedMissileType_t guidedMissileType;
float maxSteeringAccel;
int projIgnitionDelay;
FxCombinedDef projIgnitionEffect;
float fAdsAimPitch;
float fAdsCrosshairInFrac;
float fAdsCrosshairOutFrac;
float adsInCrosshairAlphaStart;
float adsInCrosshairAlphaEnd;
float adsOutCrosshairAlphaStart;
float adsOutCrosshairAlphaEnd;
bool adsShouldShowCrosshair;
int adsGunKickReducedKickBullets;
float adsGunKickReducedKickPercent;
float fAdsGunKickPitchMin;
float fAdsGunKickPitchMax;
float fAdsGunKickYawMin;
float fAdsGunKickYawMax;
float fAdsGunKickMagMin;
float fAdsGunKickAccel;
float fAdsGunKickSpeedMax;
float fAdsGunKickSpeedDecay;
float fAdsGunKickStaticDecay;
float fAdsViewKickPitchMin;
float fAdsViewKickPitchMax;
float fAdsViewKickYawMin;
float fAdsViewKickYawMax;
float fAdsViewKickMagMin;
float fAdsViewScatterMin;
float fAdsViewScatterMax;
float fAdsSpread;
int iVisibilityAxis;
float fVisibilityUpOffset;
int hipGunKickReducedKickBullets;
float hipGunKickReducedKickPercent;
float fHipGunKickPitchMin;
float fHipGunKickPitchMax;
float fHipGunKickYawMin;
float fHipGunKickYawMax;
float fHipGunKickMagMin;
float fHipGunKickAccel;
float fHipGunKickSpeedMax;
float fHipGunKickSpeedDecay;
float fHipGunKickStaticDecay;
float fHipViewKickPitchMin;
float fHipViewKickPitchMax;
float fHipViewKickYawMin;
float fHipViewKickYawMax;
float fHipViewKickMagMin;
float fHipViewScatterMin;
float fHipViewScatterMax;
float multipleReloadClipPercentage;
float kickAlignedInputScalar;
float kickOpposedInputScalar;
float fightDist;
float maxDist;
const char *accuracyGraphName[2];
vec2_t *originalAccuracyGraphKnots[2];
unsigned __int16 originalAccuracyGraphKnotCount[2];
int iPositionReloadTransTime;
float leftArc;
float rightArc;
float topArc;
float bottomArc;
bool softLeftRightArc;
char pad20[3];
float accuracy;
float aiSpread;
float playerSpread;
float minTurnSpeed[2];
float maxTurnSpeed[2];
float pitchConvergenceTime;
float yawConvergenceTime;
float suppressTime;
float maxRange;
float fAnimHorRotateInc;
float fPlayerPositionDist;
const char *szUseHintString;
const char *dropHintString;
unsigned int iUseHintStringIndex;
unsigned int dropHintStringIndex;
float horizViewJitter;
float vertViewJitter;
float scanSpeed;
float scanAccel;
int scanPauseTime;
const char *szScript;
float adsSpeedMs[2];
int minDamage;
int minPlayerDamage;
int mid1Damage;
int mid2Damage;
int mid3Damage;
int mid1PlayerDamage;
int mid2PlayerDamage;
int mid3PlayerDamage;
float maxDamageRange;
float mid1DamageRange;
float mid2DamageRange;
float mid3DamageRange;
float minDamageRange;
float destabilizationRateTime;
float destabilizationCurvatureMax;
int destabilizeDistance;
float robotechMaxPitch;
float robotechMaxYaw;
float robotechFrequency;
float robotechVariancePitch;
float robotechVarianceYaw;
float robotechVarianceFrequency;
float *locationDamageMultipliers;
unsigned __int8 *hitLocPriorityMap;
float unittypeMultipliers[9];
float pad30;
RumbleInfo *fireRumble;
RumbleInfo *meleeImpactRumble;
tracerStyle_t tracerStyle;
TracerDef *tracerType;
TracerDef *overchargeTracerType;
LaserDef *laserTypeFriendly;
LaserDef *laserTypeEnemy;
bool turretADSEnabled;
float turretADSTime;
float turretFov;
float turretFovADS;
float turretScopeZoomRate;
float turretScopeZoomMin;
float turretScopeZoomMax;
float turretOverheatUpRate;
float turretOverheatDownRate;
float turretOverheatPenalty;
SndAliasLookup turretOverheatSound;
FxCombinedDef turretOverheatEffect;
RumbleInfo *turretBarrelSpinRumble;
float turretBarrelSpinSpeed;
float turretBarrelSpinUpTime;
float turretBarrelSpinDownTime;
SndAliasLookup turretBarrelSpinMaxSnd;
SndAliasLookup turretBarrelSpinUpSnd[4];
SndAliasLookup turretBarrelSpinDownSnd[4];
SndAliasLookup missileConeSoundAlias;
SndAliasLookup missileConeSoundAliasAtBase;
float missileConeSoundRadiusAtTop;
float missileConeSoundRadiusAtBase;
float missileConeSoundHeight;
float missileConeSoundOriginOffset;
float missileConeSoundVolumescaleAtCore;
float missileConeSoundVolumescaleAtEdge;
float missileConeSoundVolumescaleCoreSize;
float missileConeSoundPitchAtTop;
float missileConeSoundPitchAtBottom;
float missileConeSoundPitchTopSize;
float missileConeSoundPitchBottomSize;
float missileConeSoundCrossfadeTopSize;
float missileConeSoundCrossfadeBottomSize;
scr_string_t knifeAttachTagOverride;
scr_string_t knifeAttachTagOverride2;
bool knifeAlwaysAttached;
bool meleeOverrideValues;
char pad26[2];
float aim_automelee_lerp;
float aim_automelee_region_height;
float aim_automelee_region_width;
float aim_automelee_maxPitchMovement;
float aim_automelee_maxYawMovement;
float player_meleeHeight;
float player_meleeWidth;
float playerMeleeRangeStanding;
float playerMeleeRangeCrouched;
float playerMeleeRangeProne;
float playerMeleeRangeChargeStanding;
float playerMeleeRangeChargeCrouched;
float playerMeleeChargeHeightTolerance;
bool shieldAllowFiring;
bool shieldUnkBool;
char pad13[2];
offhandShieldMaxSpeed_t shieldMaxSpeed;
bool shieldAlwaysDisplay;
char pad14[3];
Gesture *shieldDeployGesture;
Gesture *shieldFireWeapGesture;
Gesture *shieldDeployWhileFiring;
Gesture *shieldRetractWhileFiring;
Gesture *shieldBashGesture;
FxCombinedDef shieldMeleeFx;
float shieldMeleeFxDelay;
float HitEarthquakeScale;
float HitEarthquakeDuration;
float HitEarthquakeRadius;
RumbleInfo *shieldHitRumble;
float MissEarthquakeScale;
float MissEarthquakeDuration;
float MissEarthquakeRadius;
RumbleInfo *shieldMissRumble;
int shieldDeployButton;
bool shieldUsesEnergy;
float shieldMaxEnergy;
float shieldConsumptionRate;
float shieldMeleeEnergyCost;
float shieldMeleeHitEnergyCost;
float reactiveMotionRadiusScale;
float reactiveMotionFrequencyScale;
float reactiveMotionAmplitudeScale;
float reactiveMotionFalloff;
float reactiveMotionLifetime;
bool hasAnyTransientModels;
bool sharedAmmo;
bool lockonSupported;
bool requireLockonToFire;
bool isAirburstWeapon;
bool bigExplosion;
bool noAdsWhenMagEmpty;
bool avoidDropCleanup;
bool allowGrenadeSwitching;
bool inheritsPerks;
bool crosshairColorChange;
bool bRifleBullet;
bool bEnergyBullet;
bool armorPiercing;
bool impaling;
bool bBoltAction;
bool aimDownSight;
bool canHoldBreath;
bool meleeOnly;
bool supportsAlternateMelee;
bool canVariableZoom;
bool bRechamberWhileAds;
bool bBulletExplosiveDamage;
bool bCookOffHold;
bool reticleSpin45;
bool bClipOnly;
bool bDoesNotConsumeAmmo;
bool bRemoveWeaponOnEmpty;
bool noAmmoPickup;
bool adsFireOnly;
bool cancelAutoHolsterWhenEmpty;
bool disableSwitchToWhenEmpty;
bool suppressAmmoPrimaryDisplay;
bool suppressAmmoReserveDisplay;
bool laserSightDuringNightvision;
bool markableViewmodel;
DualWieldType dualWieldType;
bool flipKillIcon;
bool bNoPartialReload;
bool reloadDisabled;
bool blocksProne;
bool silenced;
bool isRollingGrenade;
bool dropGrenadeHeldOnDeath;
bool projExplosionEffectForceNormalUp;
bool projExplosionEffectInheritParentDirection;
bool bProjImpactExplode;
bool disableProjectileCrumpleCheck;
bool bProjTrajectoryEvents;
bool bProjWhizByEnabled;
bool stickToPlayers;
bool stickToVehicles;
bool stickToTurrets;
bool stickToNonStick;
bool projEnableMissileStickiness;
bool thrownSideways;
bool hasDetonator;
bool disableFiring;
bool firesOnWeaponSwitch;
bool disableHolding;
bool timedDetonation;
int explosiveDamageDelay;
int fuseTime;
int aiFuseTime;
int maxHoldTime;
GrenadeRotationParams rotationParams;
bool holdButtonToThrow;
bool infiniteHold;
bool freezeMovementWhenFiring;
bool offhandAllowsSprint;
bool thermalScope;
bool thermalToggle;
bool outlineEnemies;
bool outlineDepthTest;
bool outlineFill;
char pad10[3];
float enemyOutlineR;
float enemyOutlineG;
float enemyOutlineB;
float enemyOutlineA;
float allyOutlineR;
float allyOutlineG;
float allyOutlineB;
float allyOutlineA;
bool depthScan;
char pad11[3];
float depthScanThickness;
float depthScanR;
float depthScanG;
float depthScanB;
float depthScanA;
float depthScanOutlineThickness;
float depthScanOutlineR;
float depthScanOutlineG;
float depthScanOutlineB;
float depthScanOutlineA;
bool depthScanOverlay;
char pad12[3];
float depthScanOverlayStrength;
float depthScanOverlayXTiles;
float depthScanOverlayYTiles;
float depthScanOverlayXScroll;
float depthScanOverlayYScroll;
float blurSceneAdsInFraction;
float blurSceneAdsOutFraction;
bool altModeSameWeapon;
bool turretBarrelSpinEnabled;
bool missileConeSoundEnabled;
bool missileConeSoundPitchshiftEnabled;
bool missileConeSoundCrossfadeEnabled;
bool offhandHoldIsCancelable;
bool doNotAllowAttachmentsToOverrideSpread;
bool useScopeDrift;
bool alwaysShatterGlassOnImpact;
bool oldWeapon;
bool jumpSpread;
bool noFullViewmodelAnimations;
float killcamOffset;
bool useDualFOV;
bool disableDrop;
bool preferredDrop;
char pad21[5];
Gesture *gestureAnimation;
float gestureFireStateTime;
Gesture *gesturePullback;
float minPullbackTime;
Gesture *gestureThrow;
float gestureFireTime;
Gesture *gestureDetonate;
float gestureDetonationTime;
float unk_433;
const char *unknownString1;
float unk_434;
char pad22[4];
const char *unknownString2;
bool gesturesDisablePrimary;
char pad15[7];
FxCombinedDef cameraFireEffect;
float cameraFireEffectDurationSec;
float changedFireTime;
float changedFireTimeAkimbo;
int changedFireTimeNumBullets;
WeaponChargeType chargeType;
float chargeGain;
float chargeCostPerShot;
float chargeLossWhenIdle;
float chargeEmptyCooldown;
float chargeFireAtMaxDamageMultiplier;
int chargeMeterEffectBoneCount;
FxCombinedDef chargeMeterEffect;
SndAliasLookup chargeUpSound;
SndAliasLookup chargeDownSound;
SndAliasLookup chargeUpSoundPlayer;
SndAliasLookup chargeDownSoundPlayer;
SndAliasLookup chargeDownToUpSound;
SndAliasLookup chargeDownToUpSoundPlayer;
SndAliasLookup chargeUpToDownSound;
SndAliasLookup chargeUpToDownSoundPlayer;
SndAliasLookup chargeMaxSound;
SndAliasLookup chargeMaxSoundPlayer;
bool chargeHudReveal;
RumbleInfo *chargeRumble;
scr_string_t stowTag;
XModel *stowOffsetModel;
WeaponSlot slot;
float maxTargetAngle;
bool spaceshipSecondaryWeapon;
float impulseFieldRadius;
float impulseFieldInitialSpeed;
float impulseFieldMaxSpeed;
float impulseFieldAcceleration;
float impulseFieldInAirImpulseMultiplier;
float impulseFieldInAirImpulseMultiplierInterpTime;
float impulseFieldSlideMultiplier;
float impulseFieldSlideMultiplierInterpTime;
bool impulseFieldIsPush;
bool impulseFieldAffectsFriendlies;
bool impulseFieldAffectsSelf;
bool impulseFieldAffectsProne;
bool regenerationEnabled;
int regenerationTimeMs;
int regenerationAddTimeMs;
int regenerationAmount;
bool regenerationConsumeStock;
bool regenerationDisableWhileFiring;
bool deployRequireOnWalkableSurface;
bool deployRequireOnNavmesh;
bool deployRequireSkyAbove;
bool deployRequireNoOverhang;
bool deployAlwaysUpright;
bool deployEdgeSnap;
float deployCylinderRadius;
float deployCylinderHeight;
float deployMaxDistance;
float pad16;
FxCombinedDef deployEffect;
int deployValidGroupId;
int deployIndoorGroupId;
int deployInvalidGroupId;
int deployOutOfRangeGroupId;
AnimOverride animOverrides[21];
SFXOverride sfxOverrides[21];
VFXOverride vfxOverrides[21];
float reactiveEmitterDelay;
float grenadeDangerIconDistance;
bool unk_466_0;
char pad29[7];
}
;

struct ReticleDef
{
const char *name;
const char *widgetName;
bool lockReticleToTag;
bool primaryAffectedByEMP;
bool secondaryAffectedByEMP;
bool scaleWithFOV;
float naturalDistance;
float minReticleScale;
float maxReticleScale;
}
;

struct AttAmmoGeneral
{
ImpactType vfxImpactType;
ImpactType sfxImpactType;
MeleeImpactType meleeImpactType;
weapFireType_t fireType;
int burstCount;
}
;

struct AttAmmoType
{
const char *szAmmoName;
}
;

struct AttImpact
{
int vfxImpactType;
}
;

struct AttAmmoTracer
{
TracerDef *tracerType;
tracerStyle_t tracerStyle;
}
;

struct AttPenetration
{
PenetrateType penetrateType;
float penetrateMultiplier;
bool rifleBullet;
bool armorPiercing;
bool impaling;
}
;

struct AttSight
{
float blurSceneAdsInFraction;
float blurSceneAdsOutFraction;
bool aimDownSight;
bool adsFire;
bool rechamberWhileAds;
bool noAdsWhenMagEmpty;
bool canVariableZoom;
bool hideRailWithThisScope;
bool forceIronSightDown;
bool useScopeDrift;
bool useDualFOV;
}
;

struct AttHoldBreath
{
bool canHoldBreath;
}
;

struct AttReload
{
bool noPartialReload;
bool segmentedReload;
bool reloadDisabled;
}
;

struct AttAddOns
{
bool motionTracker;
bool silenced;
bool riotShield;
}
;

struct AttLaser
{
LaserDef *laserTypeFriendly;
LaserDef *laserTypeEnemy;
bool forceLaserOn;
}
;

struct AttPost
{
float adsFireMotionBlur;
int adsFireMotionBlurDurationMs;
float adsFireMotionBlurDecayExponent;
float hipFireMotionBlur;
int hipFireMotionBlurDurationMs;
float hipFireMotionBlurDecayExponent;
float dofViewModelPhysicalFstop;
float dofViewModelPhysicalFocusDistance;
float dofPhysicalFstop;
float dofPhysicalFocusDistance;
float adsCameraShakeRotationScale;
float adsCameraShakeTranslationScale;
bool dofViewModelPhysicalFocalTag;
bool dofSceneIsOverridden;
}
;

struct AttGeneral
{
bool boltAction;
bool inheritsPerks;
bool reticleSpin45;
bool adsShouldShowCrosshair;
float enemyCrosshairRange;
ReticleType reticleType;
Material *reticleCenter;
Material *reticleSide;
Material *reticleOnePiece;
int reticleCenterSize;
int reticleSideSize;
const char *szLUICrosshairWidget;
}
;

struct AttAimAssist
{
float autoAimRange;
float aimAssistRange;
float aimAssistRangeAds;
float aimAssistPitchSlowdown;
float aimAssistPitchSlowdownAds;
float aimAssistYawSlowdown;
float aimAssistYawSlowdownAds;
float aimAssistLockonStrength;
}
;

struct AttTargetAssist
{
targetAssistType_t targetAssistType;
targetAssistBehavior_t targetAssistBehavior;
float targetAssistRange;
float targetAssistAngle;
float targetAssistLosOffsetForward;
float targetAssistLosOffsetRight;
float targetAssistLosOffsetUp;
bool targetAssistOnlyAvailableInAds;
}
;

struct AttAmmunition
{
int maxAmmo;
int startAmmo;
int clipSize;
int shotCount;
int reloadAmmoAdd;
int reloadStartAdd;
bool requireAmmoUsedPerShot;
}
;

struct AttDamage
{
int damage;
int mid1Damage;
int mid2Damage;
int mid3Damage;
int minDamage;
int meleeDamage;
float maxDamageRange;
float mid1DamageRange;
float mid2DamageRange;
float mid3DamageRange;
float minDamageRange;
int playerDamage;
int mid1PlayerDamage;
int mid2PlayerDamage;
int mid3PlayerDamage;
int minPlayerDamage;
AnimDamageType deathAnimDamageType;
}
;

struct AttMelee
{
int meleeDamage;
bool hasMeleeFinisher;
int meleeCountToFinisher;
}
;

struct AttIdleSettings
{
float hipIdleAmount;
float hipIdleSpeed;
float idleCrouchFactor;
float idleProneFactor;
float adsIdleLerpStartTime;
float adsIdleLerpTime;
}
;

struct AttADSSettings
{
float adsSpread;
float adsAimPitch;
float adsTransInTime;
float adsTransOutTime;
int adsReloadTransTime;
float adsCrosshairInFrac;
float adsCrosshairOutFrac;
float adsInCrosshairAlphaStart;
float adsInCrosshairAlphaEnd;
float adsOutCrosshairAlphaStart;
float adsOutCrosshairAlphaEnd;
float adsZoomFov;
float adsZoomInFrac;
float adsZoomOutFrac;
float adsFovLerpTime;
float adsBobFactor;
float adsViewBobMult;
float adsFireRateScale;
float adsDamageRangeScale;
float adsFireAnimFrac;
float fireTimerLerpToAdsScale;
bool alwaysFireAtMaxRangeInAds;
bool adsAlignEnabled;
bool disableTagAlignX;
}
;

struct AttADSGunBobSettings
{
float fAdsGunBobPitchScale;
float fAdsGunBobYawScale;
float fAdsGunBobTiltPitchScale;
float fAdsGunBobTiltYawScale;
float fAdsGunBobTiltRollScale;
float fAdsGunBobTiltBulletDirScale;
float fAdsGunBobTiltOffset;
}
;

struct AttADSReloadSettings
{
bool reloadWhileADS;
}
;

struct AttScopeDriftSettings
{
float fScopeDriftDelay;
float fScopeDriftLerpInTime;
float fScopeDriftSteadyTime;
float fScopeDriftLerpOutTime;
float fScopeDriftSteadyFactor;
float fScopeDriftUnsteadyFactor;
}
;

struct AttADSStanceScales
{
float adsCrouchViewKickFactorScale;
float adsCrouchGunKickFactorScale;
float adsProneViewKickFactorScale;
float adsProneGunKickFactorScale;
float adsCrouchGunBobFactorScale;
WeaponOffsetPatternScaleInfo crouchedWeaponOffsetPatternScales;
WeaponOffsetPatternScaleInfo proneWeaponOffsetPatternScales;
}
;

struct AttHipSpread
{
float hipSpreadStandMin;
float hipSpreadDuckedMin;
float hipSpreadProneMin;
float hipSpreadSprintMin;
float hipSpreadInAirMin;
float hipSpreadMax;
float hipSpreadDuckedMax;
float hipSpreadProneMax;
float hipSpreadSprintMax;
float hipSpreadInAirMax;
float hipSpreadFireAdd;
float hipSpreadTurnAdd;
float hipSpreadMoveAdd;
float hipSpreadDecayRate;
float hipSpreadDuckedDecay;
float hipSpreadProneDecay;
float hipSpreadSprintDecay;
float hipSpreadInAirDecay;
}
;

struct AttSlideSpread
{
float slideSpreadMin;
float slideSpreadMax;
float slideSpreadDecayRate;
float slideSpreadFireAdd;
float slideSpreadTurnAdd;
}
;

struct AttGunKick
{
int hipGunKickReducedKickBullets;
float hipGunKickReducedKickPercent;
float hipGunKickPitchMin;
float hipGunKickPitchMax;
float hipGunKickYawMin;
float hipGunKickYawMax;
float hipGunKickMagMin;
float hipGunKickAccel;
float hipGunKickSpeedMax;
float hipGunKickSpeedDecay;
float hipGunKickStaticDecay;
int adsGunKickReducedKickBullets;
float adsGunKickReducedKickPercent;
float adsGunKickPitchMin;
float adsGunKickPitchMax;
float adsGunKickYawMin;
float adsGunKickYawMax;
float adsGunKickMagMin;
float adsGunKickAccel;
float adsGunKickSpeedMax;
float adsGunKickSpeedDecay;
float adsGunKickStaticDecay;
}
;

struct AttGunTilt
{
float adsGunTiltPitchFactor;
float adsGunTiltYawFactor;
float adsGunTiltRollFactor;
float adsGunTiltOffset;
float hipGunTiltPitchFactor;
float hipGunTiltYawFactor;
float hipGunTiltRollFactor;
float hipGunTiltOffset;
}
;

struct AttKickScaling
{
float hipStartingGunKickPercent;
float hipStartingViewKickPercent;
int adsStartingKickBullets;
float adsStartingGunKickPercent;
float adsStartingViewKickPercent;
float hipEndingGunKickPercent;
float hipEndingViewKickPercent;
int adsEndingKickBullets;
float adsEndingGunKickPercent;
float adsEndingViewKickPercent;
}
;

struct AttViewKick
{
float hipViewKickPitchMin;
float hipViewKickPitchMax;
float hipViewKickYawMin;
float hipViewKickYawMax;
float hipViewKickMagMin;
float hipViewKickCenterSpeed;
float adsViewKickPitchMin;
float adsViewKickPitchMax;
float adsViewKickYawMin;
float adsViewKickYawMax;
float adsViewKickMagMin;
float adsViewKickCenterSpeed;
float kickAlignedInputScalar;
float kickOpposedInputScalar;
}
;

struct WeaponAttachment
{
union
{
const char *szInternalName;
const char *name;
};
const char *szDisplayName;
scr_string_t internalName;
bool papAttachment;
AttachmentType type;
weapType_t weaponType;
weapClass_t weapClass;
int numReticles;
int numAltReticles;
int unk;
XModel *worldModelCamo;
XModel *viewModelCamo;
XModel **worldModelVariations;
XModel **viewModelVariations;
unsigned int numModelVariations;
Material **worldModelCamoMaterials;
Material **viewModelCamoMaterials;
unsigned int numWorldModelCamoMaterials;
unsigned int numViewModelCamoMaterials;
scr_string_t attachPoint;
ReticleDef **reticles;
ReticleDef **altReticles;
AttAmmoGeneral *ammogeneral;
AttAmmoTracer *tracer;
AttPenetration *penetration;
AttSight *sight;
AttHoldBreath *holdBreath;
AttReload *reload;
AttAddOns *addOns;
AttLaser *laser;
AttGeneral *general;
AttAimAssist *aimAssist;
AttTargetAssist *targetAssist;
AttUnknown01 *unkAtt01;
AttAmmunition *ammunition;
AttDamage *damage;
AttIdleSettings *idleSettings;
AttADSSettings *adsSettings;
AttADSSettings *adsSettingsMain;
AttScopeDriftSettings *scopeDriftSettings;
AttScopeDriftSettings *scopeDriftSettingsMain;
AttHipSpread *hipSpread;
AttSlideSpread *slideSpread;
AttGunKick *gunKick;
AttViewKick *viewKick;
AttADSOverlay *adsOverlay;
AttOutline *outline;
AttUI *ui;
AttRumbles *rumbles;
AttProjectile *projectile;
AttCharged *charged;
AdsAltSwitch *adsAltSwitch;
AttRegeneration *regeneration;
AttMovement *movement;
AttBurst *burst;
float *locationDamage;
float ammunitionScale;
float damageScale;
float stateTimersScale;
float reloadTimersScale;
int fireTimeOverride;
int fireTimeAkimboOverride;
float fireTimersScale;
float burstFireCooldownScale;
float idleHipMotionScale;
float idleADSMotionScale;
float idleADSMotionTimeScale;
float adsSettingsScale;
float adsSettingsScaleMain;
float hipSpreadScale;
float slideSpreadScale;
float gunKickScale;
float viewKickScale;
float viewCenterScale;
float adsSwayScale;
float adsSwayScale2;
float adsSwayScale3;
int iAmmoIndex;
int loadIndex;
bool useAlternateViewModelLaser;
bool hideIronSightsWithThisAttachment;
bool shareAmmoWithAlt;
bool useLeftTriggerAltFire;
bool containsIKTag;
bool isAkimboAttachment;
bool attachLeftWeaponAkimbo;
bool attachRightWeaponAkimbo;
bool luiWeaponInfoWidgetUsesScopeStencil;
const char *szAltModeName;
const char *szLUIWeaponInfoWidgetName;
const char *szLUIWeaponInfoWidgetTag;
}
;

struct AttADSOverlay
{
ADSOverlay overlay;
bool hybridToggle;
bool thermalScope;
bool thermalToggle;
bool outlineEnemies;
}
;

struct AttOutline
{
vec4_t outlineColor;
vec4_t outlineAltColor;
vec4_t outlineFillColor0;
vec4_t outlineFillColor1;
vec4_t outlineOccludedOutlineColor;
vec4_t outlineOccludedInlineColor;
vec4_t outlineOccludedInteriorColor;
float outlineWidth;
bool outlineEnemies;
bool outlineDepthTest;
bool outlineFill;
bool outlineScriptControlled;
bool outlineVehicles;
bool outlineTurrets;
bool outlineCharacters;
bool outlineAgents;
bool outlineScriptMovers;
bool outlineLockOn;
}
;

struct AttUI
{
Material *hudIcon;
Material *pickupIcon;
Material *dangerIcon;
Material *throwBackIcon;
bool hideWarningIcons;
float warningIconsDelay;
Material *dpadIcon;
Material *ammoCounterIcon;
ammoCounterClipType_t ammoCounterClip;
float grenadeDangerIconDistance;
}
;

struct AttRumbles
{
RumbleInfo *fireRumble;
RumbleInfo *meleeImpactRumble;
}
;

struct AttProjectile
{
int explosionRadius;
int explosionForceRadius;
int explosionInnerDamage;
int explosionOuterDamage;
float damageConeAngle;
int projectileSpeed;
int projectileSpeedUp;
int projectileActivateDist;
float projectileLifetime;
float projectileLifetimeStdDeviation;
XModel *projectileModel;
weapProjExposion_t projExplosionType;
FxCombinedDef projExplosionEffect;
bool projExplosionEffectForceNormalUp;
SndAliasLookup projExplosionSound;
FxCombinedDef projDudEffect;
SndAliasLookup projDudSound;
bool projImpactExplode;
bool disableProjectileCrumpleCheck;
float destabilizationRateTime;
float destabilizationCurvatureMax;
int destabilizeDistance;
FxCombinedDef projBodyEffect;
FxCombinedDef projTrailEffect;
FxCombinedDef projBeaconEffect;
int projIgnitionDelay;
FxCombinedDef projIgnitionEffect;
SndAliasLookup projIgnitionSound;
WeapStickinessType stickiness;
int fuseTime;
int aiFuseTime;
bool stickToPlayers;
bool stickToVehicles;
bool stickToTurrets;
bool stickToNonStick;
bool timedDetonation;
bool isPredictedProjectile;
bool useSteppedExplosionDamage;
float reactiveMotionRadiusScale;
float reactiveMotionFrequencyScale;
float reactiveMotionAmplitudeScale;
float reactiveMotionFalloff;
float reactiveMotionLifetime;
}
;

struct AttCharged
{
WeaponChargeType chargeType;
float chargeGain;
float chargeCostPerShot;
float chargeLossWhenIdle;
float chargeEmptyCooldown;
float chargeFireAtMaxDamageMultiplier;
int chargeMeterEffectBoneCount;
FxCombinedDef chargeMeterEffect;
SndAliasLookup chargeUpSound;
SndAliasLookup chargeDownSound;
SndAliasLookup chargeDownToUpSound;
SndAliasLookup chargeUpToDownSound;
SndAliasLookup chargeMaxSound;
SndAliasLookup chargeUpSoundPlayer;
SndAliasLookup chargeDownSoundPlayer;
SndAliasLookup chargeDownToUpSoundPlayer;
SndAliasLookup chargeUpToDownSoundPlayer;
SndAliasLookup chargeMaxSoundPlayer;
bool chargeHudReveal;
RumbleInfo *chargeRumble;
}
;

struct AdsAltSwitch
{
float fovInterpTimeSecMainToAlt;
float fovInterpDelaySecMainToAlt;
float dualFovSwitchRateMainToAlt;
AdsAltSwitchInterpType fovInterpTypeMainToAlt;
float fovInterpTimeSecAltToMain;
float fovInterpDelaySecAltToMain;
float dualFovSwitchRateAltToMain;
AdsAltSwitchInterpType fovInterpTypeAltToMain;
}
;

struct AttRegeneration
{
bool regenerationEnabled;
int regenerationTimeMs;
int regenerationAddTimeMs;
int regenerationAmount;
bool regenerationConsumeStock;
bool regenerationDisableWhileFiring;
}
;

struct AttHeat
{
float heatAccumulationPerShot;
float heatDissipationPerSecond;
float heatSmokeStartThreshold;
float heatSmokeStopThreshold;
}
;

struct AttMovement
{
float moveSpeedScale;
float adsMoveSpeedScale;
bool fastADS3PAnims;
}
;

struct AttBurst
{
int burstFireCooldown;
bool burstFireAuto;
}
;

struct AttHitmarker
{
HitmarkerType hitmarkerType;
}
;

struct AttAdsMovement
{
float forwardOffset;
float roll;
bool isOverride;
}
;

struct AttDlcOverrides
{
bool useFloatOverride[8];
float floatOverride[8];
}
;

struct AttDlcScalars
{
float floatScalar[8];
}
;

struct NoteTrackToSoundEntry
{
unsigned int numSoundMappings;
scr_string_t attachment;
scr_string_t *notetrackSoundMapKeys;
scr_string_t *notetrackSoundMapValues;
}
;

struct NoteTrackToSuitSoundEntry
{
unsigned int numSoundMappings;
SuitAnimType suitAnimState;
scr_string_t *notetrackSoundMapKeys;
scr_string_t *notetrackSoundMapValues;
}
;

struct WeaponCompleteDef
{
union
{
const char *szInternalName;
const char *name;
};
WeaponDef *weapDef;
const char *szDisplayName;
const char *szLootTable;
scr_string_t *hideTags;
unsigned int numAttachments;
unsigned int numAttachments2;
unsigned int numAttachments3;
unsigned int numAttachments4;
unsigned int numAttachments5;
unsigned int numAttachments6;
int numLootVariants;
int pad;
WeaponAttachment **attachments;
WeaponAttachment **attachments2;
WeaponAttachment **attachments3;
WeaponAttachment **attachments4;
WeaponAttachment **attachments5;
WeaponAttachment **attachments6;
unsigned int numNotetrackSoundMappings;
int pad2;
scr_string_t *notetrackSoundMapKeys;
scr_string_t *notetrackSoundMapValues;
unsigned int numNotetrackOverrides;
NoteTrackToSoundEntry *notetrackOverrides;
unsigned int numNotetrackSuitEntries;
NoteTrackToSuitSoundEntry *notetrackSuitEntries;
float fAdsZoomFov;
int iAdsTransInTime;
int iAdsTransOutTime;
int iClipSize;
ImpactType vfxImpactType;
ImpactType sfxImpactType;
MeleeImpactType meleeImpactType;
float penetrateMultiplier;
float fAdsViewKickCenterSpeed;
float fHipViewKickCenterSpeed;
const char *szAltWeaponName;
Material *killIcon;
Material *dpadIcon;
int ammoDropStockMax;
float adsDofStart;
float adsDofEnd;
unsigned __int16 accuracyGraphKnotCount[2];
vec2_t *accuracyGraphKnots[2];
bool motionTracker;
bool enhanced;
bool dpadIconShowsAmmo;
bool luiWeaponInfoWidgetUsesScopeStencil;
const char *szAdsrBaseSetting;
const char *szLUIWeaponInfoWidgetName;
const char *szLUIWeaponInfoWidgetTag;
const char *szLUICrosshairWidget;
}
;

struct MayhemModel
{
XModel *model;
}
;

struct MayhemTransBounds
{
vec3_t mins;
vec3_t size;
}
;

struct MayhemAnimFramesSplineCompressed
{
char *diskQuat;
char *diskPos;
unsigned __int16 *diskQuatFrames;
unsigned __int16 *diskPosFrames;
unsigned __int16 *numDiskQuatFrames;
unsigned __int16 *numDiskPosFrames;
MayhemTransBounds *transBounds;
GfxWrappedBaseBuffer quat;
GfxWrappedBaseBuffer pos;
unsigned int quatStride;
unsigned int posStride;
unsigned int totalCompressedQuatFrames;
unsigned int totalCompressedPosFrames;
}
;

struct MayhemAnimFramesUncompressed
{
char *diskQuat;
char *diskPos;
MayhemTransBounds *transBounds;
GfxWrappedBaseBuffer quat;
GfxWrappedBaseBuffer pos;
unsigned int quatStride;
unsigned int posStride;
}
;

struct MayhemDataKeysSplineCompressed
{
char *keys;
unsigned __int16 *numKeys;
unsigned __int16 *keyFrames;
unsigned int totalCompressedKeyFrames;
unsigned int numStreams;
unsigned int keyStride;
}
;

struct MayhemDataKeysUncompressed
{
char *keys;
unsigned int numStreams;
unsigned int keyStride;
}
;

struct MayhemAnim
{
MayhemFramesUnion frames;
unsigned int numBones;
unsigned int numFrames;
float framerate;
float frequency;
unsigned int notifyCount;
unsigned int dataChannelCount;
bool isLooping;
bool isSplineCompressed;
bool quantizeQuats;
bool quantizeTrans;
bool quantizeChannels;
XAnimNotifyInfo *notify;
MayhemDataChannelsUnion dataChannels;
}
;

struct MayhemObject
{
unsigned int modelIndex;
unsigned int animIndex;
}
;

struct MayhemData
{
const char *name;
XModel *preModel;
XModel *postModel;
unsigned int numModels;
MayhemModel *models;
unsigned int numAnims;
MayhemAnim *anims;
unsigned int numObjects;
MayhemObject *objects;
float length;
}
;

struct XAnimCurveControlPoint
{
float time;
float value;
float invTimeDelta;
}
;

struct XAnimCurve
{
const char *name;
XAnimCurveControlPoint *controlPoints;
int numControlPoints;
char curveType;
float duration;
}
;

struct FootstepVFX
{
const char *name;
FxCombinedDef footstepVFX[64];
FxCombinedDef smallFootstepVFX[64];
}
;

struct ScriptableStateDef
{
ScriptableStateBaseDef base;
Scriptable_StateType type;
ScriptableStateDefUnion data;
}
;

struct ScriptableDamageTagOverride
{
const char *tag;
scr_string_t scrTag;
}
;

struct XAnimParameterBinding
{
char type;
unsigned __int16 destAnimIndex;
unsigned __int16 destParameterOffset;
unsigned __int16 destParameterSize;
$21B5B2324A2AFD50A7F974C00D42E0F0 ___u4;
}
;

struct __declspec(align(8)) XAnimPublicNode
{
const scr_string_t *name;
unsigned __int16 nodeIndex;
char nodeType;
}
;

struct XAnim_s
{
unsigned int size;
bool initialized;
bool dirtyBindings;
char *debugName;
char **debugAnimNames;
unsigned __int16 numGameParameters;
unsigned __int16 maxGameParameters;
unsigned __int16 numBindings;
unsigned __int16 maxBindings;
const scr_string_t **gameParameterNames;
XAnimParameterBinding *bindings;
XAnimPublicNode *publicNodes;
unsigned __int16 numPublicNodes;
unsigned __int16 maxPublicNodes;
char subTreeID[2];
char numLods;
float lodDistances[4];
XAnimEntry entries[1];
}
;

struct XAnimConstraint
{
char type;
char localTargetBoneIndex;
scr_string_t sourceBoneNames[2];
XAnimConstraintUnion u;
}
;

struct XAnimProceduralBones
{
const char *name;
XAnimConstraint *constraints;
unsigned int numConstraints;
unsigned int numTargetBones;
scr_string_t *targetBoneNames;
unsigned int unk01_count;
unk_1453E1E68 *unk01;
unsigned int unk02_count;
unk_1453E1E30 *unk02;
int unk_04;
unsigned int unk03_count;
unk_1453E1EA8 *unk03;
}
;

struct XAnimDynamicBoneProperties
{
scr_string_t boneName;
int parentDynamicBoneIndex;
int dynamicBoneChainRootIndex;
float chainLength;
float invMass;
vec3_t centerOfMass;
float invInertiaTensor;
vec3_t twistAxis;
float minTwistAngle;
float maxTwistAngle;
float damping;
float gravityScale;
float cosMaxSwingAngle;
vec4_t localBasePoseQuat;
vec3_t localBasePoseTrans;
unsigned int firstCollisionShapeIndex;
unsigned int numCollisionShapes;
SndAliasLookup lightHitSound;
SndAliasLookup heavyHitSound;
float lightHitThreshold;
float heavyHitThreshold;
bool enableSwingLimit;
bool enableTwistLimit;
bool enableRotationalSpring;
bool enableTranslationalSpring;
XAnimRotationalSpringProperties rotationalSpring;
XAnimTranslationalSpringProperties translationalSpring;
XAnimDynamicBoneNoise noise;
}
;

struct XAnimCollidableBone
{
scr_string_t boneName;
unsigned int firstCollisionShapeIndex;
unsigned int numCollisionShapes;
}
;

struct XAnimCollisionShape
{
char type;
XAnimCollisionShapeUnion u;
}
;

struct XAnimCustomPinConstraint
{
int dynamicBoneIndexA;
int dynamicBoneIndexB;
int boneNameIndexA;
vec3_t localTranslationA;
vec3_t localTranslationB;
float distance;
float stiffness;
}
;

struct XAnimDynamicBones
{
const char *name;
unsigned int numDynamicBones;
XAnimDynamicBoneProperties *dynamicBoneProperties;
unsigned int numCollidableBones;
XAnimCollidableBone *collidableBones;
unsigned int numCollidableBoneCollisionShapes;
unsigned int numCollisionShapes;
XAnimCollisionShape *collisionShapes;
unsigned int numCustomPinConstraints;
XAnimCustomPinConstraint *customPinConstraints;
unsigned int numPinConstraintBoneNames;
scr_string_t *pinConstraintBoneNames;
}
;

struct DObjAnimMat
{
float quat[4];
float trans[3];
float transWeight;
}
;

struct ReactiveMotionModelPartTweaks
{
vec4_t ampScale;
vec4_t freqScale;
}
;

struct ReactiveMotionModelPart
{
float center[3];
float stiffness;
}
;

struct __declspec(align(8)) ReactiveMotionModelInfo
{
ReactiveMotionWindParams windParams;
ReactiveMotionModelPartTweaks *modelPartTweaks;
ReactiveMotionModelPart *modelParts;
GfxWrappedBaseBuffer modelPartsBuffer;
char idk[24];
char numParts;
char flags;
char pad[2];
}
;

struct XBoneInfo
{
Bounds bounds;
union
{
float radiusSquared;
unsigned int radiusSquaredAsInt;
};
}
;

struct XModelDetailCollision
{
const char *name;
unsigned int physicsLODDataSize;
char *physicsLODData;
unsigned int physicsLODDataNameCount;
scr_string_t *physicsLODDataNames;
}
;

struct __declspec(align(8)) ClothAssetBoneMapping
{
scr_string_t *scrTagNames;
unsigned int scrTagCount;
}
;

struct __declspec(align(8)) ClothAssetBoneMappingSet
{
ClothAssetBoneMapping *boneMappings;
unsigned int boneMappingCount;
}
;

struct __declspec(align(8)) ClothAssetSimulationLOD
{
float maxDistanceSq;
float hkMaxDistanceSq;
const char *stateName;
unsigned int stateIdx;
}
;

struct ClothAssetSound
{
float minSpeed;
float maxSpeed;
float pitchMinSpeed;
float pitchMaxSpeed;
float volumeMinSpeed;
float volumeMaxSpeed;
const char *soundAlias;
}
;

struct __declspec(align(8)) ClothAsset
{
const char *name;
bool standAlone;
unsigned int havokDataSize;
char *havokData;
ClothAssetBoneMappingSet *boneMappingSets;
unsigned int boneMappingSetCount;
const char *animationStateName;
unsigned int animationStateIdx;
ClothAssetSimulationLOD *simulationLods;
unsigned int simulationLodCount;
int surfaceTypeIndex;
bool vectorFieldSupport;
float vectorFieldDrag;
bool bulletSupport;
int bulletProxyUpdateRate;
float bulletForce;
float defaultSpeed;
float speedBlendSpeed;
float soundIWMaxDistanceSq;
float soundHKMaxDistanceSq;
ClothAssetSound *sounds;
unsigned int soundCount;
}
;

struct BlendShapeWeightMap
{
unsigned __int16 weightIndex;
unsigned __int16 targetIndex;
float fullWeight;
}
;

struct XModelBlendShapeInfo
{
unsigned __int16 targetCount;
unsigned __int16 numberOfWeights;
unsigned __int16 numberOfWeightMaps;
scr_string_t *weightNames;
BlendShapeWeightMap *weightMaps;
}
;

struct MdaoVolume
{
ExtentBounds bounds;
unsigned __int16 cellCount[3];
unsigned __int16 parentBoneIndex;
GfxImage *volumeData;
}
;

struct GfxDecalVolumeMask
{
const char *name;
GfxImage *blendMap;
}
;

struct __declspec(align(8)) XModelDecalVolume
{
GfxDecalVolumeObb obb;
Material *material;
GfxDecalVolumeMask *blendMapOverride;
int boneIndex;
vec2_t uvOffset;
vec4_t uvMatrix;
}
;

struct __declspec(align(8)) XModelDecalVolumesInfo
{
XModelDecalVolume *decalVolumes;
unsigned int numDecalVolumes;
}
;

struct ComputeShader
{
const char *name;
const char *debugName;
ComputeShaderProgram prog;
}
;

struct snd_volmod_info_t
{
char name[64];
SndStringHash id;
float value;
}
;

struct snd_entchannel_info_t
{
char name[64];
SndStringHash id;
int priority;
SndEntchannelSpaitalType spatialType;
bool isRestricted;
bool isPausable;
int maxVoices;
SndLimitBehavior limitBehavior;
bool centerSpeakerPan;
SndFarReverbBehavior mpFarReverbBehavior;
SndFarReverbBehavior spFarReverbBehavior;
SndFarReverbBehavior cpFarReverbBehavior;
}
;

struct SndContext
{
char *type;
SndStringHash typeId;
int numValues;
int startIndex;
bool allowDefaults;
bool includeDefaultsInRandom;
bool canBlend;
}
;

struct SndContextValue
{
char *value;
SndStringHash valueId;
}
;

struct SndMaster
{
char name[64];
SndStringHash id;
SndMasterEqParamBank dialogEq;
SndMasterEqParamBank weapExpScenesEq;
SndMasterEqParamBank sfxEq;
SndMasterEqParamBank mainEq;
SndDynamicsParams dialogDyn;
SndDynamicsParams sfxDyn;
SndDynamicsParams mainDyn;
float busDialogGain;
float busWeapExpSceneGain;
float busSfxGain;
float busReverbGain;
float busMusicGain;
float busMovieGain;
float busReferenceGain;
int busDialogEnable;
int busWeapExpSceneEnable;
int busSfxEnable;
int busReverbEnable;
int busMusicEnable;
int busMovieEnable;
int busReferenceEnable;
}
;

struct SndWhizby
{
char name[64];
unsigned int id;
float radiusNear;
float radiusMedium;
float radiusFar;
float spreadNear;
float spreadMedium;
float spreadFar;
float whizbyInTimeNear;
float whizbyInTimeMedium;
float whizbyInTimeFar;
float shooterDistNear;
float shooterDistMedium;
float shooterDistFar;
bool isSuperSonic;
float crackMaxAngle;
float bulletSpeed;
float speedOfSound;
}
;

struct snd_perk_info_t
{
char name[64];
unsigned int id;
float volume;
}
;

struct SndGlobals
{
const char *name;
float globalVolumeModifier;
float binkVolumeModifier;
unsigned int volmodinfoCount;
snd_volmod_info_t *volmodinfo;
unsigned int entchannelCount;
snd_entchannel_info_t *entchannelInfo;
unsigned int dopplerPresetCount;
DopplerPreset *dopplerPresets;
unsigned int occlusionShapeCount;
OcclusionShape *occlusionShapes;
unsigned int vfCurveCount;
SndCurve *vfCurves;
unsigned int lpfCurveCount;
SndCurve *lpfCurves;
unsigned int hpfCurveCount;
SndCurve *hpfCurves;
unsigned int rvbCurveCount;
SndCurve *rvbCurves;
unsigned int speakerMapCount;
SpeakerMap *speakerMaps;
unsigned int contextCount;
SndContext *contexts;
unsigned int contextValueCount;
SndContextValue *contextValues;
unsigned int masterCount;
SndMaster *masters;
}
;

struct SndBankResident
{
SndBank bank;
}
;

struct StaticModelCollisionInstance
{
vec3_t origin;
vec3_t angles;
float scale;
}
;

struct StaticModelCollisionModel
{
scr_string_t name;
PhysicsAsset *physicsAsset;
XModelDetailCollision *detailCollision;
unsigned int numInstances;
StaticModelCollisionInstance *instances;
}
;

struct StaticModelCollisionModelList
{
unsigned int numModels;
StaticModelCollisionModel *models;
unsigned int structureMemoryCost;
}
;

struct TriggerModel
{
int contents;
unsigned __int16 hullCount;
unsigned __int16 firstHull;
unsigned __int16 windingCount;
unsigned __int16 firstWinding;
unsigned int flags;
PhysicsAsset *physicsAsset;
unsigned __int16 physicsShapeOverrideIdx;
}
;

struct TriggerHull
{
Bounds bounds;
int contents;
unsigned __int16 slabCount;
unsigned __int16 firstSlab;
}
;

struct TriggerSlab
{
vec3_t dir;
float midPoint;
float halfSize;
}
;

struct TriggerWinding
{
vec3_t dir;
unsigned __int16 pointCount;
unsigned __int16 firstPoint;
unsigned int flags;
}
;

struct TriggerWindingPoint
{
vec3_t loc;
}
;

struct ClientTriggerModel
{
unsigned __int16 hullCount;
unsigned int firstHull;
unsigned int flags;
}
;

struct ClientTriggerHull
{
Bounds triggerSpaceBounds;
unsigned __int16 slabCount;
unsigned int firstSlab;
}
;

struct CTAudRvbPanInfo
{
bool hasCustomPosition;
vec3_t position;
float maxWallDistance;
float minReverbVolume;
}
;

struct ClientEntityLinkToDef
{
int anchorIndex;
vec3_t originOffset;
vec3_t angleOffset;
}
;

struct SpatialPartition_Tree
{
char *tree;
unsigned int *alwaysList;
unsigned int treeSize;
unsigned int alwaysListLength;
}
;

struct ClientTriggerBlendNode
{
vec3_t pointA;
vec3_t pointB;
unsigned __int16 triggerA;
unsigned __int16 triggerB;
}
;

struct SpawnPointEntityRecord
{
unsigned __int16 index;
scr_string_t name;
scr_string_t target;
scr_string_t script_noteworthy;
vec3_t origin;
vec3_t angles;
}
;

struct SplinePointEntityRecord
{
int splineId;
int splineNodeId;
scr_string_t splineNodeLabel;
scr_string_t targetname;
scr_string_t target;
scr_string_t string;
float speed;
float splineNodeTension;
vec3_t origin;
vec3_t angles;
float throttle;
vec2_t corridorDims;
vec3_t tangent;
float distToNextNode;
vec3_t positionCubic[4];
vec3_t tangentQuadratic[3];
}
;

struct SplinePointRecordList
{
unsigned __int16 splinePointCount;
float splineLength;
SplinePointEntityRecord *splinePoints;
}
;

struct cmodel_t
{
Bounds bounds;
float radius;
ClipInfo *info;
PhysicsAsset *physicsAsset;
unsigned __int16 physicsShapeOverrideIdx;
unsigned __int16 navObstacleIdx;
unsigned int edgeFirstIndex;
}
;

struct FlaggedEdgeMetadata
{
float packedAdjacentNormal;
unsigned __int16 packedOpenAngle;
char flags[1];
}
;

struct PMROctreeMetadata
{
float4 nodeHalfWidth[16];
float4 minExtent;
PMROctreeNode rootNode;
unsigned int nodeSetCount;
float nodeBoundingRadiusSq[16];
char edgeBucket[1];
char flags[1];
char pad[10];
}
;

struct PMROctreeNodeSet
{
PMROctreeNode node[8];
}
;

struct EdgeAdjacencyMetadata
{
unsigned __int32 startIndex : 23;
unsigned __int32 mountCount : 2;
unsigned __int32 interactionCount : 2;
}
;

struct EdgeAdjacency
{
unsigned __int16 start;
unsigned __int16 end;
unsigned int adjEdge;
unsigned __int16 adjStart;
unsigned __int16 adjEnd;
}
;

struct MapEdgeList
{
const char *name;
vec3_t mins;
vec3_t maxs;
unsigned int transientIndex;
unsigned int dynamicQueryTypes;
unsigned int staticQueryTypes;
unsigned int numDynamicEdges;
unsigned int numStaticEdges;
bool valid;
unsigned int numEdges;
float4 (*edges)[2];
FlaggedEdgeMetadata *edgeMetadata;
unsigned int numEdgeOctrees;
PMROctreeMetadata *edgeOctrees;
unsigned int numEdgeOctreeNodeSet;
PMROctreeNodeSet *edgeOctreeNodeSets;
unsigned int numEdgeIndices;
unsigned int *edgeIndices;
unsigned int numEdgeAdjacencyMetadata;
EdgeAdjacencyMetadata *edgeAdjacencyMetadata;
unsigned int numEdgeAdjacency;
EdgeAdjacency *edgeAdjacency;
}
;

struct CM_ClientModel
{
vec3_t spawnOrigin;
vec3_t spawnAngles;
const XModel *model;
scr_string_t animationTreeName;
XAnimParts *animation;
scr_string_t name;
bool noPhysics;
bool noCloth;
}
;

struct SpatialPartition_Population_Partition
{
float dist;
unsigned __int32 axis : 2;
unsigned __int32 rightChild : 29;
}
;

struct SpatialPartition_Population_Tree
{
SpatialPartition_Population_Partition *partitions;
ExtentBounds *extents;
unsigned int partitionCount;
}
;

struct SpatialPartition_Population
{
const SpatialPartition_Population_Tree *tree;
SpatialPartition_Population_Node *buckets;
SpatialPartition_Population_Node *(__fastcall *getNodeFromIndex)(const SpatialPartition_Population *, unsigned int);
unsigned int (__fastcall *getIndexFromNode)(const SpatialPartition_Population *, const SpatialPartition_Population_Node *);
void *userData;
unsigned int partitionCount;
unsigned int currentPopulation;
bool bucketsAreChanging;
}
;

struct SpatialPartition_Population_TransientMap
{
unsigned int transientCount;
bitarray_dynamic *partitionsForTransient;
}
;

struct ClientEntAnchor
{
scr_string_t name;
int entNum;
}
;

struct ScriptableInstance
{
unk_1453E2520 unk01;
unk_1453E2530 unk02[2];
scr_string_t unk03;
int flags;
char __pad0[8];
const char *unk04;
scr_string_t targetname;
}
;

struct ScriptableReservedDynent
{
unsigned int dynentId;
__int16 next;
}
;

struct SpawnGroupPoint
{
vec3_t origin;
vec3_t angles;
unsigned __int16 cluster;
unsigned __int16 zone;
}
;

struct SpawnGroupCluster
{
unsigned __int16 zone;
unsigned __int16 pointCount;
unsigned __int16 *points;
Bounds bounds;
}
;

struct SpawnGroupZone
{
scr_string_t name;
unsigned __int16 pointCount;
unsigned __int16 *points;
unsigned __int16 clusterCount;
unsigned __int16 *clusters;
Bounds bounds;
}
;

struct ClientOneshotEffectDef
{
vec3_t origin;
vec3_t angles;
float radius;
int delayMsec;
FxCombinedDef effect;
SndAliasLookup effectSound;
SndAliasList *aliasList;
}
;

struct ClientExploderDef
{
vec3_t origin;
vec3_t angles;
int delayMsec;
FxCombinedDef effect;
SndAliasLookup effectSound;
SndAliasList *aliasList;
scr_string_t name;
}
;

struct ClientIntervalSoundDef
{
vec3_t origin;
vec3_t angles;
SndAliasLookup effectSound;
SndAliasList *aliasList;
int delayMinMsec;
int delayMaxMsec;
}
;

struct ClientLoopSoundDef
{
vec3_t origin;
vec3_t angles;
SndAliasLookup effectSound;
SndAliasList *aliasList;
}
;

struct ClientReactiveEntDef
{
vec3_t origin;
vec3_t angles;
FxCombinedDef effect;
SndAliasLookup effectSound;
SndAliasList *aliasList;
float radius;
}
;

struct ClientRootDef
{
scr_string_t path;
vec3_t origin;
vec3_t angles;
float radius;
}
;

struct CreateFxPaletteDef
{
scr_string_t aliasName;
scr_string_t effectPath;
unsigned __int16 refCount;
}
;

struct ServerExploderDef
{
vec3_t origin;
int delayMsec;
float damageAmount;
float damageRadius;
scr_string_t name;
scr_string_t earthquakeName;
scr_string_t rumbleName;
bool damageEnvironmentOnly;
bool damageDoOcclusionTraces;
}
;

struct MayhemInstance
{
GfxPlacement initialPose;
MayhemData *mayhem;
ClientEntityLinkToDef *linkTo;
scr_string_t scriptName;
float curTime;
float oldTime;
unsigned int initialFlags;
unsigned int flags;
unsigned int mapEntLookup;
}
;

struct SpawnerField
{
scr_string_t key;
unsigned int keyCanonical;
scr_string_t value;
unsigned __int8 type;
}
;

struct Spawner
{
vec3_t origin;
vec3_t angles;
SpawnerField *fields;
int flags;
int count;
unsigned int numFields;
scr_string_t targetname;
scr_string_t classname;
}
;

struct AudioPASpeaker
{
vec3_t origin;
unsigned int nameHash;
unsigned int entChannelHash;
}
;

struct AudioPropagationNode
{
vec3_t position;
unsigned __int16 numEdges;
unsigned int edgeStartIndex;
}
;

struct AudioPropagationEdge
{
__int16 neighbor;
__int16 portalIndex;
}
;

struct CollMapLookupEntry
{
const char *targetName;
int brushIndex;
}
;

struct MapEnts
{
const char *name;
char *entityString;
int numEntityChars;
MapTriggers trigger;
ClientTriggers clientTrigger;
ClientTriggerBlend clientTriggerBlend;
SpawnPointRecordList spawnList;
SplineRecordList splineList;
unsigned int havokEntsShapeDataSize;
char *havokEntsShapeData;
unsigned int numSubModels;
cmodel_t *cmodels;
unsigned __int16 dynEntCount[2];
unsigned __int16 dynEntCountTotal;
DynEntityDef *dynEntDefList[2];
DynEntityPose *dynEntPoseList[2][2];
DynEntityClient *dynEntClientList[2][2];
__int16 unkIndexes[8];
DynEntityGlobalId *dynEntGlobalIdList[2];
char __pad1[8];
unsigned int unk2Count;
unk_1453E4298 *unk2;
unk_1453E42A8 *unk2_1[2];
unk_1453E1130 *unk2_2[2];
unsigned int unk3Count;
unsigned int *unk3;
unsigned int clientEntAnchorCount;
ClientEntAnchor *clientEntAnchors;
ScriptableMapEnts scriptableMapEnts;
unsigned int numMayhemScenes;
MayhemInstance *mayhemScenes;
SpawnerList spawners;
unsigned int audioPASpeakerCount;
AudioPASpeaker *audioPASpeakers;
}
;

struct Stage
{
const char *name;
vec3_t origin;
unsigned __int16 triggerIndex;
char sunPrimaryLightIndex;
unsigned int entityUID;
char unk[20];
}
;

struct CollisionHeatmapEntry
{
int vertexCount;
float vertexDensity;
vec3_t minExtent;
vec3_t maxExtent;
}
;

struct TopDownMapData
{
TopDownMapDataHeader header;
TopDownMapDataContent content;
}
;

struct clipMap_t
{
const char *name;
int isInUse;
unsigned int numStaticModelCollisionModelLists;
StaticModelCollisionModelList *staticModelCollisionModelLists;
MapEnts *mapEnts;
Stage *stages;
unsigned __int8 stageCount;
MapTriggers stageTrigger;
vec3_t broadphaseMin;
vec3_t broadphaseMax;
PhysicsCapacities physicsCapacities;
unsigned int havokWorldShapeDataSize;
char *havokWorldShapeData;
unsigned int numCollisionHeatmapEntries;
CollisionHeatmapEntry *collisionHeatmap;
unsigned int totalGlassInitPieceCount;
unsigned int totalGlassPieceLimit;
TopDownMapData *topDownMapData;
const char *visionSetName;
unsigned int checksum;
}
;

struct ComPrimaryLight
{
GfxLightType type;
unsigned __int8 canUseShadowMap;
unsigned __int8 needsDynamicShadows;
unsigned __int8 isVolumetric;
unsigned __int8 exponent;
char unused[3];
unsigned int transientZoneList;
unsigned int entityId;
float uvIntensity;
float irIntensity;
float color[3];
float dir[3];
float up[3];
float origin[3];
float radius;
vec2_t fadeOffsetRt;
float bulbRadius;
vec3_t bulbLength;
float cosHalfFovOuter;
float cosHalfFovInner;
float shadowSoftness;
float shadowBias;
float shadowArea;
float distanceFalloff;
float rotationLimit;
float translationLimit;
const char *defName;
}
;

struct ComWorld
{
const char *name;
int isInUse;
int useForwardPlus;
unsigned int bakeQuality;
unsigned int primaryLightCount;
ComPrimaryLight *primaryLights;
unsigned int scriptablePrimaryLightCount;
unsigned int firstScriptablePrimaryLight;
unsigned int primaryLightEnvCount;
ComPrimaryLightEnv *primaryLightEnvs;
ComChangeListInfo changeListInfo;
unsigned int numUmbraGates;
const char **umbraGateNames;
int umbraGateInitialStates[4];
}
;

struct G_GlassPiece
{
unsigned __int16 damageTaken;
unsigned __int16 collapseTime;
int lastStateChangeTime;
char impactDir;
char impactPos[2];
}
;

struct G_GlassName
{
char *nameStr;
scr_string_t name;
unsigned __int16 pieceCount;
unsigned __int16 *pieceIndices;
}
;

struct G_GlassData
{
G_GlassPiece *glassPieces;
unsigned int pieceCount;
unsigned __int16 damageToWeaken;
unsigned __int16 damageToDestroy;
unsigned int glassNameCount;
G_GlassName *glassNames;
}
;

struct GlassWorld
{
const char *name;
G_GlassData *g_glassData;
}
;

struct pathlink_s
{
float fDist;
unsigned __int16 nodeNum;
char disconnectCount;
char negotiationLink;
char flags;
char ubBadPlaceCount[3];
}
;

struct pathnode_t
{
pathnode_constant_t constant;
char pad[104];
}
;

struct pathnode_tree_t
{
int axis;
float dist;
pathnode_tree_info_t u;
}
;

struct PathDynamicNodeGroup
{
unsigned __int16 parentIndex;
int nodeTreeCount;
pathnode_tree_t *nodeTree;
}
;

struct PathData
{
const char *name;
unsigned int nodeCount;
pathnode_t *nodes;
bool parentIndexResolved;
unsigned __int16 version;
int visBytes;
char *pathVis;
int nodeTreeCount;
pathnode_tree_t *nodeTree;
int dynamicNodeGroupCount;
PathDynamicNodeGroup *dynamicNodeGroups;
bool usePathExtraData;
int exposureBytes;
char *pathExposure;
int noPeekVisBytes;
char *pathNoPeekVis;
int zoneCount;
int zonesBytes;
char *pathZones;
int fixedNodeCount;
int maxDynamicSpawnedNodeCount;
int dynStatesBytes;
char *pathDynStates;
int visDistDataSize;
}
;

struct nav_space_s
{
SpaceHandle hSpace;
int parentEntNum;
scr_string_t name;
nav_link_s linkList;
int numLinks;
nav_obstacle_s obstacleList;
int numObstacles;
nav_repulsor_s repulsorList;
int numRepulsors;
nav_resource_s resourceList;
int numResources;
__int16 nextIdx;
__int16 prevIdx;
}
;

struct nav_boundary_plane_s
{
vec3_t m_Normal;
float m_DistFromCenter;
}
;

struct nav_resource_s
{
scr_string_t targetName;
int modelIdx;
int targetEntNum;
void *pSpace;
int graphSize;
vec3_t localOffsetPos;
vec4_t localOffsetRot;
int offsetLayer;
nav_resource_s *pPrev;
nav_resource_s *pNext;
unsigned int layerFlags;
bool bDockable;
bool bIsVolume;
char *pGraphBuffer;
char *pWorkingGraph;
}
;

struct nav_obstacle_hull_s
{
nav_boundary_plane_s *m_Boundaries;
int m_NumBoundaries;
float m_BoundsX;
float m_BoundsY;
float m_BoundsZ;
vec3_t m_Offset;
bool m_bUseBounds;
}
;

struct nav_obstacle_bounds_s
{
nav_obstacle_hull_s *m_Hulls;
int m_NumHulls;
}
;

struct nav_glass_bounds_s
{
nav_boundary_plane_s *m_Boundaries;
int m_NumBoundaries;
vec3_t m_Pos;
unsigned int m_GlassPieceIndex;
}
;

struct nav_modifier_s
{
nav_boundary_plane_s *m_Boundaries;
int m_NumBoundaries;
vec3_t m_Pos;
scr_string_t m_TargetName;
unsigned int m_UserFlags;
unsigned int m_LayerFlags;
float m_Weight;
bool m_bActiveOnLoad;
}
;

struct nav_link_creation_data_s
{
nav_link_point_s m_Start;
nav_link_point_s m_End;
scr_string_t m_Animscript;
scr_string_t m_Target;
scr_string_t m_Parent;
unsigned int m_UsageFlags;
float m_PenaltyMult;
bool m_bBidirectional;
}
;

struct nav_raw_volume_s
{
vec3_t m_MidPoint;
vec3_t m_HalfSize;
float m_MinVoxelSize;
float m_ClearanceXY;
float m_ClearanceZ;
}
;

struct nav_raw_custom_volume_tri_s
{
vec3_t m_Verts[3];
}
;

struct nav_raw_custom_volume_s
{
nav_raw_custom_volume_tri_s *m_Tris;
int m_NumTris;
}
;

struct NavMeshData
{
const char *name;
int version;
int hasExposureData;
int numNavResources;
nav_resource_s *navResources;
int numObstacleBounds;
nav_obstacle_bounds_s *obstacleBounds;
int numGlassBounds;
nav_glass_bounds_s *glassBounds;
int numModifiers;
nav_modifier_s *modifiers;
int numLinkCreationData;
nav_link_creation_data_s *linkCreationData;
int numVolumeSeeds;
vec3_t *volumeSeeds;
int numRawVolumes;
nav_raw_volume_s *rawVolumes;
int numRawCustomVolumes;
nav_raw_custom_volume_s *rawCustomVolumes;
}
;

struct tacpoint_t
{
vec3_t m_Pos;
__int16 m_VerticalClearance;
char m_GraphIdx;
char m_Type;
AreaHandle m_hArea;
unsigned __int16 m_SearchNext;
unsigned __int16 m_SearchPrev;
int m_Padding;
}
;

struct TacVisGraphRow
{
unsigned __int16 m_Start;
unsigned __int16 m_End;
char *m_Vis;
}
;

struct TacticalGraph
{
tacpoint_t *m_Points;
unsigned __int16 m_NumPoints;
int m_NumVisBytes;
TacVisGraphRow *m_VisGraph;
tacpoint_search_node_t m_SearchRoot;
}
;

struct TacticalGraphData
{
const char *m_Name;
TacticalGraph *m_TacGraphs;
int m_NumTacGraphs;
}
;

struct FxGlassDef
{
float halfThickness;
vec2_t texVecs[2];
GfxColor color;
Material *material;
Material *materialShattered;
PhysicsAsset *physicsAsset;
FxCombinedDef pieceBreakEffect;
FxCombinedDef shatterEffect;
FxCombinedDef shatterSmallEffect;
FxCombinedDef crackDecalEffect;
SndAliasLookup damagedSound;
SndAliasLookup destroyedSound;
SndAliasLookup destroyedQuietSound;
float highMipRadiusInvSq;
float shatteredHighMipRadiusInvSq;
int numCrackRings;
bool isOpaque;
}
;

struct FxGlassTransientLookup
{
unsigned __int16 globalPieceStartIndex;
unsigned __int16 transientIndex;
}
;

union FxGlassPiecePlace
{
$03A8A7B39FA20F64B5AB79125E07CD62 __s0;
unsigned int nextFree;
}
;

struct FxGlassPieceState
{
float texCoordOrigin[2];
unsigned int supportMask;
unsigned __int16 initIndex;
unsigned __int16 geoDataStart;
unsigned __int16 lightingIndex;
unsigned __int8 defIndex;
unsigned __int8 pad[3];
unsigned __int8 vertCount;
unsigned __int8 holeDataCount;
unsigned __int8 crackDataCount;
unsigned __int8 fanDataCount;
unsigned __int16 flags;
float areaX2;
}
;

struct FxGlassPieceDynamics
{
int fallTime;
unsigned int physicsInstance;
unsigned int physicsDetailInstance;
vec3_t vel;
vec3_t avel;
bool pendingCreation;
}
;

union FxGlassGeometryData
{
FxGlassVertex vert;
FxGlassHoleHeader hole;
FxGlassCrackHeader crack;
unsigned __int8 asBytes[4];
__int16 anonymous[2];
}
;

struct FxGlassInitPieceState
{
FxSpatialFrame frame;
float radius;
vec2_t texCoordOrigin;
unsigned int supportMask;
float areaX2;
unsigned __int16 lightingIndex;
unsigned __int8 defIndex;
unsigned __int8 vertCount;
unsigned __int8 fanDataCount;
unsigned __int8 pad[1];
}
;

struct FxWorldTransientZone
{
const char *name;
unsigned int transientZoneIndex;
FxGlassSystem glassSys;
}
;

struct FxWorld
{
const char *name;
FxGlassSystem glassSys;
}
;

struct GfxWorldDpvsPlane
{
vec3_t normal;
float dist;
char type;
char pad[3];
}
;

struct GfxPortal
{
GfxPortalWritable writable;
DpvsPlane plane;
vec3_t *vertices;
unsigned __int16 cellIndex;
unsigned __int16 closeDistance;
char vertexCount;
vec3_t hullAxis[2];
}
;

struct GfxCell
{
Bounds bounds;
unsigned __int16 portalCount;
GfxPortal *portals;
}
;

struct GfxCellTransientInfo
{
unsigned __int16 aabbTreeIndex;
unsigned __int16 transientZone;
}
;

struct GfxSurface
{
srfTriangles_t tris;
Material *material;
unsigned __int8 lightmapIndex;
unsigned __int8 flags;
unsigned __int16 sortKey;
unsigned __int8 unk1;
unsigned __int8 unk2;
unsigned __int8 unk3;
unsigned __int8 unk4;
unsigned __int8 transientZone;
}
;

struct GfxSurfaceBounds
{
Bounds bounds;
unsigned int unk[3];
}
;

struct GfxWorldSurfData
{
unsigned int transientZone;
unsigned int layerCount;
unsigned int xyzOffset;
unsigned int tangentFrameOffset;
unsigned int lmapCoordOffset;
unsigned int colorOffset;
unsigned int texCoordOffset;
unsigned int normalTransformOffset[7];
unsigned int displacementOffset[8];
}
;

struct GfxStaticModelSurface
{
unsigned __int16 smodelIndex;
char lodIndex;
char surfIndex;
}
;

struct GfxStaticModel
{
XModel *model;
char flags;
char firstMtlSkinIndex;
unsigned __int8 firstStaticModelSurfaceIndex;
}
;

struct GfxClutterCollection
{
unsigned int maxInstanceCount;
unsigned __int16 smodelCollectionIndex;
unsigned __int16 terrainSurfaceIndex;
unsigned __int16 clutterLayerIndex;
unsigned __int16 pad;
}
;

struct GfxStaticModelCollection
{
unsigned int firstInstance;
unsigned int instanceCount;
unsigned __int16 smodelIndex;
unsigned __int16 transientGfxWorldPlaced;
unsigned __int16 clutterIndex;
char flags[1];
char pad;
}
;

struct GfxStaticModelInstance
{
unsigned int lgvIndex_transientIndex;
}
;

struct GfxModelStaticIndirection
{
unsigned int lightingInstanceIndirection;
unsigned __int16 splinedModelInstanceIndex;
unsigned __int16 worldTransient;
}
;

struct GfxLightmapTransformPacked
{
unsigned int topLeft_indexLow_packed;
unsigned int bottomRight_indexHigh_packed;
}
;

struct __cppobj GfxBaseSModelSurfData : GfxBaseSurfData
{
vec3_t boundingBoxHalfSize;
float drawDistance;
}
;

struct GfxSModelInstanceData
{
vec3_t translation;
unsigned int orientation[2];
float scale;
}
;

struct GfxClutterCollectionGPUData
{
unsigned int firstInstance;
unsigned int instanceCount;
}
;

struct GfxStaticModelExpansion
{
unsigned __int16 firstStaticModelSurfaceIndex;
char numSurfs;
char numLods;
Bounds modelBounds;
GfxStaticModelLOD lodInfo[6];
float lodDist[6];
}
;

struct GfxSModelSurfMatIndirection
{
unsigned __int16 smodelMaterialIndex;
char cullType;
char emitType;
}
;

struct GfxReflectionProbe
{
char *livePath;
vec3_t origin;
vec3_t angles;
unsigned int *probeInstances;
unsigned int probeInstanceCount;
unsigned int probeRelightingIndex;
}
;

struct GfxReflectionProbeRelightingData
{
unsigned int reflectionProbeIndex;
unsigned __int16 relightingFlags;
unsigned int gBufferAlbedoImageIndex;
unsigned int gBufferNormalImageIndex;
float relightingScale;
}
;

struct GfxReflectionProbeInstance
{
char *livePath;
char *livePath2;
vec3_t probePosition;
unsigned __int16 probeImageIndex;
char lodLevel;
char flags;
vec4_t probeRotation;
GfxReflectionProbeObb volumeObb;
float priority;
vec3_t feather;
vec3_t expandProjectionNeg;
vec3_t expandProjectionPos;
}
;

struct GfxSH3Vec
{
float coeffs[9];
}
;

struct GfxWorldLightmapReindexDataElement
{
unsigned int index;
unsigned int posX;
unsigned int posY;
unsigned int width;
unsigned int height;
}
;

struct GfxWorldPackedLightmap
{
unsigned int imageWidth;
unsigned int imageHeight;
}
;

struct GfxStaticDecalVolumeMaterial
{
Material *material;
}
;

struct GfxStaticDecalVolumeMask
{
GfxDecalVolumeMask *blendMapOverride;
}
;

struct GfxDecalVolumeCollection
{
char __pad0[388];
}
;

struct GfxCellTreeCount
{
int aabbTreeCount;
}
;

struct GfxAabbTree
{
Bounds bounds;
unsigned __int16 childCount;
unsigned __int16 surfaceCount;
unsigned int startSurfIndex;
unsigned __int16 smodelIndexCount;
unsigned __int16 *smodelIndexes;
int childrenOffset;
}
;

struct GfxCellTree
{
GfxAabbTree *aabbTree;
}
;

struct GfxLightmapAtlasDataTemp
{
unsigned int transientZone;
unsigned int numLightmaps;
unsigned int firstLightmapIndex;
GfxLightmapType lightmapType;
GfxLightmapRaw lightmaps[32];
}
;

struct GfxSHCompressedProbeData
{
unsigned int data[8];
}
;

struct GfxGpuLightGridZone
{
unsigned int numProbes;
unsigned int firstProbe;
unsigned int numTetrahedrons;
unsigned int firstTetrahedron;
unsigned int firstVoxelTetrahedronIndex;
unsigned int numVoxelTetrahedronIndices;
GfxSHProbeData fallbackProbeData;
}
;

struct GfxVoxelTreeHeader
{
int rootNodeDimension[4];
int nodeCoordBitShift[4];
vec4_t boundMin;
vec4_t boundMax;
}
;

struct GfxVoxelTopDownViewNode
{
int firstNodeIndex;
int zMin;
int zMax;
}
;

struct GfxVoxelInternalNode
{
int firstNodeIndex[2];
int childNodeMask[2];
}
;

struct GfxGpuLightGridVoxelTree
{
int topDownViewNodeCount;
int internalNodeCount;
GfxVoxelTreeHeader *treeHeader;
GfxVoxelTopDownViewNode *topDownViewNodes;
GfxVoxelInternalNode *internalNodes;
GfxWrappedBuffer treeHeaderBuffer;
GfxWrappedBuffer topDownViewNodeBuffer;
GfxWrappedBuffer internalNodeBuffer;
}
;

struct GfxGpuLightGridTetrahedron
{
unsigned int indexFlags[4];
}
;

struct GfxGpuLightGridTetrahedronNeighbors
{
unsigned int neighbors[4];
}
;

struct GfxGpuLightGridTetrahedronVisibility
{
unsigned int visibility[16];
}
;

struct GfxGpuLightGridVoxelStartTetrahedron
{
unsigned int index;
}
;

struct GfxGpuLightGrid
{
unsigned int probeCount;
GfxSHCompressedProbeData *probes;
GfxWrappedRWBuffer probesBuffer;
vec3_t *probePositions;
GfxWrappedBuffer probePositionsBuffer;
bool fallbackProbeUseCustomSample;
vec3_t fallbackProbeCustomSamplePos;
unsigned int zoneCount;
GfxGpuLightGridZone *zones;
GfxGpuLightGridVoxelTree *zoneVoxelTrees;
unsigned int tetrahedronCount;
unsigned int tetrahedronCountVisible;
GfxGpuLightGridTetrahedron *tetrahedrons;
GfxWrappedBuffer tetrahedronBuffer;
GfxGpuLightGridTetrahedronNeighbors *tetrahedronNeighbors;
GfxWrappedBuffer tetrahedronNeighborsBuffer;
GfxGpuLightGridTetrahedronVisibility *tetrahedronVisibility;
GfxWrappedBuffer tetrahedronVisibilityBuffer;
unsigned int voxelStartTetrahedronCount;
GfxGpuLightGridVoxelStartTetrahedron *voxelStartTetrahedron;
GfxWrappedBuffer voxelStartTetrahedronBuffer;
vec4_t boundingVolume[7];
}
;

struct GfxStaticModelLightGridVolume
{
unsigned int firstProbe;
unsigned __int16 m_volumeSize[3];
char flags[2];
unsigned int singleProbe;
vec4_t volumeTransform[3];
}
;

struct GfxGpuDynLightsetsLGVReference
{
unsigned int volumeIndex;
unsigned int volumeProbeIndex;
}
;

struct GfxDynamicLightsetGChunk
{
GfxDynamicLightsetGChunkInfo chunkInfo;
char flags[2];
unsigned __int16 whichLM;
unsigned int bfChunkStart;
}
;

struct GfxDynamicLightsetLGppZoneInfo
{
unsigned int numOrigSMLGppProbes;
GfxGpuDynLightsetsLGVReference *lgvReferences;
GfxWrappedBuffer lgvReferencesBuffer;
GfxDynamicLightsetProbeData lgvProbeData;
}
;

struct LightGridVolumePackingParams
{
vec3_t m_uvScale;
char m_volumeSize[3];
char m_basisType;
vec3_t m_uvBias;
unsigned int m_firstStaticProbeIndex;
}
;

struct LightGridVolumeTransformParams
{
vec4_t m_volumeTransform[3];
unsigned int m_historyEntryOrStaticFlags;
unsigned int m_lastFrameHistoryEntryAndSetup;
unsigned int m_lodAndSamplingOffsetZ;
unsigned int m_samplingOffsetXY;
}
;

struct GfxLightGridVolumeAtlasDataTemp
{
unsigned int m_widthAndHeight;
unsigned int m_depth;
unsigned int m_numVolumes;
unsigned __int16 m_worldTransient;
GfxImageRaw m_textures[2];
LightGridVolumePackingParams *m_packingParams;
GfxWrappedBuffer m_packingParamsBuffer;
LightGridVolumeTransformParams *m_transformParams;
GfxWrappedBuffer m_transformParamsBuffer;
}
;

struct GfxImageReflectionProbe
{
unsigned int totalSize;
unsigned int elementCount;
GfxTextureId textureId;
char *pixels;
}
;

struct GfxReflectionProbeTransientData
{
unsigned __int16 reflectionProbeInstanceCount;
unsigned __int16 exclusiveProbeBucketCount;
unsigned int exclusiveProbeCount;
unsigned __int16 *reflectionProbeInstances;
GfxImageReflectionProbe *exclusiveProbeBuckets;
}
;

struct GfxStaticDecalVolumePacked
{
vec3_t obbCenter;
unsigned int obbQuat;
unsigned int obbHalfSize;
unsigned int opacityParams;
unsigned int uvMatrixAndOffset[3];
unsigned int packedMaterialInfo;
unsigned int reserved;
}
;

struct GfxStaticDecalVolumeUid
{
unsigned int uid;
}
;

struct GfxDecalVolumeTransientData
{
unsigned int decalVolumeCount;
unsigned __int16 firstCollection;
unsigned __int16 collectionCount;
GfxStaticDecalVolumePacked *decalVolumes;
GfxWrappedBuffer decalVolumesBuffer;
Bounds *collectionBounds;
GfxStaticDecalVolumeUid *decalVolumeUids;
}
;

struct GfxCompressedSunShadowDataTemp
{
unsigned __int16 zoneIndex;
unsigned __int16 tilesCount;
unsigned int *tilesIndices;
unsigned int *tilesOffsets;
unsigned int tilesDataSize;
char *tilesData;
GfxWrappedBuffer tilesDataBuffer;
}
;

struct GfxWorldTransientZone
{
const char *name;
unsigned int transientZoneIndex;
unsigned int vertexCount;
GfxWorldVertexData vd;
unsigned int vertexLayerDataSize;
GfxWorldVertexLayerData vld;
unsigned int cellCount;
GfxCellTreeCount *aabbTreeCounts;
GfxCellTree *aabbTrees;
}
;

struct GfxVolumetric
{
char *livePath;
unsigned int flags;
GfxOrientedBoundingBox obb;
float density;
float falloff;
GfxVolumetricMask masks[4];
}
;

struct GfxDynamicLightsetBFData
{
float curValue[3];
float lastValue[3];
char bfType[2];
unsigned __int16 numRec;
unsigned int startRec;
}
;

struct GfxDynamicLightsetLightInfo
{
float baseIntensity[3];
float lastIntensity[3];
unsigned int whichPrimary;
}
;

struct GfxDynamicLightsetPortalGIData
{
unsigned int whichEntity;
vec4_t baseQuat;
vec4_t extraInfo;
}
;

struct GfxDynamicLightsetProductGIData
{
unsigned __int16 whichDLS;
unsigned __int16 whichPortal;
}
;

struct GfxLightViewFrustum
{
unsigned int planeCount;
vec4_t *planes;
unsigned int indexCount;
unsigned __int16 *indices;
unsigned int vertexCount;
vec3_t *vertices;
}
;

struct GfxLight
{
char type;
char canUseShadowMap;
char needsDynamicShadows;
char isVolumetric;
float uvIntensity;
float irIntensity;
float heatIntensity;
float intensity;
vec3_t colorLinearSrgb;
vec3_t dir;
vec3_t up;
vec3_t origin;
float radius;
vec2_t fadeOffsetRt;
float bulbRadius;
vec3_t bulbLength;
float cosHalfFovOuter;
float cosHalfFovInner;
unsigned int entityId;
unsigned int flags;
float tonemappingScaleFactor;
float shadowNearPlaneBias;
float shadowSoftness;
float shadowBias;
float shadowArea;
float distanceFalloff;
float cosHalfFovCollimation;
GfxLightDef *def;
}
;

struct GfxVoxelLeafNode
{
unsigned __int16 lightListAddress;
}
;

struct GfxVoxelTree
{
Bounds zoneBound;
int voxelTopDownViewNodeCount;
int voxelInternalNodeCount;
int voxelLeafNodeCount;
int lightListArraySize;
GfxVoxelTreeHeader *voxelTreeHeader;
GfxVoxelTopDownViewNode *voxelTopDownViewNodeArray;
GfxVoxelInternalNode *voxelInternalNodeArray;
GfxVoxelLeafNode *voxelLeafNodeArray;
unsigned __int16 *lightListArray;
unsigned int *voxelInternalNodeDynamicLightList;
char __pad0[24];
}
;

struct GfxHeightfield
{
Bounds bounds;
vec4_t lookupMatrix[4];
GfxImage *image;
}
;

struct GfxBrushModel
{
GfxBrushModelWritable writable;
Bounds bounds;
float radius;
unsigned int startSurfIndex;
unsigned __int16 surfaceCount;
}
;

struct GfxModelLightmapInst
{
unsigned __int16 lmapIndex;
vec2_t offset;
float scaleWidth;
float scaleHeight;
}
;

struct GfxMapEntLookup
{
unsigned int lmapLookup;
vec3_t probesOffset;
unsigned int flags;
unsigned int lgvData;
int portalGISlot;
}
;

struct GfxSceneDynModel
{
XModelDrawInfo info;
unsigned __int16 dynEntId;
}
;

struct GfxSceneDynBrush
{
BModelDrawInfo info;
unsigned __int16 dynEntId;
}
;

struct GfxShadowGeometry
{
unsigned __int16 surfaceCount;
unsigned __int16 smodelCount;
unsigned int *sortedSurfIndex;
unsigned __int16 *smodelIndex;
}
;

struct GfxLightAABBNode
{
Bounds bound;
unsigned __int16 firstChild;
unsigned __int16 childCount;
}
;

struct UmbraGate
{
unsigned int objID;
Bounds bounds;
float closeDistance;
}
;

struct StreamMtlImgInfo
{
float minHimipInvSqRadii;
unsigned __int16 assetIndex;
unsigned __int16 volumeCount;
}
;

struct StreamTreeNode
{
StreamKey *volumesKey;
StreamMtlImgInfo *infos;
unsigned __int16 childNodes[2];
unsigned __int16 mtlInfoCount;
unsigned __int16 imgInfoCount;
unsigned __int16 transientZone;
unsigned __int16 unused;
ExtentBounds aabb;
float minLomipInvSqRadiiOfChildren;
}
;

struct GfxWorld
{
const char *name;
const char *baseName;
unsigned int bspVersion;
int planeCount;
int nodeCount;
unsigned int surfaceCount;
int skyCount;
GfxSky *skies;
unsigned int lastSunPrimaryLightIndex;
unsigned int primaryLightCount;
unsigned int movingScriptablePrimaryLightCount;
unsigned int sortKeyLitDecal;
unsigned int sortKeyEffectDecal;
unsigned int sortKeyTopDecal;
unsigned int sortKeyEffectAuto;
unsigned int sortKeyDistortion;
unsigned int sortKeyEffectDistortion;
unsigned int sortKey2D;
unsigned int sortKeyOpaqueBegin;
unsigned int sortKeyOpaqueEnd;
unsigned int sortKeyDecalBegin;
unsigned int sortKeyDecalEnd;
unsigned int sortKeyTransBegin;
unsigned int sortKeyTransEnd;
unsigned int sortKeyEmissiveBegin;
unsigned int sortKeyEmissiveEnd;
GfxWorldDpvsPlanes dpvsPlanes;
GfxCellTransientInfo *cellTransientInfos;
GfxCell *cells;
GfxWorldDraw draw;
GfxLightGrid lightGrid;
GfxFrustumLights *frustumLights;
GfxLightViewFrustum *lightViewFrustums;
int voxelTreeCount;
GfxVoxelTree *voxelTree;
int heightfieldCount;
GfxHeightfield *heightfields;
unk_1453E4B08 unk01;
int modelCount;
GfxBrushModel *models;
Bounds bounds;
unsigned int checksum;
int materialMemoryCount;
MaterialMemory *materialMemory;
sunflare_t sun;
float outdoorLookupMatrix[4][4];
GfxImage *outdoorImage;
Material *dustMaterial;
float materialLod0SizeThreshold;
unsigned int *cellCasterBits;
unsigned int *cellHasSunLitSurfsBits;
GfxSceneDynModel *sceneDynModel;
GfxSceneDynBrush *sceneDynBrush;
unsigned int primaryLightMotionDetectBitsEntries;
unsigned int *primaryLightMotionDetectBits;
unsigned int entityMotionBitsEntries;
unsigned int *entityMotionBits;
unsigned int staticSpotOmniPrimaryLightCountAligned;
unsigned int numPrimaryLightEntityShadowVisEntries;
unsigned int *primaryLightEntityShadowVis;
unsigned int dynEntMotionBitsEntries[2];
unsigned int *dynEntMotionBits[2];
unsigned int numPrimaryLightDynEntShadowVisEntries[2];
unsigned int *primaryLightDynEntShadowVis[2];
GfxShadowGeometry *shadowGeomOptimized;
GfxLightRegion *lightRegion;
GfxLightAABB lightAABB;
GfxWorldDpvsStatic dpvs;
GfxWorldDpvsDynamic dpvsDyn;
unsigned int mapVtxChecksum;
unsigned int heroOnlyLightCount;
GfxHeroOnlyLight *heroOnlyLights;
unsigned int numUmbraGates;
UmbraGate *umbraGates;
unsigned int umbraTomeSize;
char *umbraTomeData;
umbraTomePtr_t umbraTomePtr;
unsigned int numUmbraGates2;
UmbraGate2 *umbraGates2;
unsigned int umbraTomeSize2;
char *umbraTomeData2;
umbraTomePtr_t umbraTomePtr2;
unsigned int umbraUnkSize;
char *umbraUnkData;
}
;

struct GfxIESProfile
{
const char *name;
float firstAvailableAngle;
float lastAvailableAngle;
unsigned int lookupTableSize;
char *lookupTable;
unsigned int firstPixel;
unsigned int spotLightRefCount;
unsigned int omniLightRefCount;
}
;

struct GfxGradingClut
{
const char *name;
char inputColorimetry;
char outputColorimetry;
unsigned int clutSize;
GfxImage *hdrClutImage;
}
;

struct GfxFogSpline
{
const char *name;
GfxImage *image;
GfxFogSplineData data;
}
;

struct AnimationEntry
{
scr_string_t animName;
int aimSetIndex;
}
;

struct AnimAliasInfo
{
char animIndex;
float animWeight;
}
;

struct AnimAlias
{
scr_string_t aliasName;
char animCount;
AnimAliasInfo *aliasInfo;
}
;

struct AnimationState
{
scr_string_t name;
scr_string_t notify;
float blendTime;
float blendOutTime;
char flags;
char entryCount;
char aliasCount;
int aimSetIndex;
AnimationEntry *animEntries;
unsigned __int64 *animIndices;
AnimAlias *aliasList;
}
;

struct AnimationAimSet
{
scr_string_t name;
scr_string_t rootName;
int animCount;
scr_string_t *animName;
unsigned __int64 rootIndex;
unsigned __int64 *animIndices;
unsigned __int64 *aimNodeIndices;
}
;

struct AnimationStateMachine
{
scr_string_t name;
unsigned __int16 stateCount;
unsigned __int16 aimSetCount;
AnimationState *states;
AnimationAimSet *aimSets;
}
;

struct AnimationClass
{
const char *className;
AnimationStateMachine *stateMachine;
AnimationController animCtrl;
scr_string_t animTree;
ScriptableDef *scriptable;
unsigned __int16 soundCount;
unsigned __int16 effectCount;
scr_string_t *soundNotes;
scr_string_t *soundNames;
scr_string_t *soundOptions;
scr_string_t *effectNotes;
FxCombinedDef *effectDefs;
scr_string_t *effectTags;
}
;

struct PlayerAnimEntry
{
scr_string_t entryName;
scr_string_t animName;
scr_string_t shadowAnimName;
char __pad0[68];
}
;

struct PlayerAnimScriptEntry
{
unsigned int flags;
unsigned int itemCount;
unsigned int transitionCount;
unsigned int *items;
unsigned int *transitions;
}
;

struct PlayerAnimScriptCondition
{
unsigned int index;
unsigned int value[4];
}
;

struct PlayerAnimScriptCommand
{
unsigned __int16 bodyPart;
unsigned __int16 animIndex;
unsigned __int16 animDuration;
}
;

struct PlayerAnimScriptItem
{
unsigned int conditionCount;
unsigned int commandCount;
PlayerAnimScriptCondition *conditions;
PlayerAnimScriptCommand *commands;
}
;

struct PlayerAnimScriptTransition
{
PlayerAnimScriptTransitionTriggerType triggerType;
PlayerAnimTriggerParam triggerParam;
PlayerAnimScriptMoveType targetMoveType;
}
;

struct PlayerAnimScriptIdleTurn
{
unsigned __int16 turnleft;
unsigned __int16 turnright;
}
;

struct PlayerAnimScriptIdleTwitch
{
unsigned int twitchCount;
float cycleLength;
unsigned __int16 *twitches;
}
;

struct PlayerAnimScriptAimSet
{
unsigned __int16 aim_1;
unsigned __int16 aim_2;
unsigned __int16 aim_3;
unsigned __int16 aim_4;
unsigned __int16 aim_6;
unsigned __int16 aim_7;
unsigned __int16 aim_8;
unsigned __int16 aim_9;
}
;

struct PlayerAnimScriptLeanSet
{
unsigned __int16 lean_left;
unsigned __int16 lean_right;
}
;

struct PlayerAnimScript
{
const char *scriptName;
unsigned int animationCount;
unsigned int scriptItemCount;
unsigned int scriptTransitionCount;
unsigned int scriptIdleTurnCount;
unsigned int scriptIdleTwitchCount;
unsigned int scriptAimSetCount;
unsigned int scriptLeanSetCount;
unsigned int unkCount;
unsigned int torsoAnimCount;
unsigned int legsAnimCount;
PlayerAnimEntry *animations;
PlayerAnimScriptEntry *scriptAnims;
PlayerAnimScriptEntry *scriptEvents;
PlayerAnimScriptItem *scriptItems;
PlayerAnimScriptTransition *scriptTransitions;
PlayerAnimScriptIdleTurn *scriptIdleTurns;
PlayerAnimScriptIdleTwitch *scriptIdleTwitches;
PlayerAnimScriptAimSet *scriptAimSets;
PlayerAnimScriptLeanSet *scriptLeanSets;
unk_1453E3788 *unk;
unsigned __int16 *torsoAnimPackMap;
unsigned __int16 *torsoAnimUnpackMap;
unsigned __int16 *legsAnimPackMap;
unsigned __int16 *legsAnimUnpackMap;
unsigned int xAnimCount;
XAnimParts **xAnims;
}
;

struct LocalizeEntry
{
const char *value;
const char *name;
}
;

struct FxImpactPack
{
FxCombinedDef nonflesh[64];
FxCombinedDef flesh[23];
}
;

struct FxImpactEntry
{
FxCombinedDef nonflesh[64];
FxCombinedDef flesh[23];
}
;

struct FxImpactTable
{
const char *name;
FxImpactEntry *table;
}
;

struct SurfaceFxEntry
{
FxCombinedDef surfaceEffect[64];
}
;

struct SurfaceFxTable
{
const char *name;
SurfaceFxEntry *table;
}
;

struct RawFile
{
const char *name;
int compressedLen;
int len;
const char *buffer;
}
;

struct ScriptFile
{
const char *name;
int compressedLen;
int len;
int bytecodeLen;
const char *buffer;
char *bytecode;
}
;

struct ScriptDebugDataProfileString
{
const char *profileString;
}
;

struct ScriptDebugData
{
const char *name;
unsigned int nameCRC;
unsigned int profileStringCount;
ScriptDebugDataProfileString *profileStrings;
}
;

struct StringTable
{
const char *name;
int columnCount;
int rowCount;
StringTableCell *values;
}
;

struct LbColumnDef
{
const char *name;
int id;
int propertyId;
bool hidden;
int statsGroup;
const char *statName;
LbColType type;
int precision;
LbAggType agg;
int uiCalColX;
int uiCalColY;
}
;

struct LeaderboardDef
{
const char *name;
int id;
int sourceLbId;
int sourceLbWidth;
int columnCount;
int xpColId;
int prestigeColId;
LbColumnDef *columns;
LbUpdateType updateType;
int trackTypes;
int rankColIdX;
int rankColIdY;
}
;

struct LbVrColumnDef
{
const char *name;
int id;
int uiCalColX;
int uiCalColY;
}
;

struct VirtualLeaderboardDef
{
const char *name;
const char *sourceName;
int id;
int sourceId;
LbVrColumnDef *columns;
int columnCount;
int rankColIdX;
int rankColIdY;
}
;

struct DDLMember
{
const char *name;
int index;
void *parent;
int bitSize;
int limitSize;
int offset;
int type;
int externalIndex;
unsigned int rangeLimit;
unsigned int serverDelta;
unsigned int clientDelta;
int arraySize;
int enumIndex;
int permission;
}
;

struct DDLHash
{
unsigned int hash;
int index;
}
;

struct DDLStruct
{
const char *name;
int bitSize;
int memberCount;
DDLMember *members;
DDLHashTable hashTableUpper;
DDLHashTable hashTableLower;
}
;

struct DDLEnum
{
const char *name;
int memberCount;
const char **members;
DDLHashTable hashTable;
}
;

struct DDLDef
{
char *name;
unsigned __int16 version;
unsigned int checksum;
unsigned __int8 flags;
int bitSize;
int byteSize;
DDLStruct *structList;
int structCount;
DDLEnum *enumList;
int enumCount;
DDLDef *next;
int headerBitSize;
int headerByteSize;
int reserveSize;
int userFlagsSize;
bool paddingUsed;
}
;

struct DDLFile
{
char *name;
DDLDef *ddlDef;
}
;

struct CameraDef
{
const char *name;
CameraProfile profile;
bool keepShakeCam;
bool serverFeedback;
bool behaveAsRemoteCont;
bool interpolateViewTransition;
bool overrideFOV;
float FOV;
LensProfileMode overrideLensProfile;
float lensFocalLength;
float lensFStop;
float lensScale;
float lensUVScale;
bool overrideRadialBlur;
float radialBlurRadius;
float radialBlurStrength;
bool overrideZPlanes;
float zPlaneVMNear;
float zPlaneSceneNear;
CameraFirstPersonProfileDef firstPerson;
CameraOrbitProfileDef orbit;
CameraSentryDroneDef sentryDrone;
CameraKillCamVehicleDef kcamVeh;
CameraTransitionDef transitionIn;
CameraTransitionDef transitionOut;
vec3_t fpveh_offsetLs;
float fpveh_offsetYaw;
float fpveh_offsetPitch;
float fpveh_rollContrib;
float fpveh_rollMax;
bool fpveh_freeLook;
bool fpveh_freeLookReturn;
bool dlcBool[1];
bool fadeOutPlayerOnTransitionIn;
}
;

struct VehicleDef
{
const char *name;
char type;
const char *useHintString;
int health;
int hitClientScriptables;
float texScrollScale;
float topSpeed;
float accel;
float rotRate;
float rotAccel;
float maxBodyPitch;
float maxBodyRoll;
float fakeBodyAccelPitch;
float fakeBodyAccelRoll;
float fakeBodyVelPitch;
float fakeBodyVelRoll;
float fakeBodySideVelPitch;
float fakeBodyPitchStrength;
float fakeBodyRollStrength;
float fakeBodyPitchDampening;
float fakeBodyRollDampening;
float fakeBodyBoatRockingAmplitude;
float fakeBodyBoatRockingPeriod;
float fakeBodyBoatRockingRotationPeriod;
float fakeBodyBoatRockingFadeoutSpeed;
float boatBouncingMinForce;
float boatBouncingMaxForce;
float boatBouncingRate;
float boatBouncingFadeinSpeed;
float boatBouncingFadeoutSteeringAngle;
float collisionDamage;
float collisionSpeed;
vec3_t killcamOffset;
int playerProtected;
int bulletDamage;
int armorPiercingDamage;
int grenadeDamage;
int projectileDamage;
int projectileSplashDamage;
int heavyExplosiveDamage;
VehiclePhysicsDef vehPhysDef;
int vehHelicopterLockAltitude;
int vehHelicopterOffsetFromMesh;
float vehHelicopterAltitudeOffset;
float vehHelicopterPitchOffset;
float vehHelicopterBoundsRadius;
float vehHelicopterMaxSpeed;
float vehHelicopterMaxSpeedVertical;
float vehHelicopterMaxAccel;
float vehHelicopterMaxAccelVertical;
float vehHelicopterDecelerationFwd;
float vehHelicopterDecelerationSide;
float vehHelicopterDecelerationUp;
float vehHelicopterMaxYawRate;
float vehHelicopterMaxYawAccel;
float vehHelicopterTiltFromVelocity;
float vehHelicopterTiltFromControllerAxes;
float vehHelicopterTiltFromAcceleration;
float vehHelicopterTiltFromDeceleration;
float vehHelicopterTiltFromFwdAndYaw_VelAtMaxTilt;
float vehHelicopterTiltFromFwdAndYaw;
float vehHelicopterTiltMomentum;
float vehHelicopterTiltSpeed;
float vehHelicopterMaxPitch;
float vehHelicopterMaxRoll;
float vehHelicopterHoverSpeedThreshold;
float vehHelicopterJitterJerkyness;
float vehHelicopterLookaheadTime;
int vehHelicopterSoftCollisions;
int vehHelicopterUseGroundFX;
FxCombinedDef vehHelicopterGroundFx;
FxCombinedDef vehHelicopterGroundWaterFx;
float vehHelicopterGroundFxDefaultRepeatRate;
float vehHelicopterGroundFxSlowestRepeatRate;
float vehHelicopterGroundFxFastestRepeatRate;
float vehHelicopterGroundFxMinGroundDist;
float vehHelicopterGroundFxMaxGroundDist;
float ssWeaponSwitchDelaySeconds;
float ssWeaponRaiseDelaySeconds;
vec3_t ssFlyMaxSpeedMph;
vec3_t ssFlyMaxAccelerationMphps;
vec3_t ssFlyMaxCounterAccelerationMphps;
vec3_t ssFlyFrictionMphps;
vec3_t ssFlyJukeSpeedMph;
vec3_t ssHoverMaxSpeedMph;
vec3_t ssHoverMaxAccelerationMphps;
vec3_t ssHoverMaxCounterAccelerationMphps;
vec3_t ssHoverFrictionMphps;
vec3_t ssHoverJukeSpeedMph;
float ssFlyJukeRampTime;
float ssHoverJukeRampTime;
float ssBrakingFrictionMphps;
float ssFlyRedirectVelocityScale;
float ssFlySlowRedirectVelocityScale;
float ssHoverRedirectVelocityScale;
float ssFlyMinSpeedRatio;
bool ssProjectVelocityDesiresToXY;
bool ssEnforceMinGroundClearance;
float ssMinGroundClearanceHeight;
float ssMinGroundClearanceLiftForce;
bool ssTerrainModeMovement;
float ssTerrainModeGravity;
float ssTerrainModeFlyTime;
vec2_t ssAimOffsetClampDeg;
vec2_t ssPitchUpSoftLimitDeg;
vec2_t ssPitchDownSoftLimitDeg;
float ssClutchPitchSpeedScale;
vec2_t ssFlyAimSpeedDps;
vec2_t ssFlySlowAimSpeedDps;
vec2_t ssHoverAimSpeedDps;
vec2_t ssFlyAimDriftScale;
vec2_t ssFlySlowAimDriftScale;
vec2_t ssHoverAimDriftScale;
float ssMinSpringOscillation;
float ssMaxSpringOscillation;
float ssMinSpringStrength;
float ssMaxSpringStrength;
float ssMinSpringAngSpeed;
float ssMaxSpringAngSpeed;
float ssSpringControllerContribution;
float ssTiltSpringOscillation;
float ssTiltSpringStrength;
float ssTiltSpringAngleContribution;
int ssADSToLockOn;
float ssLockOnMinAngle;
float ssLockOnMaxAngle;
float ssPilotAssistNoLockAimScale;
float ssPilotAssistNoLockMinAngle;
float ssPilotAssistNoLockMaxAngle;
float ssPilotAssistAimScale;
float ssPilotAssistMinAngle;
float ssPilotAssistMaxAngle;
vec2_t ssAutoLevelStrengthHover;
vec2_t ssAutoLevelStrengthFly;
bool ssIsRollRightStick;
float ssRollMaxSpeedDps;
float ssBoostSpeedScale;
float ssBoostThrustScale;
bool ssEnableBoostToFly;
float ssBoostTime;
float ssBoostRegenTime;
float ssBoostRegenStallTime;
float ssBoostMinThresholdRatio;
float ssCollisionRadiusI;
float ssCylinderHeightI;
bool ssUseCylinder;
bool ssUsePlayerClip;
float ssCollisionOffsetI;
float ssCollisionHardThresholdMph;
float ssFOVFlyOffsetDeg;
float ssFOVFlyBoostOffsetDeg;
float ssFOVHoverOffsetDeg;
float ssZoomBlendSpeedDps;
float ssZoomMaxAngle;
float ssZoomMinDistI;
float ssZoomIdealDistI;
float ssZoomMaxDistI;
bool ssShowPilotViewmodel;
float ssCameraAnimScale;
vec2_t ssFlyTiltShipAngles;
float ssFlyTiltCameraScale;
vec2_t ssHoverTiltShipAngles;
float ssHoverTiltCameraScale;
float ssMotionIdleRateMinSpeed;
float ssMotionIdleRateMaxSpeed;
float ssMotionIdleMinRate;
float ssMotionIdleMaxRate;
scr_string_t ssAnimTree;
SpaceshipAnimParts ssAnimParts[33];
FxCombinedDef ssThrustFxLoop;
FxCombinedDef ssJukeFx;
RumbleInfo *ssIdleRumble;
RumbleInfo *ssSmallRumble;
RumbleInfo *ssMedRumble;
RumbleInfo *ssLargeRumble;
SndAliasLookup rattleLoop;
float rattleLoopMinVolume;
float rattleLoopMaxVolume;
SndAliasLookup airLoop;
float airLoopMinVolume;
float airLoopMaxVolume;
SndAliasLookup engineLoop;
float engineLoopMinVolume;
float engineLoopMaxVolume;
float engineLoopMinPitch;
float engineLoopMaxPitch;
SndAliasLookup hoverLoop;
float hoverLoopMinVolume;
float hoverLoopMaxVolume;
float hoverLoopMinPitch;
float hoverLoopMaxPitch;
SndAliasLookup boostLoop;
float boostLoopMaxVolume;
float boostLoopMaxPitch;
SndAliasLookup ssThrustLoop;
float ssThrustLoopMaxVolume;
float ssThrustLoopMaxPitch;
SndAliasLookup boostStart;
SndAliasLookup boostStop;
SndAliasLookup boostDepleted;
SndAliasLookup boostUnavailable;
SndAliasLookup jukeLeft;
SndAliasLookup jukeRight;
SndAliasLookup jukeUpDown;
SndAliasLookup jukeBack;
SndAliasLookup jukeFront;
SndAliasLookup flightOn;
SndAliasLookup flightOff;
float flightOnTimer;
float flightOffTimer;
SndAliasLookup hardCollision;
SndAliasLookup softCollision;
int camLookEnabled;
int camRelativeControl;
int camRemoteDrive;
float camLerp;
float camHeight;
float camRadius;
float camPitchInfluence;
float camYawInfluence;
float camRollInfluence;
float camFovIncrease;
float camFovOffset;
float camFovSpeed;
float camReturnSpeed;
float camReturnLerp;
float camVehicleAnglePitchRate;
float camVehicleAngleYawRate;
float camVehicleAngleRollRate;
int vehCam_UseGDT;
float vehCam_anglesPitch;
float vehCam_anglesYaw;
float vehCam_anglesRoll;
float vehCam_offsetX;
float vehCam_offsetY;
float vehCam_offsetZ;
float vehCam_radius;
float vehCam_speedInfluence;
float vehCam_pitchTurnRate;
float vehCam_pitchClamp;
float vehCam_yawTurnRate;
float vehCam_yawClamp;
VehCamZOffsetMode vehCam_zOffsetMode;
float vehCam_anglesPitch3P;
float vehCam_anglesYaw3P;
float vehCam_anglesRoll3P;
float vehCam_offsetX3P;
float vehCam_offsetY3P;
float vehCam_offsetZ3P;
float vehCam_radius3P;
float vehCam_speedInfluence3P;
float vehCam_pitchTurnRate3P;
float vehCam_pitchClamp3P;
float vehCam_yawTurnRate3P;
float vehCam_yawClamp3P;
VehCamZOffsetMode vehCam_zOffsetMode3P;
const char *turretWeaponName;
WeaponCompleteDef *turretWeapon;
float turretHorizSpanLeft;
float turretHorizSpanRight;
float turretVertSpanUp;
float turretVertSpanDown;
float turretHorizResistLeft;
float turretHorizResistRight;
float turretVertResistUp;
float turretVertResistDown;
float turretRotRate;
VehicleTurretFireType turretFireType;
SndAliasLookup turretSpinSnd;
SndAliasLookup turretStopSnd;
int trophyEnabled;
float trophyRadius;
float trophyInactiveRadius;
int trophyAmmoCount;
float trophyReloadTime;
scr_string_t trophyTags[4];
FxCombinedDef trophyExplodeFx;
FxCombinedDef trophyFlashFx;
Material *compassFriendlyIcon;
Material *compassEnemyIcon;
Material *compassFriendlyAltIcon;
Material *compassEnemyAltIcon;
int compassIconWidth;
int compassIconHeight;
SndAliasLookup idleLowSnd;
SndAliasLookup idleHighSnd;
SndAliasLookup engineLowSnd;
SndAliasLookup engineHighSnd;
float engineSndSpeed;
scr_string_t audioOriginTag;
SndAliasLookup engineStartUpSnd;
int engineStartUpLength;
SndAliasLookup engineShutdownSnd;
SndAliasLookup engineIdleSnd;
SndAliasLookup engineSustainSnd;
SndAliasLookup engineRampUpSnd;
int engineRampUpLength;
SndAliasLookup engineRampDownSnd;
int engineRampDownLength;
SndAliasLookup suspensionSoftSnd;
float suspensionSoftCompression;
SndAliasLookup suspensionHardSnd;
float suspensionHardCompression;
SndAliasLookup collisionSnd;
float collisionBlendSpeed;
SndAliasLookup speedSnd;
float speedSndBlendSpeed;
SndAliasLookup surfaceSndName;
float surfaceSndBlendSpeed;
float slideVolume;
float slideBlendSpeed;
float inAirPitch;
const char *soundTriggerOverrideZone;
bool soundTriggerOverrideReverb;
bool soundTriggerOverrideMix;
bool soundTriggerOverrideFilter;
bool soundTriggerOverrideOcclusion;
bool soundTriggerOverrideAmbient;
bool soundTriggerOverrideAmbientEvents;
bool soundTriggerOverrideADSR;
int visionBlendTime;
const char *globalVisionSettings;
const char *mapVisionSettings;
}
;

struct AddonMapEnts
{
const char *name;
char *entityString;
int numEntityChars;
MapTriggers trigger;
ClipInfo *info;
unsigned int numSubModels;
cmodel_t *cmodels;
GfxBrushModel *models;
}
;

struct NetConstStrings
{
const char *name;
NetConstStringType stringType;
NetConstStringSource sourceType;
unsigned int flags;
unsigned int entryCount;
const char **stringList;
}
;

struct LuaFile
{
const char *name;
int len;
char strippingType;
const char *buffer;
}
;

struct EquipmentClothData
{
char *foleyName;
char *footstepName;
}
;

struct EquipmentWeaponRattleData
{
char *szName;
float priority;
}
;

struct EquipmentSndChance
{
float rattleChance;
float accentChance;
float silentChance;
}
;

struct EquipmentChanceMoveTypes
{
EquipmentSndChance *chances;
}
;

struct EquipmentChanceRattleTypes
{
EquipmentChanceMoveTypes *chances;
}
;

struct EquipmentSoundSet
{
SndAliasList *soundPLR;
SndAliasList *soundNPC;
}
;

struct EquipSoundSetFootsteps
{
EquipSoundSetFootstepActionType actionType[4];
}
;

struct EquipSoundSetMoveLeadTypes
{
EquipmentSoundSet *soundSetsPre;
EquipmentSoundSet *soundSetsPost;
}
;

struct EquipSoundSetMoveTypes
{
EquipmentSoundSet *soundSets;
}
;

struct EquipSoundSetMantleTypes
{
EquipmentSoundSet soundSets[12];
}
;

struct EquipSoundSetStanceTypes
{
EquipmentSoundSet soundSets[4];
}
;

struct EquipSoundSetMeleeVM
{
EquipmentSoundSet soundSets[56];
}
;

struct EquipSoundSetMeleeWM
{
EquipmentSoundSet soundSets[8][3][2][4][3];
}
;

struct EquipmentSoundTable
{
const char *szName;
unsigned int numClothTypes;
unsigned int numWeaponRattleTypes;
unsigned int numMoveTypes;
unsigned int numStanceTypes;
EquipmentClothData *clothTypes;
EquipmentWeaponRattleData *weaponRattleTypes;
EquipmentChanceRattleTypes *chancesPLR;
EquipmentChanceRattleTypes *chancesNPC;
EquipSoundSetMoveTypes *mvmtClothFootstepCeilingSoundSets;
EquipSoundSetMoveTypes *mvmtClothFoleySoundSets;
EquipSoundSetMoveTypes *mvmtRattleSoundSets;
EquipSoundSetMoveTypes mvmtAccentSoundSets;
EquipSoundSetMantleTypes *mvmtMantleSoundSets;
EquipSoundSetStanceTypes *mvmtStanceSoundSets;
EquipSoundSetMeleeVM *meleeAttackVMSoundSets;
}
;

struct AlwaysloadedFlagsPerModel
{
XModel *model;
AlwaysloadedFlags flags;
}
;

struct StreamingItemContents
{
unsigned __int16 *contents;
}
;

struct StreamingItemCost
{
unsigned int costInBytes;
}
;

struct ForcedStubAssetReference
{
const char *name;
char type;
}
;

struct TransientFileReference
{
const char *name;
char type;
}
;

struct TransientAssetReference
{
const char *name;
char type;
unsigned int mpParentIndex;
}
;

struct TransientWorldGridTileMap
{
unsigned __int16 *spaceToTransientIndex;
unsigned int indexCount;
unsigned int level;
unsigned int lodCount;
unsigned int gridCountX;
unsigned int gridCountY;
float cellSize;
float minX;
float minY;
}
;

struct TransientInfo
{
TransientSets transientSets;
TransientCosts transientCosts;
ForcedStubAssetReference *forcedStubAssets;
TransientFileReference *transientFiles;
TransientAssetReference *transientAssets;
TransientWorldGridTileMap *transientWorldGridTileMaps;
SpatialPartition_Tree *transientWorldGridSpatial;
Bounds *transientWorldGridBounds;
unsigned __int16 *transientWorldGridHighLodLookup;
unsigned int transientWorldGridBoundsCount;
unsigned int transientWorldGridHighLodCount;
unsigned int transientWorldGridHighLodLookupCount;
char transientInfoFlags[1];
bitarray_dynamic transientSkipLoading;
unsigned int forcedStubAssetCount;
unsigned int transientFileCount;
unsigned int transientAssetCount;
unsigned int transientWorldGridTileMapCount;
unsigned int transientCollisionTileCount;
unsigned int transientMPCommonCount;
}
;

struct StreamingInfo
{
const char *name;
int unused[2];
AlwaysloadedFlags flags;
int transientInfoFlags;
unsigned int modelCount;
AlwaysloadedFlagsPerModel *perModelFlags;
unsigned int forcedStubAssetCount;
ForcedStubAssetReference *forcedStubAssets;
unsigned int transientFileCount;
TransientFileReference *transientFiles;
unsigned int transientAssetCount;
TransientAssetReference *transientAssets;
unsigned int stringCount;
const char **strings;
TransientSets transientSets;
}
;

struct TTFDef
{
const char *name;
int fileLen;
const char *file;
void *ftFace;
}
;

struct SuitAnimEntry
{
XAnimParts *anim;
int time;
unsigned int animType;
}
;

struct SuitGestureEntry
{
Gesture *gesture;
gestureAnimType_t gestureAnimType;
}
;

struct CinematicMotionDef
{
const char *name;
vec3_t camera_spring_velocitySmoothingSec;
vec3_t camera_spring_velocityScale;
vec3_t camera_spring_springConstant;
vec3_t camera_spring_damperConstant;
vec3_t camera_spring_maxOffset;
vec3_t viewmodel_spring_velocitySmoothingSec;
vec3_t viewmodel_spring_velocityScale;
vec3_t viewmodel_spring_springConstant;
vec3_t viewmodel_spring_damperConstant;
vec3_t viewmodel_spring_maxOffset;
vec3_t viewmodel_spring_offsetToAnglesX;
vec3_t viewmodel_spring_offsetToAnglesY;
vec3_t viewmodel_spring_offsetToAnglesZ;
CinematicShakeDef camera_shake_rotation;
CinematicShakeDef camera_shake_translation;
CinematicImpactShakeDef camera_shake_impact_rotation;
CinematicImpactShakeDef camera_shake_impact_translation;
}
;

struct SuitAnimPerWeapClass
{
SuitAnimEntry *anims;
unsigned int numAnims;
int weapClasses;
SuitGestureEntry *gestures;
int numGestures;
FootstepAnim footstep[9];
}
;

struct SuitAnimPackage
{
const char *name;
SuitAnimPerWeapClass animOverrides[6];
}
;

struct ASM_Function_Param
{
ASM_Function_Param_Value u;
ASM_Function_Param_Type m_Type;
}
;

struct ASM_Transition
{
int m_ToStateID;
ASM_Function m_FuncName;
int m_Priority;
}
;

struct AI_ASM_State
{
int m_PowerDownState;
int m_PowerDownNumParams;
ASM_Function_Param *m_PowerDownParams;
scr_string_t m_AnimMode;
scr_string_t m_OrientMode;
scr_string_t m_AnimModeMP;
scr_string_t m_OrientModeMP;
}
;

struct Player_ASM_State
{
ASM_Function m_UpdateFunc;
int m_ScriptedAnimState;
int m_ScriptedAnimNumParams;
ASM_Function_Param *m_ScriptedAnimParams;
char padding[16];
}
;

struct ASM_State
{
ASM_Transition *m_Transitions;
int m_NumTransitions;
scr_string_t m_Name;
int m_ID;
int m_Type;
int m_Stance;
int m_Flags;
int m_LogicGroupID;
scr_string_t *m_Subtrees;
int m_NumSubtrees;
ASM_Function m_PlayAnim;
ASM_Function m_PlayAnimNoteHandler;
ASM_Function m_ChooseAnimFunc;
ASM_Function m_TerminateFunc;
scr_string_t m_ExitNotify;
scr_string_t m_FacialState;
int m_DeathState;
int m_DeathNumParams;
ASM_Function_Param *m_DeathParams;
int m_PainState;
int m_PainNumParams;
ASM_Function_Param *m_PainParams;
int m_ExecutionState;
int m_ExecutionNumParams;
ASM_Function_Param *m_ExecutionParams;
ASM_State_Union u;
}
;

struct ASM_LogicGroup
{
int m_ID;
int m_ParentID;
ASM_Function m_InitFunc;
ASM_Function m_TerminateFunc;
}
;

struct AI_ASM
{
int *m_Services;
int m_NumServices;
int m_ScriptHandler;
}
;

struct Player_ASM
{
}
;

struct ASM
{
const char *m_szName;
scr_string_t m_Name;
ASM_State *m_States;
int m_NumStates;
ASM_LogicGroup *m_LogicGroups;
int m_NumLogicGroups;
int *m_FuncIDs;
int m_NumFuncIDs;
int m_StartState;
ASM_Mode m_Mode;
ASM_Union u;
}
;

struct AnimsetAnim
{
scr_string_t name;
float weight;
scr_string_t aimsetName;
scr_anim_t anim;
}
;

struct AIAnimsetAlias
{
AnimsetAnim *redAnims;
unsigned int numRedAnims;
}
;

struct PlayerAnimsetAddonEntry
{
scr_string_t stateName;
scr_string_t aliasName;
scr_string_t addonType;
float blendtime;
float rate;
bool restart;
char padding[8];
}
;

struct PlayerAnimsetAlias
{
float duration;
float rate;
unsigned __int64 flags;
unsigned int moveTypes[8];
scr_string_t aimState;
scr_string_t aimAlias;
scr_string_t moveTurnState;
scr_string_t moveTurnAlias;
float aimBlendTime;
float moveTurnBlendTime;
float lerpStrengthMin;
float lerpStrengthMax;
char customAnimHandlerType[1];
PlayerAnimsetAddonEntry *aliasAddons;
unsigned int numAliasAddons;
PlayerAnimsetAliasPatch patchInfo;
char fingerPoseSlot[1];
char padding[12];
}
;

struct AnimsetAlias
{
scr_string_t name;
AnimsetAnim *anims;
unsigned int numAnims;
ASM_Function *funcs;
unsigned int numFuncs;
scr_string_t *addons;
unsigned int numAddons;
scr_string_t animAlias;
int blenderIndex;
float blendtime;
scr_string_t curveName;
AnimsetAlias_Union u;
}
;

struct AnimsetBlendtreeAlias
{
scr_string_t name;
scr_string_t graftNode;
scr_string_t blendtreeName;
ASM_Function *funcs;
unsigned int numFuncs;
scr_anim_t graftAnim;
scr_animtree_t blendtree;
bool bNotify;
}
;

struct AnimsetStateEntryMap
{
unsigned __int16 aliasIndex;
unsigned __int16 aliasAnimEntryIndex;
}
;

struct AIAnimsetState
{
}
;

struct PlayerAnimsetState_BakedAliasesPerConditionType
{
bitarray_dynamic *aliases;
unsigned __int16 numConditionValues;
unsigned __int16 conditionType;
}
;

struct PlayerAnimsetState
{
PlayerASM_ConditionMask conditionMask;
bool excludePackMap;
char padding[4];
char numConditionTypes;
PlayerAnimsetState_BakedAliasesPerConditionType *aliasesPerConditionType;
}
;

struct AnimsetState
{
scr_string_t name;
AnimsetAlias *animAliases;
unsigned int numAnimAliases;
AnimsetBlendtreeAlias *blendtreeAliases;
unsigned int numBlendtreeAliases;
scr_string_t notify;
scr_string_t aimsetTarget;
scr_string_t aimsetRoot;
scr_anim_t aimsetRootAnim;
unsigned int numAnims;
AnimsetStateEntryMap *animEntryMap;
char stateType[1];
float lookAtMinYaw;
float lookAtMaxYaw;
float lookAtMinPitch;
float lookAtMaxPitch;
float lookAtNeutralPitch;
float lookAtNeutralYaw;
bool restart;
bool isAimset;
bool clearAlwaysOn;
AnimsetState_Union u;
}
;

struct AIAnimset
{
AIAnimsetNodeOffset coverLeftOffset;
AIAnimsetNodeOffset coverLeftCrouchOffset;
AIAnimsetNodeOffset coverRightOffset;
AIAnimsetNodeOffset coverRightCrouchOffset;
AIAnimsetNodeOffset coverCrouchOffset;
AIAnimsetNodeOffset coverStandOffset;
}
;

struct PlayerAnimset
{
unsigned __int16 *packMap;
unsigned __int16 numPackMapEntries;
unsigned __int16 *unpackMap;
unsigned __int16 numUnpackMapEntries;
scr_string_t patchName;
bool isDefault;
char padding[24];
}
;

struct Animset
{
const char *name;
scr_string_t constName;
scr_string_t animTree;
AnimsetState *states;
unsigned int numStates;
AnimationController animCtrl;
ASM_Mode mode;
scr_string_t startStateName;
scr_string_t startAliasName;
scr_string_t clearStateName;
scr_string_t clearAliasName;
scr_string_t alwaysOnStateName;
Animset_Union u;
}
;

struct SuitDef
{
const char *name;
float cam_minVelocityForFovIncrease;
float cam_maxVelocityForFovIncrease;
float cam_velocityToDecreaseFov;
float cam_fovIncreaseAtMaxVelocity;
float cam_oneOverFovEaseInTimeMs;
float cam_oneOverFovEaseOutTimeMs;
bool enableIKOverride;
float player_globalAccelScale;
float player_sprintSpeedScale;
bool player_sprintUnlimited;
float player_viewBobScale;
bool jump_slowdownEnable;
float jump_height;
float sprintLeap_height;
float sprintLeap_forwardVelocityScale;
int sprintLeap_minSprintTimeMs;
float doubleJump_accel;
float doubleJump_speed;
float doubleJump_speedNoBoost;
float doubleJump_frictionMin;
float doubleJump_frictionMax;
float doubleJump_initialUpBoostAccel;
float doubleJump_standardUpBoostAccel;
float doubleJump_energyNeededForInitialUpBoost;
float doubleJump_energyNeededForStandardUpBoost;
float doubleJump_maxUpwardsVelocity;
bool doubleJump_enableMinZVelocity;
float doubleJump_minZVelocity;
float doubleJump_energyInitialCost;
float doubleJump_boostUpEnergyUseRate;
float doubleJump_energyUsePerButtonPress;
bool doubleJump_hoverOnly;
float doubleJump_maxViewPitchDip;
float doubleJump_maxViewBackwardsPitchDip;
float doubleJump_maxViewRoll;
float doubleJump_oneOverEaseInTime;
float doubleJump_oneOverEaseOutTimeMs;
bool doubleJump_alwaysDipView;
SndAliasLookup doubleJump_sound;
SndAliasLookup doubleJump_soundPlayer;
SndAliasLookup doubleJump_releaseSound;
SndAliasLookup doubleJump_releaseSoundPlayer;
int slide_sprint_penalty_ms;
bool slide_allow_firing;
bool slide_allow_ads;
bool slide_allow_weapon_switch;
float slide_jump_speed_scale;
float slide_energy_cost_ratio;
float slide_energy_unknown01;
float slide_energy_unknown02;
float slide_energy_unknown03;
int slide_max_time_ms;
int slide_max_time_reduced_ms;
int slide_max_time_base_ms;
float wallRun_minZVel;
float wallRun_minTriggerSpeed;
float wallRun_minMaintainSpeed;
int wallRun_delayPeriodMs;
float wallRun_minJumpHeight;
int wallRun_maxTimeMs;
int wallRun_fallStageTimeMs;
float wallRun_maxHeight;
float wallRun_jumpHeight;
float wallRun_jumpVelocity;
float wallRun_frictionScale;
float wallRun_speedScale;
float wallRun_speedScaleADS;
float wallRun_energyInitialCost;
float wallRun_energyChangePerSecond;
float wallRun_energyUnknown01;
float wallRun_energyUnknown02;
SuitAnimType suitAnimType;
SuitAnimPackage *animPackage;
SuitAnimPackage *animPackageL;
SuitAnimPackage *animPackageR;
SuitAnimPackage *animPackageRelaxed;
SuitAnimPackage *animPackageSafe;
char facialAnimType;
SuitBodyAnimType bodyAnimType;
ScriptableDef *scriptableDef;
int viewheight_stand;
int viewheight_crouch;
int viewheight_prone;
int viewheight_laststand;
int viewheight_dead;
int viewheight_swim;
int viewheight_slide;
int bounds_radius;
int bounds_height_stand;
int bounds_height_crouch;
int bounds_height_prone;
Bounds bounds_stand;
Bounds bounds_crouch;
Bounds bounds_prone;
int radialMotionBlur_interpTimeIn;
int radialMotionBlur_interpTimeOut;
float radialMotionBlur_sprintMinRadius;
float radialMotionBlur_sprintMaxRadius;
float radialMotionBlur_sprintMinStrength;
float radialMotionBlur_sprintMaxStrength;
float radialMotionBlur_slideMinRadius;
float radialMotionBlur_slideMaxRadius;
float radialMotionBlur_slideMinStrength;
float radialMotionBlur_slideMaxStrength;
float radialMotionBlur_doubleJumpMinRadius;
float radialMotionBlur_doubleJumpMaxRadius;
float radialMotionBlur_doubleJumpMinStrength;
float radialMotionBlur_doubleJumpMaxStrength;
float radialMotionBlur_wallRunMinRadius;
float radialMotionBlur_wallRunMaxRadius;
float radialMotionBlur_wallRunMinStrength;
float radialMotionBlur_wallRunMaxStrength;
float radialMotionBlur_groundPoundMinRadius;
float radialMotionBlur_groundPoundMaxRadius;
float radialMotionBlur_groundPoundMinStrength;
float radialMotionBlur_groundPoundMaxStrength;
float radialMotionBlur_rewindMinRadius;
float radialMotionBlur_rewindMaxRadius;
float radialMotionBlur_rewindMinStrength;
float radialMotionBlur_rewindMaxStrength;
float radialMotionBlur_dodgeMinRadius;
float radialMotionBlur_dodgeMaxRadius;
float radialMotionBlur_dodgeMinStrength;
float radialMotionBlur_dodgeMaxStrength;
RumbleInfo *groundPound_activationRumble;
SndAliasLookup groundPound_activationSound;
SndAliasLookup groundPound_activationSoundPlayer;
RumbleInfo *groundPound_landingRumble;
SndAliasLookup groundPound_landingSound;
SndAliasLookup groundPound_landingSoundPlayer;
RumbleInfo *landing_rumbleLowHeight;
RumbleInfo *landing_rumbleMediumHeight;
RumbleInfo *landing_rumbleHighHeight;
RumbleInfo *landing_rumbleExtremeHeight;
float landing_speedScale;
float footstep_shakeBroadcastRadiusInches;
int footstep_shakeDurationMs;
float footstep_shakeAmplitude;
RumbleInfo *footstep_rumble;
}
;

struct HudOutlineDef
{
const char *name;
char outlineType;
bool drawInStencil;
bool drawNonOccludedPixels;
bool drawOccludedPixels;
bool drawFill;
bool fadeOverTimeEnable;
bool fadeOverTimeLooping;
char fadeOverTimeCurveType;
float fadeOverTimeMinAlpha;
float fadeOverTimeMaxAlphaTime;
float fadeOverTimeMinAlphaTime;
float fadeOverTimeInTime;
float fadeOverTimeOutTime;
bool distanceFadeEnable;
float distanceFadeMinAlpha;
float distanceFadeStartDistance;
float distanceFadeEndDistance;
unsigned int outlineColor;
int outlineWidth;
bool drawOnLocalPlayerCharacter;
bool drawOnVehicleOccupants;
bool drawOnLocalPlayerVehicleOccupants;
bool drawOnLocalPlayerVehicle;
bool dlcBool0;
bool dlcBool1;
bool dlcBool2;
bool dlcBool3;
bool dlcBool4;
bool dlcBool5;
float dlcFloat0;
float dlcFloat1;
float dlcFloat2;
float dlcFloat3;
float dlcFloat4;
float dlcFloat5;
}
;

struct SpaceshipTargetDef
{
const char *name;
scr_string_t targetTag;
float annotationOffsetX;
float annotationOffsetY;
float annotationOffsetZ;
bool disableLeading;
float lock_minDistance;
float lock_maxDistance;
float lock_cutoffDistance;
float lock_minRate;
float lock_maxRate;
float lock_maxZoomFOV;
float lock_followDistance;
float lockCooldown_rate;
float aa_closeDistance;
float aa_farDistance;
float aa_closeMinAngle;
float aa_closeMaxAngle;
float aa_closeBlendMax;
float aa_farMinAngle;
float aa_farMaxAngle;
float aa_farBlendMax;
float aaLocked_closeDistance;
float aaLocked_farDistance;
float aaLocked_closeMinAngle;
float aaLocked_closeMaxAngle;
float aaLocked_closeBlendMax;
float aaLocked_farMinAngle;
float aaLocked_farMaxAngle;
float aaLocked_farBlendMax;
float pilotAssistScale;
float callout_maxDistance;
float callout_maxAngle;
}
;

struct BehaviorTreeNode
{
const char *name;
char __pad0[16];
}
;

struct BehaviorTreeDecorator
{
char type[1];
BehaviorTreeDecoratorUnion data;
}
;

struct BehaviorTree
{
const char *name;
unsigned __int16 nodeCount;
BehaviorTreeNode *nodes;
}
;

struct CoverSelector
{
const char *name;
scr_string_t scrName;
float distanceWeight;
bool useDistanceSquared;
float distanceMin;
float distanceOptimal;
float distanceMax;
float engagementWeight;
float dangerousWeight;
float targetDirWeight;
float moveTowardsTargetWeight;
float visibleWeight;
float notVisibleWeight;
bool considerCoverNodes;
bool considerTacPoints;
float strictCoverNodeWeight;
float concealNodeWeight;
float exposedNodeWeight;
float ambushNodeWeight;
float nodeAngleWeight;
float secondaryTargetWeight;
float playerSightWeight;
float priorityWeight;
float lmgMountableWeight;
float indoorWeight;
float outdoorWeight;
float heightWeight;
float targetHeightWeight;
float stickinessWeight;
}
;

struct EnemySelector
{
const char *name;
scr_string_t scrName;
int threatVisible;
int threatFullyAware;
int threatFriendlyTimingOutOnEnemy;
int threatScarinessScale;
int threatScarinessMin;
int threatScarinessMax;
int threatSharedEnemy;
int threatSharedEnemyCap;
int threatSharedMeleeEnemy;
int threatFriendliesCurrentEnemy;
int threatFriendliesTimeoutEnemy;
int threatCurrentEnemy;
int threatCurrentEnemyPlayerDamaged;
int threatFlashedEnemy;
int threatNoCover;
int threatPlayerAttackedMeRecently;
int threatKilledTeammateRecently;
int threatBehindEnemyLinesMax;
int threatBehindEnemyLinesDist;
int threatCoveringFire;
int threatCoverFireCurTarget;
float threatDistanceMaxRange;
float threatDistanceMaxThreat;
float threatMinDistToIgnore;
float threatMinSuppression;
}
;

struct ClientCharacter
{
const char *name;
ScriptableDef *scriptableDef;
const char *animFileName;
const char *animTreeName;
}
;

struct AccessoryDef
{
const char *name;
scr_string_t VMAttachBone;
bool VMAttachRightHand;
Gesture *interactGesture;
const char *widgetVM;
int widgetVMRTTWidth;
int widgetVMRTTHeight;
int widgetVMRTTMipCount;
}
;

struct LocDmgTable
{
const char *name;
float mults[22];
}
;

struct BulletPenetration
{
const char *name;
float depthTable[4][64];
}
;

struct ScriptBundle
{
const char *name;
int strPoolSize;
const char *strPool;
int rootSize;
$EE658B4838C130B7503228AEB3560558 ___u4;
}
;

struct XCamTargetModelFrame
{
vec3_t offset;
vec4_t axisQuat;
}
;

struct XCamTargetModel
{
const char *name;
int frames;
XCamTargetModelFrame *animationFrames;
}
;

struct XCamFrame
{
int frameNum;
vec3_t origin;
vec4_t angleQuat;
float fov;
float focalLength;
float focalDistance;
float fStop;
}
;

struct XCamCamera
{
const char *name;
int index;
int frames;
XCamApertureType apertureType;
XCamFrame *animationFrames;
float farZ;
float focusShiftMultiplier;
}
;

struct XCamCameraSwitchParams
{
int frameNum;
int camera1Index;
int camera2Index;
float dissolve;
}
;

struct XCamFrameName
{
const char *frameName;
int frameNum;
}
;

struct XCam
{
const char *name;
const char *parent_scene;
bool hideHud;
bool looping;
bool hideLocalPlayerModel;
bool useFirstPersonPlayerModel;
bool disableNearDof;
bool autoMotionBlur;
bool easeAnimationsOut;
bool showNonAnimatedPlayers;
int cameraCount;
int frames;
int frameRate;
int frameNameCount;
vec4_t rotationOffsetInv;
vec3_t offset;
XCamTargetModel *targetModelBoneRoots;
XCamCamera *cameras;
XCamCameraSwitchParams *cameraSwitchParams;
XCamFrameName *frameNames;
XAnimNotifyInfo *notify;
char notifyCount;
XAnimNotifyInfo *startupNotify;
char startupNotifyCount;
XAnimNotifyInfo *shutdownNotify;
char shutdownNotifyCount;
vec3_t rightStickRotateOffset;
vec2_t rightStickRotateMaxDegrees;
float freeCursorParallax;
}
;

struct Camo
{
const char *name;
scr_string_t internalName;
MaterialTextureDef *textureDefs;
unsigned int textureDefCount;
unsigned int colorTint[3];
unsigned int albedoMapScaleBias[3];
unsigned int normalMapScaleBias[3];
vec4_t uvScaleOffset[3];
vec4_t uvRotate[3];
char blendMapChannels;
FxCombinedDef vehVfxTailLight;
FxCombinedDef vehVfxDoorSmoke;
}
;

struct StreamTreeOverrideImageNode
;

struct StreamTreeOverride
{
const char *name;
StreamTreeOverrideImageNode *imageNodes;
unsigned int imageNodeCount;
}
;

struct KeyValuePair
{
unsigned int keyHash;
const char *value;
}
;

struct KeyValuePairs
{
const char *name;
int numVariables;
KeyValuePair *keyValuePairs;
}
;

struct StTerrainNode
{
StMesh mesh;
Bounds wldBounds;
vec2_t compressBase;
vec2_t compressScale;
unsigned int layerMask;
unsigned __int16 xIndex;
unsigned __int16 yIndex;
}
;

struct StTerrainGridLevel
{
unsigned int width;
unsigned int height;
StTerrainNode *nodes;
}
;

struct StTerrainMaterialLayer
;

struct StTerrainDisplacementScale
{
unsigned int mask;
float scale;
}
;

struct StTerrainClutterLayer
{
unsigned int sampleOffset;
unsigned int bitmaskOffset;
int tileCount[2];
vec2_t tileSizeUV;
vec2_t layerSize;
vec3_t angleMin;
vec3_t angleMax;
vec2_t zOffsetMinMax;
vec2_t scaleMinMax;
unsigned int maxInstanceCount;
unsigned int tileInstanceCountOffset;
unsigned int useSurfaceNormal;
}
;

struct StDiskTerrainSurface
{
unsigned int highLODIndexCount;
unsigned __int16 *highLODIndices;
ID3D12Resource *highLODIndexBuffer;
StTerrainGrid grid;
GfxPlacement objToWld;
Bounds wldBounds;
unsigned int layerCount;
unsigned int surfaceMapSetIndex;
unsigned int surfaceMapXCount;
unsigned int surfaceMapYCount;
unsigned int tileVertXCount;
vec2_t surfaceMapOrigin;
float surfaceMapWorldSize;
float heightOffset;
float heightScale;
float halfWldSize;
unsigned int layerMaterialsCount;
StTerrainMaterialLayer *layerMaterials;
unsigned int layerDisplacementMask;
unsigned int displacementScalesCount;
StTerrainDisplacementScale *displacementScales;
unsigned int clutterLayerCount;
StTerrainClutterLayer *clutterLayers;
GfxWrappedBuffer clutterLayersBuffer;
StClutterSampleBitmask clutterSampleBitmask;
unsigned int clutterTileCount;
unsigned int *clutterTileInstanceCounts;
unsigned int layerMaskMapIdxsCount;
unsigned __int16 *layerMaskMapIdxs;
GfxImage *heightMap;
GfxImage *cutoutMap;
GfxImage *combinedAlbedoMap;
GfxImage *combinedNogMap;
float combinedMapStreamRadiusInvSq;
unsigned int lightmapIndex;
}
;

struct StTerrainMaps
{
int mapResolution;
int paddedResolution;
}
;

struct StLightmap
{
GfxImage *images[3];
}
;

struct StTerrain
{
const char *name;
unsigned int surfaceCount;
StDiskTerrainSurface *surfaces;
unsigned int mapsCount;
StTerrainMaps *maps;
unsigned int flattenedImagesCount;
GfxImage **flattenedImages;
StClutterSamplePoints clutterSamplePoints;
GfxLightmapType lightmapType;
unsigned int lightmapCount;
StLightmap *lightmaps;
}
;

struct NativeScriptPatchFunction
{
int nativeFunctionIndex;
unsigned int canonicalFunctionName;
const char *functionName;
}
;

struct NativeScriptPatchFile
{
const char *name;
unsigned int checksum;
ScriptFile *scriptFile;
int numPatchFunctions;
NativeScriptPatchFunction *patchFunctions;
}
;

struct StaticModelCollisionCompressedInstance
{
unsigned __int16 origin[3];
unsigned __int16 angles[3];
unsigned __int16 scale;
}
;

struct StaticModelCollisionCompressedModel
{
scr_string_t name;
PhysicsAsset *physicsAsset;
XModelDetailCollision *detailCollision;
void *shapes[2];
unsigned __int16 shapeTags[2];
unsigned int numInstances;
StaticModelCollisionCompressedInstance *instances;
}
;

struct StaticModelCollisionCompressedModelList
{
unsigned int numModels;
StaticModelCollisionCompressedModel *models;
unsigned int structureMemoryCost;
unsigned int numInstances;
unsigned int assetCost;
}
;

struct CollisionTile
{
const char *name;
unsigned int index;
char gridId[2];
vec3_t origin;
StaticModelCollisionCompressedModelList *staticModelCollision;
unsigned int havokTileShapeDataSize;
char *havokTileShapeData;
char *havokTileShapePointer;
unsigned int worldCollisionShapeIdx;
}
;

struct ExecutionDef
{
const char *name;
unsigned int animType;
bool usePropWeapon;
scr_string_t propWeaponAttachTag;
bool propWeaponInventoryOverrideEnabled;
bool propWeaponEnableIK;
weapClass_t propWeaponInventoryOverrideClass;
int durationVictimStandMs;
XCam *xcamVictimStand;
int durationVictimProneMs;
XCam *xcamVictimProne;
int durationVictimLastStandMs;
XCam *xcamVictimLastStand;
float dlcFloat[8];
}
;

struct CarryObjectDef
{
const char *name;
CarryObjectType type;
XModel *carryViewModel;
XModel *carryWorldModel;
Gesture *gesture;
scr_string_t attachTag;
bool allowADS;
}
;

struct SoundBankListDef
{
const char *name;
unsigned __int16 soundBankCount;
unsigned int *soundBankNames;
}
;

struct DynEntityDef
{
DynEntityType type;
char __pad0[31];
GfxPlacement pose;
char __pad1[4];
XModel *baseModel;
unsigned __int16 brushModel;
bool spawnActive;
char __pad2[1];
__int16 instanceIndex;
char __pad3[10];
DynEntityLinkToDef *linkTo;
bool noPhysics;
bool unk1;
bool unk2;
bool distantShadows;
bool noSpotShadows;
bool isTransient;
bool transientZoneLoaded;
char unk3;
char priority;
char __pad6[7];
}
;

struct SpatialPartition_TransientPopulation
{
SpatialPartition_Population_Node *buckets;
SpatialPartition_Population_Node *bucketTails;
unsigned int *partitionIds;
SpatialPartition_Population_Node *(__fastcall *getNodeFromIndex)(const SpatialPartition_TransientPopulation *, unsigned int);
unsigned int (__fastcall *getIndexFromNode)(const SpatialPartition_TransientPopulation *, const SpatialPartition_Population_Node *);
void *userData;
unsigned int partitionCount;
unsigned int currentPopulation;
}
;

struct DynEntityList
{
const char *name;
char index[2];
unsigned int dynEntCount[2];
unsigned int dynEntCountTotal;
DynEntityDef *dynEntDefList[2];
SpatialPartition_TransientPopulation *dynEntSpatialPopulation[2];
SpatialPartition_Population_Node dynEntNoSpatialList;
SpatialPartition_Population_Node dynEntNoSpatialListTail;
}
;

struct DLogHash
{
unsigned int hash;
int index;
}
;

struct DLogEnum
{
const char *name;
DLogHashTable values;
}
;

struct DLogColumn
{
const char *name;
int type;
int stringSize;
int arrayCount;
DLogEnum *enumRef;
}
;

struct DLogEvent
{
const char *name;
DLogRow *rows;
int rowCount;
DLogHashTable rowHashTable;
DLogColumn *columns;
int columnCount;
DLogHashTable columnHashTable;
DLogChannelRef *channelRefs;
int channelRefCount;
bool requireSchema;
DLogSampleGroup sampleGroup;
unsigned __int64 protoGuid;
}
;

struct DLogRow
{
const char *name;
const char *type;
DLogColumn *columns;
int columnCount;
int arrayCount;
DLogHashTable columnHashTable;
void (__fastcall *autoFill)(DLogContext *, const DLogEvent *);
}
;

struct DLogContext
{
int position;
char *buffer;
int bufferSize;
const DLogEvent *event;
const DLogRow *row;
const DLogColumn *column;
const DLogColumn *arrayColumn;
bool autoEndEvent;
DLogError error;
unsigned __int64 userId;
char rowArrayName[64];
int rowArrayIndex;
char arrayName[64];
int arrayIndex;
DLogType *arrayType;
int *arraySize;
char eventName[64];
char rowName[64];
}
;

struct DLogChannel
{
const char *name;
const DLogSerializer **serializers;
int serializerCount;
const DLogSerializer **postSerializers;
int postSerializerCount;
const DLogEndpoint *endpoint;
int bufferSize;
int bufferTimeMilliseconds;
int bufferFlushSize;
unsigned int envs;
unsigned int platforms;
bool requireSchema;
}
;

struct DLogChannelRef
{
const DLogChannel *channel;
const char *categoryName;
char sample[3];
}
;

struct DLogReadContext
{
const DLogEvent *event;
const DLogRow *row;
int position;
char *buffer;
int bufferSize;
DLogError error;
unsigned __int64 userId;
char rowArrayName[64];
int rowArrayIndex;
}
;

struct DLogEventInfo
{
const DLogEvent *event;
unsigned __int64 guid;
unsigned int serializationFormat;
unsigned int compressionType;
const char *categoryName;
DLogChannelBuffer *channelBuffer;
const DLogChannelRef *channelRef;
int eventCount;
}
;

struct DLogChannelBuffer
{
const char *name;
const DLogChannel *channel;
DLogContext context;
int sendTimeMilliseconds;
int eventCount;
DLogEventInfo eventInfo;
bool fullWarningDisplayed;
bool inUse;
}
;

struct DLogSerializer
{
bool requireSchema;
const char *name;
int (__fastcall *serialize)(DLogReadContext *, DLogEventInfo *, void *, int);
}
;

struct DLogEndpoint
{
bool requireSchema;
const char *name;
bool (__fastcall *endpoint)(unsigned __int64, const void *, int, DLogEventInfo *);
}
;

struct DLogVar
{
const char *name;
const char *value;
}
;

struct DLogSchema
{
const char *name;
const char *project;
DLogChannel *channels;
int channelCount;
DLogVar *vars;
int varCount;
unsigned __int64 guidSeed;
DLogEvent *events;
int eventCount;
DLogHashTable eventHashTable;
}
;

union XAssetHeader
{
void *data;
PhysicsLibrary *physicsLibrary;
PhysicsSFXEventAsset *physicsSFXEventAsset;
PhysicsVFXEventAsset *physicsVFXEventAsset;
PhysicsAsset *physicsAsset;
PhysicsFXPipeline *physicsFXPipeline;
PhysicsFXShape *physicsFXShape;
XAnimParts *parts;
XModelSurfs *modelSurfs;
XModel *model;
MayhemData *mayhem;
Material *material;
ComputeShader *computeShader;
MaterialVertexShader *vertexShader;
MaterialHullShader *hullShader;
MaterialDomainShader *domainShader;
MaterialPixelShader *pixelShader;
MaterialVertexDeclaration *vertexDecl;
MaterialTechniqueSet *techniqueSet;
GfxImage *image;
SndGlobals *soundGlobals;
SndBank *soundBank;
SndBankTransient *soundBankTransient;
clipMap_t *clipMap;
ComWorld *comWorld;
GlassWorld *glassWorld;
PathData *pathData;
NavMeshData *navMeshData;
MapEnts *mapEnts;
FxWorld *fxWorld;
GfxWorld *gfxWorld;
GfxWorldTransientZone *gfxWorldTransientZone;
GfxLightDef *lightDef;
AnimationClass *animClass;
PlayerAnimScript *playerAnim;
Gesture *gesture;
LocalizeEntry *localize;
WeaponAttachment *attachment;
WeaponCompleteDef *weapon;
ParticleSystemDef *vfx;
FxEffectDef *fx;
FxImpactTable *impactFx;
SurfaceFxTable *surfaceFx;
RawFile *rawfile;
ScriptFile *scriptfile;
StringTable *stringTable;
LeaderboardDef *leaderboardDef;
VirtualLeaderboardDef *virtualLeaderboardDef;
StructuredDataDefSet *structuredDataDefSet;
DDLFile *ddlFile;
TracerDef *tracerDef;
VehicleDef *vehDef;
AddonMapEnts *addonMapEnts;
NetConstStrings *netConstStrings;
LuaFile *luaFile;
ScriptableDef *scriptable;
EquipmentSoundTable *equipSndTable;
VectorField *vectorField;
FxParticleSimAnimation *particleSimAnimation;
StreamingInfo *streamingInfo;
LaserDef *laserDef;
TTFDef *ttfDef;
SuitDef *suitDef;
SuitAnimPackage *suitAnimPackage;
SpaceshipTargetDef *spaceshipTargetDef;
RumbleInfo *rumble;
RumbleGraph *rumbleGraph;
WeaponAnimPackage *weaponAnimPackage;
WeaponSFXPackage *weaponSFXPackage;
WeaponVFXPackage *weaponVFXPackage;
BehaviorTree *behaviorTree;
XAnimProceduralBones *proceduralBones;
ReticleDef *reticleDef;
GfxLightMap *lightMap;
}
;

struct PhysicsSFXEventAssetRules
{
int numrules;
PhysicsSFXEventAssetRule *rules;
}
;

struct PhysicsVFXEventAssetRules
{
int numrules;
PhysicsVFXEventAssetRule *rules;
}
;

enum PhysicsAssetUseCategory
{
PhysicsAssetUseCategory_StaticModel = 0x0,
PhysicsAssetUseCategory_CommonEntity = 0x1,
PhysicsAssetUseCategory_Vehicle = 0x2,
PhysicsAssetUseCategory_Dynent = 0x3,
PhysicsAssetUseCategory_EntityOrDynent = 0x4,
PhysicsAssetUseCategory_Ragdoll = 0x5,
PhysicsAssetUseCategory_Particle = 0x6,
PhysicsAssetUseCategory_Custom = 0x7,
PhysicsAssetUseCategory_NumCategories = 0x8,
}
;

enum PhysicsAssetSimulationCategory
{
PhysicsAssetSimulationCategory_Static = 0x0,
PhysicsAssetSimulationCategory_KeyframedEntity = 0x1,
PhysicsAssetSimulationCategory_KeyframedAnimated = 0x2,
PhysicsAssetSimulationCategory_KeyframedBoneOffset = 0x3,
PhysicsAssetSimulationCategory_DynamicEntity = 0x4,
PhysicsAssetSimulationCategory_DynamicDebris = 0x5,
PhysicsAssetSimulationCategory_Ragdoll = 0x6,
PhysicsAssetSimulationCategory_VehicleChassis = 0x7,
PhysicsAssetSimulationCategory_VehicleChassisNoForce = 0x8,
PhysicsAssetSimulationCategory_Custom = 0x9,
PhysicsAssetSimulationCategory_NumCategories = 0xA,
}
;

struct PhysicsAssetUsageCounter
{
int serverEnt;
int clientEnt;
int dynEnt;
}
;

enum PhysicsFXPipelineIntegratorType
{
Types_PhysicsFXPipelineIntegrator_Basic = 0x0,
Types_PhysicsFXPipelineIntegrator_LinAngDamping = 0x1,
Types_PhysicsFXPipelineIntegratorCount = 0x2,
}
;

struct PhysicsFXPipelineIntegrator_Basic
{
}
;

struct PhysicsFXPipelineIntegrator_LinVelDamping
{
PhysicsFXPipelineIntegrator_Basic basic;
float linearDamping;
float angularDamping;
}
;

union PhysicsFXPipelineIntegratorUnion
{
PhysicsFXPipelineIntegrator_Basic basicIntegrator;
PhysicsFXPipelineIntegrator_LinVelDamping linVelDampingIntegrator;
}
;

struct PhysicsFXPipelineIntegrator
{
PhysicsFXPipelineIntegratorType type;
PhysicsFXPipelineIntegratorUnion u;
}
;

struct PhysicsWorldMeshDebugData
{
int numVertsTotal;
int numTrisTotal;
int numVertsGeneral;
int numTrisGeneral;
int numVertsCharacter;
int numTrisCharacter;
int numVertsItem;
int numTrisItem;
int numVertsWeapon;
int numTrisWeapon;
int numVertsMisc;
int numTrisMisc;
unsigned int numGeneralWorldMeshDebugDrawBuffers;
PhysicsWorldMeshDebugDrawBuffer *generalWorldMeshDebugDrawBuffers;
unsigned int numCharacterWorldMeshDebugDrawBuffers;
PhysicsWorldMeshDebugDrawBuffer *characterWorldMeshDebugDrawBuffers;
unsigned int numItemWorldMeshDebugDrawBuffers;
PhysicsWorldMeshDebugDrawBuffer *itemWorldMeshDebugDrawBuffers;
unsigned int numWeaponWorldMeshDebugDrawBuffers;
PhysicsWorldMeshDebugDrawBuffer *weaponWorldMeshDebugDrawBuffers;
unsigned int numMiscWorldMeshDebugDrawBuffers;
PhysicsWorldMeshDebugDrawBuffer *miscWorldMeshDebugDrawBuffers;
}
;

struct $393C16A032292777F0C3725FFB2C0008
{
float x;
float y;
float z;
}
;

struct Bounds
{
vec3_t midPoint;
vec3_t halfSize;
}
;

typedef std::int32_t scr_string_t
;

union XAnimIndices
{
unsigned __int8 *_1;
unsigned __int16 *_2;
void *data;
}
;

struct XPakEntryInfo
{
unsigned __int64 key;
__int64 offset;
unsigned __int64 size;
unsigned __int64 xpakIndex : 8;
unsigned __int64 compressed : 1;
unsigned __int64 valid : 1;
unsigned __int64 adjacentLeftType : 3;
unsigned __int64 adjacentRightType : 3;
unsigned __int64 adjacentLeft : 19;
unsigned __int64 adjacentRight : 19;
unsigned __int64 padding : 10;
}
;

struct DObjPartBits
{
unsigned int array[8];
}
;

struct XModelPhysicsUsageCounter
{
int serverEnt;
int clientEnt;
int dynEnt;
}
;

struct XModelLodInfo
{
XModelSurfs *modelSurfsStaging;
XSurface *surfs;
float dist;
unsigned __int16 numsurfs;
unsigned __int16 surfIndex;
int partBits[8];
volatile int subdivLodValidMask;
char flags;
}
;

enum MaterialGeometryType
{
MTL_TYPE_DEFAULT = 0x0,
MTL_TYPE_MODEL = 0x80000,
MTL_TYPE_MODEL_TENSION = 0x80001,
MTL_TYPE_MODEL_MAYHEM = 0x80002,
MTL_TYPE_MODEL_VERTCOL = 0x80004,
MTL_TYPE_MODEL_VERTCOL_TENSION = 0x80005,
MTL_TYPE_MODEL_LMAP = 0x80008,
MTL_TYPE_MODEL_LMAP_VERTCOL = 0x8000C,
MTL_TYPE_MODEL_SELFVIS = 0x80010,
MTL_TYPE_MODEL_VERTCOL_SELFVIS = 0x80014,
MTL_TYPE_MODEL_LMAP_SELFVIS = 0x80018,
MTL_TYPE_MODEL_TENSION_SELFVIS = 0x80011,
MTL_TYPE_MODEL_SECONDUV_SELFVIS = 0x80030,
MTL_TYPE_MODEL_SECONDUV_VERTCOL_SELFVIS = 0x80034,
MTL_TYPE_MODEL_SUBDIV = 0x88000,
MTL_TYPE_MODEL_SUBDIV_VERTCOL = 0x88004,
MTL_TYPE_MODEL_SUBDIV_TENSION = 0x88001,
MTL_TYPE_MODEL_SUBDIV_MAYHEM = 0x88002,
MTL_TYPE_MODEL_SUBDIV_LMAP = 0x88008,
MTL_TYPE_MODEL_SUBDIV_LMAP_VERTCOL = 0x8800C,
MTL_TYPE_MODEL_SUBDIV_SELFVIS = 0x88010,
MTL_TYPE_MODEL_SUBDIV_VERTCOL_SELFVIS = 0x88014,
MTL_TYPE_MODEL_SUBDIV_TENSION_SELFVIS = 0x88011,
MTL_TYPE_MODEL_SUBDIV_LMAP_SELFVIS = 0x88018,
MTL_TYPE_STATIC_MODEL = 0x90000,
MTL_TYPE_STATIC_MODEL_TENSION = 0x90001,
MTL_TYPE_STATIC_MODEL_MAYHEM = 0x90002,
MTL_TYPE_STATIC_MODEL_VERTCOL = 0x90004,
MTL_TYPE_STATIC_MODEL_VERTCOL_TENSION = 0x90005,
MTL_TYPE_STATIC_MODEL_LMAP = 0x90008,
MTL_TYPE_STATIC_MODEL_LMAP_VERTCOL = 0x9000C,
MTL_TYPE_STATIC_MODEL_SELFVIS = 0x90010,
MTL_TYPE_STATIC_MODEL_VERTCOL_SELFVIS = 0x90014,
MTL_TYPE_STATIC_MODEL_LMAP_SELFVIS = 0x90018,
MTL_TYPE_STATIC_MODEL_TENSION_SELFVIS = 0x90011,
MTL_TYPE_STATIC_MODEL_SECONDUV_SELFVIS = 0x90030,
MTL_TYPE_STATIC_MODEL_SECONDUV_VERTCOL_SELFVIS = 0x90034,
MTL_TYPE_STATIC_MODEL_SUBDIV = 0x98000,
MTL_TYPE_STATIC_MODEL_SUBDIV_VERTCOL = 0x98004,
MTL_TYPE_STATIC_MODEL_SUBDIV_TENSION = 0x98001,
MTL_TYPE_STATIC_MODEL_SUBDIV_MAYHEM = 0x98002,
MTL_TYPE_STATIC_MODEL_SUBDIV_LMAP = 0x98008,
MTL_TYPE_STATIC_MODEL_SUBDIV_LMAP_VERTCOL = 0x9800C,
MTL_TYPE_STATIC_MODEL_SUBDIV_SELFVIS = 0x98010,
MTL_TYPE_STATIC_MODEL_SUBDIV_VERTCOL_SELFVIS = 0x98014,
MTL_TYPE_STATIC_MODEL_SUBDIV_TENSION_SELFVIS = 0x98011,
MTL_TYPE_STATIC_MODEL_SUBDIV_LMAP_SELFVIS = 0x98018,
MTL_TYPE_SPLINED_MODEL = 0xA0000,
MTL_TYPE_SPLINED_MODEL_TENSION = 0xA0001,
MTL_TYPE_SPLINED_MODEL_MAYHEM = 0xA0002,
MTL_TYPE_SPLINED_MODEL_VERTCOL = 0xA0004,
MTL_TYPE_SPLINED_MODEL_VERTCOL_TENSION = 0xA0005,
MTL_TYPE_SPLINED_MODEL_LMAP = 0xA0008,
MTL_TYPE_SPLINED_MODEL_LMAP_VERTCOL = 0xA000C,
MTL_TYPE_SPLINED_MODEL_SELFVIS = 0xA0010,
MTL_TYPE_SPLINED_MODEL_VERTCOL_SELFVIS = 0xA0014,
MTL_TYPE_SPLINED_MODEL_LMAP_SELFVIS = 0xA0018,
MTL_TYPE_SPLINED_MODEL_TENSION_SELFVIS = 0xA0011,
MTL_TYPE_SPLINED_MODEL_SECONDUV_SELFVIS = 0xA0030,
MTL_TYPE_SPLINED_MODEL_SECONDUV_VERTCOL_SELFVIS = 0xA0034,
MTL_TYPE_SPLINED_MODEL_SUBDIV = 0xA8000,
MTL_TYPE_SPLINED_MODEL_SUBDIV_VERTCOL = 0xA8004,
MTL_TYPE_SPLINED_MODEL_SUBDIV_TENSION = 0xA8001,
MTL_TYPE_SPLINED_MODEL_SUBDIV_MAYHEM = 0xA8002,
MTL_TYPE_SPLINED_MODEL_SUBDIV_LMAP = 0xA8008,
MTL_TYPE_SPLINED_MODEL_SUBDIV_LMAP_VERTCOL = 0xA800C,
MTL_TYPE_SPLINED_MODEL_SUBDIV_SELFVIS = 0xA8010,
MTL_TYPE_SPLINED_MODEL_SUBDIV_VERTCOL_SELFVIS = 0xA8014,
MTL_TYPE_SPLINED_MODEL_SUBDIV_TENSION_SELFVIS = 0xA8011,
MTL_TYPE_SPLINED_MODEL_SUBDIV_LMAP_SELFVIS = 0xA8018,
MTL_TYPE_SPLINED_STATIC_MODEL = 0xB0000,
MTL_TYPE_SPLINED_STATIC_MODEL_TENSION = 0xB0001,
MTL_TYPE_SPLINED_STATIC_MODEL_MAYHEM = 0xB0002,
MTL_TYPE_SPLINED_STATIC_MODEL_VERTCOL = 0xB0004,
MTL_TYPE_SPLINED_STATIC_MODEL_VERTCOL_TENSION = 0xB0005,
MTL_TYPE_SPLINED_STATIC_MODEL_LMAP = 0xB0008,
MTL_TYPE_SPLINED_STATIC_MODEL_LMAP_VERTCOL = 0xB000C,
MTL_TYPE_SPLINED_STATIC_MODEL_SELFVIS = 0xB0010,
MTL_TYPE_SPLINED_STATIC_MODEL_VERTCOL_SELFVIS = 0xB0014,
MTL_TYPE_SPLINED_STATIC_MODEL_LMAP_SELFVIS = 0xB0018,
MTL_TYPE_SPLINED_STATIC_MODEL_TENSION_SELFVIS = 0xB0011,
MTL_TYPE_SPLINED_STATIC_MODEL_SECONDUV_SELFVIS = 0xB0030,
MTL_TYPE_SPLINED_STATIC_MODEL_SECONDUV_VERTCOL_SELFVIS = 0xB0034,
MTL_TYPE_SPLINED_STATIC_MODEL_SUBDIV = 0xB8000,
MTL_TYPE_SPLINED_STATIC_MODEL_SUBDIV_VERTCOL = 0xB8004,
MTL_TYPE_SPLINED_STATIC_MODEL_SUBDIV_TENSION = 0xB8001,
MTL_TYPE_SPLINED_STATIC_MODEL_SUBDIV_MAYHEM = 0xB8002,
MTL_TYPE_SPLINED_STATIC_MODEL_SUBDIV_LMAP = 0xB8008,
MTL_TYPE_SPLINED_STATIC_MODEL_SUBDIV_LMAP_VERTCOL = 0xB800C,
MTL_TYPE_SPLINED_STATIC_MODEL_SUBDIV_SELFVIS = 0xB8010,
MTL_TYPE_SPLINED_STATIC_MODEL_SUBDIV_VERTCOL_SELFVIS = 0xB8014,
MTL_TYPE_SPLINED_STATIC_MODEL_SUBDIV_TENSION_SELFVIS = 0xB8011,
MTL_TYPE_SPLINED_STATIC_MODEL_SUBDIV_LMAP_SELFVIS = 0xB8018,
MTL_TYPE_GLASS = 0x100000,
MTL_TYPE_GLASS_VERTCOL = 0x100004,
MTL_TYPE_WORLD = 0x210000,
MTL_TYPE_WORLD_VERTCOL = 0x210004,
MTL_TYPE_WORLD_LAYERED = 0x210040,
MTL_TYPE_BMODEL = 0x200000,
MTL_TYPE_BMODEL_VERTCOL = 0x200004,
MTL_TYPE_BMODEL_LAYERED = 0x200040,
MTL_TYPE_SUPERTERRAIN = 0x210080,
MTL_TYPE_SUPERTERRAIN_GPUGEN = 0x210180,
MTL_TYPE_SUPERTERRAIN_VERTEX_LAYER_MASK = 0x210880,
MTL_TYPE_SUPERTERRAIN_GPUGEN_VERTEX_LAYER_MASK = 0x210980,
MTL_TYPE_AVTTERRAIN = 0x210200,
MTL_TYPE_SUPERTERRAIN_3D_VERT_POS = 0x210480,
MTL_TYPE_EFFECT_LMAP = 0x40000C,
MTL_TYPE_EFFECT_VERTLIT = 0x401004,
MTL_TYPE_EFFECT_QUAD = 0x40200C,
MTL_TYPE_EFFECT_FLARE = 0x404004,
MTL_TYPE_INFO_CARRIER = 0x800000,
}
;

union $49ECFF4638277A3511BA4CBB6D1719DF
{
unsigned __int16 runtimeFlags;
unsigned __int16 flags;
}
;

struct GfxDrawSurfFields
{
unsigned __int64 objectId : 16;
unsigned __int64 pad0 : 32;
unsigned __int64 tessellation : 2;
unsigned __int64 pad1 : 14;
unsigned __int64 pad2 : 64;
}
;

struct Packed128
{
std::uint64_t p0;
std::uint64_t p1;
}
;

union GfxDrawSurf
{
GfxDrawSurfFields fields;
Packed128 packed;
}
;

struct GfxComputeShaderLoadDef
{
unsigned __int8 *program;
unsigned int programSize;
}
;

struct ComputeShaderProgram
{
ID3D11ComputeShader *cs;
GfxComputeShaderLoadDef loadDef;
}
;

enum TechniqueMask
{
TECHNIQUE_MASK = 0x3F,
NUM_TECHNIQUE_MASK_ELEMS = 0x3,
}
;

enum GfxTextureId
{
NULLID = 0x0,
}
;

enum GfxPixelFormat
{
GFX_PF_INVALID = 0x0,
GFX_PF_R8 = 0x1,
GFX_PF_A8 = 0x2,
GFX_PF_L8 = 0x3,
GFX_PF_R8G8 = 0x4,
GFX_PF_L8A8 = 0x5,
GFX_PF_R8G8B8A8 = 0x6,
GFX_PF_R8G8B8A8_SRGB = 0x7,
GFX_PF_R8_SNORM = 0x8,
GFX_PF_R8G8_SNORM = 0x9,
GFX_PF_R16 = 0xA,
GFX_PF_R16G16 = 0xB,
GFX_PF_R16G16B16A16 = 0xC,
GFX_PF_R16_SNORM = 0xD,
GFX_PF_R16F = 0xE,
GFX_PF_R16G16F = 0xF,
GFX_PF_R16G16B16A16F = 0x10,
GFX_PF_R32F = 0x11,
GFX_PF_R32G32F = 0x12,
GFX_PF_R32G32B32A32F = 0x13,
GFX_PF_D16 = 0x14,
GFX_PF_D32F = 0x15,
GFX_PF_D32F_S8 = 0x16,
GFX_PF_R8_UINT = 0x17,
GFX_PF_R16_UINT = 0x18,
GFX_PF_R32_UINT = 0x19,
GFX_PF_R32G32_UINT = 0x1A,
GFX_PF_R32G32B32A32_UINT = 0x1B,
GFX_PF_R10G10B10A2_UINT = 0x1C,
GFX_PF_R5G6B5 = 0x1D,
GFX_PF_R10G10B10A2 = 0x1E,
GFX_PF_R9G9B9E5_SHAREDEXP = 0x1F,
GFX_PF_R11G11B10F = 0x20,
GFX_PF_BC1 = 0x21,
GFX_PF_BC1_SRGB = 0x22,
GFX_PF_BC2 = 0x23,
GFX_PF_BC2_SRGB = 0x24,
GFX_PF_BC3 = 0x25,
GFX_PF_BC3_SRGB = 0x26,
GFX_PF_BC4 = 0x27,
GFX_PF_BC5 = 0x28,
GFX_PF_BC5S = 0x29,
GFX_PF_BC6H = 0x2A,
GFX_PF_BC6HS = 0x2B,
GFX_PF_BC7 = 0x2C,
GFX_PF_BC7_SRGB = 0x2D,
GFX_PF_R8G8B8A8_SNORM = 0x2E,
GFX_PF_R1 = 0x2F,
GFX_PF_R4G4 = 0x30,
GFX_PF_R10G10B10A2_SNORM = 0x31,
GFX_PF_COUNT = 0x32,
}
;

enum GfxImageFlags : unsigned __int32
{
IMG_FLAG_NONE = 0x0,
IMG_DISK_FLAG_NOPICMIP = 0x1,
IMG_DISK_FLAG_NOMIPMAPS = 0x2,
IMG_DISK_FLAG_UNUSED = 0x4,
IMG_DISK_FLAG_NORMAL_OCCLUSON_GLOSS = 0x8,
IMG_DISK_FLAG_CLAMP_U = 0x10,
IMG_DISK_FLAG_CLAMP_V = 0x20,
IMG_DISK_FLAG_STREAMED = 0x40,
IMG_DISK_FLAG_USE_OODLE_COMPRESSION = 0x80,
IMG_DISK_FLAG_GAMMA_SRGB = 0x100,
IMG_DISK_FLAG_PACKED_ATLAS = 0x200,
IMG_CREATE_FLAG_UNTILED = 0x400,
IMG_CREATE_FLAG_CPU_READ = 0x800,
IMG_CREATE_FLAG_CPU_WRITE = 0x1000,
IMG_DISK_FLAG_AUTOMETALNESS = 0x2000,
IMG_DISK_FLAG_AUTODISPLACEMENT = 0x4000,
IMG_DISK_FLAG_MAPTYPE_2D = 0x0,
IMG_DISK_FLAG_MAPTYPE_CUBE = 0x8000,
IMG_DISK_FLAG_MAPTYPE_3D = 0x10000,
IMG_DISK_FLAG_MAPTYPE_1D = 0x18000,
IMG_DISK_FLAG_MAPTYPE_ARRAY = 0x20000,
IMG_DISK_FLAG_MAPTYPE_CUBE_ARRAY = 0x28000,
IMG_DISK_FLAG_INVERT_ALPHA = 0x40000,
IMG_DISK_FLAG_PREMUL_ALPHA = 0x80000,
IMG_DISK_FLAG_MIPGEN_ORIGINAL = 0x0,
IMG_DISK_FLAG_MIPGEN_LANCZOS3 = 0x100000,
IMG_DISK_FLAG_MIPGEN_CATMULL_ROM = 0x200000,
IMG_DISK_FLAG_MIPGEN_CUBIC_BSPLINE = 0x300000,
IMG_DISK_FLAG_MIPGEN_BOX = 0x400000,
IMG_DISK_FLAG_MIPGEN_COVERAGE_PRESERVING = 0x500000,
IMG_CREATE_FLAG_RW_VIEW = 0x800000,
IMG_CREATE_FLAG_DYNAMIC = 0x1000000,
IMG_DISK_FLAG_PREMUL_KEEP_ALPHA = 0x2000000,
IMG_DISK_FLAG_RTT = 0x4000000,
IMG_DISK_FLAG_EXTRACT_ALPHA = 0x8000000,
IMG_DISK_FLAG_OCTAHEDRON = 0x10000000,
IMG_CREATE_FLAG_STAGING = 0x20000000,
IMG_CREATE_FLAG_VOLUMETRIC_LAYOUT_OVERRIDE = 0x40000000,
IMG_CREATE_FLAG_TYPELESS = 0x80000000,
}
;

union GfxImageSemanticSpecific
{
float atlasFps;
unsigned int albedoMapScaleBias;
unsigned int normalMapScaleBias;
unsigned int maxMipMap;
}
;

struct GfxImageAtlasSize
{
char rowCount;
char colCount;
}
;

union GfxImageAtlasInfo
{
GfxImageAtlasSize atlasSize;
unsigned __int16 packedAtlasDataSize;
}
;

struct GfxImageStreamLevelCountAndSize
{
unsigned __int32 pixelSize : 26;
unsigned __int32 levelCount : 6;
}
;

struct GfxImageStreamData
{
unsigned __int16 width;
unsigned __int16 height;
GfxImageStreamLevelCountAndSize levelCountAndSize;
}
;

struct streamer_handle_t
{
unsigned __int64 data;
}
;

union GfxImagePixels
{
streamer_handle_t streamedDataHandle;
char *residentData;
}
;

struct SoundTable
{
SndStringHash id;
ADSRSetting *adsrSettings;
unsigned int adsrCount;
AmbientDef *ambientDefs;
unsigned int ambientDefCount;
AmbientEvent *ambientEvents;
unsigned int ambientEventCount;
AmbientElement *ambientElements;
unsigned int ambientElementsCount;
FilterDef *filters;
unsigned int filterCount;
MixDef *mixes;
unsigned int mixCount;
AdsrZoneEntry *npcADSRZones;
unsigned int npcADSRZoneCount;
OccludeDef *occlusionFilters;
unsigned int occlusionFilterCount;
AdsrZoneEntry *playerADSRZones;
unsigned int playerADSRZoneCount;
ReverbDef *reverbs;
unsigned int reverbCount;
TimescaleEntry *timeScaleSettings;
unsigned int timeScaleSettingCount;
ZoneDef *zones;
unsigned int zoneCount;
unk_1453E2FD8 *unk;
unsigned int unkCount;
FullOcclusionDef *fullOcclusionDefs;
unsigned int fullOcclusionDefCount;
PlayerBreathStateDef *plrBreathStateDefs;
unsigned int plrBreathStateDefCount;
}
;

struct SndBankStreamInfo
{
StreamKey *loadedStreamKey;
bool alwaysLoaded;
}
;

struct SndBank
{
const char *name;
const char *zone;
const char *gameLanguage;
const char *soundLanguage;
unsigned int aliasCount;
SndAliasList *alias;
SndIndexEntry *aliasIndex;
unsigned int sendEffectCount;
SndSendEffectParams *sendEffects;
SoundTable soundTable;
unsigned int duckCount;
SndDuck *ducks;
unsigned int musicSetCount;
SndMusicSet *musicSets;
}
;

struct MapTriggers
{
unsigned int count;
TriggerModel *models;
unsigned int hullCount;
TriggerHull *hulls;
unsigned int slabCount;
TriggerSlab *slabs;
unsigned int windingCount;
TriggerWinding *windings;
unsigned int windingPointCount;
TriggerWindingPoint *windingPoints;
}
;

struct PhysicsCapacities
{
int maxNumRigidBodiesServer;
int maxNumDetailRigidBodiesServer;
int maxNumConstraintsServer;
int maxNumMotionsServer;
int maxNumRigidBodiesClient;
int maxNumDetailRigidBodiesClient;
int maxNumConstraintsClient;
int maxNumMotionsClient;
}
;

struct ComChangeListInfo
{
unsigned int changeListNumber;
int time;
const char *userName;
}
;

struct nav_misc_s
{
vec3_t m_StaticBoundsHalfSize;
vec3_t m_StaticBoundsMidPoint;
}
;

struct ClientMapTriggers
{
unsigned int count;
ClientTriggerModel *models;
unsigned int hullCount;
ClientTriggerHull *hulls;
unsigned int slabCount;
TriggerSlab *slabs;
}
;

struct ClientTriggers
{
MapTriggers trigger;
unsigned int triggerStringLength;
char *triggerString;
__int16 *visionSetTriggers;
unsigned __int8 *triggerType;
vec3_t *origins;
float *scriptDelay;
__int16 *audioTriggers;
__int16 *blendLookup;
__int16 *npcTriggers;
__int16 *audioStateIds;
CTAudRvbPanInfo *audioRvbPanInfo;
__int16 *transientIndex;
ClientEntityLinkToDef **linkTo;
}
;

struct ClientTriggerBlend
{
unsigned __int16 numClientTriggerBlendNodes;
ClientTriggerBlendNode *blendNodes;
}
;

struct SpawnPointRecordList
{
unsigned __int16 spawnsCount;
SpawnPointEntityRecord *spawns;
}
;

struct SplineRecordList
{
unsigned __int16 splineCount;
SplinePointRecordList *splines;
}
;

enum DynEntityListId
{
DEFAULT_DYNENTITY_LIST_ID = 0x600,
DYNENTITY_LIST_ID_INVALID = 0x600,
DYNENTITY_LIST_ID_UNLOADING = 0x601,
}
;

struct SpatialPartition_Population_Node
{
unsigned int spatialNodeNext;
}
;

struct ScriptablePhysicsLimits
{
unsigned int clientStandaloneMainRigidBodyCount;
unsigned int clientStandaloneDetailedRigidBodyCount;
unsigned int clientStandaloneMotionCount;
unsigned int serverStandaloneMainRigidBodyCount;
unsigned int serverStandaloneDetailedRigidBodyCount;
unsigned int serverStandaloneMotionCount;
}
;

struct Scriptable_EventSun_Data
{
int startTime;
int transTime;
vec3_t startColorLinearSrgb;
float startIntensity;
float targetIntensity;
vec3_t startAngles;
vec3_t targetAngles;
}
;

struct ScriptableReservedDynents
{
__int16 freeHead;
unsigned int numReservedDynents;
ScriptableReservedDynent *reservedDynents;
}
;

struct ScriptableMapEnts
{
unsigned int totalInstanceCount;
unsigned int runtimeInstanceCount;
unsigned int reservedInstanceCount;
unsigned int pad;
ScriptableInstance *instances;
unk_1453E2560 unk;
Scriptable_EventSun_Data sunClientDatas[2];
ScriptableReservedDynents reservedDynents[2];
unsigned int ffMemCost;
}
;

struct SpawnGroup
{
unsigned __int16 pointCount;
SpawnGroupPoint *points;
unsigned __int16 clusterCount;
SpawnGroupCluster *clusters;
unsigned __int16 zoneCount;
SpawnGroupZone *zones;
}
;

struct ClientSideEffects
{
unsigned int oneshotEffectCount;
ClientOneshotEffectDef *oneshotEffects;
SpatialPartition_Tree *oneshotEffectsTree;
unsigned int oneshotSoundCount;
unsigned int *oneshotSoundIndices;
unsigned int exploderCount;
ClientExploderDef *exploders;
unsigned int intervalSoundCount;
ClientIntervalSoundDef *intervalSounds;
unsigned int loopSoundCount;
ClientLoopSoundDef *loopSounds;
SpatialPartition_Tree *soundsSpatialTree;
unsigned int reactiveEntCount;
ClientReactiveEntDef *reactiveEnts;
SpatialPartition_Tree *reactiveSpatialTree;
unsigned int rootCount;
ClientRootDef *roots;
unsigned int parentCount;
unsigned __int16 *parents;
}
;

struct CreateFxAssetData
{
unsigned __int16 fxPaletteCount;
CreateFxPaletteDef *fxPalette;
}
;

struct ServerSideEffects
{
unsigned __int16 exploderCount;
ServerExploderDef *exploders;
}
;

struct SpawnerList
{
unsigned int spawnerCount;
Spawner *spawnerList;
}
;

struct FxGlassGlobals
{
unsigned int defCount;
FxGlassDef *defs;
unsigned int glassTransientLookupCount;
FxGlassTransientLookup *glassTransientLookup;
float *distances;
}
;

struct FxGlassSystem
{
int time;
int prevTime;
unsigned int defCount;
unsigned int pieceLimit;
unsigned int pieceWordCount;
unsigned int initPieceCount;
unsigned int cellCount;
unsigned int activePieceCount;
unsigned int firstFreePiece;
unsigned int geoDataLimit;
unsigned int geoDataCount;
unsigned int initGeoDataCount;
FxGlassDef *defs;
FxGlassPiecePlace *piecePlaces;
FxGlassPieceState *pieceStates;
FxGlassPieceDynamics *pieceDynamics;
FxGlassGeometryData *geoData;
unsigned int *isInUse;
unsigned int *cellBits;
unsigned __int8 *visData;
vec3_t *linkOrg;
float *halfThickness;
unsigned __int16 *lightingHandles;
FxGlassInitPieceState *initPieceStates;
FxGlassGeometryData *initGeoData;
bool needToCompactData;
unsigned __int8 initCount;
float effectChanceAccum;
int lastPieceDeletionTime;
}
;

enum __shex BspVersion : __int32
{
BSP_VERSION_NAVPOWER_5_4_0 = 0xA4,
BSP_VERSION_HAVOK_2016_2_0 = 0xA5,
BSP_VERSION_HAVOK_VERTEX_ERROR_THRESHOLD = 0xA6,
BSP_VERSION_EDGE_DATA = 0xA7,
BSP_VERSION_IR_LIGHT = 0xA8,
BSP_VERSION_CONTENTS_TO_TOOLS = 0xA9,
BSP_VERSION_AUDIO_PROPAGATION = 0xAA,
BSP_VERSION_EDGE_OCTREE = 0xAB,
BSP_VERSION_HAVOK_2017_1_0 = 0xAC,
BSP_VERSION_CHANGE_STAIRS_OVERRIDE = 0xAD,
BSP_VERSION_SUPPORT_ALIGNED_LUMP = 0xAE,
BSP_VERSION_EDGE_ADJACENCY = 0xAF,
BSP_VERSION_EDGE_MANTLE_ON_OVER_FLAGS = 0xB0,
BSP_VERSION_EDGE_QUERY_BUCKETS = 0xB1,
BSP_VERSION_PHYSICS_DECAL = 0xB2,
BSP_VERSION_DYNAMIC_EDGES = 0xB3,
BSP_VERSION_DISK_AUDIO_PA_SPEAKERS = 0xB4,
BSP_VERSION_DYNAMICLIGHTSETS = 0xB5,
BSP_VERSION_SKYDOME_KVP_REMOVED = 0xB6,
BSP_VERSION_DECAL_VOLUME_BLEND_MAP = 0xB7,
BSP_VERSION_DECAL_VOLUME_NORMAL_BLEND_MODE = 0xB8,
BSP_VERSION_HAVOK_2018_1_0 = 0xB9,
BSP_VERSION_UMBRA_OCCLUDER_METADATA = 0xBA,
BSP_VERSION_DECAL_VOLUME_UV_TRANFORM = 0xBB,
BSP_VERSION_MDAO_VOLUMES = 0xBC,
BSP_VERSION_LIGHT_COLOR_INTENSITY = 0xBD,
BSP_VERSION_DISK_TRIANGULE_SOUP = 0xBE,
BSP_VERSION_COLLISON_PARTITION_FIX = 0xBF,
BSP_VERSION_RADIOMETRIC_SCALED_REMOVED = 0xC0,
BSP_VERSION_TRANSIENT_ASSETS_REMOVED = 0xC1,
BSP_VERSION_LGV = 0xC2,
BSP_VERSION_TRANSIENT_GRID = 0xC3,
BSP_VERSION_INCREASE_MAX_EDGE_OCTREE_NODE = 0xC4,
BSP_VERSION_TRANSIENT_GLASSDEF = 0xC5,
BSP_VERSION_LGV_ONLY = 0xC6,
BSP_VERSION_STATIC_MODEL_COLLECTIONS = 0xC7,
BSP_VERSION_32BIT_STATIC_MODEL_ID = 0xC8,
BSP_VERSION_SKIPPED = 0xC9,
BSP_VERSION_SUPER_TERRAIN = 0xCA,
BSP_VERSION_EDGE_INDEX_INCREASE = 0xCB,
BSP_VERSION_MORE_CLIENT_TRIGGERS = 0xCC,
BSP_VERSION_USHORT_LIGHTMAPINDEX = 0xCD,
BSP_VERSION_EDGE_ADJACENCY_CORRUPTION_FIX = 0xCE,
BSP_VERSION_NORMALIZED_REFLECTION_PROBE = 0xCF,
OLDEST_BSP_VERSION = 0xCF,
BSP_VERSION_CLIENT_SCRIPT_TRIGGERS = 0xD0,
BSP_VERSION_CLIENT_TRIGGER_PRIORITY = 0xD1,
BSP_VERSION_SURFACE_INCREASE = 0xD2,
BSP_VERSION_HAVOK_2018_2_0 = 0xD3,
BSP_VERSION_LIGHT_NEAR_PLANE_BIAS = 0xD4,
BSP_VERSION_CLIENT_TRIGGER_LOCALIZATION = 0xD5,
BSP_VERSION_SOUNDCLIP_WEAPON_ITEM_CLIP = 0xD6,
BSP_VERSION_BSP_CURVATURE = 0xD7,
BSP_VERSION_COLLISIONTILES = 0xD8,
BSP_VERSION_TRISOUP_TREE = 0xD9,
BSP_VERSION_VOLUMETRIC_FOG_HEIGHTFADE = 0xDA,
BSP_VERSION_RUNTIME_CLUTTER = 0xDB,
BSP_VERSION_SMODEL_COLLECTIONS_ADAPTIVE = 0xDC,
BSP_VERSION_COMPRESSED_SUN_SHADOW = 0xDD,
BSP_VERSION_REFLECTION_PROBE_OCTAHEDRON = 0xDE,
BSP_VERSION_LIGHT_COLLIMATION_FACTOR = 0xDF,
BSP_VERSION_SUPER_TERRAIN_LIGHTMAPS = 0xE0,
BSP_VERSION_TRISOUP_TREE_ARRAYS = 0xE1,
BSP_VERSION_SMODEL_COLLECTIONS_MODEL_VALID = 0xE2,
BSP_VERSION_EDGE_ADJACENCY_LAYOUT = 0xE3,
BSP_VERSION_MAP_ASSETS_TRI_CHART_INFO = 0xE4,
BSP_VERSION_REFLECTION_PROBE_TRANSIENTS = 0xE5,
BSP_VERSION_RPROBE_DLS_FLAG = 0xE6,
BSP_VERSION_SKYLIGHT = 0xE7,
BSP_VERSION_BSPINFO_EXPANDED = 0xE8,
BSP_VERSION_SMALLER_COLLISION_TILES = 0xE9,
BSP_VERSION_RUNTIME_CLUTTER_BOUNDS = 0xEA,
BSP_VERSION_TRANSIENT_GLASSPIECE = 0xEB,
BSP_VERSION_EDGE_TRANSIENTS = 0xEC,
BSP_VERSION_PHYSICS_TRIANGLEOBJ = 0xED,
BSP_VERSION_DECAL_VOLUME_TRANSIENTS = 0xEE,
BSP_VERSION_CLIENT_TRIGGER_SOUNDBANK = 0xEF,
BSP_VERSION_SMODEL_COLLECTIONS_MODEL_VALID2 = 0xF0,
BSP_VERSION_SIMPLE_LAYERING = 0xF1,
BSP_VERSION_CRC_FLUSH = 0xF2,
BSP_VERSION_VOLUMETRIC_ANISO_COLOR = 0xF3,
BSP_VERSION = 0xF3,
BSP_VERSION_WORLD_TRANSIENT_METADATA = 0xF4,
BSP_VERSION_SUN_VOLUME = 0xF5,
BSP_VERSION_INVALID = -0x1,
}
;

struct GfxWorldDpvsPlanes
{
int cellCount;
cplane_s *planes;
unsigned __int16 *nodes;
unsigned int *sceneEntCellBits;
}
;

struct __declspec(align(8)) GfxShaderView
{
ID3D12Resource *resource;
unsigned int view;
}
;

struct __cppobj GfxShaderBufferView : GfxShaderView
{
}
;

struct GfxWrappedBaseBuffer
{
ID3D12Resource *buffer;
GfxShaderBufferView view;
}
;

struct __cppobj GfxWrappedBuffer : GfxWrappedBaseBuffer
{
void *data;
}
;

struct GfxWorldSurfaces
{
unsigned int count;
unsigned int surfDataCount;
unsigned int litOpaqueSurfsBegin;
unsigned int litOpaqueSurfsEnd;
unsigned int litDecalSurfsBegin;
unsigned int litDecalSurfsEnd;
unsigned int litTransSurfsBegin;
unsigned int litTransSurfsEnd;
unsigned int emissiveSurfsBegin;
unsigned int emissiveSurfsEnd;
unsigned int himipRadiusInvSqCompactedCount;
unsigned int himipRadiusInvSqBModelCompactedCount;
unsigned int himipRadiusInvSqBModelSurfaceCount;
GfxSurface *surfaces;
unsigned __int16 *surfacesTransientZone;
GfxSurfaceBounds *surfaceBounds;
unsigned int *himipRadiusInvSqIndirection;
float *himipRadiusInvSqCompacted;
unsigned int *himipRadiusInvSqBmodelIndirection;
unsigned int *himipRadiusInvSqBmodelSurfaceIndirection;
float *himipRadiusInvSqBModelCompacted;
GfxDrawSurf *surfaceMaterials;
GfxWorldSurfData *surfData;
GfxWrappedBuffer surfDataBuffer;
}
;

struct GfxClutterInstancePool
{
unsigned int start;
unsigned int size;
}
;

struct GfxShaderRWView
{
ID3D12Resource *rwResource;
ID3D12Resource *rwCounterResource;
unsigned int rwView;
unsigned int rwSubresourceToTransition;
}
;

struct __cppobj GfxShaderBufferRWView : GfxShaderRWView
{
}
;

struct __cppobj GfxWrappedRWBuffer : GfxWrappedBuffer
{
GfxShaderBufferRWView rwView;
}
;

struct bitarray_dynamic
{
unsigned int *array;
unsigned int wordCount;
unsigned int bitCount;
}
;

struct GfxWorldStaticModels
{
unsigned int surfaceCount;
unsigned int modelCount;
unsigned int collectionCount;
unsigned int clutterCollectionCount;
unsigned int instanceCount;
unsigned int clutterTintCount;
unsigned int lmapTransformsCount;
unsigned int materialCount;
unsigned int collectionVisDataCount;
unsigned int splinedModelInstanceCount;
unsigned int splineSegmentCount;
GfxClutterInstancePool clutterInstancePool;
GfxStaticModelSurface *surfaces;
GfxStaticModel *models;
GfxClutterCollection *clutterCollections;
GfxStaticModelCollection *collections;
GfxStaticModelInstance *instances;
char *instanceFlags;
Bounds *collectionBounds;
Material **materials;
GfxModelStaticIndirection *modelStaticIndirection;
GfxWrappedBuffer modelStaticIndirectionBuffer;
GfxLightmapTransformPacked *lmapTransforms;
GfxWrappedBuffer lmapTransformsBuffer;
GfxBaseSModelSurfData *smodelSurfData;
GfxWrappedBuffer smodelSurfDataBuffer;
GfxSModelInstanceData *smodelInstanceData;
GfxWrappedRWBuffer smodelInstanceDataBuffer;
GfxClutterCollectionGPUData *clutterCollectionGPUData;
GfxWrappedRWBuffer clutterCollectionGPUDataBuffer;
vec3_t *clutterTintData;
GfxWrappedRWBuffer clutterTintDataBuffer;
GfxStaticModelExpansion *smodelExpansionData;
GfxWrappedBuffer smodelExpansionDataBuffer;
GfxSModelSurfMatIndirection *smodelSurfMatIndirection;
GfxWrappedBuffer smodelSurfMatIndirectionBuffer;
GfxWrappedBuffer clutterCollectionsDataBuffer;
GfxWrappedBuffer smodelCollectionsDataBuffer;
unsigned int *smodelSurfUGBIndirection;
GfxWrappedRWBuffer smodelSurfUGBIndirectionBuffer;
void *splinedModelInstanceData;
GfxWrappedBuffer splinedModelInstanceBuffer;
void *splineSegmentData;
GfxWrappedBuffer splineSegmentBuffer;
Bounds *splinedModelInstanceBounds;
GfxWrappedBuffer splinedModelInstanceBoundsBuffer;
bitarray_dynamic collectionTransientZonePrevFrameVisibility;
unsigned int *collectionVisData;
char *collectionVisRefCount;
}
;

struct GfxWorldReflectionProbeData
{
unsigned int reflectionProbeCount;
unsigned int sharedReflectionProbeCount;
GfxReflectionProbe *reflectionProbes;
GfxImage *reflectionProbeArrayImage;
unsigned int probeRelightingCount;
GfxReflectionProbeRelightingData *probeRelightingData;
unsigned int reflectionProbeGBufferImageCount;
GfxImage **reflectionProbeGBufferImages;
GfxRawTexture *reflectionProbeGBufferTextures;
unsigned int reflectionProbeInstanceCount;
GfxReflectionProbeInstance *reflectionProbeInstances;
GfxReflectionProbeSampleData *reflectionProbeLightgridSampleData;
void *reflectionProbeLightgridSampleDataBuffer;
void *reflectionProbeLightgridSampleDataBufferView;
void *reflectionProbeLightgridSampleDataBufferRWView;
}
;

struct GfxWorldLightmapReindexData
{
unsigned int imagePixelSize;
unsigned int reindexCount;
GfxWorldLightmapReindexDataElement *reindexElement;
unsigned int packedLightmapCount;
GfxWorldPackedLightmap *packedLightmap;
}
;

enum GfxLightmapType : __int32
{
GFX_LIGHTMAP_TYPE_REGULAR = 0x0,
GFX_LIGHTMAP_TYPE_REGULAR_UNCOMPRESSED = 0x1,
GFX_LIGHTMAP_TYPE_AB_LIGHTING = 0x2,
GFX_LIGHTMAP_TYPE_COUNT = 0x3,
}
;

struct GfxWorldVolumetrics
{
unsigned int volumetricCount;
GfxVolumetric *volumetrics;
}
;

struct GfxCompressedSunShadowParams
{
vec3_t sunDirection;
unsigned int resolution;
float centerX;
float centerY;
float sampleSize;
float nearPlane;
float farPlane;
unsigned int forestSize;
unsigned int flags;
unsigned int reserved;
}
;

struct GfxWorldDraw
{
GfxWorldReflectionProbeData reflectionProbeData;
GfxWorldLightmapReindexData lightmapReindexData;
GfxImage *iesLookupTexture;
unsigned int decalVolumeCollectionCount;
GfxDecalVolumeCollection *decalVolumeCollections;
GfxImage *lightmapOverridePrimary;
GfxImage *lightmapOverrideSecondary;
unsigned int lightMapCount;
GfxLightMap **lightMaps;
GfxTexture *lightmapTextures;
void *unused1;
void *unused2;
void *unused3;
unsigned int transientZoneCount;
GfxWorldTransientZone *transientZones[32];
unsigned int indexCount;
unsigned __int16 *indices;
ID3D11Buffer *indexBuffer;
GfxWorldVolumetrics volumetrics;
}
;

struct GfxDynamicLightsetLMData
{
unsigned int numGChunkLM;
GfxDynamicLightsetGChunk *chunkGLM;
unsigned int numGChunkLMBF;
unsigned __int16 *chunkGLMBF;
unsigned int numLMHave;
unsigned __int16 *chunkWhichLM;
unsigned int *chunkLMStart;
unsigned int numOrigTexels;
unsigned int *origTexels;
GfxWrappedBuffer origTexelsBuffer;
unsigned int *origRecords;
GfxWrappedBuffer origRecordsBuffer;
unsigned int numBFLMTexels;
unsigned int *bfTexels;
GfxWrappedBuffer bfTexelsBuffer;
unsigned int numSkipRecords;
unsigned __int16 *skipRecords;
GfxWrappedBuffer skipRecordsBuffer;
unsigned int *palletteRecords;
GfxWrappedBuffer palletteRecordsBuffer;
}
;

struct GfxDynamicLightsetProbeData
{
unsigned int numGChunk;
GfxDynamicLightsetGChunk *chunkG;
unsigned int numGChunkBF;
unsigned __int16 *chunkGBF;
unsigned int origStart;
unsigned int numOrigProbes;
GfxSHCompressedProbeData *origProbes;
GfxWrappedBuffer origProbesBuffer;
unsigned int numBFProbes;
GfxSHCompressedProbeData *bfProbes;
GfxWrappedBuffer bfProbesBuffer;
unsigned int numSkipRecords;
unsigned __int16 *skipRecords;
GfxWrappedBuffer skipRecordsBuffer;
unsigned int *palletteRecords;
GfxWrappedBuffer palletteRecordsBuffer;
unsigned int *tempRemap;
GfxWrappedBuffer tempRemapBuffer;
}
;

struct GfxDynamicLightset
{
unsigned int numBasisFunctions;
GfxDynamicLightsetBFData *basisFunctions;
unsigned int numBasisFunctionPrimaryLights;
GfxDynamicLightsetLightInfo *basisFunctionLights;
unsigned int numPortalGI;
GfxDynamicLightsetPortalGIData *portalGIData;
unsigned int numProductGI;
GfxDynamicLightsetProductGIData *productGIData;
GfxDynamicLightsetLMData lmData;
GfxDynamicLightsetProbeData lgProbeData;
unsigned int numDLSBasisFunctions;
}
;

struct GfxMayhemSelfVis
{
unsigned int selfVisBufferSize;
char *selfVis;
GfxWrappedRWBuffer selfVisBuffer;
}
;

struct GfxFrustumLights
{
unsigned int indexCount;
unsigned __int16 *indices;
unsigned int vertexCount;
char *vertices;
void *indexBuffer;
void *vertexBuffer;
}
;

struct $91D1B2149FAC90180ECB9AC277F76009
{
float x;
float y;
float z;
float w;
}
;

struct $43F634250C0E94E2A09AB0840E4770D1
{
float r;
float g;
float b;
float a;
}
;

struct GfxPrecomputedSkyIllumination
{
GfxImage *detailMask;
GfxImage *heightMap;
GfxImage *SVDCoefficient;
GfxImage *SVDBasis;
vec4_t boxMin;
vec4_t boxMax;
vec4_t SVDCoefficientMinMax;
vec4_t heightConsts;
}
;

struct sunflare_t
{
bool hasValidData;
Material *spriteMaterial;
Material *flareMaterial;
float spriteSize;
float flareMinSize;
float flareMinDot;
float flareMaxSize;
float flareMaxDot;
float flareMaxAlpha;
int flareFadeInTime;
int flareFadeOutTime;
float blindMinDot;
float blindMaxDot;
float blindMaxDarken;
int blindFadeInTime;
int blindFadeOutTime;
float glareMinDot;
float glareMaxDot;
float glareMaxLighten;
int glareFadeInTime;
int glareFadeOutTime;
float sunFxPosition[3];
}
;

struct $91361F66BC12660C141474223F2F947C
{
vec4_t row0;
vec4_t row1;
vec4_t row2;
vec4_t row3;
}
;

struct GfxLightAABB
{
unsigned __int16 nodeCount;
unsigned __int16 lightCount;
GfxLightAABBNode *nodeArray;
unsigned __int16 *lightArray;
}
;

struct GfxWorldDpvsStatic
{
unsigned int smodelCount;
unsigned int staticSurfaceCount;
unsigned int litOpaqueSurfsBegin;
unsigned int litOpaqueSurfsEnd;
unsigned int litDecalSurfsBegin;
unsigned int litDecalSurfsEnd;
unsigned int litTransSurfsBegin;
unsigned int litTransSurfsEnd;
unsigned int emissiveSurfsBegin;
unsigned int emissiveSurfsEnd;
unsigned int smodelVisDataCount;
unsigned int surfaceVisDataCount;
unsigned int primaryLightVisDataCount;
unsigned int reflectionProbeVisDataCount;
unsigned int volumetricVisDataCount;
unsigned int decalVisDataCount;
unsigned int *smodelVisData[30];
unsigned int *surfaceVisData[30];
unsigned int *lodData;
unsigned int *primaryLightVisData;
unsigned int *reflectionProbeVisData;
unsigned int *volumetricVisData;
unsigned int *decalVisData;
unsigned int *tessellationCutoffVisData[30];
unsigned int *sortedSurfIndex;
GfxStaticModelInst *smodelInsts;
GfxSurface *surfaces;
GfxSurfaceBounds *surfacesBounds;
GfxStaticModelDrawInst *smodelDrawInsts;
GfxDrawSurf *surfaceMaterials;
unsigned int *surfaceCastsSunShadow;
unsigned __int16 *sortedSmodelIndices;
unsigned int sunShadowOptCount;
unsigned int sunSurfVisDataCount;
unsigned int *surfaceCastsSunShadowOpt;
char **constantBuffers;
int usageCount;
}
;

struct GfxWorldDpvsDynamic
{
unsigned int dynEntClientWordCount[2];
unsigned int dynEntClientCount[2];
unsigned int *dynEntCellBits[2];
unsigned __int8 *dynEntVisData[2][30];
}
;

struct StreamMtlGroup
{
StreamKey *volumesKey;
StreamMtlImgInfo *infos;
unsigned __int16 mtlInfoCount;
unsigned __int16 imgInfoCount;
unsigned __int16 transientZone;
unsigned __int16 unused;
}
;

struct StreamTree
{
StreamTreeNode *nodes;
const Material **materials;
const GfxImage **images;
unsigned int nodeCount;
unsigned int materialCount;
unsigned int imageCount;
StreamMtlGroup largeMtlGroup;
unsigned int memorySize;
vec3_t mins;
vec3_t maxs;
}
;

struct ExtentBounds
{
vec3_t mins;
vec3_t maxs;
}
;

struct StreamTreeGrid
{
StreamKey **cellKeys;
const Material **materials;
const GfxImage **images;
const XModel **xmodels;
unsigned __int16 *smodelCollectionIndices;
ExtentBounds bounds;
unsigned int version;
unsigned __int16 rowCount;
unsigned __int16 columnCount;
unsigned int materialCount;
unsigned int imageCount;
unsigned int xmodelCount;
unsigned int smodelCollectionIndexCount;
}
;

struct GfxWorldDrawVerts
{
unsigned int posDataSize;
unsigned int auxDataSize;
char *posData;
char *auxData;
GfxWrappedBuffer posBuffer;
GfxWrappedBuffer auxBuffer;
unsigned int indexCount;
unsigned __int16 *indices;
ID3D12Resource *indexBuffer;
GfxShaderBufferView indexBufferView;
}
;

struct GfxWorldDrawCells
{
unsigned int cellCount;
GfxCellTreeCount *aabbTreeCounts;
GfxCellTree *aabbTrees;
}
;

struct GfxGpuLightGridTransientData
{
GfxGpuLightGrid *gpuLightGrid;
unsigned int staticModelVolumeCount;
GfxStaticModelLightGridVolume *staticModelVolumes;
GfxDynamicLightsetLGppZoneInfo *SMLGppZone;
GfxLightGridVolumeAtlasDataTemp *tempLightGridVolumeData;
}
;

struct GfxFogSplineData
{
float fogStartDist;
float fogHalfPlaneDistance;
float fogMaxDistance;
}
;

enum AnimationController
{
ANIMCTRL_NONE = 0x0,
ANIMCTRL_PLAYER = 0x1,
ANIMCTRL_DOG = 0x2,
ANIMCTRL_NUM = 0x3,
}
;

enum GestureType
{
GESTURE_TYPE_NON_DIRECTIONAL = 0x0,
GESTURE_TYPE_DIRECTIONAL = 0x1,
GESTURE_TYPE_LOOK_AROUND = 0x2,
GESTURE_TYPE_IK_TARGET = 0x3,
GESTURE_TYPE_NUM = 0x4,
}
;

enum GesturePriority
{
GESTURE_PRIORITY_OFFHAND_SHIELD = 0x0,
GESTURE_PRIORITY_OFFHAND_THROWN_WEAPON = 0x1,
GESTURE_PRIORITY_OFFHAND_SCRIPT_WEAPON = 0x2,
GESTURE_PRIORITY_SPACE_JUMP = 0x3,
GESTURE_PRIORITY_SCRIPT = 0x4,
GESTURE_PRIORITY_SLIDE = 0x5,
GESTURE_PRIORITY_MANTLE = 0x6,
GESTURE_PRIORITY_GROUND_POUND = 0x7,
GESTURE_PRIORITY_WALL_RUN = 0x8,
GESTURE_PRIORITY_DEMEANOR = 0x9,
GESTURE_PRIORITY_COUNT = 0xC,
}
;

struct GestureWeaponSettings
{
unsigned __int16 blendToStates;
bool hideReticle;
float fireDelay;
float sprintDelay;
bool useLeftIdleAkimbo;
bool splitAnimsAkimbo;
bool blendToDemeanorLoop;
bool blendOutRaise;
}
;

struct GestureDirectionalSettings
{
float maxAngle;
float lerpAtMaxAngle;
float widthCushionAngle;
float lerpAtMinCushionAngle;
float lerpAtMaxCushionAngle;
float limitLeft;
float limitRight;
float limitUp;
float limitDown;
}
;

struct FootstepTime
{
float time;
bool isLeft;
}
;

struct MovementTime
{
float time;
bool isLeadIn;
}
;

struct FootstepAnim
{
int leftCount;
FootstepTime step[4];
MovementTime movement[4];
}
;

struct GestureLookAroundSettings
{
float yawLerpIn;
float yawLerpOut;
unsigned __int16 walkTime;
FootstepAnim walkFootStepAnim;
}
;

struct GestureIKTargetSettings
{
scr_string_t targetEntityBoneName;
}
;

enum AttachmentType : __int32
{
ATTACHMENT_SCOPE = 0x0,
ATTACHMENT_UNDERBARREL = 0x1,
ATTACHMENT_OTHER = 0x2,
ATTACHMENT_COUNT = 0x3,
}
;

enum altModePlayerAnimType_t
{
ALTMODEPLAYERANIMTYPE_NONE = 0x0,
ALTMODEPLAYERANIMTYPE_LAUNCHER = 0x1,
ALTMODEPLAYERANIMTYPE_SHOTGUN = 0x2,
ALTMODEPLAYERANIMTYPE_FIRESELECTOR = 0x3,
NUM_ALTMODEPLAYERANIMTYPES = 0x4,
}
;

enum weapType_t : __int32
{
WEAPTYPE_NONE = 0x0,
WEAPTYPE_BULLET = 0x1,
WEAPTYPE_GRENADE = 0x2,
WEAPTYPE_PROJECTILE = 0x3,
WEAPTYPE_RIOTSHIELD = 0x4,
WEAPTYPE_SCRIPT = 0x5,
WEAPTYPE_SHIELD = 0x6,
WEAPTYPE_CHARGE_SHIELD = 0x7,
WEAPTYPE_UNK8 = 0x8,
WEAPTYPE_UNK9 = 0x9,
WEAPTYPE_UNK10 = 0xA,
WEAPTYPE_UNK11 = 0xB,
WEAPTYPE_EQUIP_DEPLOY = 0xC,
WEAPTYPE_NUM = 0xD,
}
;

enum weapClass_t : __int32
{
WEAPCLASS_RIFLE = 0x0,
WEAPCLASS_SNIPER = 0x1,
WEAPCLASS_MG = 0x2,
WEAPCLASS_SMG = 0x3,
WEAPCLASS_SPREAD = 0x4,
WEAPCLASS_PISTOL = 0x5,
WEAPCLASS_GRENADE = 0x6,
WEAPCLASS_ROCKETLAUNCHER = 0x7,
WEAPCLASS_TURRET = 0x8,
WEAPCLASS_THROWINGKNIFE = 0x9,
WEAPCLASS_NON_PLAYER = 0xA,
WEAPCLASS_ITEM = 0xB,
WEAPCLASS_BEAM = 0xC,
WEAPCLASS_BALL = 0xD,
WEAPCLASS_NONE = 0xE,
WEAPCLASS_NUM = 0xF,
}
;

enum DynamicAttachmentType
{
DYNAMIC_ATTACHMENT_TYPE_MUZZLE = 0x0,
DYNAMIC_ATTACHMENT_TYPE_UNDERBARREL = 0x1,
DYNAMIC_ATTACHMENT_TYPE_LASER = 0x2,
DYNAMIC_ATTACHMENT_TYPE_GUARD = 0x3,
DYNAMIC_ATTACHMENT_TYPE_MAGAZINE = 0x4,
DYNAMIC_ATTACHMENT_TYPE_RECEIVER = 0x5,
DYNAMIC_ATTACHMENT_TYPE_SCOPE = 0x6,
DYNAMIC_ATTACHMENT_TYPE_STOCK = 0x7,
DYNAMIC_ATTACHMENT_TYPE_FIRETYPE = 0x8,
DYNAMIC_ATTACHMENT_TYPE_BARREL = 0x9,
DYNAMIC_ATTACHMENT_TYPE_OTHER = 0xA,
DYNAMIC_ATTACHMENT_TYPE_COUNT = 0xB,
}
;

enum WeaponFlagOverride
{
WEAPON_FLAG_OVERRIDE_NO_OVERRIDE = 0x0,
WEAPON_FLAG_OVERRIDE_FALSE = 0x1,
WEAPON_FLAG_OVERRIDE_TRUE = 0x2,
WEAPON_FLAG_OVERRIDE_NUM = 0x3,
}
;

enum AdsOffsetInterpolationType
{
ADS_OFFSET_INTERP_TYPE_LINEAR = 0x0,
ADS_OFFSET_INTERP_TYPE_EASEINOUTSINE = 0x1,
ADS_OFFSET_INTERP_TYPE_EASEINOUTSMOOTHERSTEP = 0x2,
ADS_OFFSET_INTERP_TYPE_EASEINCUBIC = 0x3,
ADS_OFFSET_INTERP_TYPE_EASEOUTCUBIC = 0x4,
ADS_OFFSET_INTERP_TYPE_EASEINQUARTIC = 0x5,
ADS_OFFSET_INTERP_TYPE_EASEOUTQUARTIC = 0x6,
ADS_OFFSET_INTERP_TYPE_EASEINEXPO = 0x7,
ADS_OFFSET_INTERP_TYPE_EASEOUTEXPO = 0x8,
ADS_OFFSET_INTERP_TYPE_COUNT = 0x9,
}
;

struct AttachmentList
{
unsigned int attachmentCount;
WeaponAttachment **attachments;
}
;

struct SceneZoomSettings
{
int adsZoomCount;
float adsZoomFov[3];
float adsReloadFov[3];
int adsZoomFovTransitionTime[3];
AdsOffsetInterpolationType adsZoomFovLerpType;
float adsZoomInFrac;
float adsZoomOutFrac;
float adsFOVNVG;
float adsReloadFovNVG;
bool adsZoomLevelIndexResetOnToggle;
}
;

struct WeaponZoomSettings
{
float adsZoomFov;
AdsOffsetInterpolationType adsZoomFovLerpType;
float adsZoomFovXOffset;
float adsReloadZoomFov;
float adsReloadZoomFovXOffset;
float adsNVGZoomFov;
float adsNVGZoomFovXOffset;
float adsNVGReloadZoomFov;
float adsNVGReloadZoomFovXOffset;
float adsZoomInFrac;
float adsZoomOutFrac;
bool adsZoomFovXOffsetOverride;
bool adsReloadFovXOffsetOverride;
}
;

struct ZoomSettings
{
SceneZoomSettings scene;
WeaponZoomSettings weapon;
}
;

enum ParticlePhaseOptions
{
PARTICLE_PHASE_OPTION_PHASE_NEVER = 0x0,
PARTICLE_PHASE_OPTION_PHASE_ONLY = 0x1,
PARTICLE_PHASE_OPTION_ALWAYS = 0x2,
PARTICLE_PHASE_OPTION_COUNT = 0x3,
}
;

union __m128
{
float m128_f32[4];
unsigned __int64 m128_u64[2];
char m128_i8[16];
__int16 m128_i16[8];
int m128_i32[4];
__int64 m128_i64[2];
char m128_u8[16];
unsigned __int16 m128_u16[8];
unsigned int m128_u32[4];
}
;

enum LbUpdateType
{
LBUPDATE_TYPE_NORMAL = 0x0,
LBUPDATE_TYPE_RANK = 0x1,
LBUPDATE_TYPE_COMBINE = 0x2,
LBUPDATE_TYPE_VIRTUAL = 0x3,
LBUPDATE_TYPE_COUNT = 0x4,
}
;

enum VehiclePhysicsNetcodeType
{
VEH_NETCODE_CLIENT_PRED = 0x0,
VEH_NETCODE_CLIENT_AUTH = 0x1,
VEH_NETCODE_SERVER_AUTH = 0x2,
VEH_NETCODE_CLIENT_PRED_SB = 0x3,
}
;

enum VehiclePhysicsAnimProfile
{
VEH_ANIMPROFILE_ATANGO = 0x0,
VEH_ANIMPROFILE_TROMEO = 0x1,
VEH_ANIMPROFILE_COSCAR = 0x2,
VEH_ANIMPROFILE_STANGO = 0x3,
VEH_ANIMPROFILE_VINDIA = 0x4,
VEH_ANIMPROFILE_BROMEO = 0x5,
VEH_ANIMPROFILE_BUNIFORM = 0x6,
VEH_ANIMPROFILE_SKILO = 0x7,
VEH_ANIMPROFILE_FOSCAR = 0x8,
VEH_ANIMPROFILE_LBRAVO = 0x9,
VEH_ANIMPROFILE_BLIMA = 0xA,
VEH_ANIMPROFILE_DECHO = 0xB,
VEH_ANIMPROFILE_TECHO = 0xC,
VEH_ANIMPROFILE_PINDIA = 0xD,
VEH_ANIMPROFILE_HINDIA = 0xE,
VEH_ANIMPROFILE_ASIERRA = 0xF,
VEH_ANIMPROFILE_TROMEOTRAC = 0x10,
VEH_ANIMPROFILE_UMIKE = 0x11,
VEH_ANIMPROFILE_ZUNIFORM = 0x12,
VEH_ANIMPROFILE_MKILO = 0x13,
VEH_ANIMPROFILE_PALFA = 0x14,
VEH_ANIMPROFILE_WHOTEL = 0x15,
VEH_ANIMPROFILE_FOXTROT = 0x16,
VEH_ANIMPROFILE_ZOSCAR = 0x17,
VEH_ANIMPROFILE_SCOOTER = 0x18,
VEH_ANIMPROFILE_EXPANSION3 = 0x19,
VEH_ANIMPROFILE_EXPANSION4 = 0x1A,
VEH_ANIMPROFILE_COUNT = 0x1B,
}
;

typedef const char *SndAliasLookup
;

struct $38C3DEC81229B66F67FB6D350D75FF5A
{
float x;
float y;
}
;

enum VehicleWheelSpinBoneType
{
VEH_WHEELSPIN_NONE = 0x0,
VEH_WHEELSPIN_CHILDBONE = 0x1,
VEH_WHEELSPIN_BONE = 0x2,
VEH_WHEELSPIN_COUNT = 0x3,
}
;

struct VehiclePhysicsSoundGroup
{
SndAliasLookup alias;
float threshold;
float vmin;
float vmax;
float pmin;
float pmax;
}
;

struct VehiclePhysicsVfxGroup
{
FootstepVFX *surfaceEffects;
float threshold;
float range0;
float range1;
}
;

struct SpaceshipAnimParts
{
XAnimParts *anim[2];
}
;

enum VehCamZOffsetMode
{
VEHCAM_ZMODE_WORLD = 0x0,
VEHCAM_ZMODE_VEHICLE = 0x1,
VEHCAM_ZMODE_VIEW = 0x2,
VEHCAM_ZMODE_COUNT = 0x3,
}
;

enum VehicleTurretFireType
{
VEH_TURRET_SINGLE_FIRE = 0x0,
VEH_TURRET_DUAL_FIRE = 0x1,
VEH_TURRET_ALT_FIRE = 0x2,
VEH_TURRET_FIRE_TYPE_COUNT = 0x3,
}
;

struct VehiclePhysicsRevModifier
{
float skidSndContrib;
float noiseAmpl;
float sinewaveFreq;
float sinewaveAmpl;
float rpmMax;
float rpmScale;
float rpmSpeedEmaSamples;
float inclinedThreshold;
int gearCount;
float inclContrib;
float maxSpinSpeedRPMFw;
float maxSpinSpeedRPMBw;
}
;

struct IGraphicsUnknown
{
GraphicsUnknown_Function_Table *m_pFunction;
unsigned __int32 m_DeviceIndex : 3;
unsigned __int32 m_PrivateDataPresent : 1;
unsigned __int32 m_Reserved : 28;
unsigned int m_RefCount;
}
;

struct _SYSTEM_INFO::$A707B71C060B6D10F73A71917EA8473F::$0A6E328F8C6BBEC3EB1C186C980186CC
{
WORD wProcessorArchitecture;
WORD wReserved;
}
;

struct _SYSTEM_INFO::$D2A2A4D11B17BDD834370ABB24D07C2C::$0A6E328F8C6BBEC3EB1C186C980186CC
{
WORD wProcessorArchitecture;
WORD wReserved;
}
;

union _SYSTEM_INFO::$D2A2A4D11B17BDD834370ABB24D07C2C
{
DWORD dwOemId;
struct
{
WORD wProcessorArchitecture;
WORD wReserved;
};
}
;

struct _LARGE_INTEGER::$D15048930387FF930ACB9C649931F5AF
{
DWORD LowPart;
LONG HighPart;
}
;

union _NOTIFYICONDATAA::$4E660917F4A7E6876C9B521432DF5F5D
{
UINT uTimeout;
UINT uVersion;
}
;

struct _CONTEXT::$D2ECA93702C646ACAFACD524BE9E8FEB::$3E3F1FC69AA6B1402723265946FB010B
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

struct _CONTEXT::$2489F00E8199EB1901358A374D5E6D2D::$3E3F1FC69AA6B1402723265946FB010B
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

union _CONTEXT::$2489F00E8199EB1901358A374D5E6D2D
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

union _MINIDUMP_CALLBACK_INPUT::$00EAF266C985F05B88D76A2136BDE539
{
HRESULT Status;
MINIDUMP_THREAD_CALLBACK Thread;
MINIDUMP_THREAD_EX_CALLBACK ThreadEx;
MINIDUMP_MODULE_CALLBACK Module;
MINIDUMP_INCLUDE_THREAD_CALLBACK IncludeThread;
MINIDUMP_INCLUDE_MODULE_CALLBACK IncludeModule;
MINIDUMP_IO_CALLBACK Io;
MINIDUMP_READ_MEMORY_FAILURE_CALLBACK ReadMemoryFailure;
ULONG SecondaryFlags;
MINIDUMP_VM_QUERY_CALLBACK VmQuery;
MINIDUMP_VM_PRE_READ_CALLBACK VmPreRead;
MINIDUMP_VM_POST_READ_CALLBACK VmPostRead;
}
;

struct _MINIDUMP_CALLBACK_OUTPUT::$C9A0632DE137089B71B06FD3ADD2A485::$035690D512264406BBCD88AB75088F3C
{
ULONG64 MemoryBase;
ULONG MemorySize;
}
;

struct _MINIDUMP_CALLBACK_OUTPUT::$C9A0632DE137089B71B06FD3ADD2A485::$C8315965DEAC3E5998FC5B518649BA22
{
BOOL CheckCancel;
BOOL Cancel;
}
;

struct _MINIDUMP_CALLBACK_OUTPUT::$C9A0632DE137089B71B06FD3ADD2A485::$DED16FD5CF5578E9F552BDB588596905
{
MINIDUMP_MEMORY_INFO VmRegion;
BOOL Continue;
}
;

struct _MINIDUMP_CALLBACK_OUTPUT::$C9A0632DE137089B71B06FD3ADD2A485::$1DC921CF0B66F4AFBC22AF945C58C97F
{
HRESULT VmQueryStatus;
MINIDUMP_MEMORY_INFO VmQueryResult;
}
;

struct _MINIDUMP_CALLBACK_OUTPUT::$C9A0632DE137089B71B06FD3ADD2A485::$7C834D7C20BAB5D9C6307775ABDD0043
{
HRESULT VmReadStatus;
ULONG VmReadBytesCompleted;
}
;

struct _MINIDUMP_CALLBACK_OUTPUT::$D76131B1DC245510AD89C4B6DF939084::$035690D512264406BBCD88AB75088F3C
{
ULONG64 MemoryBase;
ULONG MemorySize;
}
;

struct _MINIDUMP_CALLBACK_OUTPUT::$D76131B1DC245510AD89C4B6DF939084::$C8315965DEAC3E5998FC5B518649BA22
{
BOOL CheckCancel;
BOOL Cancel;
}
;

struct _MINIDUMP_CALLBACK_OUTPUT::$D76131B1DC245510AD89C4B6DF939084::$DED16FD5CF5578E9F552BDB588596905
{
MINIDUMP_MEMORY_INFO VmRegion;
BOOL Continue;
}
;

struct _MINIDUMP_CALLBACK_OUTPUT::$D76131B1DC245510AD89C4B6DF939084::$1DC921CF0B66F4AFBC22AF945C58C97F
{
HRESULT VmQueryStatus;
MINIDUMP_MEMORY_INFO VmQueryResult;
}
;

struct _MINIDUMP_CALLBACK_OUTPUT::$D76131B1DC245510AD89C4B6DF939084::$7C834D7C20BAB5D9C6307775ABDD0043
{
HRESULT VmReadStatus;
ULONG VmReadBytesCompleted;
}
;

union _MINIDUMP_CALLBACK_OUTPUT::$D76131B1DC245510AD89C4B6DF939084
{
ULONG ModuleWriteFlags;
ULONG ThreadWriteFlags;
ULONG SecondaryFlags;
struct
{
ULONG64 MemoryBase;
ULONG MemorySize;
};
struct
{
BOOL CheckCancel;
BOOL Cancel;
};
HANDLE Handle;
struct
{
MINIDUMP_MEMORY_INFO VmRegion;
BOOL Continue;
};
struct
{
HRESULT VmQueryStatus;
MINIDUMP_MEMORY_INFO VmQueryResult;
};
struct
{
HRESULT VmReadStatus;
ULONG VmReadBytesCompleted;
};
HRESULT Status;
}
;

union _SYSTEM_LOGICAL_PROCESSOR_INFORMATION::$FBE3FA590C7AF156D6F2C7D67421B03D
{
_SYSTEM_LOGICAL_PROCESSOR_INFORMATION::$D6931DE555EC91ED0741B0AFB854A317::$9A3D5550B6DEF9CE405BB00483386E67 ProcessorCore;
_SYSTEM_LOGICAL_PROCESSOR_INFORMATION::$D6931DE555EC91ED0741B0AFB854A317::$DB4434C0CE3788EF589018B7D8F8E412 NumaNode;
CACHE_DESCRIPTOR Cache;
ULONGLONG Reserved[2];
}
;

struct _IP_ADAPTER_ADDRESSES_LH::$88EAB68B38CC03D4ADFF2CEE83F7F421::$7B4E16A930E088B4C5C3190D6230A32C
{
ULONG Length;
IF_INDEX IfIndex;
}
;

struct _IP_ADAPTER_ADDRESSES_LH::$180770993F653D047330E4896ADF1E09::$7B4E16A930E088B4C5C3190D6230A32C
{
ULONG Length;
IF_INDEX IfIndex;
}
;

struct _IP_ADAPTER_ADDRESSES_LH::$DDD0AE2CC24B58D9B0E89AFE3DFA184D::$E43178AF9457E50D9523A864F18E103E
{
unsigned __int32 DdnsEnabled : 1;
unsigned __int32 RegisterAdapterSuffix : 1;
unsigned __int32 Dhcpv4Enabled : 1;
unsigned __int32 ReceiveOnly : 1;
unsigned __int32 NoMulticast : 1;
unsigned __int32 Ipv6OtherStatefulConfig : 1;
unsigned __int32 NetbiosOverTcpipEnabled : 1;
unsigned __int32 Ipv4Enabled : 1;
unsigned __int32 Ipv6Enabled : 1;
unsigned __int32 Ipv6ManagedAddressConfigurationSupported : 1;
}
;

union _IP_ADAPTER_ADDRESSES_LH::$180770993F653D047330E4896ADF1E09
{
ULONGLONG Alignment;
struct
{
ULONG Length;
IF_INDEX IfIndex;
};
}
;

union _IP_ADAPTER_ADDRESSES_LH::$DDD0AE2CC24B58D9B0E89AFE3DFA184D
{
ULONG Flags;
struct
{
unsigned __int32 DdnsEnabled : 1;
unsigned __int32 RegisterAdapterSuffix : 1;
unsigned __int32 Dhcpv4Enabled : 1;
unsigned __int32 ReceiveOnly : 1;
unsigned __int32 NoMulticast : 1;
unsigned __int32 Ipv6OtherStatefulConfig : 1;
unsigned __int32 NetbiosOverTcpipEnabled : 1;
unsigned __int32 Ipv4Enabled : 1;
unsigned __int32 Ipv6Enabled : 1;
unsigned __int32 Ipv6ManagedAddressConfigurationSupported : 1;
};
}
;

struct _IP_ADAPTER_UNICAST_ADDRESS_LH::$E412F09F4EF1DCB2A353111A4C8EAC81::$D25FAD02A608409B5EC4E31E54DE6CE1
{
ULONG Length;
DWORD Flags;
}
;

struct _IP_ADAPTER_UNICAST_ADDRESS_LH::$F64263F03DBDF9ADC42C070E91D35911::$D25FAD02A608409B5EC4E31E54DE6CE1
{
ULONG Length;
DWORD Flags;
}
;

union _IP_ADAPTER_UNICAST_ADDRESS_LH::$F64263F03DBDF9ADC42C070E91D35911
{
ULONGLONG Alignment;
struct
{
ULONG Length;
DWORD Flags;
};
}
;

struct _IP_ADAPTER_ANYCAST_ADDRESS_XP::$27D98D245193571B98223D43E8FDA74A::$D25FAD02A608409B5EC4E31E54DE6CE1
{
ULONG Length;
DWORD Flags;
}
;

struct _IP_ADAPTER_ANYCAST_ADDRESS_XP::$4085A91179A5E1BEB845AFADC1E877E0::$D25FAD02A608409B5EC4E31E54DE6CE1
{
ULONG Length;
DWORD Flags;
}
;

union _IP_ADAPTER_ANYCAST_ADDRESS_XP::$4085A91179A5E1BEB845AFADC1E877E0
{
ULONGLONG Alignment;
struct
{
ULONG Length;
DWORD Flags;
};
}
;

struct _IP_ADAPTER_MULTICAST_ADDRESS_XP::$C661A151FC55CDAC9C76B7690C297B03::$D25FAD02A608409B5EC4E31E54DE6CE1
{
ULONG Length;
DWORD Flags;
}
;

struct _IP_ADAPTER_MULTICAST_ADDRESS_XP::$12FBCD380B9A0572EBBCDFEF82EF2535::$D25FAD02A608409B5EC4E31E54DE6CE1
{
ULONG Length;
DWORD Flags;
}
;

union _IP_ADAPTER_MULTICAST_ADDRESS_XP::$12FBCD380B9A0572EBBCDFEF82EF2535
{
ULONGLONG Alignment;
struct
{
ULONG Length;
DWORD Flags;
};
}
;

struct _IP_ADAPTER_DNS_SERVER_ADDRESS_XP::$C726E6BF6F48850843A5B30ADC20782D::$93390FE4CE73C1277CF1B6CF5BC94832
{
ULONG Length;
DWORD Reserved;
}
;

struct _IP_ADAPTER_DNS_SERVER_ADDRESS_XP::$2B1878807732AE035FE88B93E719DABE::$93390FE4CE73C1277CF1B6CF5BC94832
{
ULONG Length;
DWORD Reserved;
}
;

union _IP_ADAPTER_DNS_SERVER_ADDRESS_XP::$2B1878807732AE035FE88B93E719DABE
{
ULONGLONG Alignment;
struct
{
ULONG Length;
DWORD Reserved;
};
}
;

struct _IP_ADAPTER_PREFIX_XP::$062DE21AAEE92240DFF2100A002A9516::$D25FAD02A608409B5EC4E31E54DE6CE1
{
ULONG Length;
DWORD Flags;
}
;

struct _IP_ADAPTER_PREFIX_XP::$4B6B4D139C6342634BEFFFC2A33DD791::$D25FAD02A608409B5EC4E31E54DE6CE1
{
ULONG Length;
DWORD Flags;
}
;

union _IP_ADAPTER_PREFIX_XP::$4B6B4D139C6342634BEFFFC2A33DD791
{
ULONGLONG Alignment;
struct
{
ULONG Length;
DWORD Flags;
};
}
;

struct _IP_ADAPTER_WINS_SERVER_ADDRESS_LH::$ED1413B78AFF918A573A0A81B51AB7FA::$93390FE4CE73C1277CF1B6CF5BC94832
{
ULONG Length;
DWORD Reserved;
}
;

struct _IP_ADAPTER_WINS_SERVER_ADDRESS_LH::$A9BFDFF194168AE185639093B78EDEF8::$93390FE4CE73C1277CF1B6CF5BC94832
{
ULONG Length;
DWORD Reserved;
}
;

union _IP_ADAPTER_WINS_SERVER_ADDRESS_LH::$A9BFDFF194168AE185639093B78EDEF8
{
ULONGLONG Alignment;
struct
{
ULONG Length;
DWORD Reserved;
};
}
;

struct _IP_ADAPTER_GATEWAY_ADDRESS_LH::$4DEA9AA2BE4A87A26E25AF13B4F5640E::$93390FE4CE73C1277CF1B6CF5BC94832
{
ULONG Length;
DWORD Reserved;
}
;

struct _IP_ADAPTER_GATEWAY_ADDRESS_LH::$9ED0BF9B6D44E4A16DC435E49E10C39D::$93390FE4CE73C1277CF1B6CF5BC94832
{
ULONG Length;
DWORD Reserved;
}
;

union _IP_ADAPTER_GATEWAY_ADDRESS_LH::$9ED0BF9B6D44E4A16DC435E49E10C39D
{
ULONGLONG Alignment;
struct
{
ULONG Length;
DWORD Reserved;
};
}
;

struct tagCY::$D9D23C9E60E5E9564D0CA9C8B2C338FC
{
ULONG Lo;
LONG Hi;
}
;

struct tagVARIANT::$::$65D68C826D16CA47CF95571D7BFCD657::$E09503A454170B491AC1C4312CE36FE6::$3708FA4EBB8D6EAF9552E0A44363DA7A
{
PVOID pvRecord;
IRecordInfo *pRecInfo;
}
;

struct tagVARIANT::$E93DC971A089CC95F6C875332324C1E7::$65D68C826D16CA47CF95571D7BFCD657::$804D02573B963CC7B81A0F031EA28E76::$3708FA4EBB8D6EAF9552E0A44363DA7A
{
PVOID pvRecord;
IRecordInfo *pRecInfo;
}
;

union tagVARIANT::$E93DC971A089CC95F6C875332324C1E7::$65D68C826D16CA47CF95571D7BFCD657::$804D02573B963CC7B81A0F031EA28E76
{
LONGLONG llVal;
LONG lVal;
BYTE bVal;
SHORT iVal;
FLOAT fltVal;
DOUBLE dblVal;
VARIANT_BOOL boolVal;
SCODE scode;
CY cyVal;
DATE date;
BSTR bstrVal;
IUnknown *punkVal;
IDispatch *pdispVal;
SAFEARRAY *parray;
BYTE *pbVal;
SHORT *piVal;
LONG *plVal;
LONGLONG *pllVal;
FLOAT *pfltVal;
DOUBLE *pdblVal;
VARIANT_BOOL *pboolVal;
SCODE *pscode;
CY *pcyVal;
DATE *pdate;
BSTR *pbstrVal;
IUnknown **ppunkVal;
IDispatch **ppdispVal;
SAFEARRAY **pparray;
VARIANT *pvarVal;
PVOID byref;
CHAR cVal;
USHORT uiVal;
ULONG ulVal;
ULONGLONG ullVal;
INT intVal;
UINT uintVal;
DECIMAL *pdecVal;
CHAR *pcVal;
USHORT *puiVal;
ULONG *pulVal;
ULONGLONG *pullVal;
INT *pintVal;
UINT *puintVal;
struct
{
PVOID pvRecord;
IRecordInfo *pRecInfo;
};
}
;

struct tagDEC::$64EC678C49E7BE49873AFBFB7A849D34::$4359D7DBABE17A188556C63ED6ACD9B9
{
BYTE scale;
BYTE sign;
}
;

struct tagDEC::$D28E26DEC3EC762C06C2AA9D0F7AC301::$8FD70EF57C806CA3E8F52508B16C81FB
{
ULONG Lo32;
ULONG Mid32;
}
;

struct tagDEC::$F9D3E4609C51D51669792D0A04C4511A::$4359D7DBABE17A188556C63ED6ACD9B9
{
BYTE scale;
BYTE sign;
}
;

struct tagDEC::$6C0E7BD535B302F38856A5E37B809FD3::$8FD70EF57C806CA3E8F52508B16C81FB
{
ULONG Lo32;
ULONG Mid32;
}
;

union tagDEC::$F9D3E4609C51D51669792D0A04C4511A
{
struct
{
BYTE scale;
BYTE sign;
};
USHORT signscale;
}
;

union tagDEC::$6C0E7BD535B302F38856A5E37B809FD3
{
struct
{
ULONG Lo32;
ULONG Mid32;
};
ULONGLONG Lo64;
}
;

struct tagVARIANT::$E93DC971A089CC95F6C875332324C1E7::$::$12047615A60CB0DFD9450EA9E933A4F2::$3708FA4EBB8D6EAF9552E0A44363DA7A
{
PVOID pvRecord;
IRecordInfo *pRecInfo;
}
;

union tagVARIANT::$E93DC971A089CC95F6C875332324C1E7::$164E7EA85964FD237D3A305D40F8E5AA::$12047615A60CB0DFD9450EA9E933A4F2
{
LONGLONG llVal;
LONG lVal;
BYTE bVal;
SHORT iVal;
FLOAT fltVal;
DOUBLE dblVal;
VARIANT_BOOL boolVal;
SCODE scode;
CY cyVal;
DATE date;
BSTR bstrVal;
IUnknown *punkVal;
IDispatch *pdispVal;
SAFEARRAY *parray;
BYTE *pbVal;
SHORT *piVal;
LONG *plVal;
LONGLONG *pllVal;
FLOAT *pfltVal;
DOUBLE *pdblVal;
VARIANT_BOOL *pboolVal;
SCODE *pscode;
CY *pcyVal;
DATE *pdate;
BSTR *pbstrVal;
IUnknown **ppunkVal;
IDispatch **ppdispVal;
SAFEARRAY **pparray;
VARIANT *pvarVal;
PVOID byref;
CHAR cVal;
USHORT uiVal;
ULONG ulVal;
ULONGLONG ullVal;
INT intVal;
UINT uintVal;
DECIMAL *pdecVal;
CHAR *pcVal;
USHORT *puiVal;
ULONG *pulVal;
ULONGLONG *pullVal;
INT *pintVal;
UINT *puintVal;
struct
{
PVOID pvRecord;
IRecordInfo *pRecInfo;
};
}
;

struct tagVARIANT::$E93DC971A089CC95F6C875332324C1E7::$164E7EA85964FD237D3A305D40F8E5AA
{
VARTYPE vt;
WORD wReserved1;
WORD wReserved2;
WORD wReserved3;
union
{
LONGLONG llVal;
LONG lVal;
BYTE bVal;
SHORT iVal;
FLOAT fltVal;
DOUBLE dblVal;
VARIANT_BOOL boolVal;
SCODE scode;
CY cyVal;
DATE date;
BSTR bstrVal;
IUnknown *punkVal;
IDispatch *pdispVal;
SAFEARRAY *parray;
BYTE *pbVal;
SHORT *piVal;
LONG *plVal;
LONGLONG *pllVal;
FLOAT *pfltVal;
DOUBLE *pdblVal;
VARIANT_BOOL *pboolVal;
SCODE *pscode;
CY *pcyVal;
DATE *pdate;
BSTR *pbstrVal;
IUnknown **ppunkVal;
IDispatch **ppdispVal;
SAFEARRAY **pparray;
VARIANT *pvarVal;
PVOID byref;
CHAR cVal;
USHORT uiVal;
ULONG ulVal;
ULONGLONG ullVal;
INT intVal;
UINT uintVal;
DECIMAL *pdecVal;
CHAR *pcVal;
USHORT *puiVal;
ULONG *pulVal;
ULONGLONG *pullVal;
INT *pintVal;
UINT *puintVal;
struct
{
PVOID pvRecord;
IRecordInfo *pRecInfo;
};
};
}
;

struct tagVARIANT::$::$::$4CB71CDB9EC37FFF172EA74964A89E63::$3708FA4EBB8D6EAF9552E0A44363DA7A
{
PVOID pvRecord;
IRecordInfo *pRecInfo;
}
;

union tagVARIANT::$::$68A7776A00D655C059C1C0B869FD2BD0::$4CB71CDB9EC37FFF172EA74964A89E63
{
LONGLONG llVal;
LONG lVal;
BYTE bVal;
SHORT iVal;
FLOAT fltVal;
DOUBLE dblVal;
VARIANT_BOOL boolVal;
SCODE scode;
CY cyVal;
DATE date;
BSTR bstrVal;
IUnknown *punkVal;
IDispatch *pdispVal;
SAFEARRAY *parray;
BYTE *pbVal;
SHORT *piVal;
LONG *plVal;
LONGLONG *pllVal;
FLOAT *pfltVal;
DOUBLE *pdblVal;
VARIANT_BOOL *pboolVal;
SCODE *pscode;
CY *pcyVal;
DATE *pdate;
BSTR *pbstrVal;
IUnknown **ppunkVal;
IDispatch **ppdispVal;
SAFEARRAY **pparray;
VARIANT *pvarVal;
PVOID byref;
CHAR cVal;
USHORT uiVal;
ULONG ulVal;
ULONGLONG ullVal;
INT intVal;
UINT uintVal;
DECIMAL *pdecVal;
CHAR *pcVal;
USHORT *puiVal;
ULONG *pulVal;
ULONGLONG *pullVal;
INT *pintVal;
UINT *puintVal;
struct
{
PVOID pvRecord;
IRecordInfo *pRecInfo;
};
}
;

struct tagVARIANT::$6899585BD4826AA8AB7425CD8D7F838C::$68A7776A00D655C059C1C0B869FD2BD0
{
VARTYPE vt;
WORD wReserved1;
WORD wReserved2;
WORD wReserved3;
union
{
LONGLONG llVal;
LONG lVal;
BYTE bVal;
SHORT iVal;
FLOAT fltVal;
DOUBLE dblVal;
VARIANT_BOOL boolVal;
SCODE scode;
CY cyVal;
DATE date;
BSTR bstrVal;
IUnknown *punkVal;
IDispatch *pdispVal;
SAFEARRAY *parray;
BYTE *pbVal;
SHORT *piVal;
LONG *plVal;
LONGLONG *pllVal;
FLOAT *pfltVal;
DOUBLE *pdblVal;
VARIANT_BOOL *pboolVal;
SCODE *pscode;
CY *pcyVal;
DATE *pdate;
BSTR *pbstrVal;
IUnknown **ppunkVal;
IDispatch **ppdispVal;
SAFEARRAY **pparray;
VARIANT *pvarVal;
PVOID byref;
CHAR cVal;
USHORT uiVal;
ULONG ulVal;
ULONGLONG ullVal;
INT intVal;
UINT uintVal;
DECIMAL *pdecVal;
CHAR *pcVal;
USHORT *puiVal;
ULONG *pulVal;
ULONGLONG *pullVal;
INT *pintVal;
UINT *puintVal;
struct
{
PVOID pvRecord;
IRecordInfo *pRecInfo;
};
};
}
;

union tagVARIANT::$6899585BD4826AA8AB7425CD8D7F838C
{
struct
{
VARTYPE vt;
WORD wReserved1;
WORD wReserved2;
WORD wReserved3;
union
{
LONGLONG llVal;
LONG lVal;
BYTE bVal;
SHORT iVal;
FLOAT fltVal;
DOUBLE dblVal;
VARIANT_BOOL boolVal;
SCODE scode;
CY cyVal;
DATE date;
BSTR bstrVal;
IUnknown *punkVal;
IDispatch *pdispVal;
SAFEARRAY *parray;
BYTE *pbVal;
SHORT *piVal;
LONG *plVal;
LONGLONG *pllVal;
FLOAT *pfltVal;
DOUBLE *pdblVal;
VARIANT_BOOL *pboolVal;
SCODE *pscode;
CY *pcyVal;
DATE *pdate;
BSTR *pbstrVal;
IUnknown **ppunkVal;
IDispatch **ppdispVal;
SAFEARRAY **pparray;
VARIANT *pvarVal;
PVOID byref;
CHAR cVal;
USHORT uiVal;
ULONG ulVal;
ULONGLONG ullVal;
INT intVal;
UINT uintVal;
DECIMAL *pdecVal;
CHAR *pcVal;
USHORT *puiVal;
ULONG *pulVal;
ULONGLONG *pullVal;
INT *pintVal;
UINT *puintVal;
struct
{
PVOID pvRecord;
IRecordInfo *pRecInfo;
};
};
};
DECIMAL decVal;
}
;

union tagTYPEDESC::$FAC08D9B25B1F56AC285CE3FA44C7C26
{
tagTYPEDESC *lptdesc;
tagARRAYDESC *lpadesc;
HREFTYPE hreftype;
}
;

union tagELEMDESC::$4B0A9FEB7DDFACC13C8527FFBB5155F1
{
IDLDESC idldesc;
PARAMDESC paramdesc;
}
;

union tagVARDESC::$E0940EBC521ABB18B87745BC3225FF88
{
ULONG oInst;
VARIANT *lpvarValue;
}
;

struct _devicemodeA::$CB1E1A2E30D5738ACA9AADF670A603C7::$9BDCC3560A7FCB03726B83F3AAE6CAC8
{
POINTL dmPosition;
DWORD dmDisplayOrientation;
DWORD dmDisplayFixedOutput;
}
;

struct _devicemodeA::$FD139CDABA8624C54230BF3DC87781BE::$A4EE175A7532F35C00DA5CAD042934DF
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

struct _devicemodeA::$FD139CDABA8624C54230BF3DC87781BE::$9BDCC3560A7FCB03726B83F3AAE6CAC8
{
POINTL dmPosition;
DWORD dmDisplayOrientation;
DWORD dmDisplayFixedOutput;
}
;

union _devicemodeA::$FD139CDABA8624C54230BF3DC87781BE
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

union _devicemodeA::$C3F2B279FB53FF81570BCC7C07F9C20D
{
DWORD dmDisplayFlags;
DWORD dmNup;
}
;

struct _devicemodeW::$5A39A2C3BB0B1D2D5888DFEE2BA0B23C::$9BDCC3560A7FCB03726B83F3AAE6CAC8
{
POINTL dmPosition;
DWORD dmDisplayOrientation;
DWORD dmDisplayFixedOutput;
}
;

struct _devicemodeW::$BC7F733934CAD54FC176440B2255E419::$A4EE175A7532F35C00DA5CAD042934DF
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

struct _devicemodeW::$BC7F733934CAD54FC176440B2255E419::$9BDCC3560A7FCB03726B83F3AAE6CAC8
{
POINTL dmPosition;
DWORD dmDisplayOrientation;
DWORD dmDisplayFixedOutput;
}
;

union _devicemodeW::$BC7F733934CAD54FC176440B2255E419
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

union _devicemodeW::$C3F2B279FB53FF81570BCC7C07F9C20D
{
DWORD dmDisplayFlags;
DWORD dmNup;
}
;

struct tagRAWMOUSE::$BAD619BF849CA7670BF9DAFAE180B654::$F372F84FDD55C1C6CBB77C18B0D47868
{
USHORT usButtonFlags;
USHORT usButtonData;
}
;

struct tagRAWMOUSE::$6DD544162713385491D368CC942D146B::$F372F84FDD55C1C6CBB77C18B0D47868
{
USHORT usButtonFlags;
USHORT usButtonData;
}
;

union tagRAWMOUSE::$6DD544162713385491D368CC942D146B
{
ULONG ulButtons;
struct
{
USHORT usButtonFlags;
USHORT usButtonData;
};
}
;

struct _ULARGE_INTEGER::$6A2974C52DDB272BFFEB90D38030D45F
{
DWORD LowPart;
DWORD HighPart;
}
;

union tagPROPVARIANT::$B01A803922ACEB71FC353E1ACAA1539E::$1A0FF913CB6A2700FBDFFDCC16233417::$2A2DEDA2C1013DBE3D46F31E4BE75045
{
CHAR cVal;
UCHAR bVal;
SHORT iVal;
USHORT uiVal;
LONG lVal;
ULONG ulVal;
INT intVal;
UINT uintVal;
LARGE_INTEGER hVal;
ULARGE_INTEGER uhVal;
FLOAT fltVal;
DOUBLE dblVal;
VARIANT_BOOL boolVal;
SCODE scode;
CY cyVal;
DATE date;
FILETIME filetime;
CLSID *puuid;
CLIPDATA *pclipdata;
BSTR bstrVal;
BSTRBLOB bstrblobVal;
BLOB blob;
LPSTR pszVal;
LPWSTR pwszVal;
IUnknown *punkVal;
IDispatch *pdispVal;
IStream *pStream;
IStorage *pStorage;
LPVERSIONEDSTREAM pVersionedStream;
LPSAFEARRAY parray;
CAC cac;
CAUB caub;
CAI cai;
CAUI caui;
CAL cal;
CAUL caul;
CAH cah;
CAUH cauh;
CAFLT caflt;
CADBL cadbl;
CABOOL cabool;
CASCODE cascode;
CACY cacy;
CADATE cadate;
CAFILETIME cafiletime;
CACLSID cauuid;
CACLIPDATA caclipdata;
CABSTR cabstr;
CABSTRBLOB cabstrblob;
CALPSTR calpstr;
CALPWSTR calpwstr;
CAPROPVARIANT capropvar;
CHAR *pcVal;
UCHAR *pbVal;
SHORT *piVal;
USHORT *puiVal;
LONG *plVal;
ULONG *pulVal;
INT *pintVal;
UINT *puintVal;
FLOAT *pfltVal;
DOUBLE *pdblVal;
VARIANT_BOOL *pboolVal;
DECIMAL *pdecVal;
SCODE *pscode;
CY *pcyVal;
DATE *pdate;
BSTR *pbstrVal;
IUnknown **ppunkVal;
IDispatch **ppdispVal;
LPSAFEARRAY *pparray;
PROPVARIANT *pvarVal;
}
;

union tagPROPVARIANT::$B01A803922ACEB71FC353E1ACAA1539E::$AF1AB4BCF5A23476B92C4AFF75F4E3DB::$2A2DEDA2C1013DBE3D46F31E4BE75045
{
CHAR cVal;
UCHAR bVal;
SHORT iVal;
USHORT uiVal;
LONG lVal;
ULONG ulVal;
INT intVal;
UINT uintVal;
LARGE_INTEGER hVal;
ULARGE_INTEGER uhVal;
FLOAT fltVal;
DOUBLE dblVal;
VARIANT_BOOL boolVal;
SCODE scode;
CY cyVal;
DATE date;
FILETIME filetime;
CLSID *puuid;
CLIPDATA *pclipdata;
BSTR bstrVal;
BSTRBLOB bstrblobVal;
BLOB blob;
LPSTR pszVal;
LPWSTR pwszVal;
IUnknown *punkVal;
IDispatch *pdispVal;
IStream *pStream;
IStorage *pStorage;
LPVERSIONEDSTREAM pVersionedStream;
LPSAFEARRAY parray;
CAC cac;
CAUB caub;
CAI cai;
CAUI caui;
CAL cal;
CAUL caul;
CAH cah;
CAUH cauh;
CAFLT caflt;
CADBL cadbl;
CABOOL cabool;
CASCODE cascode;
CACY cacy;
CADATE cadate;
CAFILETIME cafiletime;
CACLSID cauuid;
CACLIPDATA caclipdata;
CABSTR cabstr;
CABSTRBLOB cabstrblob;
CALPSTR calpstr;
CALPWSTR calpwstr;
CAPROPVARIANT capropvar;
CHAR *pcVal;
UCHAR *pbVal;
SHORT *piVal;
USHORT *puiVal;
LONG *plVal;
ULONG *pulVal;
INT *pintVal;
UINT *puintVal;
FLOAT *pfltVal;
DOUBLE *pdblVal;
VARIANT_BOOL *pboolVal;
DECIMAL *pdecVal;
SCODE *pscode;
CY *pcyVal;
DATE *pdate;
BSTR *pbstrVal;
IUnknown **ppunkVal;
IDispatch **ppdispVal;
LPSAFEARRAY *pparray;
PROPVARIANT *pvarVal;
}
;

struct tagPROPVARIANT::$B01A803922ACEB71FC353E1ACAA1539E::$AF1AB4BCF5A23476B92C4AFF75F4E3DB
{
VARTYPE vt;
PROPVAR_PAD1 wReserved1;
PROPVAR_PAD2 wReserved2;
PROPVAR_PAD3 wReserved3;
union
{
CHAR cVal;
UCHAR bVal;
SHORT iVal;
USHORT uiVal;
LONG lVal;
ULONG ulVal;
INT intVal;
UINT uintVal;
LARGE_INTEGER hVal;
ULARGE_INTEGER uhVal;
FLOAT fltVal;
DOUBLE dblVal;
VARIANT_BOOL boolVal;
SCODE scode;
CY cyVal;
DATE date;
FILETIME filetime;
CLSID *puuid;
CLIPDATA *pclipdata;
BSTR bstrVal;
BSTRBLOB bstrblobVal;
BLOB blob;
LPSTR pszVal;
LPWSTR pwszVal;
IUnknown *punkVal;
IDispatch *pdispVal;
IStream *pStream;
IStorage *pStorage;
LPVERSIONEDSTREAM pVersionedStream;
LPSAFEARRAY parray;
CAC cac;
CAUB caub;
CAI cai;
CAUI caui;
CAL cal;
CAUL caul;
CAH cah;
CAUH cauh;
CAFLT caflt;
CADBL cadbl;
CABOOL cabool;
CASCODE cascode;
CACY cacy;
CADATE cadate;
CAFILETIME cafiletime;
CACLSID cauuid;
CACLIPDATA caclipdata;
CABSTR cabstr;
CABSTRBLOB cabstrblob;
CALPSTR calpstr;
CALPWSTR calpwstr;
CAPROPVARIANT capropvar;
CHAR *pcVal;
UCHAR *pbVal;
SHORT *piVal;
USHORT *puiVal;
LONG *plVal;
ULONG *pulVal;
INT *pintVal;
UINT *puintVal;
FLOAT *pfltVal;
DOUBLE *pdblVal;
VARIANT_BOOL *pboolVal;
DECIMAL *pdecVal;
SCODE *pscode;
CY *pcyVal;
DATE *pdate;
BSTR *pbstrVal;
IUnknown **ppunkVal;
IDispatch **ppdispVal;
LPSAFEARRAY *pparray;
PROPVARIANT *pvarVal;
};
}
;

union tagPROPVARIANT::$::$8C2ED1C9F315F453530733C05E48F7FD::$2A2DEDA2C1013DBE3D46F31E4BE75045
{
CHAR cVal;
UCHAR bVal;
SHORT iVal;
USHORT uiVal;
LONG lVal;
ULONG ulVal;
INT intVal;
UINT uintVal;
LARGE_INTEGER hVal;
ULARGE_INTEGER uhVal;
FLOAT fltVal;
DOUBLE dblVal;
VARIANT_BOOL boolVal;
SCODE scode;
CY cyVal;
DATE date;
FILETIME filetime;
CLSID *puuid;
CLIPDATA *pclipdata;
BSTR bstrVal;
BSTRBLOB bstrblobVal;
BLOB blob;
LPSTR pszVal;
LPWSTR pwszVal;
IUnknown *punkVal;
IDispatch *pdispVal;
IStream *pStream;
IStorage *pStorage;
LPVERSIONEDSTREAM pVersionedStream;
LPSAFEARRAY parray;
CAC cac;
CAUB caub;
CAI cai;
CAUI caui;
CAL cal;
CAUL caul;
CAH cah;
CAUH cauh;
CAFLT caflt;
CADBL cadbl;
CABOOL cabool;
CASCODE cascode;
CACY cacy;
CADATE cadate;
CAFILETIME cafiletime;
CACLSID cauuid;
CACLIPDATA caclipdata;
CABSTR cabstr;
CABSTRBLOB cabstrblob;
CALPSTR calpstr;
CALPWSTR calpwstr;
CAPROPVARIANT capropvar;
CHAR *pcVal;
UCHAR *pbVal;
SHORT *piVal;
USHORT *puiVal;
LONG *plVal;
ULONG *pulVal;
INT *pintVal;
UINT *puintVal;
FLOAT *pfltVal;
DOUBLE *pdblVal;
VARIANT_BOOL *pboolVal;
DECIMAL *pdecVal;
SCODE *pscode;
CY *pcyVal;
DATE *pdate;
BSTR *pbstrVal;
IUnknown **ppunkVal;
IDispatch **ppdispVal;
LPSAFEARRAY *pparray;
PROPVARIANT *pvarVal;
}
;

struct tagPROPVARIANT::$3CD648E5B9AC77D29F25559A6F8C563D::$8C2ED1C9F315F453530733C05E48F7FD
{
VARTYPE vt;
PROPVAR_PAD1 wReserved1;
PROPVAR_PAD2 wReserved2;
PROPVAR_PAD3 wReserved3;
union
{
CHAR cVal;
UCHAR bVal;
SHORT iVal;
USHORT uiVal;
LONG lVal;
ULONG ulVal;
INT intVal;
UINT uintVal;
LARGE_INTEGER hVal;
ULARGE_INTEGER uhVal;
FLOAT fltVal;
DOUBLE dblVal;
VARIANT_BOOL boolVal;
SCODE scode;
CY cyVal;
DATE date;
FILETIME filetime;
CLSID *puuid;
CLIPDATA *pclipdata;
BSTR bstrVal;
BSTRBLOB bstrblobVal;
BLOB blob;
LPSTR pszVal;
LPWSTR pwszVal;
IUnknown *punkVal;
IDispatch *pdispVal;
IStream *pStream;
IStorage *pStorage;
LPVERSIONEDSTREAM pVersionedStream;
LPSAFEARRAY parray;
CAC cac;
CAUB caub;
CAI cai;
CAUI caui;
CAL cal;
CAUL caul;
CAH cah;
CAUH cauh;
CAFLT caflt;
CADBL cadbl;
CABOOL cabool;
CASCODE cascode;
CACY cacy;
CADATE cadate;
CAFILETIME cafiletime;
CACLSID cauuid;
CACLIPDATA caclipdata;
CABSTR cabstr;
CABSTRBLOB cabstrblob;
CALPSTR calpstr;
CALPWSTR calpwstr;
CAPROPVARIANT capropvar;
CHAR *pcVal;
UCHAR *pbVal;
SHORT *piVal;
USHORT *puiVal;
LONG *plVal;
ULONG *pulVal;
INT *pintVal;
UINT *puintVal;
FLOAT *pfltVal;
DOUBLE *pdblVal;
VARIANT_BOOL *pboolVal;
DECIMAL *pdecVal;
SCODE *pscode;
CY *pcyVal;
DATE *pdate;
BSTR *pbstrVal;
IUnknown **ppunkVal;
IDispatch **ppdispVal;
LPSAFEARRAY *pparray;
PROPVARIANT *pvarVal;
};
}
;

union tagPROPVARIANT::$3CD648E5B9AC77D29F25559A6F8C563D
{
struct
{
VARTYPE vt;
PROPVAR_PAD1 wReserved1;
PROPVAR_PAD2 wReserved2;
PROPVAR_PAD3 wReserved3;
union
{
CHAR cVal;
UCHAR bVal;
SHORT iVal;
USHORT uiVal;
LONG lVal;
ULONG ulVal;
INT intVal;
UINT uintVal;
LARGE_INTEGER hVal;
ULARGE_INTEGER uhVal;
FLOAT fltVal;
DOUBLE dblVal;
VARIANT_BOOL boolVal;
SCODE scode;
CY cyVal;
DATE date;
FILETIME filetime;
CLSID *puuid;
CLIPDATA *pclipdata;
BSTR bstrVal;
BSTRBLOB bstrblobVal;
BLOB blob;
LPSTR pszVal;
LPWSTR pwszVal;
IUnknown *punkVal;
IDispatch *pdispVal;
IStream *pStream;
IStorage *pStorage;
LPVERSIONEDSTREAM pVersionedStream;
LPSAFEARRAY parray;
CAC cac;
CAUB caub;
CAI cai;
CAUI caui;
CAL cal;
CAUL caul;
CAH cah;
CAUH cauh;
CAFLT caflt;
CADBL cadbl;
CABOOL cabool;
CASCODE cascode;
CACY cacy;
CADATE cadate;
CAFILETIME cafiletime;
CACLSID cauuid;
CACLIPDATA caclipdata;
CABSTR cabstr;
CABSTRBLOB cabstrblob;
CALPSTR calpstr;
CALPWSTR calpwstr;
CAPROPVARIANT capropvar;
CHAR *pcVal;
UCHAR *pbVal;
SHORT *piVal;
USHORT *puiVal;
LONG *plVal;
ULONG *pulVal;
INT *pintVal;
UINT *puintVal;
FLOAT *pfltVal;
DOUBLE *pdblVal;
VARIANT_BOOL *pboolVal;
DECIMAL *pdecVal;
SCODE *pscode;
CY *pcyVal;
DATE *pdate;
BSTR *pbstrVal;
IUnknown **ppunkVal;
IDispatch **ppdispVal;
LPSAFEARRAY *pparray;
PROPVARIANT *pvarVal;
};
};
DECIMAL decVal;
}
;

union tagMIXERLINECONTROLSW::$7A642F10C980012F5CEF4D9B58F22CB9
{
DWORD dwControlID;
DWORD dwControlType;
}
;

struct tagMIXERCONTROLW::$3D7A05C52ED08258D8A0343834E0D122::$E7E17BB5A913B13CD24971702A8301A2
{
LONG lMinimum;
LONG lMaximum;
}
;

struct tagMIXERCONTROLW::$3D7A05C52ED08258D8A0343834E0D122::$6FA7DB52544A56A03EEAAD052C0A7F1C
{
DWORD dwMinimum;
DWORD dwMaximum;
}
;

union tMIXERCONTROLDETAILS::$889AB721400744AE3EE1E18A6E30C475
{
HWND hwndOwner;
DWORD cMultipleItems;
}
;

struct _OVERLAPPED::$742A73540840F318F86F9CEE3D494648::$6930054A243ACD713EB66CB89E7065D5
{
DWORD Offset;
DWORD OffsetHigh;
}
;

struct _OVERLAPPED::$96D9E4EB8FC67C14A14BF1FB9F976B2E::$6930054A243ACD713EB66CB89E7065D5
{
DWORD Offset;
DWORD OffsetHigh;
}
;

union _OVERLAPPED::$96D9E4EB8FC67C14A14BF1FB9F976B2E
{
struct
{
DWORD Offset;
DWORD OffsetHigh;
};
PVOID Pointer;
}
;

union _NT_TIB::$E5D95909DD4452777D218BA78F76D110
{
PVOID FiberData;
DWORD Version;
}
;

union _TEB::$6683BEDB6429B870C0F8CD11C2F22D8B
{
_PROCESSOR_NUMBER CurrentIdealProcessor;
unsigned int IdealProcessorValue;
uint8_t ReservedPad0;
}
;

enum XAssetType : __int32
{
ASSET_TYPE_PHYSICSLIBRARY = 0x0,
ASSET_TYPE_PHYSICS_SFX_EVENT_ASSET = 0x1,
ASSET_TYPE_PHYSICS_VFX_EVENT_ASSET = 0x2,
ASSET_TYPE_PHYSICSASSET = 0x3,
ASSET_TYPE_PHYSICS_FX_PIPELINE = 0x4,
ASSET_TYPE_PHYSICS_FX_SHAPE = 0x5,
ASSET_TYPE_PHYSICS_DEBUG_DATA = 0x6,
ASSET_TYPE_XANIMPARTS = 0x7,
ASSET_TYPE_XMODEL_SURFS = 0x8,
ASSET_TYPE_XMODEL = 0x9,
ASSET_TYPE_MAYHEM = 0xA,
ASSET_TYPE_MATERIAL = 0xB,
ASSET_TYPE_COMPUTESHADER = 0xC,
ASSET_TYPE_LIBSHADER = 0xD,
ASSET_TYPE_VERTEXSHADER = 0xE,
ASSET_TYPE_HULLSHADER = 0xF,
ASSET_TYPE_DOMAINSHADER = 0x10,
ASSET_TYPE_PIXELSHADER = 0x11,
ASSET_TYPE_TECHNIQUE_SET = 0x12,
ASSET_TYPE_IMAGE = 0x13,
ASSET_TYPE_SOUND_GLOBALS = 0x14,
ASSET_TYPE_SOUND_BANK = 0x15,
ASSET_TYPE_SOUND_BANK_TRANSIENT = 0x16,
ASSET_TYPE_CLIPMAP = 0x17,
ASSET_TYPE_COMWORLD = 0x18,
ASSET_TYPE_GLASSWORLD = 0x19,
ASSET_TYPE_PATHDATA = 0x1A,
ASSET_TYPE_NAVMESH = 0x1B,
ASSET_TYPE_TACGRAPH = 0x1C,
ASSET_TYPE_MAP_ENTS = 0x1D,
ASSET_TYPE_FXWORLD = 0x1E,
ASSET_TYPE_GFXWORLD = 0x1F,
ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE = 0x20,
ASSET_TYPE_IESPROFILE = 0x21,
ASSET_TYPE_LIGHT_DEF = 0x22,
ASSET_TYPE_GRADING_CLUT = 0x23,
ASSET_TYPE_UI_MAP = 0x24,
ASSET_TYPE_FOG_SPLINE = 0x25,
ASSET_TYPE_ANIMCLASS = 0x26,
ASSET_TYPE_PLAYERANIM = 0x27,
ASSET_TYPE_GESTURE = 0x28,
ASSET_TYPE_LOCALIZE_ENTRY = 0x29,
ASSET_TYPE_ATTACHMENT = 0x2A,
ASSET_TYPE_WEAPON = 0x2B,
ASSET_TYPE_VFX = 0x2C,
ASSET_TYPE_IMPACT_FX = 0x2D,
ASSET_TYPE_SURFACE_FX = 0x2E,
ASSET_TYPE_AITYPE = 0x2F,
ASSET_TYPE_MPTYPE = 0x30,
ASSET_TYPE_CHARACTER = 0x31,
ASSET_TYPE_XMODELALIAS = 0x32,
ASSET_TYPE_RAWFILE = 0x33,
ASSET_TYPE_SCRIPTFILE = 0x34,
ASSET_TYPE_SCRIPT_DEBUG_DATA = 0x35,
ASSET_TYPE_STRINGTABLE = 0x36,
ASSET_TYPE_LEADERBOARD = 0x37,
ASSET_TYPE_VIRTUAL_LEADERBOARD = 0x38,
ASSET_TYPE_DDL = 0x39,
ASSET_TYPE_TRACER = 0x3A,
ASSET_TYPE_VEHICLE = 0x3B,
ASSET_TYPE_ADDON_MAP_ENTS = 0x3C,
ASSET_TYPE_NET_CONST_STRINGS = 0x3D,
ASSET_TYPE_LUA_FILE = 0x3E,
ASSET_TYPE_SCRIPTABLE = 0x3F,
ASSET_TYPE_EQUIPMENT_SND_TABLE = 0x40,
ASSET_TYPE_VECTORFIELD = 0x41,
ASSET_TYPE_PARTICLE_SIM_ANIMATION = 0x42,
ASSET_TYPE_STREAMING_INFO = 0x43,
ASSET_TYPE_LASER = 0x44,
ASSET_TYPE_TTF = 0x45,
ASSET_TYPE_SUIT = 0x46,
ASSET_TYPE_SUITANIMPACKAGE = 0x47,
ASSET_TYPE_CAMERA = 0x48,
ASSET_TYPE_HUD_OUTLINE = 0x49,
ASSET_TYPE_SPACESHIPTARGET = 0x4A,
ASSET_TYPE_RUMBLE = 0x4B,
ASSET_TYPE_RUMBLE_GRAPH = 0x4C,
ASSET_TYPE_ANIM_PACKAGE = 0x4D,
ASSET_TYPE_SFX_PACKAGE = 0x4E,
ASSET_TYPE_VFX_PACKAGE = 0x4F,
ASSET_TYPE_FOOTSTEP_VFX = 0x50,
ASSET_TYPE_BEHAVIOR_TREE = 0x51,
ASSET_TYPE_AI_ANIMSET = 0x52,
ASSET_TYPE_AI_ASM = 0x53,
ASSET_TYPE_XANIM_PROCEDURALBONES = 0x54,
ASSET_TYPE_XANIM_DYNAMICBONES = 0x55,
ASSET_TYPE_RETICLE = 0x56,
ASSET_TYPE_XANIM_CURVE = 0x57,
ASSET_TYPE_COVER_SELECTOR = 0x58,
ASSET_TYPE_ENEMY_SELECTOR = 0x59,
ASSET_TYPE_CLIENT_CHARACTER = 0x5A,
ASSET_TYPE_CLOTH_ASSET = 0x5B,
ASSET_TYPE_CINEMATIC_MOTION = 0x5C,
ASSET_TYPE_ACCESSORY = 0x5D,
ASSET_TYPE_LOC_DMG_TABLE = 0x5E,
ASSET_TYPE_BULLET_PENETRATION = 0x5F,
ASSET_TYPE_SCRIPT_BUNDLE = 0x60,
ASSET_TYPE_BLEND_SPACE_2D = 0x61,
ASSET_TYPE_XCAM = 0x62,
ASSET_TYPE_CAMO = 0x63,
ASSET_TYPE_XCOMPOSITEMODEL = 0x64,
ASSET_TYPE_XMODEL_DETAIL_COLLISION = 0x65,
ASSET_TYPE_STREAM_KEY = 0x66,
ASSET_TYPE_STREAM_TREE_OVERRIDE = 0x67,
ASSET_TYPE_KEY_VALUE_PAIRS = 0x68,
ASSET_TYPE_ST_TERRAIN = 0x69,
ASSET_TYPE_NATIVE_SCRIPT_PATCH = 0x6A,
ASSET_TYPE_COLLISION_TILE = 0x6B,
ASSET_TYPE_EXECUTION = 0x6C,
ASSET_TYPE_CARRY_OBJECT = 0x6D,
ASSET_TYPE_SOUND_BANK_LIST = 0x6E,
ASSET_TYPE_DECAL_VOLUME_MATERIAL = 0x6F,
ASSET_TYPE_DECAL_VOLUME_MASK = 0x70,
ASSET_TYPE_DYN_ENTITY_LIST = 0x71,
ASSET_TYPE_FX_MAP_TRANSIENTZONE = 0x72,
ASSET_TYPE_DLOGSCHEMA = 0x73,
ASSET_TYPE_EDGELIST = 0x74,
ASSET_TYPE_COUNT = 0x75,
}
;

typedef vec_t vec2_t[2]
;

enum PhysicsSFXEventAssetRuleType
{
Types_PhysicsSFXEventRule = 0x0,
Types_PhysicsSFXEventSoundRule = 0x1,
Types_PhysicsSFXEventMaterialRule = 0x2,
Types_PhysicsSFXEventAngleRule = 0x3,
Types_PhysicsSFXEventSpeedRule = 0x4,
Types_PhysicsSFXEventMassRule = 0x5,
Types_PhysicsSFXEventGravityRule = 0x6,
Types_PhysicsSFXEventAtmosphereRule = 0x7,
Types_PhysicsSFXEventCameraDistanceRule = 0x8,
Types_PhysicsSFXEventFrequencyRule = 0x9,
Types_PhysicsSFXEventRuleCount = 0xA,
}
;

struct PhysicsSFXEventAssetSoundRule
{
char *hitSoundAlias;
char *scrapeSoundAlias;
float scrapeSoundVolumeSpeedThresholdMin;
float scrapeSoundVolumeSpeedThresholdMax;
float scrapeSoundVolumeBlendSpeed;
}
;

struct PhysicsSFXEventAssetMaterialRule
{
int numentries;
PhysicsSFXEventAssetMaterialEntry *entries;
}
;

struct PhysicsSFXEventAssetAngleRule
{
int numentries;
PhysicsSFXEventAssetAngleEntry *entries;
}
;

struct PhysicsSFXEventAssetSpeedRule
{
int numentries;
PhysicsSFXEventAssetSpeedEntry *entries;
}
;

struct PhysicsSFXEventAssetMassRule
{
int numentries;
PhysicsSFXEventAssetMassEntry *entries;
}
;

struct PhysicsSFXEventAssetGravityRule
{
int numentries;
PhysicsSFXEventAssetGravityEntry *entries;
}
;

struct PhysicsSFXEventAssetAtmosphereRule
{
int numentries;
PhysicsSFXEventAssetAtmosphereEntry *entries;
}
;

struct PhysicsSFXEventAssetCameraDistanceRule
{
int numentries;
PhysicsSFXEventAssetCameraDistanceEntry *entries;
}
;

struct PhysicsSFXEventAssetFrequencyRule
{
int numentries;
PhysicsSFXEventAssetFrequencyEntry *entries;
}
;

union PhysicsSFXEventAssetRuleUnion
{
PhysicsSFXEventAssetSoundRule soundRule;
PhysicsSFXEventAssetMaterialRule materialRule;
PhysicsSFXEventAssetAngleRule angleRule;
PhysicsSFXEventAssetSpeedRule speedRule;
PhysicsSFXEventAssetMassRule massRule;
PhysicsSFXEventAssetGravityRule gravityRule;
PhysicsSFXEventAssetAtmosphereRule atmosphereRule;
PhysicsSFXEventAssetCameraDistanceRule cameraDistanceRule;
PhysicsSFXEventAssetFrequencyRule frequencyRule;
}
;

enum PhysicsVFXEventAssetRuleType
{
Types_PhysicsVFXEventRule = 0x0,
Types_PhysicsVFXEventParticleEffectRule = 0x1,
Types_PhysicsVFXEventMaterialRule = 0x2,
Types_PhysicsVFXEventAngleRule = 0x3,
Types_PhysicsVFXEventSpeedRule = 0x4,
Types_PhysicsVFXEventMassRule = 0x5,
Types_PhysicsVFXEventGravityRule = 0x6,
Types_PhysicsVFXEventAtmosphereRule = 0x7,
Types_PhysicsVFXEventCameraDistanceRule = 0x8,
Types_PhysicsVFXEventFrequencyRule = 0x9,
Types_PhysicsVFXEventRuleCount = 0xA,
}
;

struct PhysicsVFXEventAssetParticleEffectRule
{
char *hitParticleEffectAlias;
char *scrapeParticleEffectAlias;
}
;

struct PhysicsVFXEventAssetMaterialRule
{
int numentries;
PhysicsVFXEventAssetMaterialEntry *entries;
}
;

struct PhysicsVFXEventAssetAngleRule
{
int numentries;
PhysicsVFXEventAssetAngleEntry *entries;
}
;

struct PhysicsVFXEventAssetSpeedRule
{
int numentries;
PhysicsVFXEventAssetSpeedEntry *entries;
}
;

struct PhysicsVFXEventAssetMassRule
{
int numentries;
PhysicsVFXEventAssetMassEntry *entries;
}
;

struct PhysicsVFXEventAssetGravityRule
{
int numentries;
PhysicsVFXEventAssetGravityEntry *entries;
}
;

struct PhysicsVFXEventAssetAtmosphereRule
{
int numentries;
PhysicsVFXEventAssetAtmosphereEntry *entries;
}
;

struct PhysicsVFXEventAssetCameraDistanceRule
{
int numentries;
PhysicsVFXEventAssetCameraDistanceEntry *entries;
}
;

struct PhysicsVFXEventAssetFrequencyRule
{
int numentries;
PhysicsVFXEventAssetFrequencyEntry *entries;
}
;

union PhysicsVFXEventAssetRuleUnion
{
PhysicsVFXEventAssetParticleEffectRule particleEffectRule;
PhysicsVFXEventAssetMaterialRule materialRule;
PhysicsVFXEventAssetAngleRule angleRule;
PhysicsVFXEventAssetSpeedRule speedRule;
PhysicsVFXEventAssetMassRule massRule;
PhysicsVFXEventAssetGravityRule gravityRule;
PhysicsVFXEventAssetAtmosphereRule atmosphereRule;
PhysicsVFXEventAssetCameraDistanceRule cameraDistanceRule;
PhysicsVFXEventAssetFrequencyRule frequencyRule;
}
;

union XAnimDynamicFrames
{
unsigned __int8 (*_1)[3];
unsigned __int16 (*_2)[3];
}
;

union XAnimDynamicIndices
{
unsigned __int8 _1[1];
unsigned __int16 _2[1];
}
;

struct XAnimPartTransFrames
{
vec3_t mins;
vec3_t size;
XAnimDynamicFrames frames;
XAnimDynamicIndices indices;
}
;

union XAnimPartTransData
{
XAnimPartTransFrames frames;
vec3_t frame0;
}
;

struct XAnimDeltaPartQuatDataFrames2
{
__int16 (*frames)[2];
XAnimDynamicIndices indices;
}
;

union XAnimDeltaPartQuatData2
{
XAnimDeltaPartQuatDataFrames2 frames;
__int16 frame0[2];
}
;

struct XAnimDeltaPartQuatDataFrames
{
__int16 (*frames)[4];
XAnimDynamicIndices indices;
}
;

union XAnimDeltaPartQuatData
{
XAnimDeltaPartQuatDataFrames frames;
__int16 frame0[4];
}
;

union XSurfaceSharedData
{
streamer_handle_t streamedDataHandle;
char *residentData;
}
;

struct __cppobj ID3D12Object : IGraphicsUnknown
{
}
;

struct __cppobj ID3D12DeviceChild : ID3D12Object
{
}
;

struct __cppobj ID3D12Pageable : ID3D12DeviceChild
{
}
;

struct __cppobj ID3D12Resource : ID3D12Pageable
{
}
;

struct GfxSubdivCache
{
unsigned int size;
char *vertexData;
GfxWrappedRWBuffer vertexBuffer;
}
;

struct XAnimParent
{
unsigned int flags;
unsigned __int16 children;
char syncGroup;
}
;

union $543CAB31775AEAEF35DCAEB789491CCA
{
XAnimParts *parts;
XAnimParent animParent;
}
;

struct XAnimEntry
{
char nodeType;
char lod;
unsigned __int16 numAnims;
unsigned __int16 parent;
unsigned __int16 bindingIndex;
$F8E94A00F93B437FA06178475B4E431B ___u5;
}
;

struct ScriptableStateBaseDef
{
const char *name;
int flags;
unsigned int numEvents;
ScriptableEventDef *events;
}
;

enum Scriptable_StateType
{
Scriptable_StateType_Simple = 0x0,
Scriptable_StateType_Health = 0x1,
Scriptable_StateType_Scripted = 0x2,
Scriptable_StateType_Usable = 0x3,
Scriptable_StateType_Count = 0x4,
}
;

struct ScriptableStateSimpleDef
{
ScriptableStateBaseDef *base;
}
;

enum Scriptable_DamageCause
{
Scriptable_DamageCause_All = 0x0,
Scriptable_DamageCause_SplashOnly = 0x1,
Scriptable_DamageCause_NoSplash = 0x2,
Scriptable_DamageCause_NoMelee = 0x3,
Scriptable_DamageCause_None = 0x4,
Scriptable_DamageCause_MeleeOnly = 0x5,
Scriptable_DamageCause_MeleeAndSplashOnly = 0x6,
Scriptable_DamageCause_CrushOnly = 0x7,
Scriptable_DamageCause_CrushAndSplashOnly = 0x8,
Scriptable_DamageCause_Count = 0x9,
}
;

enum Scriptable_Attackers
{
Scriptable_Attackers_All = 0x0,
Scriptable_Attackers_NoPlayer = 0x1,
Scriptable_Attackers_PlayerOnly = 0x2,
Scriptable_Attackers_NoAI = 0x3,
Scriptable_Attackers_AIOnly = 0x4,
Scriptable_Attackers_None = 0x5,
Scriptable_Attackers_Count = 0x6,
}
;

enum Scriptable_TeamFilter
{
Scriptable_Team_Filter_All = 0x0,
Scriptable_Team_Filter_Bad = 0x1,
Scriptable_Team_Filter_Axis = 0x2,
Scriptable_Team_Filter_Allies = 0x3,
Scriptable_Team_Filter_Three = 0x4,
Scriptable_Team_Filter_Neutral = 0x5,
Scriptable_Team_Filter_Dead = 0x6,
Scriptable_Team_Filter_My_Team = 0x7,
Scriptable_Team_Filter_Enemy_Team = 0x8,
Scriptable_Team_Filter_All_But_My_Team = 0x9,
Scriptable_Team_Filter_All_But_Enemy_Team = 0xA,
Scriptable_Team_Filter_Enemy_Team_And_Me = 0xB,
Scriptable_Team_Filter_Just_Me = 0xC,
Scriptable_Team_Filter_Count = 0xD,
}
;

struct ScriptableStateHealthDef
{
ScriptableStateBaseDef *base;
int health;
int validDamageCause;
int minimumDamage;
float damagePropagationToParent;
float damagePropagationToChild;
float damagePropagationFromParent;
float damagePropagationFromChild;
const char *script_id;
scr_string_t scrScript_id;
bool localizeSplashDamage;
}
;

struct ScriptableStateScriptedDef
{
ScriptableStateBaseDef *base;
const char *script_id;
scr_string_t scrScript_id;
}
;

struct __declspec(align(8)) ScriptableStateUsableDef
{
ScriptableStateBaseDef *base;
const char *script_id;
scr_string_t scrScript_id;
float faceFOV;
float useFOV;
float useRadius;
float useRadiusAuto;
float useRadiusDisplay;
bool useRadiusMinimized;
bool gotoNextStateOnUse;
bool allowMissingTag;
bool usableWhenObstructed;
vec3_t useOffset;
vec3_t angleOffset;
unsigned int hintHoldDuration;
char priority;
bool use2DFOV;
__int16 hintBinding;
scr_string_t hintTag;
const char *hintString;
const char *hintStringDisabled;
const char *hintIcon;
GfxImage *hintIconImage;
unsigned int hintStringIndex;
unsigned int hintStringDisabledIndex;
unsigned int hintIconIndex;
}
;

union ScriptableStateDefUnion
{
ScriptableStateSimpleDef simple;
ScriptableStateHealthDef health;
ScriptableStateScriptedDef scripted;
ScriptableStateUsableDef usable;
}
;

struct __declspec(align(4)) XAnimDynamicBinding
{
scr_string_t fieldName;
unsigned __int16 sourceParameterIndex;
}
;

union __declspec(align(8)) XAnimConstant
{
bool boolValue;
float floatValue;
unsigned int uint32Value;
int int32Value;
void *pointerValue;
vec3_t vec3Value;
char rawBytes[12];
}
;

union $E5D29A0C1B5484B0F7AA46CADB1B1DF2
{
XAnimDynamicBinding dynamic;
XAnimConstant constant;
}
;

struct ReactiveMotionWindParams
{
float windAreaScale;
float windAmplitudeScale;
float windFrequencyScale;
float unused0;
}
;

union $3F5B938DF5E4F97F8E1F05029085C3EA
{
float radiusSquared;
unsigned int radiusSquaredAsInt;
}
;

struct ScriptableEventBaseDef
{
const char *name;
int flags;
}
;

enum Scriptable_EventType
{
Scriptable_EventType_StateChange = 0x0,
Scriptable_EventType_Wait = 0x1,
Scriptable_EventType_Random = 0x2,
Scriptable_EventType_Script = 0x3,
Scriptable_EventType_Model = 0x4,
Scriptable_EventType_Collision = 0x5,
Scriptable_EventType_Animation = 0x6,
Scriptable_EventType_HideShowBone = 0x7,
Scriptable_EventType_DisablePhysicsSubShape = 0x8,
Scriptable_EventType_NoteTrack = 0x9,
Scriptable_EventType_ChunkDynent = 0xA,
Scriptable_EventType_SpawnDynent = 0xB,
Scriptable_EventType_PFX = 0xC,
Scriptable_EventType_Sound = 0xD,
Scriptable_EventType_Explosion = 0xE,
Scriptable_EventType_Light = 0xF,
Scriptable_EventType_Sun = 0x10,
Scriptable_EventType_Rumble = 0x11,
Scriptable_EventType_Screenshake = 0x12,
Scriptable_EventType_PartDamage = 0x13,
Scriptable_EventType_SetMayhem = 0x14,
Scriptable_EventType_PlayMayhem = 0x15,
Scriptable_EventType_ViewmodelShaderParam = 0x16,
Scriptable_EventType_ViewmodelChangeImage = 0x17,
Scriptable_EventType_ClientViewSelector = 0x18,
Scriptable_EventType_TeamSelector = 0x19,
Scriptable_EventType_AddModel = 0x1A,
Scriptable_EventType_ApplyForce = 0x1B,
Scriptable_EventType_ApplyAngularForce = 0x1C,
Scriptable_EventType_ApplyConstantForce = 0x1D,
Scriptable_EventType_ApplyConstantAngularForce = 0x1E,
Scriptable_EventType_CompassIcon = 0x1F,
Scriptable_EventType_MaterialOverride = 0x20,
Scriptable_EventType_DynamicBoneNoiseCurve = 0x21,
Scriptable_EventType_Move = 0x22,
Scriptable_EventType_Footstep = 0x23,
Scriptable_EventType_GravityArc = 0x24,
Scriptable_EventType_ViewTrigger = 0x25,
Scriptable_EventType_Objective = 0x26,
Scriptable_EventType_SpatialDisable = 0x27,
Scriptable_EventType_Hover = 0x28,
Scriptable_EventType_VehicleBlowUpTire = 0x29,
Scriptable_EventType_Dlc1 = 0x2A,
Scriptable_EventType_ScriptDamage = 0x2B,
Scriptable_EventType_Count = 0x2C,
}
;

struct ScriptableEventAnonymousDef
{
ScriptableEventBaseDef *base;
char buffer[144];
}
;

struct ScriptablePartReference
{
unsigned int flatId;
unsigned int partStringHash;
}
;

struct ScriptableEventStateChangeDef
{
ScriptableEventBaseDef *base;
ScriptablePartReference partReference;
unsigned int stateIdx;
ScriptablePartDef *part;
}
;

struct ScriptableEventWaitDef
{
ScriptableEventBaseDef *base;
float delayMin;
float delayMax;
unsigned __int16 eventStreamBufferOffsetServer;
unsigned __int16 eventStreamBufferOffsetClient;
}
;

struct ScriptableEventRandomDef
{
ScriptableEventBaseDef *base;
float threshold;
unsigned int eventACount;
ScriptableEventDef *eventsA;
unsigned int eventBCount;
ScriptableEventDef *eventsB;
unsigned __int16 eventStreamBufferOffsetServer;
unsigned __int16 eventStreamBufferOffsetClient;
}
;

struct ScriptableEventScriptDef
{
ScriptableEventBaseDef *base;
const char *notification;
scr_string_t scrNotification;
int param;
bool doNotifyCallback;
}
;

union ScriptableModelUnion
{
XModel *model;
}
;

struct ScriptableEventModelDef
{
ScriptableEventBaseDef *base;
char dataType;
ScriptableModelUnion model;
char hudOutlineColor;
bool hudOutlineActive;
bool hudOutlineFill;
bool neverMoves;
bool dynamicSimulation;
bool activatePhysics;
}
;

struct ScriptableEventCollisionDef
{
ScriptableEventBaseDef *base;
const char *collmapName;
bool aiObstacle;
int clipmapCModelIndex;
bool neverMoves;
bool canPush;
bool canTouch;
bool destroyOnExit;
ScriptableModelUnion model;
}
;

struct $6CB7272563F4458FB40A4A5E123C4ABA
{
unsigned __int16 index;
unsigned __int16 tree;
}
;

union $754892263A8270AD7855E320C3BF2A51
{
$6CB7272563F4458FB40A4A5E123C4ABA __s0;
unsigned __int64 linkPointer;
}
;

struct scr_anim_t
{
$754892263A8270AD7855E320C3BF2A51 ___u0;
}
;

struct ScriptableEventAnimationDef
{
ScriptableEventBaseDef *base;
XAnimParts *animation;
bool stateful;
bool overrideAnimation;
bool pauseOnExit;
bool clientOnlyInMP;
float startTimeMin;
float startTimeMax;
bool startTimeNormalized;
float playbackRateMin;
float playbackRateMax;
float blendTime;
unsigned int eventAtEndCount;
ScriptableEventDef *eventsAtEnd;
unsigned __int16 eventStreamBufferOffsetClient;
unsigned __int16 eventStreamBufferOffsetServer;
scr_anim_t animationIndex[2];
}
;

struct ScriptableEventHideShowBoneDef
{
ScriptableEventBaseDef *base;
const char *tagName;
scr_string_t scrTagName;
bool hide;
bool allowMissingTag;
bool hideShowChildren;
}
;

struct ScriptableEventDisablePhysicsSubShapeDef
{
ScriptableEventBaseDef *base;
const char *mutableShapeName;
unsigned __int64 mutableShapeHash;
bool allowMissingShape;
}
;

struct ScriptableEventNoteTrackDef
{
ScriptableEventBaseDef *base;
unsigned int noteTrackCount;
ScriptableNoteTrackDef *noteTracks;
}
;

struct ScriptableEventChunkDynentDef
{
ScriptableEventBaseDef *base;
ScriptablePartReference partReference;
vec3_t launchLinVel;
vec3_t launchAngVel;
ScriptablePartDef *part;
}
;

struct ScriptableEventSpawnDynentDef
{
ScriptableEventBaseDef *base;
bool stateful;
XModel *model;
const char *tagName;
scr_string_t scrTagName;
bool allowCauseVelocity;
bool allowNearestVehicleVelocity;
bool worldSpaceLinVel;
bool worldSpaceAngVel;
bool randomRangeLinVel;
bool randomRangeAngVel;
vec3_t launchLinVel;
vec3_t launchAngVel;
unsigned __int16 eventStreamBufferOffsetClient;
bool allowMissingTag;
bool useRootOnMissingTag;
}
;

struct ScriptableEventPFXDef
{
ScriptableEventBaseDef *base;
bool stateful;
scr_string_t *scrTagNames;
unsigned int scrTagCount;
scr_string_t *scrEndTagNames;
unsigned int scrEndTagCount;
bool useTagAngles;
const char *effectAlias;
FxCombinedDef effectDef;
float loopRate;
bool oneshotLooping;
unsigned __int16 eventStreamBufferOffsetClient;
bool allowMissingTag;
bool useRootOnMissingTag;
bool allowNearestVehicleVelocity;
bool isViewmodel;
bool killOnExit;
bool useDynamicScriptedBeamLength;
int beamBoneAxis;
float beamLength;
char __pad0[8];
}
;

struct ScriptableEventSoundDef
{
ScriptableEventBaseDef *base;
bool stateful;
const char *tagName;
scr_string_t scrTagName;
const char *soundAlias;
bool looping;
bool externalPlay;
bool allowMissingTag;
bool useRootOnMissingTag;
const char *explosionReflClass;
bool doGroundTrace;
float groundTraceLength;
vec3_t worldOffset;
SndAliasList *soundAliasCache;
}
;

struct ScriptableEventExplosionDef
{
ScriptableEventBaseDef *base;
const char *tagName;
scr_string_t scrTagName;
WeaponCompleteDef *weapon;
float radius;
float dmgOuter;
float dmgInner;
bool stateful;
bool allowMissingTag;
bool useRootOnMissingTag;
}
;

struct ScriptableEventLightDef
{
ScriptableEventBaseDef *base;
const char *name;
scr_string_t scrName;
float intensityScaleMin;
float intensityScaleMax;
float radiusMin;
float radiusMax;
bool useColor;
vec4_t colorLinearSrgb;
bool useStateTransitionTime;
float transitionMin;
float transitionMax;
unsigned __int16 eventStreamBufferOffsetClient;
}
;

struct ScriptableEventSunDef
{
ScriptableEventBaseDef *base;
bool useIntensity;
float intensityOverrideMin;
float intensityOverrideMax;
bool useColor;
vec4_t colorLinearSrgb;
bool useStateTransitionTime;
float transitionMin;
float transitionMax;
float pitchMin;
float pitchMax;
float headingMin;
float headingMax;
bool useDirection;
}
;

struct ScriptableEventRumbleDef
{
ScriptableEventBaseDef *base;
bool stateful;
const char *tagName;
scr_string_t scrTagName;
const char *rumble;
RumbleInfo *rumbleAsset;
bool allowMissingTag;
bool useRootOnMissingTag;
}
;

struct ScriptableEventScreenshakeDef
{
ScriptableEventBaseDef *base;
bool stateful;
bool isEarthquake;
bool isScreenShake;
const char *tagName;
scr_string_t scrTagName;
float duration;
float durationFadeUp;
float durationFadeDown;
int radius;
float scaleEarthquake;
int scalePitch;
int scaleYaw;
int scaleRoll;
int frequencyPitch;
int frequencyYaw;
int frequencyRoll;
bool allowMissingTag;
bool useRootOnMissingTag;
}
;

struct ScriptableEventPartDamageDef
{
ScriptableEventBaseDef *base;
ScriptablePartReference partReference;
float amount;
bool destroy;
float amountDoT;
float intervalDoT;
unsigned __int16 eventStreamBufferOffsetServer;
unsigned __int16 eventStreamBufferOffsetClient;
ScriptablePartDef *part;
}
;

struct ScriptableEventSetMayhemDef
{
ScriptableEventBaseDef *base;
MayhemData *mayhem;
unsigned __int16 eventStreamBufferOffsetClient;
}
;

enum Scriptable_MayhemAction
{
Scriptable_MayhemAction_Play = 0x0,
Scriptable_MayhemAction_Pause = 0x1,
Scriptable_MayhemAction_Count = 0x2,
}
;

struct ScriptableEventPlayMayhemDef
{
ScriptableEventBaseDef *base;
Scriptable_MayhemAction action;
unsigned __int16 eventStreamBufferOffsetClient;
}
;

enum Scriptable_ShaderParamType
{
Scriptable_ShaderParamType_Unknown = 0x0,
Scriptable_ShaderParamType_ScrollRate_X = 0x1,
Scriptable_ShaderParamType_ScrollRate_Y = 0x2,
Scriptable_ShaderParamType_ScrollRate_R = 0x3,
Scriptable_ShaderParamType_Rotation = 0x4,
Scriptable_ShaderParamType_Tiling_X = 0x5,
Scriptable_ShaderParamType_Tiling_Y = 0x6,
Scriptable_ShaderParamType_Alpha = 0x7,
Scriptable_ShaderParamType_Emissive = 0x8,
Scriptable_ShaderParamType_AtlasFrame = 0x9,
Scriptable_ShaderParamType_Placeholder1 = 0xA,
Scriptable_ShaderParamType_Placeholder2 = 0xB,
Scriptable_ShaderParamType_Placeholder3 = 0xC,
Scriptable_ShaderParamType_Placeholder4 = 0xD,
Scriptable_ShaderParamType_Count = 0xE,
}
;

struct ScriptableEventViewmodelShaderParamDef
{
ScriptableEventBaseDef *base;
Scriptable_ShaderParamType shaderParam;
float floatValue;
float placeHolderValue1;
float placeHolderValue2;
float placeHolderValue3;
float placeHolderValue4;
float transitionTime;
unsigned __int16 eventStreamBufferOffsetClient;
}
;

enum Scriptable_ImageId
{
Scriptable_ImageId_Unknown = 0x0,
Scriptable_ImageId_Count = 0x1,
}
;

struct ScriptableEventViewmodelChangeImageDef
{
ScriptableEventBaseDef *base;
Scriptable_ImageId imageId;
int imageValue;
}
;

struct ScriptableEventClientViewSelectorDef
{
ScriptableEventBaseDef *base;
unsigned int event1pCount;
ScriptableEventDef *events1p;
unsigned int event3pCount;
ScriptableEventDef *events3p;
unsigned __int16 eventStreamBufferOffsetClient;
}
;

struct ScriptableEventTeamSelectorDef
{
ScriptableEventBaseDef *base;
Scriptable_TeamFilter teamFilter;
bool testLocalPlayer;
unsigned int eventPassCount;
ScriptableEventDef *eventsPass;
unsigned int eventFailCount;
ScriptableEventDef *eventsFail;
unsigned __int16 eventStreamBufferOffsetClient;
unsigned __int16 eventStreamBufferOffsetServer;
}
;

struct ScriptableEventAddModelDef
{
ScriptableEventBaseDef *base;
const char *tagName;
scr_string_t scrTagName;
XModel *model;
}
;

struct ScriptableEventApplyForceDef
{
ScriptableEventBaseDef *base;
vec3_t forcePos;
vec3_t forceVector;
}
;

struct ScriptableEventApplyAngularForceDef
{
ScriptableEventBaseDef *base;
bool worldSpace;
bool randomRange;
vec3_t forceVector;
vec3_t forceVector2;
}
;

struct ScriptableEventApplyConstantForceDef
{
ScriptableEventBaseDef *base;
bool worldSpace;
bool randomRange;
vec3_t forcePos;
vec3_t forceVector;
vec3_t forcePos2;
vec3_t forceVector2;
unsigned __int16 eventStreamBufferOffsetServer;
unsigned __int16 eventStreamBufferOffsetClient;
}
;

struct ScriptableEventApplyConstantAngularForceDef
{
ScriptableEventBaseDef *base;
bool worldSpace;
bool randomRange;
vec3_t forceVector;
vec3_t forceVector2;
unsigned __int16 eventStreamBufferOffsetServer;
unsigned __int16 eventStreamBufferOffsetClient;
}
;

struct ScriptableEventCompassIconDef
{
ScriptableEventBaseDef *base;
bool stateful;
bool friendlyCompassIconAlwaysVisible;
bool enemyCompassIconAlwaysVisible;
Material *friendlyArrow;
Material *friendlyFiring;
Material *friendlyChatting;
Material *friendlyYelling;
Material *partyArrow;
Material *partyFiring;
Material *partyChatting;
Material *partyYelling;
Material *squadArrow;
Material *squadFiring;
Material *enemyCompassIconQuiet[3];
Material *enemyCompassIconFiring[3];
Material *enemyCompassIconDirectional;
}
;

enum Scriptable_MaterialOverrideType
{
Scriptable_MaterialOverrideType_Off = 0x0,
Scriptable_MaterialOverrideType_Refraction = 0x1,
Scriptable_MaterialOverrideType_Dissolve = 0x2,
Scriptable_MaterialOverrideType_FlagAmplitudeScale = 0x3,
Scriptable_MaterialOverrideType_Count = 0x4,
}
;

struct ScriptableEventMaterialOverrideDef
{
ScriptableEventBaseDef *base;
Scriptable_MaterialOverrideType materialOverrideType;
char __pad0[4];
Material *material;
int transitionTime;
char finalValue;
unsigned __int16 eventStreamBufferOffsetClient;
char __pad1[8];
}
;

struct __declspec(align(8)) ScriptableEventDynamicBoneNoiseCurveDef
{
ScriptableEventBaseDef *base;
bool stateful;
XAnimCurve *curve;
float strengthMultiplier;
float rateMultiplier;
bool isViewmodel;
}
;

struct ScriptableEventMoveDef
{
ScriptableEventBaseDef *base;
vec3_t originOffset;
vec3_t angleOffset;
float seconds;
float secondsAccel;
float secondsDecel;
unsigned __int16 eventStreamBufferOffsetServer;
unsigned __int16 eventStreamBufferOffsetClient;
}
;

struct ScriptableEventFootstepDef
{
ScriptableEventBaseDef *base;
const char *tagName;
scr_string_t scrTagName;
const char *soundAlias;
bool allowMissingTag;
bool useRootOnMissingTag;
char flags;
FootstepVFX *rightFootstepVFX;
FootstepVFX *leftFootstepVFX;
}
;

struct __declspec(align(8)) ScriptableEventGravityArcDef
{
ScriptableEventBaseDef *base;
vec3_t angleOffset;
float gravityScale;
float upwardVelocity;
float yawOffsetForXYDirection;
bool endAtInitialPose;
bool randomSpin;
bool playLootTrailFX;
unsigned int eventAtEndCount;
ScriptableEventDef *eventsAtEnd;
unsigned __int16 eventStreamBufferOffsetServer;
unsigned __int16 eventStreamBufferOffsetClient;
}
;

struct ScriptableEventViewTriggerDef
{
ScriptableEventBaseDef *base;
float triggerDistanceSq;
float triggerFOVCosHalfAngle;
unsigned __int16 eventStreamBufferOffsetClient;
}
;

struct ObjectiveSettings
{
char flags[2];
char state;
char background;
int zOffset;
bool cached;
bool alwaysShowInWorld;
bool alwaysSnapToScreenEdge;
char size;
unsigned int label;
unsigned int description;
unsigned int icon;
}
;

struct ScriptableEventObjectiveDef
{
ScriptableEventBaseDef *base;
ObjectiveSettings settings;
const char *description;
const char *icon;
const char *label;
GfxImage *iconImage;
}
;

struct __declspec(align(8)) ScriptableEventSpatialDisableDef
{
ScriptableEventBaseDef *base;
bool disabled;
}
;

struct ScriptableEventHoverDef
{
ScriptableEventBaseDef *base;
float amplitudeUpDown;
float frequencyUpDown;
float frequencyPitch;
float frequencyYaw;
}
;

struct __declspec(align(8)) ScriptableEventVehicleBlowUpTireDef
{
ScriptableEventBaseDef *base;
int tireIndex;
}
;

struct ScriptableEventDlc1Def
{
ScriptableEventBaseDef *base;
float dlcFloat0;
float dlcFloat1;
float dlcFloat2;
float dlcFloat3;
}
;

struct ScriptableEventScriptDamageDef
{
ScriptableEventBaseDef *base;
const char *notification;
scr_string_t scrNotification;
int param;
}
;

union ScriptableEventDefUnion
{
ScriptableEventAnonymousDef anonymous;
ScriptableEventStateChangeDef stateChange;
ScriptableEventWaitDef wait;
ScriptableEventRandomDef random;
ScriptableEventScriptDef script;
ScriptableEventModelDef model;
ScriptableEventCollisionDef collision;
ScriptableEventAnimationDef animation;
ScriptableEventHideShowBoneDef hideShowBone;
ScriptableEventDisablePhysicsSubShapeDef disablePhysicsSubShape;
ScriptableEventNoteTrackDef noteTrack;
ScriptableEventChunkDynentDef chunkDynent;
ScriptableEventSpawnDynentDef spawnDynent;
ScriptableEventPFXDef particleFX;
ScriptableEventSoundDef sound;
ScriptableEventExplosionDef explosion;
ScriptableEventLightDef light;
ScriptableEventSunDef sun;
ScriptableEventRumbleDef rumble;
ScriptableEventScreenshakeDef screenshake;
ScriptableEventPartDamageDef partDamage;
ScriptableEventSetMayhemDef setMayhem;
ScriptableEventPlayMayhemDef playMayhem;
ScriptableEventViewmodelShaderParamDef viewmodelShaderParam;
ScriptableEventViewmodelChangeImageDef viewmodelChangeImage;
ScriptableEventClientViewSelectorDef clientViewSelector;
ScriptableEventTeamSelectorDef teamSelector;
ScriptableEventAddModelDef addModel;
ScriptableEventApplyForceDef applyForce;
ScriptableEventApplyAngularForceDef applyAngularForce;
ScriptableEventApplyConstantForceDef applyConstantForce;
ScriptableEventApplyConstantAngularForceDef applyConstantAngularForce;
ScriptableEventCompassIconDef compassIcon;
ScriptableEventMaterialOverrideDef materialOverride;
ScriptableEventDynamicBoneNoiseCurveDef dynamicBoneNoiseCurve;
ScriptableEventMoveDef move;
ScriptableEventFootstepDef footstep;
ScriptableEventGravityArcDef gravityArc;
ScriptableEventViewTriggerDef viewTrigger;
ScriptableEventObjectiveDef objective;
ScriptableEventSpatialDisableDef spatialDisable;
ScriptableEventHoverDef hover;
ScriptableEventVehicleBlowUpTireDef vehicleBlowUpTire;
ScriptableEventDlc1Def dlc1;
ScriptableEventScriptDamageDef scriptDamage;
}
;

struct XAnimPositionConstraint
{
float blendFraction;
vec3_t minLocalTranslationLimit;
vec3_t maxLocalTranslationLimit;
vec3_t offset;
vec3_t sourceBoneOffsets[2];
vec3_t axisMask;
}
;

struct XAnimRotationConstraint
{
vec4_t rotationOffsetQuat;
vec3_t axisMask;
float blendFraction;
}
;

enum XAnimUpVectorType
{
XANIM_UP_VECTOR_TYPE_WORLD_SPACE = 0x0,
XANIM_UP_VECTOR_TYPE_BONE_RELATIVE = 0x1,
XANIM_UP_VECTOR_TYPE_AIM_AT_BONE = 0x2,
}
;

struct XAnimAimConstraint
{
vec3_t aimTargetLocalOffset;
vec3_t localAimVector;
vec3_t localUpVector;
vec3_t axisMask;
vec3_t upVector;
XAnimUpVectorType upVectorType;
}
;

struct XAnimDistanceDrivenOffsetConstraint
{
vec4_t minLocalOffset;
vec4_t maxLocalOffset;
vec3_t sourceBoneOffsets[2];
float distanceScale;
float distanceBias;
char curveType;
}
;

union XAnimConstraintUnion
{
XAnimPositionConstraint positionConstraint;
XAnimRotationConstraint rotationConstraint;
XAnimAimConstraint aimConstraint;
XAnimDistanceDrivenOffsetConstraint distanceDrivenOffsetConstraint;
}
;

struct XAnimRotationalSpringProperties
{
float swingStrength;
float twistStrength;
}
;

struct XAnimTranslationalSpringProperties
{
vec3_t strength;
vec3_t damping;
vec3_t minLimit;
vec3_t maxLimit;
}
;

struct __declspec(align(8)) XAnimDynamicBoneNoiseAxis
{
XAnimCurve *curve;
float strength;
float rate;
float start;
}
;

struct XAnimDynamicBoneNoiseProperties
{
char relative;
XAnimDynamicBoneNoiseAxis axis[3];
}
;

struct XAnimDynamicBoneNoise
{
bool enable;
XAnimDynamicBoneNoiseProperties translation;
XAnimDynamicBoneNoiseProperties rotation;
}
;

struct XAnimSphereCollisionShape
{
vec3_t center;
float radius;
}
;

struct XAnimCapsuleCollisionShape
{
vec3_t start;
vec3_t end;
float radius;
}
;

struct XAnimBoxCollisionShape
{
vec3_t center;
vec3_t extents;
}
;

union XAnimCollisionShapeUnion
{
XAnimSphereCollisionShape sphere;
XAnimCapsuleCollisionShape capsule;
XAnimBoxCollisionShape box;
}
;

struct GfxConstantBufferDesc
{
void *bufferData;
unsigned int bufferSize;
}
;

struct __cppobj GfxConstantBuffer : GfxConstantBufferDesc
{
ID3D12Resource *buffer;
}
;

struct GfxDecalVolumeObb
{
vec3_t center;
tmat33_t axes;
vec3_t halfSize;
}
;

struct WeaponDamageCommon
{
int minDamage;
int mid1Damage;
int mid2Damage;
int mid3Damage;
int damage;
float maxDamageRange;
float mid1DamageRange;
float mid2DamageRange;
float mid3DamageRange;
float minDamageRange;
}
;

enum AnimDamageType : __int32
{
ANIM_DAMAGETYPE_NONE = 0x0,
ANIM_DAMAGETYPE_EXPLOSION_LIGHT = 0x1,
ANIM_DAMAGETYPE_EXPLOSION = 0x2,
ANIM_DAMAGETYPE_MELEE = 0x3,
ANIM_DAMAGETYPE_MELEE_DOG = 0x4,
ANIM_DAMAGETYPE_MELEE_ALIEN = 0x5,
ANIM_DAMAGETYPE_ENERGY_BULLET = 0x6,
ANIM_DAMAGETYPE_FIRE_EXPLOSION = 0x7,
ANIM_DAMAGETYPE_BULLET_SM = 0x8,
ANIM_DAMAGETYPE_BULLET_MD = 0x9,
ANIM_DAMAGETYPE_BULLET_LG = 0xA,
ANIM_DAMAGETYPE_FALL = 0xB,
ANIM_DAMAGETYPE_COUNT = 0xC,
}
;

struct WeaponDamageInfo
{
WeaponDamageCommon damageData[3];
AnimDamageType deathAnimDamageType;
float terminateAtRange;
}
;

enum scriptedAnimEvent_t : __int32
{
SCRIPTEDANIMEVENT_POWER_ACTIVE = 0x0,
SCRIPTEDANIMEVENT_POWER_ACTIVE_CP = 0x1,
SCRIPTEDANIMEVENT_COUNT = 0x2,
}
;

enum PenetrateType : __int32
{
PENETRATE_TYPE_NONE = 0x0,
PENETRATE_TYPE_SMALL = 0x1,
PENETRATE_TYPE_MEDIUM = 0x2,
PENETRATE_TYPE_LARGE = 0x3,
PENETRATE_TYPE_MAXIMUM = 0x4,
PENETRATE_TYPE_TABLE_ENTRY_COUNT = 0x4,
PENETRATE_TYPE_RICOCHET = 0x5,
PENETRATE_TYPE_COUNT = 0x6,
}
;

enum weapInventoryType_t : __int32
{
WEAPINVENTORY_PRIMARY = 0x0,
WEAPINVENTORY_OFFHAND = 0x1,
WEAPINVENTORY_ITEM = 0x2,
WEAPINVENTORY_ALTMODE = 0x3,
WEAPINVENTORY_EXCLUSIVE = 0x4,
WEAPINVENTORY_SCAVENGER = 0x5,
WEAPINVENTORY_MODEL_ONLY = 0x6,
WEAPINVENTORYCOUNT = 0x7,
}
;

enum weapFireType_t : __int32
{
WEAPON_FIRETYPE_FULLAUTO = 0x0,
WEAPON_FIRETYPE_SINGLESHOT = 0x1,
WEAPON_FIRETYPE_BURST = 0x2,
WEAPON_FIRETYPE_DOUBLEBARREL = 0x3,
WEAPON_FIRETYPE_BEAM = 0x4,
WEAPON_FIRETYPECOUNT = 0x5,
}
;

enum turretFireType_t
{
TURRET_SINGLE_FIRE = 0x0,
TURRET_ALTERNATE_FIRE = 0x1,
TURRET_FIRE_TYPE_COUNT = 0x2,
}
;

enum targetAssistType_t : __int32
{
TARGET_ASSISTTYPPE_DEFAULT = 0x0,
TARGET_ASSISTTYPPE_CONE = 0x1,
TARGET_ASSISTTYPPE_COUNT = 0x2,
}
;

enum targetAssistBehavior_t : __int32
{
TARGET_ASSISTBEHAVE_DEFAULT = 0x0,
TARGET_ASSISTBEHAVE_DIRECT_DAMAGE = 0x1,
TARGET_ASSISTBEHAVE_TARGET_ONLY = 0x2,
TARGET_ASSISTBEHAVE_BEAM = 0x3,
TARGET_ASSISTBEHAVE_SMART_PELLET = 0x4,
TARGET_ASSISTBEHAVE_COUNT = 0x5,
}
;

enum ReloadType
{
RELOAD_TYPE_NORMAL = 0x0,
RELOAD_TYPE_SEGMENTED = 0x1,
RELOAD_TYPE_MULTIPLE_BULLET = 0x2,
RELOAD_TYPE_MULTIPLE_PERCENTAGE = 0x3,
RELOAD_TYPE_COUNT = 0x4,
}
;

enum OffhandClass : __int32
{
OFFHAND_CLASS_NONE = 0x0,
OFFHAND_CLASS_FRAG_GRENADE = 0x1,
OFFHAND_CLASS_SMOKE_GRENADE = 0x2,
OFFHAND_CLASS_FLASH_GRENADE = 0x3,
OFFHAND_CLASS_THROWINGKNIFE = 0x4,
OFFHAND_CLASS_SHIELD = 0x5,
OFFHAND_CLASS_OTHER = 0x6,
OFFHAND_CLASS_COUNT = 0x7,
}
;

enum weapStance_t : __int32
{
WEAPSTANCE_STAND = 0x0,
WEAPSTANCE_DUCK = 0x1,
WEAPSTANCE_PRONE = 0x2,
WEAPSTANCE_NUM = 0x3,
}
;

enum ReticleType : __int32
{
RETICLE_TYPE_FOUR_SIDED = 0x0,
RETICLE_TYPE_ONE_PIECE = 0x1,
RETICLE_TYPE_COUNT = 0x2,
}
;

enum HitmarkerType
{
HITMARKER_TYPE_SMALL = 0x0,
HITMARKER_TYPE_MEDIUM = 0x1,
HITMARKER_TYPE_LARGE = 0x2,
HITMARKER_TYPE_COUNT = 0x3,
}
;

enum activeReticleType_t : __int32
{
VEH_ACTIVE_RETICLE_NONE = 0x0,
VEH_ACTIVE_RETICLE_PIP_ON_A_STICK = 0x1,
VEH_ACTIVE_RETICLE_BOUNCING_DIAMOND = 0x2,
VEH_ACTIVE_RETICLE_COUNT = 0x3,
}
;

enum turret3pAnimType_t
{
TURRET_3PANIM_MOUNTED = 0x0,
TURRET_3PANIM_MOUNTED_INPLACE = 0x1,
TURRET_3PANIM_MOUNTED_INPLACE_LEAN = 0x2,
TURRET_3PANIM_COUNT = 0x3,
}
;

enum ammoCounterClipType_t : __int32
{
AMMO_COUNTER_CLIP_NONE = 0x0,
AMMO_COUNTER_CLIP_MAGAZINE = 0x1,
AMMO_COUNTER_CLIP_SHORTMAGAZINE = 0x2,
AMMO_COUNTER_CLIP_SHOTGUN = 0x3,
AMMO_COUNTER_CLIP_ROCKET = 0x4,
AMMO_COUNTER_CLIP_BELTFED = 0x5,
AMMO_COUNTER_CLIP_ALTWEAPON = 0x6,
AMMO_COUNTER_CLIP_COUNT = 0x7,
}
;

enum weapOverlayReticle_t : __int32
{
WEAPOVERLAYRETICLE_NONE = 0x0,
WEAPOVERLAYRETICLE_CROSSHAIR = 0x1,
WEAPOVERLAYRETICLE_NUM = 0x2,
}
;

struct ADSOverlay
{
Material *shader;
Material *shaderLowRes;
Material *shaderEMP;
Material *shaderEMPLowRes;
weapOverlayReticle_t reticle;
float width;
float height;
float widthSplitscreen;
float heightSplitscreen;
}
;

enum WeapOverlayInteface_t : __int32
{
WEAPOVERLAYINTERFACE_NONE = 0x0,
WEAPOVERLAYINTERFACE_JAVELIN = 0x1,
WEAPOVERLAYINTERFACE_TURRETSCOPE = 0x2,
WEAPOVERLAYINTERFACECOUNT = 0x3,
}
;

struct CommonSwaySettings
{
float maxAngle;
float lerpSpeed;
float pitchScale;
float yawScale;
float horizScale;
float vertScale;
float gunAngleScale;
}
;

struct HipSwaySettings
{
CommonSwaySettings common;
float maxAngleSteadyAim;
}
;

struct AdsSwaySettings
{
CommonSwaySettings common;
float swayTransitionLerpSpeed;
float adsSwayScale[3];
}
;

struct AdvancedHipSwaySettings
{
bool enabled;
float torsoGoalSmoothSpeed;
int torsoGoalViewSmoothDurationMs;
vec2_t torsoGoalDeadzoneAdjustSpeed;
RumbleGraph *torsoGoalViewSpeedToMaxDeadzone_graph;
vec2_t torsoGoalViewSpeedToMaxDeadzone_viewspeed;
vec2_t torsoGoalViewSpeedToMaxDeadzone_maxDeadzone;
vec2_t torsoMass;
vec2_t torsoSpring;
vec2_t torsoDamper;
int gunGoalViewSmoothDurationMs;
RumbleGraph *gunGoalViewSpeedToOffset_graph;
vec2_t gunGoalViewSpeedToOffset_viewspeed;
vec2_t gunGoalViewSpeedToOffset_offset;
vec2_t gunMass;
vec2_t gunSpring;
vec2_t gunDamper;
vec3_t gunPivotPoint;
float gunYawToRollScale;
int fireDurationMs;
int fireStartBlendDurationMs;
int fireFinishBlendDurationMs;
float fireTorsoGoalSmoothSpeed;
float fireTorsoDeadzoneScale;
float fireTorsoToGunDirScale;
}
;

enum weaponIconRatioType_t : __int32
{
WEAPON_ICON_RATIO_1TO1 = 0x0,
WEAPON_ICON_RATIO_2TO1 = 0x1,
WEAPON_ICON_RATIO_4TO1 = 0x2,
WEAPON_ICON_RATIO_COUNT = 0x3,
}
;

enum weapProjExposion_t : __int32
{
WEAPPROJEXP_GRENADE = 0x0,
WEAPPROJEXP_ROCKET = 0x1,
WEAPPROJEXP_FLASHBANG = 0x2,
WEAPPROJEXP_NONE = 0x3,
WEAPPROJEXP_DUD = 0x4,
WEAPPROJEXP_SMOKE = 0x5,
WEAPPROJEXP_HEAVY = 0x6,
WEAPPROJEXP_NUM = 0x7,
}
;

enum WeapStickinessType : __int32
{
WEAPSTICKINESS_NONE = 0x0,
WEAPSTICKINESS_ALL = 0x1,
WEAPSTICKINESS_ALL_ORIENT = 0x2,
WEAPSTICKINESS_GROUND = 0x3,
WEAPSTICKINESS_GROUND_WITH_YAW = 0x4,
WEAPSTICKINESS_GROUND_ORIENT = 0x5,
WEAPSTICKINESS_KNIFE = 0x6,
WEAPSTICKINESS_COUNT = 0x7,
}
;

enum guidedMissileType_t : __int32
{
MISSILE_GUIDANCE_NONE = 0x0,
MISSILE_GUIDANCE_SIDEWINDER = 0x1,
MISSILE_GUIDANCE_HELLFIRE = 0x2,
MISSILE_GUIDANCE_JAVELIN = 0x3,
MISSILE_GUIDANCE_ROBOTECH = 0x4,
MISSILE_GUIDANCE_SOFTLOCKON = 0x5,
MISSILE_GUIDANCE_COUNT = 0x6,
}
;

enum WeaponOffsetBlendInterpolationType
{
WOBIT_LINEAR = 0x0,
WOBIT_CUBIC_EASE_IN = 0x1,
WOBIT_CUBIC_EASE_OUT = 0x2,
WOBIT_QUARTIC_EASE_IN = 0x3,
WOBIT_QUARTIC_EASE_OUT = 0x4,
WOBIT_EXPONENTIAL_EASE_IN = 0x5,
WOBIT_EXPONENTIAL_EASE_OUT = 0x6,
NUM_WEAPON_OFFSET_BLEND_INTERPOLATION_TYPES = 0x7,
}
;

struct WeaponOffsetCurveDescription
{
float blendTime;
float decayTime;
float shotDecayFireTimeFrac;
float holdTime;
float adsFractionBegin;
float adsFractionEnd;
WeaponOffsetBlendInterpolationType interpType;
WeaponOffsetBlendInterpolationType interpTypeOut;
}
;

enum tracerStyle_t : __int32
{
TRACERSTYLE_TRACER = 0x0,
TRACERSTYLE_BEAM = 0x1,
TRACERSTYLE_NUM = 0x2,
}
;

enum offhandShieldMaxSpeed_t : __int32
{
WEAP_OFFHAND_SHIELD_SPEED_WALK = 0x0,
WEAP_OFFHAND_SHIELD_SPEED_RUN = 0x1,
WEAP_OFFHAND_SHIELD_SPEED_SPRINT = 0x2,
WEAP_OFFHAND_SHIELD_SPEED_COUNT = 0x3,
}
;

enum offhandShieldButton_t
{
WEAP_OFFHAND_SHIELD_BUTTON_DEFAULT = 0x0,
WEAP_OFFHAND_SHIELD_BUTTON_ADS = 0x1,
WEAP_OFFHAND_SHIELD_BUTTON_COUNT = 0x2,
}
;

struct LaserSettings
{
bool forceLaserOn;
bool localPlayerADSLaserEnabled;
bool localPlayerHipLaserEnabled;
bool localPlayerNVGADSLaserEnabled;
bool localPlayerNVGHipLaserEnabled;
bool remotePlayerADSLaserEnabled;
bool remotePlayerHipLaserEnabled;
bool remotePlayerNVGADSLaserEnabled;
bool remotePlayerNVGHipLaserEnabled;
bool laserViewCenterInAds;
}
;

enum DualWieldType : __int32
{
DUAL_WIELD_TYPE_DEFAULT = 0x0,
DUAL_WIELD_TYPE_ALT_MODE = 0x1,
DUAL_WIELD_TYPE_COUNT = 0x2,
}
;

enum WeapDetonateType
{
DETONATE_TYPE_NONE = 0x0,
DETONATE_TYPE_GESTURE = 0x1,
DETONATE_TYPE_VIEWMODEL = 0x2,
DETONATE_TYPE_COUNT = 0x3,
}
;

enum GrenadeRotationDirection : __int32
{
GRD_RANDOM = 0x0,
GRD_CLOCKWISE = 0x1,
GRD_COUNTERCLOCKWISE = 0x2,
GRD_COUNT = 0x3,
}
;

struct GrenadeRotationParams
{
float initialPitch;
float initialYaw;
float initialRoll;
GrenadeRotationDirection rotationPitchDir;
int rotationPitchMin;
int rotationPitchMax;
GrenadeRotationDirection rotationYawDir;
int rotationYawMin;
int rotationYawMax;
GrenadeRotationDirection rotationRollDir;
int rotationRollMin;
int rotationRollMax;
bool rotate;
}
;

enum WeaponSlot : __int32
{
WEAPON_SLOT_NONE = 0x0,
WEAPON_SLOT_PRIMARY = 0x1,
WEAPON_SLOT_HEAVY = 0x2,
WEAPON_SLOT_MELEE = 0x3,
WEAPON_SLOT_EXECUTION = 0x4,
WEAPON_SLOT_ACCESSORY = 0x5,
WEAPON_SLOT_NUM = 0x6,
}
;

enum EnergyType
{
MOVEMENT = 0x0,
DODGE = 0x1,
COUNT = 0x2,
}
;

typedef EnergyType DeployType
;

enum CarryObjectType
{
CARRYOBJECTTYPE_GENERIC = 0x0,
CARRYOBJECTTYPE_HUMAN = 0x1,
CARRYOBJECTTYPE_COUNT = 0x2,
}
;

struct CarryAnimOverride
{
CarryObjectType carryObjectType;
WeaponAnimPackage *animPackage;
WeaponAnimPackage *animPackageAlt;
}
;

struct AdvancedIdleSettings
{
bool useAdvancedIdleSettings;
bool useRandomPointsAlgorithm;
float breathGaspScaleOverride;
float idleSwaySetting1_HipBulletDirScale;
float idleSwaySetting1_HipIdleSpeed;
float idleSwaySetting1_HipWeaponMagnitudeX;
float idleSwaySetting1_HipWeaponMagnitudeY;
float idleSwaySetting1_HipWeaponMagnitudeZ;
float idleSwaySetting1_HipWeaponMagnitudeF;
float idleSwaySetting1_HipWeaponRotationOffset;
float idleSwaySetting1_HipViewMagnitudeX;
float idleSwaySetting1_HipViewMagnitudeY;
float idleSwaySetting1_AdsBulletDirScale;
float idleSwaySetting1_AdsIdleSpeed;
float idleSwaySetting1_AdsWeaponMagnitudeX;
float idleSwaySetting1_AdsWeaponMagnitudeY;
float idleSwaySetting1_AdsWeaponMagnitudeZ;
float idleSwaySetting1_AdsWeaponMagnitudeF;
float idleSwaySetting1_AdsWeaponRotationOffset;
float idleSwaySetting1_AdsViewMagnitudeX;
float idleSwaySetting1_AdsViewMagnitudeY;
float idleSwaySetting2_HipBulletDirScale;
float idleSwaySetting2_HipIdleSpeed;
float idleSwaySetting2_HipWeaponMagnitudeX;
float idleSwaySetting2_HipWeaponMagnitudeY;
float idleSwaySetting2_HipWeaponMagnitudeZ;
float idleSwaySetting2_HipWeaponMagnitudeF;
float idleSwaySetting2_HipWeaponRotationOffset;
float idleSwaySetting2_HipViewMagnitudeX;
float idleSwaySetting2_HipViewMagnitudeY;
float idleSwaySetting2_AdsBulletDirScale;
float idleSwaySetting2_AdsIdleSpeed;
float idleSwaySetting2_AdsWeaponMagnitudeX;
float idleSwaySetting2_AdsWeaponMagnitudeY;
float idleSwaySetting2_AdsWeaponMagnitudeZ;
float idleSwaySetting2_AdsWeaponMagnitudeF;
float idleSwaySetting2_AdsWeaponRotationOffset;
float idleSwaySetting2_AdsViewMagnitudeX;
float idleSwaySetting2_AdsViewMagnitudeY;
}
;

enum SuitAnimType : __int32
{
ANIM_SUIT_HUMAN = 0x0,
ANIM_SUIT_ROBOTIC = 0x1,
ANIM_SUIT_C8 = 0x2,
ANIM_SUIT_REAPER = 0x3,
NUM_ANIM_SUIT_STATE = 0x4,
ANIM_SUIT_INVALID = 0x4,
}
;

enum MaterialTechniqueType
{
TECHNIQUE_DEPTH_PREPASS = 0x0,
TECHNIQUE_DEPTH_PREPASS_VELOCITY_RIGID = 0x1,
TECHNIQUE_DEPTH_PREPASS_VELOCITY_SKINNED = 0x2,
TECHNIQUE_DEPTH_PREPASS_OBJECTID = 0x3,
TECHNIQUE_DEPTH_PREPASS_OBJECTID_VELOCITY_RIGID = 0x1,
TECHNIQUE_DEPTH_PREPASS_OBJECTID_VELOCITY_SKINNED = 0x2,
TECHNIQUE_BUILD_SHADOWMAP_DEPTH = 0x6,
TECHNIQUE_BUILD_SHADOWMAP_COLOR = 0x7,
TECHNIQUE_UNLIT = 0x8,
TECHNIQUE_UNLIT_UNK = 0x9,
TECHNIQUE_EMISSIVE = 0xA,
TECHNIQUE_EMISSIVE_UNK = 0xB,
TECHNIQUE_EMISSIVE_DEBUG = 0xC,
TECHNIQUE_LIT = 0xD,
TECHNIQUE_THERMAL = 0x1B,
TECHNIQUE_VELOCITY_RIGID = 0x1C,
TECHNIQUE_VELOCITY_SKINNED = 0x1D,
TECHNIQUE_DEBUG_BUMPMAP = 0x1E,
TECHNIQUE_INSTANCED_DEPTH_PREPASS = 0x26,
TECHNIQUE_INSTANCED_DEPTH_PREPASS_VELOCITY_RIGID = 0x27,
TECHNIQUE_INSTANCED_DEPTH_PREPASS_VELOCITY_SKINNED = 0x28,
TECHNIQUE_INSTANCED_DEPTH_PREPASS_OBJECTID = 0x29,
TECHNIQUE_INSTANCED_DEPTH_PREPASS_OBJECTID_VELOCITY_RIGID = 0x2A,
TECHNIQUE_INSTANCED_DEPTH_PREPASS_OBJECTID_VELOCITY_SKINNED = 0x2B,
TECHNIQUE_INSTANCED_BUILD_SHADOWMAP_DEPTH = 0x2C,
TECHNIQUE_INSTANCED_BUILD_SHADOWMAP_COLOR = 0x2D,
TECHNIQUE_INSTANCED_UNLIT = 0x2E,
TECHNIQUE_INSTANCED_UNLIT_UNK = 0x2F,
TECHNIQUE_INSTANCED_EMISSIVE = 0x30,
TECHNIQUE_INSTANCED_EMISSIVE_UNK = 0x31,
TECHNIQUE_INSTANCED_EMISSIVE_DEBUG = 0x32,
TECHNIQUE_INSTANCED_LIT = 0x33,
TECHNIQUE_INSTANCED_THERMAL = 0x41,
TECHNIQUE_INSTANCED_VELOCITY_RIGID = 0x42,
TECHNIQUE_INSTANCED_VELOCITY_SKINNED = 0x43,
TECHNIQUE_INSTANCED_DEBUG_BUMPMAP = 0x44,
TECHNIQUE_SUBDIV_DEPTH_PREPASS = 0x4C,
TECHNIQUE_SUBDIV_DEPTH_PREPASS_VELOCITY_RIGID = 0x4D,
TECHNIQUE_SUBDIV_DEPTH_PREPASS_VELOCITY_SKINNED = 0x4E,
TECHNIQUE_SUBDIV_DEPTH_PREPASS_OBJECTID = 0x4F,
TECHNIQUE_SUBDIV_DEPTH_PREPASS_OBJECTID_VELOCITY_RIGID = 0x50,
TECHNIQUE_SUBDIV_DEPTH_PREPASS_OBJECTID_VELOCITY_SKINNED = 0x51,
TECHNIQUE_SUBDIV_BUILD_SHADOWMAP_DEPTH = 0x52,
TECHNIQUE_SUBDIV_BUILD_SHADOWMAP_COLOR = 0x53,
TECHNIQUE_SUBDIV_UNLIT = 0x54,
TECHNIQUE_SUBDIV_UNLIT_UNK = 0x55,
TECHNIQUE_SUBDIV_EMISSIVE = 0x56,
TECHNIQUE_SUBDIV_EMISSIVE_UNK = 0x57,
TECHNIQUE_SUBDIV_EMISSIVE_DEBUG = 0x58,
TECHNIQUE_SUBDIV_LIT = 0x59,
TECHNIQUE_SUBDIV_THERMAL = 0x67,
TECHNIQUE_SUBDIV_VELOCITY_RIGID = 0x68,
TECHNIQUE_SUBDIV_VELOCITY_SKINNED = 0x69,
TECHNIQUE_SUBDIV_DEBUG_BUMPMAP = 0x6A,
TECHNIQUE_NO_DISPLACEMENT_DEPTH_PREPASS = 0x72,
TECHNIQUE_NO_DISPLACEMENT_DEPTH_PREPASS_VELOCITY_RIGID = 0x73,
TECHNIQUE_NO_DISPLACEMENT_DEPTH_PREPASS_VELOCITY_SKINNED = 0x74,
TECHNIQUE_NO_DISPLACEMENT_DEPTH_PREPASS_OBJECTID = 0x75,
TECHNIQUE_NO_DISPLACEMENT_DEPTH_PREPASS_OBJECTID_VELOCITY_RIGID = 0x76,
TECHNIQUE_NO_DISPLACEMENT_DEPTH_PREPASS_OBJECTID_VELOCITY_SKINNED = 0x77,
TECHNIQUE_NO_DISPLACEMENT_BUILD_SHADOWMAP_DEPTH = 0x78,
TECHNIQUE_NO_DISPLACEMENT_BUILD_SHADOWMAP_COLOR = 0x79,
TECHNIQUE_NO_DISPLACEMENT_UNLIT = 0x7A,
TECHNIQUE_NO_DISPLACEMENT_UNLIT_UNK = 0x7B,
TECHNIQUE_NO_DISPLACEMENT_EMISSIVE = 0x7C,
TECHNIQUE_NO_DISPLACEMENT_EMISSIVE_UNK = 0x7D,
TECHNIQUE_NO_DISPLACEMENT_EMISSIVE_DEBUG = 0x7E,
TECHNIQUE_NO_DISPLACEMENT_LIT = 0x7F,
TECHNIQUE_NO_DISPLACEMENT_THERMAL = 0x8D,
TECHNIQUE_NO_DISPLACEMENT_VELOCITY_RIGID = 0x8E,
TECHNIQUE_NO_DISPLACEMENT_VELOCITY_SKINNED = 0x8F,
TECHNIQUE_NO_DISPLACEMENT_DEBUG_BUMPMAP = 0x90,
TECHNIQUE_COUNT = 0x98,
}
;

struct MaterialRenderTargetFormat
{
char dsFormat;
char colorRtFormats[4];
}
;

struct GfxStateBits
{
unsigned int loadBits[6];
unsigned __int16 zone;
unsigned __int8 depthStencilState[14];
unsigned __int8 blendState;
unsigned __int8 rasterizerState;
}
;

struct ParticleFloatRange
{
float min;
float max;
}
;

struct ParticleIntRange
{
int min;
int max;
}
;

struct ParticleCurveDef
{
ParticleCurveControlPointDef *controlPoints;
int numControlPoints;
float scale;
}
;

enum ParticleGravityOptions
{
PARTICLE_GRAVITY_OPTION_NONE = 0x0,
PARTICLE_GRAVITY_OPTION_GRAVITY_ONLY = 0x1,
PARTICLE_GRAVITY_OPTION_GRAVITY_NEVER = 0x2,
PARTICLE_GRAVITY_OPTION_COUNT = 0x3,
}
;

enum ParticleInstancePoolAction
{
PARTICLE_INSTANCE_POOL_ACTION_NO_SPAWN = 0x0,
PARTICLE_INSTANCE_POOL_ACTION_KILL_OLDEST_AND_SPAWN = 0x1,
PARTICLE_INSTANCE_POOL_ACTION_COUNT = 0x2,
}
;

enum ParticleDataFlags
{
USE_NONE = 0x0,
USE_POSITION = 0x1,
USE_VELOCITY = 0x2,
USE_COLOR = 0x4,
USE_ROTATION_ANGLE = 0x8,
USE_ROTATION_RATE = 0x10,
USE_SPAWN_POS = 0x20,
USE_SPAWN_QUAT = 0x40,
USE_SIZE = 0x80,
USE_EMISSIVE = 0x100,
USE_PARENT_VELOCITY = 0x200,
USE_CHILD_SYSTEM = 0x400,
USE_SPAWN_TIME = 0x800,
USE_LIFE = 0x1000,
USE_STATE = 0x2000,
USE_RANDOM_SEED = 0x4000,
USE_FLAGS = 0x8000,
USE_MODULE_TESTS = 0x10000,
USE_HANDLE = 0x20000,
USE_INTENSITY = 0x40000,
USE_TEMPERATURE = 0x80000,
USE_CAMERA_DISTANCE = 0x100000,
USE_INHERIT_PERCENTAGE = 0x200000,
USE_RAY_CAST_HANDLE = 0x400000,
USE_ATLAS_INDEX = 0x800000,
USE_VECTOR_FIELD_SCALE = 0x1000000,
USE_LIGHTING_FRAC = 0x2000000,
USE_PREV_PLACEMENT = 0x4000000,
USE_SHADER_PARAMS = 0x8000000,
CHECK_FIELD_COUNT = 0x8000001,
USE_ALL = 0xFFFFFFF,
USE_BASE = 0x192F8FF,
USE_EMISSIVE_CURVES = 0x2040100,
}
;

enum ParticleScriptedInputNodeType
{
PARTICLE_SCRIPTED_INPUT_NODE_INVALID = 0xFFFFFFFF,
PARTICLE_SCRIPTED_INPUT_NODE_KVP = 0x0,
PARTICLE_SCRIPTED_INPUT_NODE_EMITTER_DISABLE = 0x1,
PARTICLE_SCRIPTED_INPUT_NODE_COUNT = 0x2,
}
;

struct ParticleScriptedInputNode
{
ParticleScriptedInputNodeType m_type;
unsigned int m_flags;
unsigned int m_emitterIndex;
unsigned int m_pad[5];
}
;

enum ParticleScriptedInputKey
{
}
;

struct ParticleScriptedInputRecord
{
ParticleScriptedInputKey key;
bool dirty;
bool value;
unsigned int pad[2];
}
;

struct __cppobj ParticleScriptedInputNodeKVP : ParticleScriptedInputNode
{
ParticleScriptedInputRecord m_inputRecord;
}
;

struct __cppobj ParticleScriptedInputNodeEmitterDisable : ParticleScriptedInputNode
{
}
;

union MayhemFramesUnion
{
void *data;
MayhemAnimFramesSplineCompressed *splineCompressedFrames;
MayhemAnimFramesUncompressed *uncompressedFrames;
}
;

union MayhemDataChannelsUnion
{
void *data;
MayhemDataKeysSplineCompressed *splineCompressedKeys;
MayhemDataKeysUncompressed *uncompressedKeys;
}
;

struct MayhemSelfVisChannel
{
unsigned int numFrames;
unsigned int numVerts;
unsigned int vertexOffset;
}
;

typedef EnergyType MeleeAnimType
;

enum MeleeImpactType : __int32
{
MELEE_IMPACT_TYPE_NONE = 0x0,
}
;

typedef EnergyType WeaponMaterialType
;

struct TriggerDisconnectSoundData
{
float analogTriggerValue;
bool enabledForFireType[6];
}
;

enum WeaponOffsetBlendCurveId
{
WOBC_HOLD_FIRE_BLEND_SLOW = 0x0,
WOBC_HOLD_FIRE_BLEND_FAST = 0x1,
WOBC_KICK_BLEND = 0x2,
WOBC_SNAP_DECAY_BLEND = 0x3,
WOBC_ADS_BLEND = 0x4,
WOBC_ALWAYS_ON = 0x5,
NUM_WEAPON_OFFSET_BLEND_CURVES = 0x6,
}
;

enum WeaponOffsetPatternId
{
WOP_KEYFRAME = 0x0,
WOP_NOISY_SINE = 0x1,
WOP_RANDOM_SQUARE = 0x2,
WOP_SINE = 0x3,
NUM_WEAPON_OFFSET_PATTERNS = 0x4,
}
;

enum WeaponOffsetTransformType
{
WOTT_VIEW_ORIGIN = 0x0,
WOTT_VIEW_ANGLES = 0x1,
WOTT_WEAPON_ORIGIN = 0x2,
WOTT_WEAPON_ANGLES = 0x3,
NUM_WEAPON_OFFSET_TRANSFORM_TYPES = 0x4,
}
;

enum WeaponEntityNotifyFlags
{
WEAP_NOTIFY_FLAG_NONE = 0x0,
WEAP_NOFITY_FLAG_CHECK_TEAM = 0x1,
WEAP_NOFITY_FLAG_CHECK_VISIBLE = 0x2,
}
;

enum AdsAltSwitchInterpType : __int32
{
ADS_ALT_SWITCH_INTERP_TYPE_LINEAR = 0x0,
ADS_ALT_SWITCH_INTERP_TYPE_COSINE = 0x1,
ADS_ALT_SWITCH_INTERP_TYPE_SINE = 0x2,
ADS_ALT_SWITCH_INTERP_TYPE_COUNT = 0x3,
}
;

enum ADSRCurve
{
ADSR_CURVE_LINEAR = 0x0,
ADSR_CURVE_LOG_FAST = 0x1,
ADSR_CURVE_LOG_SLOW = 0x2,
ADSR_CURVE_FLAT_ENDED = 0x3,
ADSR_CURVE_FLAT_MIDDLE = 0x4,
}
;

enum SND_EQTYPE
{
SND_EQTYPE_FIRST = 0x0,
SND_EQTYPE_LOWPASS = 0x0,
SND_EQTYPE_HIGHPASS = 0x1,
SND_EQTYPE_LOWSHELF = 0x2,
SND_EQTYPE_HIGHSHELF = 0x3,
SND_EQTYPE_BELL = 0x4,
SND_EQTYPE_LAST = 0x4,
SND_EQTYPE_COUNT = 0x5,
SND_EQTYPE_INVALID = 0x5,
}
;

enum WeapReflRoomType
{
SND_WR_ROOM_EXTERIOR = 0x0,
SND_WR_ROOM_INTERIOR = 0x1,
}
;

union StreamKeyData
{
streamer_handle_t dataHandle;
char *residentData;
char *residentDataGPU;
}
;

union $073322FE7C1053F6D96A83C00F8F6D30
{
void *behaviorUserPtr;
unsigned __int64 assetHash;
}
;

enum AttachmentSlot
{
ATT_SLOT_RECEIVER = 0x0,
ATT_SLOT_FRONT_PIECE = 0x1,
ATT_SLOT_BACK_PIECE = 0x2,
ATT_SLOT_MAGAZINE = 0x3,
ATT_SLOT_OVERRIDE = 0x4,
ATT_SLOT_MUZZLE = 0x5,
ATT_SLOT_REAR_GRIP = 0x6,
ATT_SLOT_TRIGGER = 0x7,
ATT_SLOT_EXTRA = 0x8,
ATT_SLOT_SCOPE = 0x9,
ATT_SLOT_UNDERBARREL = 0xA,
ATT_SLOT_MODIFIER = 0xB,
ATT_SLOT_VISUAL = 0xC,
ATT_SLOT_OTHER = 0xD,
ATT_SLOT_COUNT = 0xE,
ATT_SLOT_PRIMARY_COUNT = 0xD,
ATT_SLOT_OTHER_COUNT = 0x10,
}
;

struct ParticleModule
{
ParticleModuleType type;
unsigned int m_flags;
}
;

struct __cppobj ParticleModuleInitAtlas : ParticleModule
{
int m_playRate;
int m_startFrame;
int m_loopCount;
}
;

struct __cppobj ParticleModuleInitAttributes : ParticleModule
{
bool m_useNonUniformInterpolationForColor;
bool m_useNonUniformInterpolationForSize;
char m_pad[6];
float4 m_sizeMin;
float4 m_sizeMax;
float4 m_colorMin;
float4 m_colorMax;
float4 m_velocityMin;
float4 m_velocityMax;
}
;

enum ParticleModuleInitBeamFlags
{
PARTICLE_MODULE_INIT_BEAM_FLAG_TILE = 0x1,
PARTICLE_MODULE_INIT_BEAM_FLAG_USE_CURVE_POINTS = 0x2,
PARTICLE_MODULE_INIT_BEAM_FLAG_CAMERA_FACING = 0x4,
}
;

struct ParticleLinkedAssetListDef
{
ParticleLinkedAssetDef *assetList;
int numAssets;
}
;

struct __cppobj ParticleModuleInitBeam : ParticleModule
{
ParticleModuleInitBeamFlags m_beamFlags;
float m_splitDistance;
float4 m_startPos;
float4 m_endPos;
float4 m_offset;
float4 m_curvePoint1;
float4 m_curvePoint2;
ParticleLinkedAssetListDef m_linkedAssetList;
float m_scrollRateUV;
float m_scrollRateSpiralGraph;
float m_spiralRepeatDistance;
}
;

struct __cppobj ParticleModuleInitCameraOffset : ParticleModule
{
unsigned int m_pad[2];
}
;

struct __cppobj ParticleModuleInitCloud : ParticleModule
{
unsigned int m_pad[2];
ParticleCurveDef curves[2];
}
;

struct __cppobj ParticleModuleInitDecal : ParticleModule
{
unsigned __int16 m_fadeInTime;
unsigned __int16 m_fadeOutTime;
unsigned __int16 m_stoppableFadeOutTime;
unsigned __int16 m_lerpWaitTime;
float4 m_lerpColor;
ParticleLinkedAssetListDef m_linkedAssetList;
}
;

struct __cppobj ParticleModuleInitFlare : ParticleModule
{
unsigned int m_pad[1];
float m_position;
float4 m_direction;
int m_angularRotCount;
unsigned int m_flareFlags;
ParticleFloatRange m_depthScaleRange;
ParticleFloatRange m_depthScaleValue;
ParticleFloatRange m_radialRot;
ParticleFloatRange m_radialScaleX;
ParticleFloatRange m_radialScaleY;
ParticleCurveDef m_curves[4];
}
;

struct __cppobj ParticleModuleInitGeoTrail : ParticleModule
{
unsigned int m_numPointsMax;
float m_splitDistance;
float m_splitAngle;
float m_centerOffset;
unsigned int m_numSheets;
float m_fadeInDistance;
float m_fadeOutDistance;
float m_tileDistance;
ParticleFloatRange m_tileOffset;
float m_scrollTime;
bool m_useLocalVelocity;
bool m_useVerticalTexture;
bool m_cameraFacing;
bool m_fixLeadingEdge;
bool m_clampUVs;
char m_pad[3];
unsigned int m_pad2[1];
}
;

struct __cppobj ParticleModuleInitKillWrapBox : ParticleModule
{
char m_planesKill[1];
char m_pad[7];
float4 m_bounds;
float4 m_offset;
}
;

struct __cppobj ParticleModuleInitLightOmni : ParticleModule
{
ParticleLinkedAssetListDef m_linkedAssetList;
float m_tonemappingScaleFactor;
float m_intensityIR;
bool m_disableVolumetric;
unsigned __int8 m_exponent;
char __pad0[2];
float m_shadowSoftness;
float m_shadowBias;
float m_shadowArea;
}
;

struct __cppobj ParticleModuleInitLightSpot : ParticleModule
{
float m_fovOuter;
float m_fovInner;
float m_bulbRadius;
float m_bulbLength;
float m_brightness;
float unk1;
float m_intensityUV;
float m_intensityIR;
float m_shadowNearPlane;
float m_toneMappingScaleFactor;
float m_shadowSoftness;
float m_shadowBias;
float m_shadowArea;
bool m_disableVolumetric;
bool m_disableShadowMap;
bool m_disableDynamicShadows;
bool m_exponent;
ParticleLinkedAssetListDef m_linkedAssetList;
}
;

enum ParticleRenderOptions
{
PARTICLE_RENDER_OPTION_NONE = 0x0,
PARTICLE_RENDER_OPTION_THERMAL_VIEW_ONLY = 0x1,
PARTICLE_RENDER_OPTION_THERMAL_VIEW_NEVER = 0x2,
PARTICLE_RENDER_OPTION_COUNT = 0x3,
}
;

enum ParticleShaderGraphOptions
{
PARTICLE_SHADER_GRAPH_NONE = 0x0,
PARTICLE_SHADER_GRAPH_DEPTHSCAN = 0x1,
PARTICLE_SHADER_GRAPH_SPHERICALZ = 0x2,
PARTICLE_SHADER_GRAPH_OPTION_COUNT = 0x3,
}
;

struct Particle_MaterialData
{
vec4_t falloffParms;
vec4_t opaqueColor;
vec4_t transparentColor;
vec2_t translucentShadowParams;
vec2_t beamParms;
vec4_t eyeOffsetParms;
vec4_t alphaDissolveParms;
vec4_t featherParms0;
vec4_t featherParms1;
vec4_t regionHighlightParms;
vec4_t depthScanColor;
vec4_t depthScanOutlineColor;
vec2_t thermalParams;
float anisotropy;
unsigned int featureEnabledFlags;
}
;

struct __cppobj ParticleModuleInitMaterial : ParticleModule
{
unsigned int m_pad[2];
ParticleLinkedAssetListDef m_linkedAssetList;
}
;

enum ParticleMirrorTextureType
{
PARTICLE_MIRROR_TEXTURE_TYPE_NONE = 0x0,
PARTICLE_MIRROR_TEXTURE_TYPE_STANDARD = 0x1,
PARTICLE_MIRROR_TEXTURE_TYPE_RANDOM = 0x2,
PARTICLE_MIRROR_TEXTURE_TYPE_COUNT = 0x3,
}
;

struct __cppobj ParticleModuleInitMirrorTexture : ParticleModule
{
ParticleMirrorTextureType m_mirrorHorizontal;
ParticleMirrorTextureType m_mirrorVertical;
}
;

struct __cppobj ParticleModuleInitModel : ParticleModule
{
bool m_usePhysics;
bool m_motionBlurHQ;
ParticleLinkedAssetListDef m_linkedAssetList;
}
;

struct __cppobj ParticleModuleInitOcclusionQuery : ParticleModule
{
float m_fadeInTime;
float m_fadeOutTime;
ParticleFloatRange m_scale;
vec2_t m_worldSize;
}
;

struct __cppobj ParticleModuleInitOrientedSprite : ParticleModule
{
unsigned int m_pad[2];
float4 m_orientationQuat;
}
;

struct __cppobj ParticleModuleInitParticleSim : ParticleModule
{
ParticleLinkedAssetListDef m_linkedAssetList;
ParticleFloatRange m_scaleFactor;
bool m_holdLastFrame;
unsigned int m_pad[3];
}
;

struct __cppobj ParticleModuleInitPlayerFacing : ParticleModule
{
unsigned int m_pad[2];
}
;

enum ParticleRelativeVelocityType
{
PARTICLE_RELATIVE_VELOCITY_TYPE_LOCAL = 0x0,
PARTICLE_RELATIVE_VELOCITY_TYPE_WORLD = 0x1,
PARTICLE_RELATIVE_VELOCITY_TYPE_RELATIVE_TO_EFFECT_ORIGIN = 0x2,
PARTICLE_RELATIVE_VELOCITY_TYPE_COUNT = 0x3,
PARTICLE_RELATIVE_VELOCITY_TYPE_LOCAL_WITH_BOLT_INFO = 0x3,
PARTICLE_RELATIVE_VELOCITY_TYPE_WORLD_WITH_BOLT_INFO = 0x4,
PARTICLE_RELATIVE_VELOCITY_TYPE_RELATIVE_TO_EFFECT_ORIGIN_WITH_BOLT_INFO = 0x5,
}
;

struct __cppobj ParticleModuleInitRelativeVelocity : ParticleModule
{
ParticleRelativeVelocityType m_velocityType;
bool m_useBoltInfo;
}
;

struct __cppobj ParticleModuleInitRotation : ParticleModule
{
unsigned int m_pad[2];
ParticleFloatRange m_rotationAngle;
ParticleFloatRange m_rotationRate;
}
;

struct __cppobj ParticleModuleInitRotation3D : ParticleModule
{
unsigned int m_pad[2];
float4 m_rotationAngleMin;
float4 m_rotationAngleMax;
float4 m_rotationRateMin;
float4 m_rotationRateMax;
}
;

struct ParticleModifier
{
float4 min;
float4 max;
}
;

struct __cppobj ParticleModuleInitRunner : ParticleModule
{
unsigned int m_pad[2];
ParticleLinkedAssetListDef m_linkedAssetList;
}
;

struct __cppobj ParticleModuleInitSound : ParticleModule
{
unsigned int m_pad[2];
ParticleLinkedAssetListDef m_linkedAssetList;
}
;

struct __cppobj ParticleModuleInitSpawn : ParticleModule
{
unsigned int m_pad[2];
ParticleCurveDef m_curves[1];
}
;

struct __cppobj ParticleModuleInitSpawnShape : ParticleModule
{
char m_axisFlags;
char m_spawnFlags;
char m_normalAxis;
char m_spawnType;
float m_volumeCubeRoot;
float4 m_offset;
}
;

struct __cppobj ParticleModuleInitSpawnShapeBox : ParticleModuleInitSpawnShape
{
bool m_useBeamInfo;
char m_pad[15];
float4 m_dimensionsMin;
float4 m_dimensionsMax;
}
;

struct __cppobj ParticleModuleInitSpawnShapeCylinder : ParticleModuleInitSpawnShape
{
bool m_hasRotation;
bool m_rotateCalculatedOffset;
float m_halfHeight;
ParticleFloatRange m_radius;
float4 unk;
float4 m_directionQuat;
}
;

struct __cppobj ParticleModuleInitSpawnShapeEllipsoid : ParticleModuleInitSpawnShape
{
float4 m_radiusMin;
float4 m_radiusMax;
unsigned int m_pad[4];
}
;

struct __cppobj ParticleModuleInitSpawnShapeMesh : ParticleModuleInitSpawnShape
{
unsigned int m_numMeshAssets;
ParticleLinkedAssetListDef m_linkedAssetList;
ParticleSpawnMeshAssetDef *m_meshAssetData;
unsigned int m_pad[3];
}
;

struct __cppobj ParticleModuleInitSpawnShapeSphere : ParticleModuleInitSpawnShape
{
unsigned int m_pad[2];
ParticleFloatRange m_radius;
float4 m_offset2;
}
;

struct __cppobj ParticleModuleInitTail : ParticleModule
{
unsigned __int16 m_averagePastVelocities;
unsigned __int16 m_maxParentSpeed;
bool m_tailLeading;
bool m_scaleWithVelocity;
bool m_rotateAroundPivot;
}
;

struct __cppobj ParticleModuleInitVectorField : ParticleModule
{
unsigned int m_pad[2];
ParticleLinkedAssetListDef m_linkedAssetList;
}
;

struct __cppobj ParticleModuleInitVolumetric : ParticleModule
{
float m_density;
float m_falloff;
float m_noiseAmplitude;
unsigned int m_noiseMode;
unsigned int m_pad[2];
}
;

struct __cppobj ParticleModuleInitDismember : ParticleModule
{
unsigned int m_chance;
char m_dismembermentType[1];
char m_pad[3];
}
;

struct __cppobj ParticleModuleAttractor : ParticleModule
{
unsigned int m_pad[3];
bool m_killWhenNear;
float m_forceMag;
float m_nearDistanceSq;
float4 m_attractPoint;
}
;

struct __cppobj ParticleModuleColorGraph : ParticleModule
{
bool firstCurve;
char m_pad[3];
bool m_modulateColorByAlpha;
ParticleCurveDef m_curves[8];
}
;

struct __cppobj ParticleModuleColorLerp : ParticleModule
{
unsigned int m_pad[2];
float4 m_colorBegin;
float4 m_colorEnd;
}
;

struct __cppobj ParticleModuleEmissionGraph : ParticleModule
{
unsigned int m_pad[2];
ParticleCurveDef m_curves[2];
}
;

struct __cppobj ParticleModuleEmissiveGraph : ParticleModule
{
bool firstCurve;
char m_pad[7];
ParticleCurveDef m_curves[2];
}
;

struct __cppobj ParticleModuleShaderGraph : ParticleModule
{
bool firstCurve;
char m_pad[7];
ParticleCurveDef m_curves[12];
}
;

struct __cppobj ParticleModuleForce : ParticleModule
{
unsigned int m_pad[2];
float4 m_forceMin;
float4 m_forceMax;
}
;

struct __cppobj ParticleModuleForceDragGraph : ParticleModule
{
unsigned int m_pad[2];
ParticleCurveDef m_curves[2];
}
;

struct __cppobj ParticleModuleGravity : ParticleModule
{
ParticleFloatRange m_gravityPercentage;
}
;

struct __cppobj ParticleModuleIntensityGraph : ParticleModule
{
bool firstCurve;
char m_pad[7];
ParticleCurveDef m_curves[2];
}
;

struct __cppobj ParticleModuleTemperatureGraph : ParticleModule
{
bool firstCurve;
char m_pad[7];
ParticleCurveDef m_curves[2];
}
;

struct __cppobj ParticleModuleParentVelocityGraph : ParticleModule
{
unsigned int m_pad[2];
ParticleCurveDef m_curves[2];
}
;

struct __cppobj ParticleModulePhysicsLight : ParticleModule
{
bool m_ignoreEmitterOrientation;
bool m_useSurfaceType;
char m_pad[6];
ParticleLinkedAssetListDef m_linkedAssetList;
}
;

struct __cppobj ParticleModulePhysicsRayCast : ParticleModule
{
ParticleFloatRange m_bounce;
Bounds m_bounds;
bool m_useItemClip;
bool m_useSurfaceType;
bool m_collideWithWater;
bool m_ignoreContentItem;
char m_pad[3];
}
;

struct __cppobj ParticleModulePositionGraph : ParticleModule
{
unsigned int m_pad[2];
ParticleCurveDef m_curves[6];
}
;

struct __cppobj ParticleModuleRotationGraph : ParticleModule
{
bool m_useRotationRate;
unsigned int m_pad[1];
ParticleCurveDef m_curves[2];
}
;

struct __cppobj ParticleModuleRotationGraph3D : ParticleModule
{
bool m_useRotationRate;
unsigned int m_pad[1];
ParticleCurveDef m_curves[6];
}
;

struct __cppobj ParticleModuleScaleByDistance : ParticleModule
{
unsigned int m_pad[2];
ParticleCurveDef m_curves[2];
float m_maxDistance;
float m_scaleBegin;
float m_scaleEnd;
unsigned int m_pad2[1];
}
;

struct __cppobj ParticleModuleSizeGraph : ParticleModule
{
bool firstCurve;
char m_pad[7];
ParticleCurveDef m_curves[6];
float4 m_sizeBegin;
float4 m_sizeEnd;
}
;

struct __cppobj ParticleModuleSizeLerp : ParticleModule
{
unsigned int m_pad[2];
float4 m_sizeBegin;
float4 m_sizeEnd;
}
;

struct __cppobj ParticleModuleVelocityGraph : ParticleModule
{
unsigned int m_pad[2];
ParticleCurveDef m_curves[6];
float4 m_velocityBegin;
float4 m_velocityEnd;
}
;

struct __cppobj ParticleModuleVectorFieldGraph : ParticleModule
{
bool firstCurve;
char m_pad[7];
ParticleCurveDef m_curves[2];
}
;

struct ParticleModuleTestEventHandlerData
{
unsigned int m_nextState;
ParticleLinkedAssetListDef m_linkedAssetList;
bool m_kill;
unsigned int m_pad[1];
}
;

struct __cppobj ParticleModuleTest : ParticleModule
{
unsigned __int16 m_moduleIndex;
char m_useOrientationOptions[1];
char m_useScaleOptions[1];
char m_useVelocityOptions[1];
ParticleModuleTestEventHandlerData m_eventHandlerData;
}
;

struct __cppobj ParticleModuleTestAge : ParticleModuleTest
{
float m_age;
}
;

struct __cppobj ParticleModuleTestBirth : ParticleModuleTest
{
}
;

struct __cppobj ParticleModuleTestDeath : ParticleModuleTest
{
}
;

struct __cppobj ParticleModuleTestGravity : ParticleModuleTest
{
}
;

struct __cppobj ParticleModuleTestImpact : ParticleModuleTest
{
}
;

struct __cppobj ParticleModuleTestPos : ParticleModuleTest
{
}
;

struct __cppobj ParticleModuleTestTimeInState : ParticleModuleTest
{
ParticleFloatRange m_timeInState;
char m_pad[8];
}
;

union ParticleModuleTypeDef
{
ParticleModule moduleBase;
ParticleModuleInitAtlas initAtlas;
ParticleModuleInitAttributes initAttributes;
ParticleModuleInitBeam initBeam;
ParticleModuleInitCameraOffset initCameraOffset;
ParticleModuleInitCloud initCloud;
ParticleModuleInitDecal initDecal;
ParticleModuleInitFlare initFlare;
ParticleModuleInitGeoTrail initGeoTrail;
ParticleModuleInitLightOmni initLightOmni;
ParticleModuleInitLightSpot initLightSpot;
ParticleModuleInitMaterial initMaterial;
ParticleModuleInitMirrorTexture initMirrorTexture;
ParticleModuleInitModel initModel;
ParticleModuleInitOcclusionQuery initOcclusionQuery;
ParticleModuleInitOrientedSprite initOrientedSprite;
ParticleModuleInitParticleSim initParticleSim;
ParticleModuleInitPlayerFacing initPlayerFacing;
ParticleModuleInitRelativeVelocity initRelativeVelocity;
ParticleModuleInitRotation initRotation;
ParticleModuleInitRotation3D initRotation3D;
ParticleModuleInitRunner initRunner;
ParticleModuleInitSound initSound;
ParticleModuleInitSpawn initSpawn;
ParticleModuleInitSpawnShapeBox initSpawnShapeBox;
ParticleModuleInitSpawnShapeCylinder initSpawnShapeCylinder;
ParticleModuleInitSpawnShapeEllipsoid initSpawnShapeEllipsoid;
ParticleModuleInitSpawnShapeMesh initSpawnShapeMesh;
ParticleModuleInitSpawnShapeSphere initSpawnShapeSphere;
ParticleModuleInitTail initTail;
ParticleModuleInitVectorField initVectorField;
ParticleModuleInitVolumetric initVolumetric;
ParticleModuleAttractor attractor;
ParticleModuleColorGraph colorGraph;
ParticleModuleColorLerp colorLerp;
ParticleModuleEmissionGraph emissionGraph;
ParticleModuleEmissiveGraph emissiveGraph;
ParticleModuleForce force;
ParticleModuleGravity gravity;
ParticleModuleIntensityGraph intensityGraph;
ParticleModuleParentVelocityGraph parentVelocityGraph;
ParticleModulePhysicsLight physicsLight;
ParticleModulePhysicsRayCast physicsRayCast;
ParticleModulePositionGraph positionGraph;
ParticleModuleRotationGraph rotationGraph;
ParticleModuleRotationGraph3D rotationGraph3D;
ParticleModuleSizeGraph sizeGraph;
ParticleModuleSizeLerp sizeLerp;
ParticleModuleVelocityGraph velocityGraph;
ParticleModuleTestAge testAge;
ParticleModuleTestBirth testBirth;
ParticleModuleTestDeath testDeath;
ParticleModuleTestGravity testGravity;
ParticleModuleTestImpact testImpact;
ParticleModuleTestPos testPos;
}
;

struct SndMusicAsset
{
char alias[64];
SndStringHash aliasId;
int looping;
int completeLoop;
int removeAfterPlay;
int firstRandom;
int startSync;
int stopSync;
int completeOnStop;
unsigned int loopStartOffset;
int bpm;
int assetType;
int loopNumber;
int order;
int startDelayBeats;
int startFadeBeats;
int stopDelayBeats;
int stopFadeBeats;
int startOffsetFrames;
int meter;
}
;

enum SndMusicStateStatus
{
SND_MUSIC_STATE_INACTIVE = 0x0,
SND_MUSIC_STATE_ACTIVE = 0x1,
}
;

struct ParticlePhysicsFXData
{
PhysicsFXPipeline *physicsFXPipeline;
PhysicsFXShape *physicsFXShape;
}
;

struct ParticleMarkVisuals
{
Material *materials[3];
}
;

struct FxParticleSimAnimationHeader
{
float playbackRate;
float duration;
unsigned int frameCount;
float minX;
float minY;
float minZ;
float boundsXDelta;
float boundsYDelta;
float boundsZDelta;
float maxWidth;
float maxHeight;
unsigned int colorTableSize;
unsigned int particleDataCount;
unsigned int maxActiveParticles;
bool evalVisStatePerParticle;
bool sortParticlesAtRuntime;
bool hasOrientation3D;
bool hasIndex;
}
;

enum VectorFieldType : __int32
{
VECTOR_FIELD_TYPE_NONE = 0x0,
VECTOR_FIELD_TYPE_FORCE = 0x1,
VECTOR_FIELD_TYPE_VELOCITY = 0x2,
VECTOR_FIELD_NUM_TYPES = 0x3,
}
;

enum SndEntchannelSpaitalType
{
SND_ENTCHAN_TYPE_2D = 0x0,
SND_ENTCHAN_TYPE_3D = 0x1,
SND_ENTCHAN_TYPE_PA_SPEAKER = 0x2,
}
;

enum SndLimitBehavior
{
LIMIT_BEHAVIOR_NO_NEW = 0x0,
LIMIT_BEHAVIOR_REPLACE_QUIETER = 0x1,
LIMIT_BEHAVIOR_REPLACE_OLDEST = 0x2,
LIMIT_BEHAVIOR_REPLACE_FARTHEST = 0x3,
LIMIT_BEHAVIOR_COUNT = 0x4,
}
;

enum SndFarReverbBehavior
{
SND_FAR_REVERB_NONE = 0x0,
SND_FAR_REVERB_FULL = 0x1,
SND_FAR_REVERB_WET_ONLY = 0x2,
SND_FAR_REVERB_UNK3 = 0x3,
SND_FAR_REVERB_UNK4 = 0x4,
SND_FAR_REVERB_UNK5 = 0x5,
}
;

enum SndSendType
{
SENDTYPE_REVERB = 0x0,
SENDTYPE_DELAY = 0x1,
SENDTYPE_NONE = 0x2,
}
;

struct SndMasterEqParam
{
int enabled;
float gain;
float frequency;
float q;
}
;

struct SndMasterEqParamBank
{
SndMasterEqParam low;
SndMasterEqParam peak1;
SndMasterEqParam peak2;
SndMasterEqParam high;
}
;

struct SndDynamicsParams
{
int enable;
float preGain;
float postGain;
float threshold;
float ratio;
float timeAttack;
float timeRelease;
float spread;
int linear;
int ITU1770;
}
;

struct SndPeakLimiterParams
{
int enable;
float inputGain;
float outputGain;
float threshold;
float releaseMin;
float releaseMax;
}
;

union $12D90001A93DF9A723CD6F1803FB77E7
{
unsigned int parentNodeIndex;
unsigned int qEdgeTableIndex;
}
;

union $08C5987D687396C54F83AAC68626D4F4
{
unsigned int childNodeSetIndex;
unsigned int qEdgeTableCount;
}
;

struct PMROctreeNode
{
unsigned __int16 uniformGridIndex[3];
char depth;
char flags[1];
$12D90001A93DF9A723CD6F1803FB77E7 ___u3;
$08C5987D687396C54F83AAC68626D4F4 ___u4;
}
;

struct GfxPlacement
{
vec4_t quat;
vec3_t origin;
}
;

struct TopDownMapDataHeader
{
int version;
int samplesPerAxis;
vec2_t bottomLeft;
vec2_t topRight;
}
;

struct TopDownMapDataContent
{
char navMeshVisible[8193];
}
;

struct pathnode_yaworient_t
{
float fLocalAngle;
vec2_t localForward;
}
;

union $A51B38D9D4BC5EEA51923CFD048B9195
{
pathnode_yaworient_t yaw_orient;
vec3_t angles;
}
;

union PathNodeParentUnion
{
scr_string_t name;
unsigned __int16 index;
}
;

enum PathNodeErrorCode
{
PNERR_NONE = 0x0,
PNERR_INSOLID = 0x1,
PNERR_FLOATING = 0x2,
PNERR_NOLINK = 0x3,
PNERR_DUPLICATE = 0x4,
PNERR_NOSTANCE = 0x5,
PNERR_INVALIDDOOR = 0x6,
PNERR_NOANGLES = 0x7,
PNERR_NOPEEKOUT = 0x8,
PNERR_NEARSOLID = 0x9,
PNERR_NOCOVER = 0xA,
NUM_PATH_NODE_ERRORS = 0xB,
}
;

union $96C58BBB7F3ED2CCC5F07A7E763FE6AE
{
float minUseDistSq;
PathNodeErrorCode error;
}
;

struct __declspec(align(2)) tacpoint_ref_t
{
unsigned __int16 m_PointID;
char m_GraphIDPlusOne;
}
;

struct pathnode_constant_t
{
unsigned int spawnflags;
scr_string_t targetname;
scr_string_t script_linkName;
scr_string_t script_noteworthy;
scr_string_t target;
scr_string_t animscript;
int animscriptfunc;
scr_string_t interactable;
vec3_t vLocalOrigin;
PathNodeOrientationUnion orientation;
PathNodeParentUnion parent;
$C8940DDD382E4D47E5830444CF5FFEAD ___u11;
unsigned __int16 wOverlapNode[2];
tacpoint_ref_t tacpointRef;
pathlink_s *Links;
unsigned __int16 totalLinkCount;
unsigned __int16 type;
}
;

struct SentientHandle
{
unsigned __int16 number;
unsigned __int16 infoIndex;
}
;

union $9BFF497B6D2ECBAAFD4D725F640A09A6
{
__int16 turretEntNumber;
unsigned __int16 coverMultiType;
}
;

struct PathnodeDynamicActors
{
int dangerousNodeTime[3];
int inPlayerLOSTime;
}
;

struct PathnodeDynamicBots
{
int recentUseProxTimes[2];
__int16 flags;
char hasGlobalBadPlaceLink;
}
;

union $03EB0ABD1C992236941B49CC250C8734
{
PathnodeDynamicActors actors;
PathnodeDynamicBots bots;
}
;

struct pathnode_dynamic_t
{
SentientHandle pOwners[2];
int iFreeTime[2];
int iValidTime[3];
__int16 wLinkCount;
__int16 turretEntNumber;
unsigned __int16 coverMultiType;
char bInactive;
char wOverlapCount[2];
PathnodeDynamicActors actors;
PathnodeDynamicBots bots;
}
;

union $73F238679C0419BE2C31C6559E8604FC
{
float nodeCost;
int linkIndex;
}
;

struct __declspec(align(8)) pathnode_transient_t
{
int iSearchFrame;
pathnode_t *pNextOpen;
pathnode_t *pPrevOpen;
pathnode_t *pParent;
float fCost;
float fHeuristic;
$73F238679C0419BE2C31C6559E8604FC ___u6;
}
;

struct pathnode_tree_nodes_t
{
int nodeCount;
unsigned __int16 *nodes;
}
;

union pathnode_tree_info_t
{
pathnode_tree_t *child[2];
pathnode_tree_nodes_t s;
}
;

struct SpaceHandle
{
void *m_pProxy;
}
;

struct nav_link_s
{
void *m_pProxy;
void *m_pSpace;
scr_string_t name;
nav_space_s *pSpace;
int id;
nav_link_s *pPrev;
nav_link_s *pNext;
__int16 nextIdx;
__int16 prevIdx;
}
;

struct ObstacleHandle
{
void *m_pProxy;
void *m_pSpace;
}
;

enum ObstacleBehavior
{
OBSTACLE_BEHAVIOR_AUTOMATIC = 0x0,
OBSTACLE_BEHAVIOR_NOEFFECT_PENALTY = 0x1,
OBSTACLE_BEHAVIOR_NOEFFECT_IMPASSABLE = 0x2,
OBSTACLE_BEHAVIOR_PENALTY_IMPASSABLE = 0x3,
NUM_OBSTACLE_BEHAVIORS = 0x4,
}
;

struct ObstacleDat
{
unsigned int m_layerMask;
ObstacleBehavior m_obstacleBehavior;
float m_penaltyMult;
unsigned int m_obstacleBlockageFlags;
unsigned __int64 m_userData;
const char *m_obstacleName;
}
;

struct nav_obstacle_s
{
ObstacleHandle hObstacle;
nav_space_s *pSpace;
unsigned int id;
vec3_t pos;
vec4_t qRot;
ObstacleDat obsData;
vec3_t extents;
nav_boundary_plane_s m_BoundaryPlanes[12];
int m_NumBoundaryPlanes;
bool m_bUsesExtents;
bool m_bGlass;
bool m_bDeleteMe;
bool m_bPending;
nav_obstacle_s *pPrev;
nav_obstacle_s *pNext;
__int16 nextIdx;
__int16 prevIdx;
}
;

struct nav_repulsor_s
{
scr_string_t name;
nav_space_s *pSpace;
int duration;
int entNum;
vec3_t velocity;
vec3_t origin;
float radius;
int usageFlags;
bool bBadplace;
nav_repulsor_s *pPrev;
nav_repulsor_s *pNext;
__int16 nextIdx;
__int16 prevIdx;
}
;

struct nav_link_point_s
{
vec3_t m_Pt1;
vec3_t m_Pt2;
}
;

struct AreaHandle
{
void *m_handleImpl;
void *m_pSpace;
}
;

struct tacpoint_search_node_t
{
vec2_t m_MidPoint;
vec2_t m_HalfSize;
unsigned __int16 m_PointListRoot;
tacpoint_search_node_t **m_ChildNodes;
}
;

union GfxColor
{
unsigned __int8 array[4];
unsigned int packed;
}
;

struct FxSpatialFrame
{
vec4_t quat;
vec3_t origin;
}
;

struct $04E242618DA998C36768FD44FBA55446
{
FxSpatialFrame frame;
float radius;
}
;

struct FxGlassVertex
{
__int16 x;
__int16 y;
}
;

struct FxGlassHoleHeader
{
unsigned __int16 uniqueVertCount;
unsigned __int8 touchVert;
unsigned __int8 pad[1];
}
;

struct FxGlassCrackHeader
{
unsigned __int16 uniqueVertCount;
unsigned __int8 beginVertIndex;
unsigned __int8 endVertIndex;
}
;

struct GfxPortalWritable
{
bool isQueued;
bool isAncestor;
char recursionDepth;
char hullPointCount;
vec2_t *hullPoints;
GfxPortal *queuedParent;
}
;

struct DpvsPlane
{
vec4_t coeffs;
}
;

struct srfTriangles_t
{
unsigned int vertexLayerData;
unsigned int firstVertex;
float maxEdgeLength;
unsigned __int16 vertexCount;
unsigned __int16 triCount;
unsigned int baseIndex;
}
;

struct GfxBaseSurfData
{
vec4_t boundingBoxData;
}
;

struct GfxStaticModelLOD
{
char surfOffset;
char surfCount;
}
;

struct GfxReflectionProbeObb
{
vec3_t center;
vec3_t xAxis;
vec3_t yAxis;
vec3_t zAxis;
vec3_t halfSize;
}
;

struct GfxImageDesc
{
GfxPixelFormat format;
GfxImageFlags flags;
unsigned int totalSize;
unsigned __int16 width;
unsigned __int16 height;
unsigned __int16 depth;
unsigned __int16 numElements;
char levelCount;
}
;

struct GfxImageRaw
{
GfxImageDesc desc;
GfxTextureId textureId;
char *pixels;
}
;

struct GfxLightmapRaw
{
GfxImageRaw textures[3];
}
;

struct GfxSHProbeData
{
unsigned __int16 coeffs[29];
unsigned __int16 pad[3];
}
;

struct GfxDynamicLightsetGChunkInfo
{
unsigned int numRecords;
unsigned int bfStart;
unsigned int orgStart;
unsigned int tmpStart;
}
;

struct GfxOrientedBoundingBox
{
vec3_t center;
vec3_t xAxis;
vec3_t yAxis;
vec3_t zAxis;
vec3_t halfSize;
}
;

enum VolumetricMaskType
{
VOLUMETRIC_MASK_UNUSED = 0x0,
VOLUMETRIC_MASK_HEIGHTMAP = 0x1,
VOLUMETRIC_MASK_OPACITYMAP = 0x2,
VOLUMETRIC_MASK_TYPE_COUNT = 0x3,
}
;

enum VolumetricAxis
{
VOLUMETRIC_X_AXIS = 0x0,
VOLUMETRIC_Y_AXIS = 0x1,
VOLUMETRIC_Z_AXIS = 0x2,
VOLUMETRIC_AXIS_COUNT = 0x3,
VOLUMETRIC_NEGATVE_AXIS_FLAG = 0x4,
}
;

struct GfxVolumetricMask
{
VolumetricMaskType type;
VolumetricAxis axis;
GfxImage *image;
vec2_t scale;
vec2_t offset;
vec2_t scroll;
}
;

struct GfxBrushModelWritable
{
Bounds bounds;
}
;

struct XModelDrawInfo
{
unsigned __int32 hasGfxEntIndex : 1;
unsigned __int32 lod : 4;
unsigned __int32 materialLod : 1;
unsigned __int32 surfBufSize : 24;
unsigned __int32 surfId : 24;
}
;

struct BModelDrawInfo
{
unsigned int surfId;
}
;

enum PlayerAnimScriptTransitionTriggerType
{
ANIM_TRANSITIONTRIGGERTYPE_MOVETYPE = 0x0,
ANIM_TRANSITIONTRIGGERTYPE_MOVETYPE_NOT_STRAFING = 0x1,
ANIM_TRANSITIONTRIGGERTYPE_ANIMCOMPLETE = 0x2,
ANIM_TRANSITIONTRIGGERTYPE_ANIMCHANGE = 0x3,
NUM_ANIM_TRANSITION_TRIGGER_TYPES = 0x4,
}
;

enum PlayerAnimScriptMoveType
{
ANIM_MT_UNUSED = 0x0,
ANIM_MT_IDLE = 0x1,
ANIM_MT_IDLECR = 0x2,
ANIM_MT_IDLEPRONE = 0x3,
ANIM_MT_WALK = 0x4,
ANIM_MT_WALKBK = 0x5,
ANIM_MT_WALKCR = 0x6,
ANIM_MT_WALKCRBK = 0x7,
ANIM_MT_WALKPRONE = 0x8,
ANIM_MT_WALKPRONEBK = 0x9,
ANIM_MT_RUN = 0xA,
ANIM_MT_RUNBK = 0xB,
ANIM_MT_RUNCR = 0xC,
ANIM_MT_RUNCRBK = 0xD,
ANIM_MT_CLIMBUP = 0xE,
ANIM_MT_LADDERSLIDE = 0xF,
ANIM_MT_CLIMBDOWN = 0x10,
ANIM_MT_JOG = 0x11,
ANIM_MT_SPRINT = 0x12,
ANIM_MT_SUPERSPRINT = 0x13,
ANIM_MT_MANTLE = 0x14,
ANIM_MT_IDLELASTSTAND = 0x15,
ANIM_MT_CRAWLLASTSTAND = 0x16,
ANIM_MT_CRAWLLASTSTANDBK = 0x17,
ANIM_MT_SLIDE = 0x18,
ANIM_MT_SLIDEBK = 0x19,
ANIM_MT_MOUNTED_TOP = 0x1A,
ANIM_MT_MOUNTED_LEFT = 0x1B,
ANIM_MT_MOUNTED_RIGHT = 0x1C,
ANIM_MT_BOOSTAIRDODGE = 0x1D,
ANIM_MT_BOOSTAIRDODGELEFT = 0x1E,
ANIM_MT_BOOSTAIRDODGERIGHT = 0x1F,
ANIM_MT_BOOSTAIRDODGEBACK = 0x20,
ANIM_MT_WALLRUN = 0x21,
ANIM_MT_AIR = 0x22,
ANIM_MT_KNOCKBACK = 0x23,
ANIM_MT_KNOCKBACKBK = 0x24,
ANIM_MT_PARACHUTE = 0x25,
NUM_ANIM_CODEMOVETYPES = 0x26,
FIRST_TRANSITION_MOVETYPE = 0x26,
NUM_ANIM_MOVETYPES = 0x80,
}
;

struct PlayerAnimTriggerParam
{
PlayerAnimScriptMoveType moveType;
}
;

enum LbColType
{
LBCOL_TYPE_NUMBER = 0x0,
LBCOL_TYPE_TIME = 0x1,
LBCOL_TYPE_LEVELXP = 0x2,
LBCOL_TYPE_PRESTIGE = 0x3,
LBCOL_TYPE_BIGNUMBER = 0x4,
LBCOL_TYPE_PERCENT = 0x5,
LBCOL_TYPE_TIME_FULL = 0x6,
LBCOL_TYPE_COUNT = 0x7,
}
;

enum LbAggType
{
LBAGG_TYPE_MIN = 0x0,
LBAGG_TYPE_MAX = 0x1,
LBAGG_TYPE_SUM = 0x2,
LBAGG_TYPE_LAST = 0x3,
LBAGG_TYPE_COUNT = 0x4,
}
;

struct DDLHashTable
{
DDLHash *list;
int count;
int max;
}
;

enum CameraProfile
{
CAMERAPROFILE_FIRST_PERSON_VIEW = 0x0,
CAMERAPROFILE_ORBIT = 0x1,
CAMERAPROFILE_SENTRY_DRONE = 0x2,
CAMERAPROFILE_KILLCAM_VEHICLE = 0x3,
CAMERAPROFILE_FIRST_PERSON_VEHICLE = 0x4,
CAMERAPROFILE_MAX = 0x5,
}
;

enum LensProfileMode
{
LENS_PROFILE_NONE = 0x0,
LENS_PROFILE_COMPACT_PORTABLE = 0x1,
LENS_PROFILE_SMARTPHONE = 0x2,
LENS_PROFILE_CINEMATIC_PRIME = 0x3,
LENS_PROFILE_ACTIONCAM_1 = 0x4,
LENS_PROFILE_ACTIONCAM_2 = 0x5,
LENS_PROFILE_DRONECAM = 0x6,
LENS_PROFILE_TELESCOPIC = 0x7,
LENS_PROFILE_COUNT = 0x8,
}
;

struct CameraFirstPersonProfileDef
{
bool applyOffset;
vec3_t offsetTranslation;
bool disableFOVWeaponOffset;
float cameraAnimationTransScale;
float cameraAnimationRotScale;
float handheldCameraTransScale;
float handheldCameraRotScale;
float handheldCameraImpulseTransScale;
float handheldCameraImpulseRotScale;
}
;

struct CameraOrbitProfileDef
{
float lerpSpeed;
vec3_t pivotOffset;
float initialAzimuth;
float initialPolar;
float polarMin;
float polarMax;
float range;
float returnAzimuthSpeed;
float returnPolarSpeed;
float easeInFactor;
float returnTimeTo;
float turningSpeedPolar;
float turningSpeedAzimuth;
float traceCollRadius;
float mouseInputMultiplier;
float rollContrib;
float pitchContrib;
int buttonToggleReturn;
int buttonCyclePerspective;
int buttonRecenter;
bool initialReturn;
bool returnAzimuth;
bool returnPolar;
float alignToMovement;
bool interpolateFocus;
bool cameraReturnOnGas;
float cameraReturnOnSpeed;
float yawTurnAdjust;
}
;

struct CameraSentryDroneDef
{
float range;
float thresHitToSpeedUp;
float speedUpFactor;
float lerpSpeedEye;
float lerpSpeedFocus;
float lerpSpeedUp;
float focusTraceLen;
float eyeHeight;
float horizontalOffset;
float upOffsetFactorIncl;
}
;

struct CameraKillCamVehicleDef
{
float lookAtRadius;
float fov;
float distBack;
float distUp;
float distBackRadiusMod;
float distUpRadiusMod;
float fovRadiusMod;
float sideMag;
float traceCollRad;
}
;

enum CameraTransitionCurve
{
CAMTCURVE_NONE = 0x0,
CAMTCURVE_LINEAR = 0x1,
CAMTCURVE_SMOOTHSTEP = 0x2,
CAMTCURVE_SMOOTHERSTEP = 0x3,
}
;

struct CameraTransitionDef
{
bool active;
float time;
float timewaitEyepos;
float timewaitAxis;
CameraTransitionCurve curveEyepos;
CameraTransitionCurve curveAxis;
}
;

struct VehiclePhysicsDef
{
int physicsEnabled;
float unk_1;
float unk_2;
int unk_3;
int unk_4;
int unk_5;
float unk_6;
float unk_7;
float unk_8;
float unk_9;
float unk_10;
float unk_11;
float unk_12;
float unk_13;
float unk_14;
float unk_15;
float unk_16;
float unk_17;
float unk_18;
float unk_19;
float unk_20;
float unk_21;
float unk_22;
float unk_23;
float unk_24;
float unk_25;
float unk_26;
float unk_27;
float unk_28;
float unk_29;
float unk_30;
float unk_31;
float unk_32;
float unk_33;
float unk_34;
float unk_35;
float unk_36;
float unk_37;
float unk_38;
float unk_39;
float unk_40;
float unk_41;
float unk_42;
float unk_43;
float unk_44;
float unk_45;
float unk_46;
float unk_47;
float unk_48;
float unk_49;
int unk_50;
float unk_51;
float unk_52;
float unk_53;
float unk_54;
float unk_55;
float unk_56;
float unk_57;
float unk_58;
float unk_59;
float unk_60;
float unk_61;
float unk_62;
float unk_63;
float unk_64;
float unk_65;
float unk_66;
float unk_67;
float unk_68;
float unk_69;
float unk_70;
float unk_71;
float unk_72;
float unk_73;
float unk_74;
float unk_75;
float unk_76;
float unk_77;
float unk_78;
float unk_79;
float unk_80;
float unk_81;
float unk_82;
float unk_83;
float unk_84;
float unk_85;
float unk_86;
float unk_87;
float unk_88;
float unk_89;
float unk_90;
float unk_91;
float unk_92;
float unk_93;
float unk_94;
float unk_95;
float unk_96;
float unk_97;
float unk_98;
float unk_99;
float unk_100;
float unk_101;
float unk_102;
float unk_103;
float unk_104;
float unk_105;
float unk_106;
float unk_107;
float unk_108;
float unk_109;
float unk_110;
float unk_111;
}
;

enum NetConstStringType
{
NETCONSTSTRINGTYPE_XMODEL = 0x0,
NETCONSTSTRINGTYPE_MATERIAL = 0x1,
NETCONSTSTRINGTYPE_RUMBLE = 0x2,
NETCONSTSTRINGTYPE_RUMBLE_GRAPH = 0x3,
NETCONSTSTRINGTYPE_VEHICLES = 0x4,
NETCONSTSTRINGTYPE_FX = 0x5,
NETCONSTSTRINGTYPE_VFX = 0x6,
NETCONSTSTRINGTYPE_LOCSTRING = 0x7,
NETCONSTSTRINGTYPE_SHOCK = 0x8,
NETCONSTSTRINGTYPE_CLIENT_TAGS = 0x9,
NETCONSTSTRINGTYPE_HEADICON = 0xA,
NETCONSTSTRINGTYPE_STATUSICON = 0xB,
NETCONSTSTRINGTYPE_NAMEPLATE = 0xC,
NETCONSTSTRINGTYPE_MINIMAPICON = 0xD,
NETCONSTSTRINGTYPE_WEAPON = 0xE,
NETCONSTSTRINGTYPE_HINTSTRING = 0xF,
NETCONSTSTRINGTYPE_ANIM = 0x10,
NETCONSTSTRINGTYPE_ANIMCLASS = 0x11,
NETCONSTSTRINGTYPE_LUI = 0x12,
NETCONSTSTRINGTYPE_SUIT = 0x13,
NETCONSTSTRINGTYPE_GESTURE = 0x14,
NETCONSTSTRINGTYPE_TGT = 0x15,
NETCONSTSTRINGTYPE_VISION = 0x16,
NETCONSTSTRINGTYPE_COUNT = 0x19,
NETCONSTSTRINGTYPE_NONE = 0x19,
}
;

enum NetConstStringSource
{
NETCONSTSTRINGSOURCE_MAP = 0x0,
NETCONSTSTRINGSOURCE_PRE_MAP = 0x1,
NETCONSTSTRINGSOURCE_COMMON = 0x2,
NETCONSTSTRINGSOURCE_COUNT = 0x3,
NETCONSTSTRINGSOURCE_NONE = 0x3,
}
;

struct EquipSoundSetFootstepActionType
{
EquipmentSoundSet left[11];
EquipmentSoundSet right[11];
}
;

struct EquipSoundSetMeleeWorld
{
EquipmentSoundSet soundSets[8][3][2][4];
}
;

struct AlwaysloadedFlags
{
unsigned int imageCount;
GfxImage **images;
unsigned int *imageFlags;
unsigned int stringCount;
const char **strings;
unsigned int *stringFlags;
unsigned int materialCount;
Material **materials;
unsigned int *materialFlags;
}
;

struct TransientSets
{
unsigned int count;
const char **names;
unsigned int *masks;
}
;

struct StreamingContentSet
{
unsigned __int16 *contentCounts;
StreamingItemContents *contents;
}
;

struct TransientCosts
{
StreamingContentSet streamContents[3];
StreamingItemCost *streamingCosts;
unsigned __int16 *residentPageSizes;
unsigned __int16 *tempPageSizes;
unsigned int streamingItemCount;
unsigned int transientCostCount;
}
;

struct AlwaysloadedFlagSet
{
AlwaysloadedFlags globalFlags;
unsigned int modelCount;
AlwaysloadedFlagsPerModel *perModelFlags;
unsigned int modelSurfCount;
XModelSurfs **modelSurfs;
}
;

enum gestureAnimType_t
{
GESTUREANIMTYPE_NONE = 0x0,
GESTUREANIMTYPE_SLIDE = 0x1,
GESTUREANIMTYPE_SLIDE_AKIMBO = 0x2,
GESTUREANIMTYPE_MANTLE_ON_F_LOW = 0x3,
GESTUREANIMTYPE_MANTLE_ON_F_MIDDLE = 0x4,
GESTUREANIMTYPE_MANTLE_ON_F_HIGH = 0x5,
GESTUREANIMTYPE_MANTLE_ON_F_EXTREME = 0x6,
GESTUREANIMTYPE_MANTLE_ON_B_LOW = 0x7,
GESTUREANIMTYPE_MANTLE_ON_B_MIDDLE = 0x8,
GESTUREANIMTYPE_MANTLE_ON_B_HIGH = 0x9,
GESTUREANIMTYPE_MANTLE_ON_B_EXTREME = 0xA,
GESTUREANIMTYPE_MANTLE_ON_L_LOW = 0xB,
GESTUREANIMTYPE_MANTLE_ON_L_MIDDLE = 0xC,
GESTUREANIMTYPE_MANTLE_ON_L_HIGH = 0xD,
GESTUREANIMTYPE_MANTLE_ON_L_EXTREME = 0xE,
GESTUREANIMTYPE_MANTLE_ON_R_LOW = 0xF,
GESTUREANIMTYPE_MANTLE_ON_R_MIDDLE = 0x10,
GESTUREANIMTYPE_MANTLE_ON_R_HIGH = 0x11,
GESTUREANIMTYPE_MANTLE_ON_R_EXTREME = 0x12,
GESTUREANIMTYPE_MANTLE_OVER_F_LOW = 0x13,
GESTUREANIMTYPE_MANTLE_OVER_F_MIDDLE = 0x14,
GESTUREANIMTYPE_MANTLE_OVER_F_HIGH = 0x15,
GESTUREANIMTYPE_MANTLE_OVER_F_EXTREME = 0x16,
GESTUREANIMTYPE_MANTLE_OVER_B_LOW = 0x17,
GESTUREANIMTYPE_MANTLE_OVER_B_MIDDLE = 0x18,
GESTUREANIMTYPE_MANTLE_OVER_B_HIGH = 0x19,
GESTUREANIMTYPE_MANTLE_OVER_B_EXTREME = 0x1A,
GESTUREANIMTYPE_MANTLE_OVER_L_LOW = 0x1B,
GESTUREANIMTYPE_MANTLE_OVER_L_MIDDLE = 0x1C,
GESTUREANIMTYPE_MANTLE_OVER_L_HIGH = 0x1D,
GESTUREANIMTYPE_MANTLE_OVER_L_EXTREME = 0x1E,
GESTUREANIMTYPE_MANTLE_OVER_R_LOW = 0x1F,
GESTUREANIMTYPE_MANTLE_OVER_R_MIDDLE = 0x20,
GESTUREANIMTYPE_MANTLE_OVER_R_HIGH = 0x21,
GESTUREANIMTYPE_MANTLE_OVER_R_EXTREME = 0x22,
GESTUREANIMTYPE_MANTLE_LADDER_LOW = 0x23,
GESTUREANIMTYPE_MANTLE_LADDER_MIDDLE = 0x24,
GESTUREANIMTYPE_MANTLE_LADDER_HIGH = 0x25,
GESTUREANIMTYPE_WALLRUN_LEFT = 0x26,
GESTUREANIMTYPE_WALLRUN_RIGHT = 0x27,
GESTUREANIMTYPE_WALLRUN_LEFT_AKIMBO = 0x28,
GESTUREANIMTYPE_WALLRUN_RIGHT_AKIMBO = 0x29,
GESTUREANIMTYPE_MOUNT_REACH = 0x2A,
GESTUREANIMTYPE_MOUNT_TOP = 0x2B,
GESTUREANIMTYPE_MOUNT_LEFT = 0x2C,
GESTUREANIMTYPE_MOUNT_RIGHT = 0x2D,
GESTUREANIMTYPE_GROUND_POUND = 0x2E,
GESTUREANIMTYPE_NVG_ON = 0x2F,
GESTUREANIMTYPE_NVG_OFF = 0x30,
GESTUREANIMTYPE_NUM = 0x36,
}
;

typedef EnergyType CinematicShakeModifierType
;

struct CinematicShakeModifierDef
{
CinematicShakeModifierType type;
int inBlendInTimeMs;
int inBlendOutTimeMs;
vec2_t inSpeedRange;
vec2_t outAmplitudeScale;
vec2_t outPersistenceScale;
vec2_t outSpeedScale;
int outBlendInTimeMs;
int outBlendOutTimeMs;
RumbleGraph *nonLinearMap;
}
;

typedef EnergyType CinematicShakeModifierCombineType
;

struct CinematicShakeDef
{
bool enabled;
vec3_t frequency;
vec3_t amplitude;
vec3_t persistence;
CinematicShakeModifierDef moveSpeedModifiers;
CinematicShakeModifierDef viewSpeedModifiers;
CinematicShakeModifierCombineType modifierCombineType;
}
;

struct CinematicImpactShakeDef
{
vec3_t frequency;
vec3_t amplitude;
vec3_t persistence;
}
;

union ASM_Function_Param_Value
{
bool m_Bool;
int m_Int;
float m_Float;
scr_string_t m_String;
}
;

enum ASM_Function_Param_Type
{
ParamType_Undefined = 0x0,
ParamType_Bool = 0x1,
ParamType_String = 0x2,
ParamType_Int = 0x3,
ParamType_Float = 0x4,
}
;

struct ASM_Function
{
int m_FuncID;
int m_NumParams;
ASM_Function_Param *m_Params;
bool m_bBuiltin;
bool m_bNegate;
char m_Flags;
unsigned __int16 m_Hash;
char padding[8];
}
;

union ASM_State_Union
{
AI_ASM_State *m_AIState;
Player_ASM_State *m_PlayerState;
}
;

enum ASM_Mode
{
ASM_MODE_AI = 0x0,
ASM_MODE_PLAYER = 0x1,
ASM_MODE_COUNT = 0x2,
}
;

union ASM_Union
{
AI_ASM *m_AIASM;
Player_ASM *m_PlayerASM;
}
;

struct PlayerAnimsetAliasPatch
{
bool isPatchable;
scr_string_t *strings;
unsigned int numStrings;
char padding[8];
}
;

union AnimsetAlias_Union
{
AIAnimsetAlias *m_AIAnimsetAlias;
PlayerAnimsetAlias *m_PlayerAnimsetAlias;
}
;

struct scr_animtree_t
{
XAnim_s *anims;
}
;

struct PlayerASM_ConditionMask
{
bool invalidMask;
bool usesNotetrackEvents;
unsigned int animConditions[8];
unsigned int animEvents[8];
char padding[12];
}
;

union AnimsetState_Union
{
AIAnimsetState *m_AIAnimsetState;
PlayerAnimsetState *m_PlayerAnimsetState;
}
;

struct AIAnimsetNodeOffset
{
int hideYaw;
int snapYaw;
int leanYaw;
int leanPitch;
}
;

union Animset_Union
{
AIAnimset *m_AIAnimset;
PlayerAnimset *m_PlayerAnimset;
}
;

enum SuitBodyAnimType
{
SUITBODYANIMTYPE_HUMAN = 0x0,
SUITBODYANIMTYPE_JUGGERNAUT = 0x1,
SUITBODYANIMTYPE_COUNT = 0x2,
}
;

struct BehaviorTreeActionData
{
unsigned __int16 initFn;
unsigned __int16 processFn;
unsigned __int16 terminateFn;
unsigned __int16 processArgsFn;
bool bProcessBuiltin;
bool bInitBuiltin;
bool bTerminateBuiltin;
}
;

struct BehaviorTreeNodeChildren
{
unsigned __int16 firstChildOffset;
unsigned __int16 childCount;
}
;

union BehaviorTreeNodeDataUnion
{
BehaviorTreeActionData actionData;
BehaviorTreeNodeChildren children;
}
;

struct BehaviorTreeDecoratorChildren
{
unsigned __int16 firstDecorator;
unsigned __int16 decoratorCount;
}
;

struct BehaviorTreeDecoratorRootInit
{
unsigned __int16 initFn;
bool bInitBuiltin;
}
;

struct BehaviorTreeDecoratorTerminate
{
unsigned __int16 terminateFn;
bool bTerminateBuiltin;
}
;

struct BehaviorTreeDecoratorInit
{
unsigned __int16 initFn;
bool bInitBuiltin;
}
;

union BehaviorTreeDecoratorUnion
{
BehaviorTreeDecoratorRootInit rootInit;
BehaviorTreeDecoratorTerminate terminate;
BehaviorTreeDecoratorInit init;
}
;

union $EE658B4838C130B7503228AEB3560558
{
const char *jsonblob;
int *root;
}
;

enum XCamApertureType
{
XCAM_FOV_APERTURE = 0x0,
XCAM_FOCAL_LENGTH_APERTURE = 0x1,
COUNT_XCAM_APERTURE = 0x2,
}
;

struct StVertexData0
{
char *data;
ID3D12Resource *vb;
}
;

struct StMeshLayerMaskData
{
char *data;
ID3D12Resource *buffer;
}
;

struct $577681CFABA6E7EDBCF2FD37C745124F
{
__int8 clippedOut : 1;
__int8 renderable : 1;
__int8 gpuGenerate : 1;
__int8 vertPos3d : 1;
__int8 layerMask : 1;
}
;

union StMeshFlags
{
char packed;
$577681CFABA6E7EDBCF2FD37C745124F __s1;
}
;

struct StMesh
{
StVertexData0 vd0;
StMeshLayerMaskData layerMask;
ID3D12Resource *indexBuffer;
unsigned __int16 *indices;
StreamKey *streamKey;
ID3D12Resource *vertexBufferToRelease;
ID3D12Resource *indexBufferToRelease;
ID3D12Resource *layerMaskBufferToRelease;
unsigned __int16 vertexCount;
unsigned __int16 triCount;
unsigned __int16 cutoutTriCount;
StMeshFlags flags;
}
;

struct StTerrainGrid
{
unsigned int numLevels;
StTerrainGridLevel *levels;
}
;

struct StClutterSampleBitmask
{
void *streamKey;
unsigned int wordCount;
unsigned int *bitmask;
GfxWrappedBuffer bitmaskBuffer;
GfxWrappedBuffer relocatedBitmaskBuffer;
}
;

struct StClutterSamplePoints
{
unsigned int sampleCount;
vec2_t *samplePoints;
GfxWrappedBuffer samplePointBuffer;
}
;

union $F893CBF51C9DD18F024244744E0DDE67
{
char scriptableMaxPartCount;
char physicsAssetRigidBodiesCount;
}
;

struct DLogHashTable
{
DLogHash *list;
int count;
int max;
}
;

struct DLogSampleGroup
{
char type[1];
float rate;
const char *name;
}
;

enum DLogError
{
DLOG_ERROR_NONE = 0x0,
DLOG_ERROR_OUT_OF_MEMORY = 0x1,
DLOG_ERROR_UNEXPECTED_END = 0x2,
DLOG_ERROR_MEMBER_NOT_FOUND = 0x3,
DLOG_ERROR_TYPE_MISMATCH = 0x4,
DLOG_ERROR_ARRAY_TRUNCATED = 0x5,
DLOG_ERROR_UNEXPECTED_FIELD_NAME = 0x6,
DLOG_ERROR_TIME_NOT_SYNCED = 0x7,
DLOG_ERROR_UNFINISHED_EVENT = 0x8,
DLOG_ERROR_UNFINISHED_ROW = 0x9,
DLOG_ERROR_UNEXPECTED = 0xA,
DLOG_ERROR_SCHEMA_NOT_FOUND = 0xB,
DLOG_ERROR_SCHEMA_NOT_AVAILABLE = 0xC,
DLOG_ERROR_NOT_ACTIVE = 0xD,
}
;

enum DLogType
{
DLOG_TYPE_INVALID = 0x0,
DLOG_TYPE_OBJECT = 0x1,
DLOG_TYPE_ARRAY = 0x2,
DLOG_TYPE_NULL = 0x3,
DLOG_TYPE_BOOL = 0x4,
DLOG_TYPE_FLOAT32 = 0x5,
DLOG_TYPE_FLOAT64 = 0x6,
DLOG_TYPE_INT8 = 0x7,
DLOG_TYPE_INT16 = 0x8,
DLOG_TYPE_INT32 = 0x9,
DLOG_TYPE_INT64 = 0xA,
DLOG_TYPE_UINT8 = 0xB,
DLOG_TYPE_UINT16 = 0xC,
DLOG_TYPE_UINT32 = 0xD,
DLOG_TYPE_UINT64 = 0xE,
DLOG_TYPE_STRING = 0xF,
DLOG_TYPEcharS = 0x10,
DLOG_TYPE_COUNT = 0x11,
}
;

struct $5A147756DA7E26A533E5CEAFC83E9B25
{
unsigned int Header;
unsigned int ContextHash;
unsigned int Data2;
unsigned int Data3;
unsigned int Data4;
unsigned int Data5;
}
;

union $38B5A486A67173B5848B60FEFF92A07D
{
unsigned int Data[6];
$5A147756DA7E26A533E5CEAFC83E9B25 __s1;
}
;

struct D3D12XBOX_DESCRIPTOR_PIPELINE_STATE
{
$38B5A486A67173B5848B60FEFF92A07D ___u0;
}
;

union __m128i
{
char m128i_i8[16];
__int16 m128i_i16[8];
int m128i_i32[4];
__int64 m128i_i64[2];
char m128i_u8[16];
unsigned __int16 m128i_u16[8];
unsigned int m128i_u32[4];
unsigned __int64 m128i_u64[2];
}
;

struct $E903C2A7C2E773B40766EDD788E6C3F6
{
unsigned int x;
unsigned int y;
}
;

struct $53338196DE2D448073C64CFEFFCF22DC
{
vec3_t row0;
vec3_t row1;
vec3_t row2;
}
;

struct XAsset
{
XAssetType type;
XAssetHeader header;
}
;

struct ScriptCodePos
{
char *m_scriptPos;
unsigned __int64 m_genericPos;
}
;

struct function_stack_t
{
ScriptCodePos pos;
unsigned int localId;
unsigned int localVarCount;
void *top;
void *startTop;
}
;

struct function_frame_t
{
function_stack_t fs;
int topType;
}
;

struct scrContext_t
{
char __pad0[3184];
bool script_loading;
char __pad1[3];
unsigned int m_funcBegin;
unsigned int m_funcEnd;
unsigned int m_funcCount;
unsigned int m_methBegin;
unsigned int m_methEnd;
unsigned int m_methCount;
char __pad2[4];
void *m_pFuncTable;
void *m_pMethTable;
char __pad3[10248];
void *objectVariableValue;
void *objectVariableChildren;
void *unk_0;
void *childVariableValue;
function_stack_t m_fs;
char __pad3_1[34984];
unsigned int *localVars;
void *maxstack;
int function_count;
int __pad4;
void *function_frame;
void *top;
int __pad5;
unsigned int inparamcount;
unsigned int outparamcount;
unsigned int breakpointOutparamcount;
function_frame_t function_frame_start[64];
char __pad6[35376];
ScriptCodePos pos;
}
;

struct ID3D11Device
{
ID3D11DeviceVtbl *lpVtbl;
}
;

struct ID3D11DeviceVtbl
{
HRESULT (__stdcall *QueryInterface)(ID3D11Device *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ID3D11Device *This);
ULONG (__stdcall *Release)(ID3D11Device *This);
HRESULT (__stdcall *CreateBuffer)(ID3D11Device *This, const D3D11_BUFFER_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Buffer **ppBuffer);
HRESULT (__stdcall *CreateTexture1D)(ID3D11Device *This, const D3D11_TEXTURE1D_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Texture1D **ppTexture1D);
HRESULT (__stdcall *CreateTexture2D)(ID3D11Device *This, const D3D11_TEXTURE2D_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Texture2D **ppTexture2D);
HRESULT (__stdcall *CreateTexture3D)(ID3D11Device *This, const D3D11_TEXTURE3D_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Texture3D **ppTexture3D);
HRESULT (__stdcall *CreateShaderResourceView)(ID3D11Device *This, ID3D11Resource *pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc, ID3D11ShaderResourceView **ppSRView);
HRESULT (__stdcall *CreateUnorderedAccessView)(ID3D11Device *This, ID3D11Resource *pResource, const D3D11_UNORDERED_ACCESS_VIEW_DESC *pDesc, ID3D11UnorderedAccessView **ppUAView);
HRESULT (__stdcall *CreateRenderTargetView)(ID3D11Device *This, ID3D11Resource *pResource, const D3D11_RENDER_TARGET_VIEW_DESC *pDesc, ID3D11RenderTargetView **ppRTView);
HRESULT (__stdcall *CreateDepthStencilView)(ID3D11Device *This, ID3D11Resource *pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc, ID3D11DepthStencilView **ppDepthStencilView);
HRESULT (__stdcall *CreateInputLayout)(ID3D11Device *This, const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs, UINT NumElements, const void *pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength, ID3D11InputLayout **ppInputLayout);
HRESULT (__stdcall *CreateVertexShader)(ID3D11Device *This, const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11VertexShader **ppVertexShader);
HRESULT (__stdcall *CreateGeometryShader)(ID3D11Device *This, const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11GeometryShader **ppGeometryShader);
HRESULT (__stdcall *CreateGeometryShaderWithStreamOutput)(ID3D11Device *This, const void *pShaderBytecode, SIZE_T BytecodeLength, const D3D11_SO_DECLARATION_ENTRY *pSODeclaration, UINT NumEntries, const UINT *pBufferStrides, UINT NumStrides, UINT RasterizedStream, ID3D11ClassLinkage *pClassLinkage, ID3D11GeometryShader **ppGeometryShader);
HRESULT (__stdcall *CreatePixelShader)(ID3D11Device *This, const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11PixelShader **ppPixelShader);
HRESULT (__stdcall *CreateHullShader)(ID3D11Device *This, const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11HullShader **ppHullShader);
HRESULT (__stdcall *CreateDomainShader)(ID3D11Device *This, const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11DomainShader **ppDomainShader);
HRESULT (__stdcall *CreateComputeShader)(ID3D11Device *This, const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11ComputeShader **ppComputeShader);
HRESULT (__stdcall *CreateClassLinkage)(ID3D11Device *This, ID3D11ClassLinkage **ppLinkage);
HRESULT (__stdcall *CreateBlendState)(ID3D11Device *This, const D3D11_BLEND_DESC *pBlendStateDesc, ID3D11BlendState **ppBlendState);
HRESULT (__stdcall *CreateDepthStencilState)(ID3D11Device *This, const D3D11_DEPTH_STENCIL_DESC *pDepthStencilDesc, ID3D11DepthStencilState **ppDepthStencilState);
HRESULT (__stdcall *CreateRasterizerState)(ID3D11Device *This, const D3D11_RASTERIZER_DESC *pRasterizerDesc, ID3D11RasterizerState **ppRasterizerState);
HRESULT (__stdcall *CreateSamplerState)(ID3D11Device *This, const D3D11_SAMPLER_DESC *pSamplerDesc, ID3D11SamplerState **ppSamplerState);
HRESULT (__stdcall *CreateQuery)(ID3D11Device *This, const D3D11_QUERY_DESC *pQueryDesc, ID3D11Query **ppQuery);
HRESULT (__stdcall *CreatePredicate)(ID3D11Device *This, const D3D11_QUERY_DESC *pPredicateDesc, ID3D11Predicate **ppPredicate);
HRESULT (__stdcall *CreateCounter)(ID3D11Device *This, const D3D11_COUNTER_DESC *pCounterDesc, ID3D11Counter **ppCounter);
HRESULT (__stdcall *CreateDeferredContext)(ID3D11Device *This, UINT ContextFlags, ID3D11DeviceContext **ppDeferredContext);
HRESULT (__stdcall *OpenSharedResource)(ID3D11Device *This, HANDLE hResource, const IID *const ReturnedInterface, void **ppResource);
HRESULT (__stdcall *CheckFormatSupport)(ID3D11Device *This, DXGI_FORMAT Format, UINT *pFormatSupport);
HRESULT (__stdcall *CheckMultisampleQualityLevels)(ID3D11Device *This, DXGI_FORMAT Format, UINT SampleCount, UINT *pNumQualityLevels);
void (__stdcall *CheckCounterInfo)(ID3D11Device *This, D3D11_COUNTER_INFO *pCounterInfo);
HRESULT (__stdcall *CheckCounter)(ID3D11Device *This, const D3D11_COUNTER_DESC *pDesc, D3D11_COUNTER_TYPE *pType, UINT *pActiveCounters, LPSTR szName, UINT *pNameLength, LPSTR szUnits, UINT *pUnitsLength, LPSTR szDescription, UINT *pDescriptionLength);
HRESULT (__stdcall *CheckFeatureSupport)(ID3D11Device *This, D3D11_FEATURE Feature, void *pFeatureSupportData, UINT FeatureSupportDataSize);
HRESULT (__stdcall *GetPrivateData)(ID3D11Device *This, const GUID *const guid, UINT *pDataSize, void *pData);
HRESULT (__stdcall *SetPrivateData)(ID3D11Device *This, const GUID *const guid, UINT DataSize, const void *pData);
HRESULT (__stdcall *SetPrivateDataInterface)(ID3D11Device *This, const GUID *const guid, const IUnknown *pData);
D3D_FEATURE_LEVEL (__stdcall *GetFeatureLevel)(ID3D11Device *This);
UINT (__stdcall *GetCreationFlags)(ID3D11Device *This);
HRESULT (__stdcall *GetDeviceRemovedReason)(ID3D11Device *This);
void (__stdcall *GetImmediateContext)(ID3D11Device *This, ID3D11DeviceContext **ppImmediateContext);
HRESULT (__stdcall *SetExceptionMode)(ID3D11Device *This, UINT RaiseFlags);
UINT (__stdcall *GetExceptionMode)(ID3D11Device *This);
}
;

struct D3D11_BUFFER_DESC
{
UINT ByteWidth;
D3D11_USAGE Usage;
UINT BindFlags;
UINT CPUAccessFlags;
UINT MiscFlags;
UINT StructureByteStride;
}
;

enum D3D11_USAGE
{
D3D11_USAGE_DEFAULT = 0x0,
D3D11_USAGE_IMMUTABLE = 0x1,
D3D11_USAGE_DYNAMIC = 0x2,
D3D11_USAGE_STAGING = 0x3,
}
;

struct D3D11_SUBRESOURCE_DATA
{
const void *pSysMem;
UINT SysMemPitch;
UINT SysMemSlicePitch;
}
;

typedef void ID3D11Buffer
;

struct ID3D11BufferVtbl
{
HRESULT (__stdcall *QueryInterface)(ID3D11Buffer *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ID3D11Buffer *This);
ULONG (__stdcall *Release)(ID3D11Buffer *This);
void (__stdcall *GetDevice)(ID3D11Buffer *This, ID3D11Device **ppDevice);
HRESULT (__stdcall *GetPrivateData)(ID3D11Buffer *This, const GUID *const guid, UINT *pDataSize, void *pData);
HRESULT (__stdcall *SetPrivateData)(ID3D11Buffer *This, const GUID *const guid, UINT DataSize, const void *pData);
HRESULT (__stdcall *SetPrivateDataInterface)(ID3D11Buffer *This, const GUID *const guid, const IUnknown *pData);
void (__stdcall *GetType)(ID3D11Buffer *This, D3D11_RESOURCE_DIMENSION *pResourceDimension);
void (__stdcall *SetEvictionPriority)(ID3D11Buffer *This, UINT EvictionPriority);
UINT (__stdcall *GetEvictionPriority)(ID3D11Buffer *This);
void (__stdcall *GetDesc)(ID3D11Buffer *This, D3D11_BUFFER_DESC *pDesc);
}
;

enum D3D11_RESOURCE_DIMENSION
{
D3D11_RESOURCE_DIMENSION_UNKNOWN = 0x0,
D3D11_RESOURCE_DIMENSION_BUFFER = 0x1,
D3D11_RESOURCE_DIMENSION_TEXTURE1D = 0x2,
D3D11_RESOURCE_DIMENSION_TEXTURE2D = 0x3,
D3D11_RESOURCE_DIMENSION_TEXTURE3D = 0x4,
}
;

struct D3D11_TEXTURE1D_DESC
{
UINT Width;
UINT MipLevels;
UINT ArraySize;
DXGI_FORMAT Format;
D3D11_USAGE Usage;
UINT BindFlags;
UINT CPUAccessFlags;
UINT MiscFlags;
}
;

enum DXGI_FORMAT
{
DXGI_FORMAT_UNKNOWN = 0x0,
DXGI_FORMAT_R32G32B32A32_TYPELESS = 0x1,
DXGI_FORMAT_R32G32B32A32_FLOAT = 0x2,
DXGI_FORMAT_R32G32B32A32_UINT = 0x3,
DXGI_FORMAT_R32G32B32A32_SINT = 0x4,
DXGI_FORMAT_R32G32B32_TYPELESS = 0x5,
DXGI_FORMAT_R32G32B32_FLOAT = 0x6,
DXGI_FORMAT_R32G32B32_UINT = 0x7,
DXGI_FORMAT_R32G32B32_SINT = 0x8,
DXGI_FORMAT_R16G16B16A16_TYPELESS = 0x9,
DXGI_FORMAT_R16G16B16A16_FLOAT = 0xA,
DXGI_FORMAT_R16G16B16A16_UNORM = 0xB,
DXGI_FORMAT_R16G16B16A16_UINT = 0xC,
DXGI_FORMAT_R16G16B16A16_SNORM = 0xD,
DXGI_FORMAT_R16G16B16A16_SINT = 0xE,
DXGI_FORMAT_R32G32_TYPELESS = 0xF,
DXGI_FORMAT_R32G32_FLOAT = 0x10,
DXGI_FORMAT_R32G32_UINT = 0x11,
DXGI_FORMAT_R32G32_SINT = 0x12,
DXGI_FORMAT_R32G8X24_TYPELESS = 0x13,
DXGI_FORMAT_D32_FLOAT_S8X24_UINT = 0x14,
DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS = 0x15,
DXGI_FORMAT_X32_TYPELESS_G8X24_UINT = 0x16,
DXGI_FORMAT_R10G10B10A2_TYPELESS = 0x17,
DXGI_FORMAT_R10G10B10A2_UNORM = 0x18,
DXGI_FORMAT_R10G10B10A2_UINT = 0x19,
DXGI_FORMAT_R11G11B10_FLOAT = 0x1A,
DXGI_FORMAT_R8G8B8A8_TYPELESS = 0x1B,
DXGI_FORMAT_R8G8B8A8_UNORM = 0x1C,
DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 0x1D,
DXGI_FORMAT_R8G8B8A8_UINT = 0x1E,
DXGI_FORMAT_R8G8B8A8_SNORM = 0x1F,
DXGI_FORMAT_R8G8B8A8_SINT = 0x20,
DXGI_FORMAT_R16G16_TYPELESS = 0x21,
DXGI_FORMAT_R16G16_FLOAT = 0x22,
DXGI_FORMAT_R16G16_UNORM = 0x23,
DXGI_FORMAT_R16G16_UINT = 0x24,
DXGI_FORMAT_R16G16_SNORM = 0x25,
DXGI_FORMAT_R16G16_SINT = 0x26,
DXGI_FORMAT_R32_TYPELESS = 0x27,
DXGI_FORMAT_D32_FLOAT = 0x28,
DXGI_FORMAT_R32_FLOAT = 0x29,
DXGI_FORMAT_R32_UINT = 0x2A,
DXGI_FORMAT_R32_SINT = 0x2B,
DXGI_FORMAT_R24G8_TYPELESS = 0x2C,
DXGI_FORMAT_D24_UNORM_S8_UINT = 0x2D,
DXGI_FORMAT_R24_UNORM_X8_TYPELESS = 0x2E,
DXGI_FORMAT_X24_TYPELESS_G8_UINT = 0x2F,
DXGI_FORMAT_R8G8_TYPELESS = 0x30,
DXGI_FORMAT_R8G8_UNORM = 0x31,
DXGI_FORMAT_R8G8_UINT = 0x32,
DXGI_FORMAT_R8G8_SNORM = 0x33,
DXGI_FORMAT_R8G8_SINT = 0x34,
DXGI_FORMAT_R16_TYPELESS = 0x35,
DXGI_FORMAT_R16_FLOAT = 0x36,
DXGI_FORMAT_D16_UNORM = 0x37,
DXGI_FORMAT_R16_UNORM = 0x38,
DXGI_FORMAT_R16_UINT = 0x39,
DXGI_FORMAT_R16_SNORM = 0x3A,
DXGI_FORMAT_R16_SINT = 0x3B,
DXGI_FORMAT_R8_TYPELESS = 0x3C,
DXGI_FORMAT_R8_UNORM = 0x3D,
DXGI_FORMAT_R8_UINT = 0x3E,
DXGI_FORMAT_R8_SNORM = 0x3F,
DXGI_FORMAT_R8_SINT = 0x40,
DXGI_FORMAT_A8_UNORM = 0x41,
DXGI_FORMAT_R1_UNORM = 0x42,
DXGI_FORMAT_R9G9B9E5_SHAREDEXP = 0x43,
DXGI_FORMAT_R8G8_B8G8_UNORM = 0x44,
DXGI_FORMAT_G8R8_G8B8_UNORM = 0x45,
DXGI_FORMAT_BC1_TYPELESS = 0x46,
DXGI_FORMAT_BC1_UNORM = 0x47,
DXGI_FORMAT_BC1_UNORM_SRGB = 0x48,
DXGI_FORMAT_BC2_TYPELESS = 0x49,
DXGI_FORMAT_BC2_UNORM = 0x4A,
DXGI_FORMAT_BC2_UNORM_SRGB = 0x4B,
DXGI_FORMAT_BC3_TYPELESS = 0x4C,
DXGI_FORMAT_BC3_UNORM = 0x4D,
DXGI_FORMAT_BC3_UNORM_SRGB = 0x4E,
DXGI_FORMAT_BC4_TYPELESS = 0x4F,
DXGI_FORMAT_BC4_UNORM = 0x50,
DXGI_FORMAT_BC4_SNORM = 0x51,
DXGI_FORMAT_BC5_TYPELESS = 0x52,
DXGI_FORMAT_BC5_UNORM = 0x53,
DXGI_FORMAT_BC5_SNORM = 0x54,
DXGI_FORMAT_B5G6R5_UNORM = 0x55,
DXGI_FORMAT_B5G5R5A1_UNORM = 0x56,
DXGI_FORMAT_B8G8R8A8_UNORM = 0x57,
DXGI_FORMAT_B8G8R8X8_UNORM = 0x58,
DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 0x59,
DXGI_FORMAT_B8G8R8A8_TYPELESS = 0x5A,
DXGI_FORMAT_B8G8R8A8_UNORM_SRGB = 0x5B,
DXGI_FORMAT_B8G8R8X8_TYPELESS = 0x5C,
DXGI_FORMAT_B8G8R8X8_UNORM_SRGB = 0x5D,
DXGI_FORMAT_BC6H_TYPELESS = 0x5E,
DXGI_FORMAT_BC6H_UF16 = 0x5F,
DXGI_FORMAT_BC6H_SF16 = 0x60,
DXGI_FORMAT_BC7_TYPELESS = 0x61,
DXGI_FORMAT_BC7_UNORM = 0x62,
DXGI_FORMAT_BC7_UNORM_SRGB = 0x63,
DXGI_FORMAT_AYUV = 0x64,
DXGI_FORMAT_Y410 = 0x65,
DXGI_FORMAT_Y416 = 0x66,
DXGI_FORMAT_NV12 = 0x67,
DXGI_FORMAT_P010 = 0x68,
DXGI_FORMAT_P016 = 0x69,
DXGI_FORMAT_420_OPAQUE = 0x6A,
DXGI_FORMAT_YUY2 = 0x6B,
DXGI_FORMAT_Y210 = 0x6C,
DXGI_FORMAT_Y216 = 0x6D,
DXGI_FORMAT_NV11 = 0x6E,
DXGI_FORMAT_AI44 = 0x6F,
DXGI_FORMAT_IA44 = 0x70,
DXGI_FORMAT_P8 = 0x71,
DXGI_FORMAT_A8P8 = 0x72,
DXGI_FORMAT_B4G4R4A4_UNORM = 0x73,
DXGI_FORMAT_P208 = 0x82,
DXGI_FORMAT_V208 = 0x83,
DXGI_FORMAT_V408 = 0x84,
DXGI_FORMAT_FORCE_UINT = 0xFFFFFFFF,
}
;

typedef void ID3D11Texture1D
;

struct ID3D11Texture1DVtbl
{
HRESULT (__stdcall *QueryInterface)(ID3D11Texture1D *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ID3D11Texture1D *This);
ULONG (__stdcall *Release)(ID3D11Texture1D *This);
void (__stdcall *GetDevice)(ID3D11Texture1D *This, ID3D11Device **ppDevice);
HRESULT (__stdcall *GetPrivateData)(ID3D11Texture1D *This, const GUID *const guid, UINT *pDataSize, void *pData);
HRESULT (__stdcall *SetPrivateData)(ID3D11Texture1D *This, const GUID *const guid, UINT DataSize, const void *pData);
HRESULT (__stdcall *SetPrivateDataInterface)(ID3D11Texture1D *This, const GUID *const guid, const IUnknown *pData);
void (__stdcall *GetType)(ID3D11Texture1D *This, D3D11_RESOURCE_DIMENSION *pResourceDimension);
void (__stdcall *SetEvictionPriority)(ID3D11Texture1D *This, UINT EvictionPriority);
UINT (__stdcall *GetEvictionPriority)(ID3D11Texture1D *This);
void (__stdcall *GetDesc)(ID3D11Texture1D *This, D3D11_TEXTURE1D_DESC *pDesc);
}
;

struct D3D11_TEXTURE2D_DESC
{
UINT Width;
UINT Height;
UINT MipLevels;
UINT ArraySize;
DXGI_FORMAT Format;
DXGI_SAMPLE_DESC SampleDesc;
D3D11_USAGE Usage;
UINT BindFlags;
UINT CPUAccessFlags;
UINT MiscFlags;
}
;

struct DXGI_SAMPLE_DESC
{
UINT Count;
UINT Quality;
}
;

typedef void ID3D11Texture2D
;

struct ID3D11Texture2DVtbl
{
HRESULT (__stdcall *QueryInterface)(ID3D11Texture2D *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ID3D11Texture2D *This);
ULONG (__stdcall *Release)(ID3D11Texture2D *This);
void (__stdcall *GetDevice)(ID3D11Texture2D *This, ID3D11Device **ppDevice);
HRESULT (__stdcall *GetPrivateData)(ID3D11Texture2D *This, const GUID *const guid, UINT *pDataSize, void *pData);
HRESULT (__stdcall *SetPrivateData)(ID3D11Texture2D *This, const GUID *const guid, UINT DataSize, const void *pData);
HRESULT (__stdcall *SetPrivateDataInterface)(ID3D11Texture2D *This, const GUID *const guid, const IUnknown *pData);
void (__stdcall *GetType)(ID3D11Texture2D *This, D3D11_RESOURCE_DIMENSION *pResourceDimension);
void (__stdcall *SetEvictionPriority)(ID3D11Texture2D *This, UINT EvictionPriority);
UINT (__stdcall *GetEvictionPriority)(ID3D11Texture2D *This);
void (__stdcall *GetDesc)(ID3D11Texture2D *This, D3D11_TEXTURE2D_DESC *pDesc);
}
;

struct D3D11_TEXTURE3D_DESC
{
UINT Width;
UINT Height;
UINT Depth;
UINT MipLevels;
DXGI_FORMAT Format;
D3D11_USAGE Usage;
UINT BindFlags;
UINT CPUAccessFlags;
UINT MiscFlags;
}
;

typedef void ID3D11Texture3D
;

struct ID3D11Texture3DVtbl
{
HRESULT (__stdcall *QueryInterface)(ID3D11Texture3D *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ID3D11Texture3D *This);
ULONG (__stdcall *Release)(ID3D11Texture3D *This);
void (__stdcall *GetDevice)(ID3D11Texture3D *This, ID3D11Device **ppDevice);
HRESULT (__stdcall *GetPrivateData)(ID3D11Texture3D *This, const GUID *const guid, UINT *pDataSize, void *pData);
HRESULT (__stdcall *SetPrivateData)(ID3D11Texture3D *This, const GUID *const guid, UINT DataSize, const void *pData);
HRESULT (__stdcall *SetPrivateDataInterface)(ID3D11Texture3D *This, const GUID *const guid, const IUnknown *pData);
void (__stdcall *GetType)(ID3D11Texture3D *This, D3D11_RESOURCE_DIMENSION *pResourceDimension);
void (__stdcall *SetEvictionPriority)(ID3D11Texture3D *This, UINT EvictionPriority);
UINT (__stdcall *GetEvictionPriority)(ID3D11Texture3D *This);
void (__stdcall *GetDesc)(ID3D11Texture3D *This, D3D11_TEXTURE3D_DESC *pDesc);
}
;

typedef void ID3D11Resource
;

struct ID3D11ResourceVtbl
{
HRESULT (__stdcall *QueryInterface)(ID3D11Resource *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ID3D11Resource *This);
ULONG (__stdcall *Release)(ID3D11Resource *This);
void (__stdcall *GetDevice)(ID3D11Resource *This, ID3D11Device **ppDevice);
HRESULT (__stdcall *GetPrivateData)(ID3D11Resource *This, const GUID *const guid, UINT *pDataSize, void *pData);
HRESULT (__stdcall *SetPrivateData)(ID3D11Resource *This, const GUID *const guid, UINT DataSize, const void *pData);
HRESULT (__stdcall *SetPrivateDataInterface)(ID3D11Resource *This, const GUID *const guid, const IUnknown *pData);
void (__stdcall *GetType)(ID3D11Resource *This, D3D11_RESOURCE_DIMENSION *pResourceDimension);
void (__stdcall *SetEvictionPriority)(ID3D11Resource *This, UINT EvictionPriority);
UINT (__stdcall *GetEvictionPriority)(ID3D11Resource *This);
}
;

struct D3D11_SHADER_RESOURCE_VIEW_DESC
{
DXGI_FORMAT Format;
D3D11_SRV_DIMENSION ViewDimension;
union
{
D3D11_BUFFER_SRV Buffer;
D3D11_TEX1D_SRV Texture1D;
D3D11_TEX1D_ARRAY_SRV Texture1DArray;
D3D11_TEX2D_SRV Texture2D;
D3D11_TEX2D_ARRAY_SRV Texture2DArray;
D3D11_TEX2DMS_SRV Texture2DMS;
D3D11_TEX2DMS_ARRAY_SRV Texture2DMSArray;
D3D11_TEX3D_SRV Texture3D;
D3D11_TEXCUBE_SRV TextureCube;
D3D11_TEXCUBE_ARRAY_SRV TextureCubeArray;
D3D11_BUFFEREX_SRV BufferEx;
};
}
;

typedef D3D_SRV_DIMENSION D3D11_SRV_DIMENSION
;

enum D3D_SRV_DIMENSION
{
D3D_SRV_DIMENSION_UNKNOWN = 0x0,
D3D_SRV_DIMENSION_BUFFER = 0x1,
D3D_SRV_DIMENSION_TEXTURE1D = 0x2,
D3D_SRV_DIMENSION_TEXTURE1DARRAY = 0x3,
D3D_SRV_DIMENSION_TEXTURE2D = 0x4,
D3D_SRV_DIMENSION_TEXTURE2DARRAY = 0x5,
D3D_SRV_DIMENSION_TEXTURE2DMS = 0x6,
D3D_SRV_DIMENSION_TEXTURE2DMSARRAY = 0x7,
D3D_SRV_DIMENSION_TEXTURE3D = 0x8,
D3D_SRV_DIMENSION_TEXTURECUBE = 0x9,
D3D_SRV_DIMENSION_TEXTURECUBEARRAY = 0xA,
D3D_SRV_DIMENSION_BUFFEREX = 0xB,
D3D10_SRV_DIMENSION_UNKNOWN = 0x0,
D3D10_SRV_DIMENSION_BUFFER = 0x1,
D3D10_SRV_DIMENSION_TEXTURE1D = 0x2,
D3D10_SRV_DIMENSION_TEXTURE1DARRAY = 0x3,
D3D10_SRV_DIMENSION_TEXTURE2D = 0x4,
D3D10_SRV_DIMENSION_TEXTURE2DARRAY = 0x5,
D3D10_SRV_DIMENSION_TEXTURE2DMS = 0x6,
D3D10_SRV_DIMENSION_TEXTURE2DMSARRAY = 0x7,
D3D10_SRV_DIMENSION_TEXTURE3D = 0x8,
D3D10_SRV_DIMENSION_TEXTURECUBE = 0x9,
D3D10_1_SRV_DIMENSION_UNKNOWN = 0x0,
D3D10_1_SRV_DIMENSION_BUFFER = 0x1,
D3D10_1_SRV_DIMENSION_TEXTURE1D = 0x2,
D3D10_1_SRV_DIMENSION_TEXTURE1DARRAY = 0x3,
D3D10_1_SRV_DIMENSION_TEXTURE2D = 0x4,
D3D10_1_SRV_DIMENSION_TEXTURE2DARRAY = 0x5,
D3D10_1_SRV_DIMENSION_TEXTURE2DMS = 0x6,
D3D10_1_SRV_DIMENSION_TEXTURE2DMSARRAY = 0x7,
D3D10_1_SRV_DIMENSION_TEXTURE3D = 0x8,
D3D10_1_SRV_DIMENSION_TEXTURECUBE = 0x9,
D3D10_1_SRV_DIMENSION_TEXTURECUBEARRAY = 0xA,
D3D11_SRV_DIMENSION_UNKNOWN = 0x0,
D3D11_SRV_DIMENSION_BUFFER = 0x1,
D3D11_SRV_DIMENSION_TEXTURE1D = 0x2,
D3D11_SRV_DIMENSION_TEXTURE1DARRAY = 0x3,
D3D11_SRV_DIMENSION_TEXTURE2D = 0x4,
D3D11_SRV_DIMENSION_TEXTURE2DARRAY = 0x5,
D3D11_SRV_DIMENSION_TEXTURE2DMS = 0x6,
D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY = 0x7,
D3D11_SRV_DIMENSION_TEXTURE3D = 0x8,
D3D11_SRV_DIMENSION_TEXTURECUBE = 0x9,
D3D11_SRV_DIMENSION_TEXTURECUBEARRAY = 0xA,
D3D11_SRV_DIMENSION_BUFFEREX = 0xB,
}
;

union D3D11_SHADER_RESOURCE_VIEW_DESC::$9432D0D16C541F7FDBD711DB3059C804
{
D3D11_BUFFER_SRV Buffer;
D3D11_TEX1D_SRV Texture1D;
D3D11_TEX1D_ARRAY_SRV Texture1DArray;
D3D11_TEX2D_SRV Texture2D;
D3D11_TEX2D_ARRAY_SRV Texture2DArray;
D3D11_TEX2DMS_SRV Texture2DMS;
D3D11_TEX2DMS_ARRAY_SRV Texture2DMSArray;
D3D11_TEX3D_SRV Texture3D;
D3D11_TEXCUBE_SRV TextureCube;
D3D11_TEXCUBE_ARRAY_SRV TextureCubeArray;
D3D11_BUFFEREX_SRV BufferEx;
}
;

struct D3D11_BUFFER_SRV
{
union
{
UINT FirstElement;
UINT ElementOffset;
};
union
{
UINT NumElements;
UINT ElementWidth;
};
}
;

union D3D11_BUFFER_SRV::$636FCB046847EEDF15856CE7C35FCB7F
{
UINT FirstElement;
UINT ElementOffset;
}
;

union D3D11_BUFFER_SRV::$5EA6C023B6E595B5E45F8B856510107D
{
UINT NumElements;
UINT ElementWidth;
}
;

struct D3D11_TEX1D_SRV
{
UINT MostDetailedMip;
UINT MipLevels;
}
;

struct D3D11_TEX1D_ARRAY_SRV
{
UINT MostDetailedMip;
UINT MipLevels;
UINT FirstArraySlice;
UINT ArraySize;
}
;

struct D3D11_TEX2D_SRV
{
UINT MostDetailedMip;
UINT MipLevels;
}
;

struct D3D11_TEX2D_ARRAY_SRV
{
UINT MostDetailedMip;
UINT MipLevels;
UINT FirstArraySlice;
UINT ArraySize;
}
;

struct D3D11_TEX2DMS_SRV
{
UINT UnusedField_NothingToDefine;
}
;

struct D3D11_TEX2DMS_ARRAY_SRV
{
UINT FirstArraySlice;
UINT ArraySize;
}
;

struct D3D11_TEX3D_SRV
{
UINT MostDetailedMip;
UINT MipLevels;
}
;

struct D3D11_TEXCUBE_SRV
{
UINT MostDetailedMip;
UINT MipLevels;
}
;

struct D3D11_TEXCUBE_ARRAY_SRV
{
UINT MostDetailedMip;
UINT MipLevels;
UINT First2DArrayFace;
UINT NumCubes;
}
;

struct D3D11_BUFFEREX_SRV
{
UINT FirstElement;
UINT NumElements;
UINT Flags;
}
;

typedef void ID3D11ShaderResourceView
;

struct ID3D11ShaderResourceViewVtbl
{
HRESULT (__stdcall *QueryInterface)(ID3D11ShaderResourceView *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ID3D11ShaderResourceView *This);
ULONG (__stdcall *Release)(ID3D11ShaderResourceView *This);
void (__stdcall *GetDevice)(ID3D11ShaderResourceView *This, ID3D11Device **ppDevice);
HRESULT (__stdcall *GetPrivateData)(ID3D11ShaderResourceView *This, const GUID *const guid, UINT *pDataSize, void *pData);
HRESULT (__stdcall *SetPrivateData)(ID3D11ShaderResourceView *This, const GUID *const guid, UINT DataSize, const void *pData);
HRESULT (__stdcall *SetPrivateDataInterface)(ID3D11ShaderResourceView *This, const GUID *const guid, const IUnknown *pData);
void (__stdcall *GetResource)(ID3D11ShaderResourceView *This, ID3D11Resource **ppResource);
void (__stdcall *GetDesc)(ID3D11ShaderResourceView *This, D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc);
}
;

struct D3D11_UNORDERED_ACCESS_VIEW_DESC
{
DXGI_FORMAT Format;
D3D11_UAV_DIMENSION ViewDimension;
union
{
D3D11_BUFFER_UAV Buffer;
D3D11_TEX1D_UAV Texture1D;
D3D11_TEX1D_ARRAY_UAV Texture1DArray;
D3D11_TEX2D_UAV Texture2D;
D3D11_TEX2D_ARRAY_UAV Texture2DArray;
D3D11_TEX3D_UAV Texture3D;
};
}
;

enum D3D11_UAV_DIMENSION
{
D3D11_UAV_DIMENSION_UNKNOWN = 0x0,
D3D11_UAV_DIMENSION_BUFFER = 0x1,
D3D11_UAV_DIMENSION_TEXTURE1D = 0x2,
D3D11_UAV_DIMENSION_TEXTURE1DARRAY = 0x3,
D3D11_UAV_DIMENSION_TEXTURE2D = 0x4,
D3D11_UAV_DIMENSION_TEXTURE2DARRAY = 0x5,
D3D11_UAV_DIMENSION_TEXTURE3D = 0x8,
}
;

union D3D11_UNORDERED_ACCESS_VIEW_DESC::$11FBB28DCA371DE929B44BAF9661EF48
{
D3D11_BUFFER_UAV Buffer;
D3D11_TEX1D_UAV Texture1D;
D3D11_TEX1D_ARRAY_UAV Texture1DArray;
D3D11_TEX2D_UAV Texture2D;
D3D11_TEX2D_ARRAY_UAV Texture2DArray;
D3D11_TEX3D_UAV Texture3D;
}
;

struct D3D11_BUFFER_UAV
{
UINT FirstElement;
UINT NumElements;
UINT Flags;
}
;

struct D3D11_TEX1D_UAV
{
UINT MipSlice;
}
;

struct D3D11_TEX1D_ARRAY_UAV
{
UINT MipSlice;
UINT FirstArraySlice;
UINT ArraySize;
}
;

struct D3D11_TEX2D_UAV
{
UINT MipSlice;
}
;

struct D3D11_TEX2D_ARRAY_UAV
{
UINT MipSlice;
UINT FirstArraySlice;
UINT ArraySize;
}
;

struct D3D11_TEX3D_UAV
{
UINT MipSlice;
UINT FirstWSlice;
UINT WSize;
}
;

typedef void ID3D11UnorderedAccessView
;

struct ID3D11UnorderedAccessViewVtbl
{
HRESULT (__stdcall *QueryInterface)(ID3D11UnorderedAccessView *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ID3D11UnorderedAccessView *This);
ULONG (__stdcall *Release)(ID3D11UnorderedAccessView *This);
void (__stdcall *GetDevice)(ID3D11UnorderedAccessView *This, ID3D11Device **ppDevice);
HRESULT (__stdcall *GetPrivateData)(ID3D11UnorderedAccessView *This, const GUID *const guid, UINT *pDataSize, void *pData);
HRESULT (__stdcall *SetPrivateData)(ID3D11UnorderedAccessView *This, const GUID *const guid, UINT DataSize, const void *pData);
HRESULT (__stdcall *SetPrivateDataInterface)(ID3D11UnorderedAccessView *This, const GUID *const guid, const IUnknown *pData);
void (__stdcall *GetResource)(ID3D11UnorderedAccessView *This, ID3D11Resource **ppResource);
void (__stdcall *GetDesc)(ID3D11UnorderedAccessView *This, D3D11_UNORDERED_ACCESS_VIEW_DESC *pDesc);
}
;

struct D3D11_RENDER_TARGET_VIEW_DESC
{
DXGI_FORMAT Format;
D3D11_RTV_DIMENSION ViewDimension;
union
{
D3D11_BUFFER_RTV Buffer;
D3D11_TEX1D_RTV Texture1D;
D3D11_TEX1D_ARRAY_RTV Texture1DArray;
D3D11_TEX2D_RTV Texture2D;
D3D11_TEX2D_ARRAY_RTV Texture2DArray;
D3D11_TEX2DMS_RTV Texture2DMS;
D3D11_TEX2DMS_ARRAY_RTV Texture2DMSArray;
D3D11_TEX3D_RTV Texture3D;
};
}
;

enum D3D11_RTV_DIMENSION
{
D3D11_RTV_DIMENSION_UNKNOWN = 0x0,
D3D11_RTV_DIMENSION_BUFFER = 0x1,
D3D11_RTV_DIMENSION_TEXTURE1D = 0x2,
D3D11_RTV_DIMENSION_TEXTURE1DARRAY = 0x3,
D3D11_RTV_DIMENSION_TEXTURE2D = 0x4,
D3D11_RTV_DIMENSION_TEXTURE2DARRAY = 0x5,
D3D11_RTV_DIMENSION_TEXTURE2DMS = 0x6,
D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY = 0x7,
D3D11_RTV_DIMENSION_TEXTURE3D = 0x8,
}
;

union D3D11_RENDER_TARGET_VIEW_DESC::$C25145134444453513A60B25C9F48069
{
D3D11_BUFFER_RTV Buffer;
D3D11_TEX1D_RTV Texture1D;
D3D11_TEX1D_ARRAY_RTV Texture1DArray;
D3D11_TEX2D_RTV Texture2D;
D3D11_TEX2D_ARRAY_RTV Texture2DArray;
D3D11_TEX2DMS_RTV Texture2DMS;
D3D11_TEX2DMS_ARRAY_RTV Texture2DMSArray;
D3D11_TEX3D_RTV Texture3D;
}
;

struct D3D11_BUFFER_RTV
{
union
{
UINT FirstElement;
UINT ElementOffset;
};
union
{
UINT NumElements;
UINT ElementWidth;
};
}
;

union D3D11_BUFFER_RTV::$636FCB046847EEDF15856CE7C35FCB7F
{
UINT FirstElement;
UINT ElementOffset;
}
;

union D3D11_BUFFER_RTV::$5EA6C023B6E595B5E45F8B856510107D
{
UINT NumElements;
UINT ElementWidth;
}
;

struct D3D11_TEX1D_RTV
{
UINT MipSlice;
}
;

struct D3D11_TEX1D_ARRAY_RTV
{
UINT MipSlice;
UINT FirstArraySlice;
UINT ArraySize;
}
;

struct D3D11_TEX2D_RTV
{
UINT MipSlice;
}
;

struct D3D11_TEX2D_ARRAY_RTV
{
UINT MipSlice;
UINT FirstArraySlice;
UINT ArraySize;
}
;

struct D3D11_TEX2DMS_RTV
{
UINT UnusedField_NothingToDefine;
}
;

struct D3D11_TEX2DMS_ARRAY_RTV
{
UINT FirstArraySlice;
UINT ArraySize;
}
;

struct D3D11_TEX3D_RTV
{
UINT MipSlice;
UINT FirstWSlice;
UINT WSize;
}
;

typedef void ID3D11RenderTargetView
;

struct ID3D11RenderTargetViewVtbl
{
HRESULT (__stdcall *QueryInterface)(ID3D11RenderTargetView *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ID3D11RenderTargetView *This);
ULONG (__stdcall *Release)(ID3D11RenderTargetView *This);
void (__stdcall *GetDevice)(ID3D11RenderTargetView *This, ID3D11Device **ppDevice);
HRESULT (__stdcall *GetPrivateData)(ID3D11RenderTargetView *This, const GUID *const guid, UINT *pDataSize, void *pData);
HRESULT (__stdcall *SetPrivateData)(ID3D11RenderTargetView *This, const GUID *const guid, UINT DataSize, const void *pData);
HRESULT (__stdcall *SetPrivateDataInterface)(ID3D11RenderTargetView *This, const GUID *const guid, const IUnknown *pData);
void (__stdcall *GetResource)(ID3D11RenderTargetView *This, ID3D11Resource **ppResource);
void (__stdcall *GetDesc)(ID3D11RenderTargetView *This, D3D11_RENDER_TARGET_VIEW_DESC *pDesc);
}
;

struct D3D11_DEPTH_STENCIL_VIEW_DESC
{
DXGI_FORMAT Format;
D3D11_DSV_DIMENSION ViewDimension;
UINT Flags;
union
{
D3D11_TEX1D_DSV Texture1D;
D3D11_TEX1D_ARRAY_DSV Texture1DArray;
D3D11_TEX2D_DSV Texture2D;
D3D11_TEX2D_ARRAY_DSV Texture2DArray;
D3D11_TEX2DMS_DSV Texture2DMS;
D3D11_TEX2DMS_ARRAY_DSV Texture2DMSArray;
};
}
;

enum D3D11_DSV_DIMENSION
{
D3D11_DSV_DIMENSION_UNKNOWN = 0x0,
D3D11_DSV_DIMENSION_TEXTURE1D = 0x1,
D3D11_DSV_DIMENSION_TEXTURE1DARRAY = 0x2,
D3D11_DSV_DIMENSION_TEXTURE2D = 0x3,
D3D11_DSV_DIMENSION_TEXTURE2DARRAY = 0x4,
D3D11_DSV_DIMENSION_TEXTURE2DMS = 0x5,
D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY = 0x6,
}
;

union D3D11_DEPTH_STENCIL_VIEW_DESC::$739C91EBC9F294EBF282B4AF07A94D76
{
D3D11_TEX1D_DSV Texture1D;
D3D11_TEX1D_ARRAY_DSV Texture1DArray;
D3D11_TEX2D_DSV Texture2D;
D3D11_TEX2D_ARRAY_DSV Texture2DArray;
D3D11_TEX2DMS_DSV Texture2DMS;
D3D11_TEX2DMS_ARRAY_DSV Texture2DMSArray;
}
;

struct D3D11_TEX1D_DSV
{
UINT MipSlice;
}
;

struct D3D11_TEX1D_ARRAY_DSV
{
UINT MipSlice;
UINT FirstArraySlice;
UINT ArraySize;
}
;

struct D3D11_TEX2D_DSV
{
UINT MipSlice;
}
;

struct D3D11_TEX2D_ARRAY_DSV
{
UINT MipSlice;
UINT FirstArraySlice;
UINT ArraySize;
}
;

struct D3D11_TEX2DMS_DSV
{
UINT UnusedField_NothingToDefine;
}
;

struct D3D11_TEX2DMS_ARRAY_DSV
{
UINT FirstArraySlice;
UINT ArraySize;
}
;

typedef void ID3D11DepthStencilView
;

struct ID3D11DepthStencilViewVtbl
{
HRESULT (__stdcall *QueryInterface)(ID3D11DepthStencilView *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ID3D11DepthStencilView *This);
ULONG (__stdcall *Release)(ID3D11DepthStencilView *This);
void (__stdcall *GetDevice)(ID3D11DepthStencilView *This, ID3D11Device **ppDevice);
HRESULT (__stdcall *GetPrivateData)(ID3D11DepthStencilView *This, const GUID *const guid, UINT *pDataSize, void *pData);
HRESULT (__stdcall *SetPrivateData)(ID3D11DepthStencilView *This, const GUID *const guid, UINT DataSize, const void *pData);
HRESULT (__stdcall *SetPrivateDataInterface)(ID3D11DepthStencilView *This, const GUID *const guid, const IUnknown *pData);
void (__stdcall *GetResource)(ID3D11DepthStencilView *This, ID3D11Resource **ppResource);
void (__stdcall *GetDesc)(ID3D11DepthStencilView *This, D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc);
}
;

struct D3D11_INPUT_ELEMENT_DESC
{
LPCSTR SemanticName;
UINT SemanticIndex;
DXGI_FORMAT Format;
UINT InputSlot;
UINT AlignedByteOffset;
D3D11_INPUT_CLASSIFICATION InputSlotClass;
UINT InstanceDataStepRate;
}
;

typedef const CHAR *LPCSTR
;

enum D3D11_INPUT_CLASSIFICATION
{
D3D11_INPUT_PER_VERTEX_DATA = 0x0,
D3D11_INPUT_PER_INSTANCE_DATA = 0x1,
}
;

typedef void ID3D11InputLayout
;

struct ID3D11InputLayoutVtbl
{
HRESULT (__stdcall *QueryInterface)(ID3D11InputLayout *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ID3D11InputLayout *This);
ULONG (__stdcall *Release)(ID3D11InputLayout *This);
void (__stdcall *GetDevice)(ID3D11InputLayout *This, ID3D11Device **ppDevice);
HRESULT (__stdcall *GetPrivateData)(ID3D11InputLayout *This, const GUID *const guid, UINT *pDataSize, void *pData);
HRESULT (__stdcall *SetPrivateData)(ID3D11InputLayout *This, const GUID *const guid, UINT DataSize, const void *pData);
HRESULT (__stdcall *SetPrivateDataInterface)(ID3D11InputLayout *This, const GUID *const guid, const IUnknown *pData);
}
;

struct ID3D11ClassLinkage
{
ID3D11ClassLinkageVtbl *lpVtbl;
}
;

struct ID3D11ClassLinkageVtbl
{
HRESULT (__stdcall *QueryInterface)(ID3D11ClassLinkage *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ID3D11ClassLinkage *This);
ULONG (__stdcall *Release)(ID3D11ClassLinkage *This);
void (__stdcall *GetDevice)(ID3D11ClassLinkage *This, ID3D11Device **ppDevice);
HRESULT (__stdcall *GetPrivateData)(ID3D11ClassLinkage *This, const GUID *const guid, UINT *pDataSize, void *pData);
HRESULT (__stdcall *SetPrivateData)(ID3D11ClassLinkage *This, const GUID *const guid, UINT DataSize, const void *pData);
HRESULT (__stdcall *SetPrivateDataInterface)(ID3D11ClassLinkage *This, const GUID *const guid, const IUnknown *pData);
HRESULT (__stdcall *GetClassInstance)(ID3D11ClassLinkage *This, LPCSTR pClassInstanceName, UINT InstanceIndex, ID3D11ClassInstance **ppInstance);
HRESULT (__stdcall *CreateClassInstance)(ID3D11ClassLinkage *This, LPCSTR pClassTypeName, UINT ConstantBufferOffset, UINT ConstantVectorOffset, UINT TextureOffset, UINT SamplerOffset, ID3D11ClassInstance **ppInstance);
}
;

struct ID3D11ClassInstance
{
ID3D11ClassInstanceVtbl *lpVtbl;
}
;

struct ID3D11ClassInstanceVtbl
{
HRESULT (__stdcall *QueryInterface)(ID3D11ClassInstance *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ID3D11ClassInstance *This);
ULONG (__stdcall *Release)(ID3D11ClassInstance *This);
void (__stdcall *GetDevice)(ID3D11ClassInstance *This, ID3D11Device **ppDevice);
HRESULT (__stdcall *GetPrivateData)(ID3D11ClassInstance *This, const GUID *const guid, UINT *pDataSize, void *pData);
HRESULT (__stdcall *SetPrivateData)(ID3D11ClassInstance *This, const GUID *const guid, UINT DataSize, const void *pData);
HRESULT (__stdcall *SetPrivateDataInterface)(ID3D11ClassInstance *This, const GUID *const guid, const IUnknown *pData);
void (__stdcall *GetClassLinkage)(ID3D11ClassInstance *This, ID3D11ClassLinkage **ppLinkage);
void (__stdcall *GetDesc)(ID3D11ClassInstance *This, D3D11_CLASS_INSTANCE_DESC *pDesc);
void (__stdcall *GetInstanceName)(ID3D11ClassInstance *This, LPSTR pInstanceName, SIZE_T *pBufferLength);
void (__stdcall *GetTypeName)(ID3D11ClassInstance *This, LPSTR pTypeName, SIZE_T *pBufferLength);
}
;

struct D3D11_CLASS_INSTANCE_DESC
{
UINT InstanceId;
UINT InstanceIndex;
UINT TypeId;
UINT ConstantBuffer;
UINT BaseConstantBufferOffset;
UINT BaseTexture;
UINT BaseSampler;
BOOL Created;
}
;

typedef void ID3D11VertexShader
;

struct ID3D11VertexShaderVtbl
{
HRESULT (__stdcall *QueryInterface)(ID3D11VertexShader *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ID3D11VertexShader *This);
ULONG (__stdcall *Release)(ID3D11VertexShader *This);
void (__stdcall *GetDevice)(ID3D11VertexShader *This, ID3D11Device **ppDevice);
HRESULT (__stdcall *GetPrivateData)(ID3D11VertexShader *This, const GUID *const guid, UINT *pDataSize, void *pData);
HRESULT (__stdcall *SetPrivateData)(ID3D11VertexShader *This, const GUID *const guid, UINT DataSize, const void *pData);
HRESULT (__stdcall *SetPrivateDataInterface)(ID3D11VertexShader *This, const GUID *const guid, const IUnknown *pData);
}
;

typedef void ID3D11GeometryShader
;

struct ID3D11GeometryShaderVtbl
{
HRESULT (__stdcall *QueryInterface)(ID3D11GeometryShader *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ID3D11GeometryShader *This);
ULONG (__stdcall *Release)(ID3D11GeometryShader *This);
void (__stdcall *GetDevice)(ID3D11GeometryShader *This, ID3D11Device **ppDevice);
HRESULT (__stdcall *GetPrivateData)(ID3D11GeometryShader *This, const GUID *const guid, UINT *pDataSize, void *pData);
HRESULT (__stdcall *SetPrivateData)(ID3D11GeometryShader *This, const GUID *const guid, UINT DataSize, const void *pData);
HRESULT (__stdcall *SetPrivateDataInterface)(ID3D11GeometryShader *This, const GUID *const guid, const IUnknown *pData);
}
;

struct D3D11_SO_DECLARATION_ENTRY
{
UINT Stream;
LPCSTR SemanticName;
UINT SemanticIndex;
BYTE StartComponent;
BYTE ComponentCount;
BYTE OutputSlot;
}
;

typedef void ID3D11PixelShader
;

struct ID3D11PixelShaderVtbl
{
HRESULT (__stdcall *QueryInterface)(ID3D11PixelShader *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ID3D11PixelShader *This);
ULONG (__stdcall *Release)(ID3D11PixelShader *This);
void (__stdcall *GetDevice)(ID3D11PixelShader *This, ID3D11Device **ppDevice);
HRESULT (__stdcall *GetPrivateData)(ID3D11PixelShader *This, const GUID *const guid, UINT *pDataSize, void *pData);
HRESULT (__stdcall *SetPrivateData)(ID3D11PixelShader *This, const GUID *const guid, UINT DataSize, const void *pData);
HRESULT (__stdcall *SetPrivateDataInterface)(ID3D11PixelShader *This, const GUID *const guid, const IUnknown *pData);
}
;

typedef void ID3D11HullShader
;

struct ID3D11HullShaderVtbl
{
HRESULT (__stdcall *QueryInterface)(ID3D11HullShader *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ID3D11HullShader *This);
ULONG (__stdcall *Release)(ID3D11HullShader *This);
void (__stdcall *GetDevice)(ID3D11HullShader *This, ID3D11Device **ppDevice);
HRESULT (__stdcall *GetPrivateData)(ID3D11HullShader *This, const GUID *const guid, UINT *pDataSize, void *pData);
HRESULT (__stdcall *SetPrivateData)(ID3D11HullShader *This, const GUID *const guid, UINT DataSize, const void *pData);
HRESULT (__stdcall *SetPrivateDataInterface)(ID3D11HullShader *This, const GUID *const guid, const IUnknown *pData);
}
;

typedef void ID3D11DomainShader
;

struct ID3D11DomainShaderVtbl
{
HRESULT (__stdcall *QueryInterface)(ID3D11DomainShader *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ID3D11DomainShader *This);
ULONG (__stdcall *Release)(ID3D11DomainShader *This);
void (__stdcall *GetDevice)(ID3D11DomainShader *This, ID3D11Device **ppDevice);
HRESULT (__stdcall *GetPrivateData)(ID3D11DomainShader *This, const GUID *const guid, UINT *pDataSize, void *pData);
HRESULT (__stdcall *SetPrivateData)(ID3D11DomainShader *This, const GUID *const guid, UINT DataSize, const void *pData);
HRESULT (__stdcall *SetPrivateDataInterface)(ID3D11DomainShader *This, const GUID *const guid, const IUnknown *pData);
}
;

typedef void ID3D11ComputeShader
;

struct ID3D11ComputeShaderVtbl
{
HRESULT (__stdcall *QueryInterface)(ID3D11ComputeShader *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ID3D11ComputeShader *This);
ULONG (__stdcall *Release)(ID3D11ComputeShader *This);
void (__stdcall *GetDevice)(ID3D11ComputeShader *This, ID3D11Device **ppDevice);
HRESULT (__stdcall *GetPrivateData)(ID3D11ComputeShader *This, const GUID *const guid, UINT *pDataSize, void *pData);
HRESULT (__stdcall *SetPrivateData)(ID3D11ComputeShader *This, const GUID *const guid, UINT DataSize, const void *pData);
HRESULT (__stdcall *SetPrivateDataInterface)(ID3D11ComputeShader *This, const GUID *const guid, const IUnknown *pData);
}
;

struct D3D11_BLEND_DESC
{
BOOL AlphaToCoverageEnable;
BOOL IndependentBlendEnable;
D3D11_RENDER_TARGET_BLEND_DESC RenderTarget[8];
}
;

struct D3D11_RENDER_TARGET_BLEND_DESC
{
BOOL BlendEnable;
D3D11_BLEND SrcBlend;
D3D11_BLEND DestBlend;
D3D11_BLEND_OP BlendOp;
D3D11_BLEND SrcBlendAlpha;
D3D11_BLEND DestBlendAlpha;
D3D11_BLEND_OP BlendOpAlpha;
UINT8 RenderTargetWriteMask;
}
;

enum D3D11_BLEND
{
D3D11_BLEND_ZERO = 0x1,
D3D11_BLEND_ONE = 0x2,
D3D11_BLEND_SRC_COLOR = 0x3,
D3D11_BLEND_INV_SRC_COLOR = 0x4,
D3D11_BLEND_SRC_ALPHA = 0x5,
D3D11_BLEND_INV_SRC_ALPHA = 0x6,
D3D11_BLEND_DEST_ALPHA = 0x7,
D3D11_BLEND_INV_DEST_ALPHA = 0x8,
D3D11_BLEND_DEST_COLOR = 0x9,
D3D11_BLEND_INV_DEST_COLOR = 0xA,
D3D11_BLEND_SRC_ALPHA_SAT = 0xB,
D3D11_BLEND_BLEND_FACTOR = 0xE,
D3D11_BLEND_INV_BLEND_FACTOR = 0xF,
D3D11_BLEND_SRC1_COLOR = 0x10,
D3D11_BLEND_INV_SRC1_COLOR = 0x11,
D3D11_BLEND_SRC1_ALPHA = 0x12,
D3D11_BLEND_INV_SRC1_ALPHA = 0x13,
}
;

enum D3D11_BLEND_OP
{
D3D11_BLEND_OP_ADD = 0x1,
D3D11_BLEND_OP_SUBTRACT = 0x2,
D3D11_BLEND_OP_REV_SUBTRACT = 0x3,
D3D11_BLEND_OP_MIN = 0x4,
D3D11_BLEND_OP_MAX = 0x5,
}
;

typedef void ID3D11BlendState
;

struct ID3D11BlendStateVtbl
{
HRESULT (__stdcall *QueryInterface)(ID3D11BlendState *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ID3D11BlendState *This);
ULONG (__stdcall *Release)(ID3D11BlendState *This);
void (__stdcall *GetDevice)(ID3D11BlendState *This, ID3D11Device **ppDevice);
HRESULT (__stdcall *GetPrivateData)(ID3D11BlendState *This, const GUID *const guid, UINT *pDataSize, void *pData);
HRESULT (__stdcall *SetPrivateData)(ID3D11BlendState *This, const GUID *const guid, UINT DataSize, const void *pData);
HRESULT (__stdcall *SetPrivateDataInterface)(ID3D11BlendState *This, const GUID *const guid, const IUnknown *pData);
void (__stdcall *GetDesc)(ID3D11BlendState *This, D3D11_BLEND_DESC *pDesc);
}
;

struct D3D11_DEPTH_STENCIL_DESC
{
BOOL DepthEnable;
D3D11_DEPTH_WRITE_MASK DepthWriteMask;
D3D11_COMPARISON_FUNC DepthFunc;
BOOL StencilEnable;
UINT8 StencilReadMask;
UINT8 StencilWriteMask;
D3D11_DEPTH_STENCILOP_DESC FrontFace;
D3D11_DEPTH_STENCILOP_DESC BackFace;
}
;

enum D3D11_DEPTH_WRITE_MASK
{
D3D11_DEPTH_WRITE_MASK_ZERO = 0x0,
D3D11_DEPTH_WRITE_MASK_ALL = 0x1,
}
;

enum D3D11_COMPARISON_FUNC
{
D3D11_COMPARISON_NEVER = 0x1,
D3D11_COMPARISON_LESS = 0x2,
D3D11_COMPARISON_EQUAL = 0x3,
D3D11_COMPARISON_LESS_EQUAL = 0x4,
D3D11_COMPARISON_GREATER = 0x5,
D3D11_COMPARISON_NOT_EQUAL = 0x6,
D3D11_COMPARISON_GREATER_EQUAL = 0x7,
D3D11_COMPARISON_ALWAYS = 0x8,
}
;

struct D3D11_DEPTH_STENCILOP_DESC
{
D3D11_STENCIL_OP StencilFailOp;
D3D11_STENCIL_OP StencilDepthFailOp;
D3D11_STENCIL_OP StencilPassOp;
D3D11_COMPARISON_FUNC StencilFunc;
}
;

enum D3D11_STENCIL_OP
{
D3D11_STENCIL_OP_KEEP = 0x1,
D3D11_STENCIL_OP_ZERO = 0x2,
D3D11_STENCIL_OP_REPLACE = 0x3,
D3D11_STENCIL_OP_INCR_SAT = 0x4,
D3D11_STENCIL_OP_DECR_SAT = 0x5,
D3D11_STENCIL_OP_INVERT = 0x6,
D3D11_STENCIL_OP_INCR = 0x7,
D3D11_STENCIL_OP_DECR = 0x8,
}
;

typedef void ID3D11DepthStencilState
;

struct ID3D11DepthStencilStateVtbl
{
HRESULT (__stdcall *QueryInterface)(ID3D11DepthStencilState *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ID3D11DepthStencilState *This);
ULONG (__stdcall *Release)(ID3D11DepthStencilState *This);
void (__stdcall *GetDevice)(ID3D11DepthStencilState *This, ID3D11Device **ppDevice);
HRESULT (__stdcall *GetPrivateData)(ID3D11DepthStencilState *This, const GUID *const guid, UINT *pDataSize, void *pData);
HRESULT (__stdcall *SetPrivateData)(ID3D11DepthStencilState *This, const GUID *const guid, UINT DataSize, const void *pData);
HRESULT (__stdcall *SetPrivateDataInterface)(ID3D11DepthStencilState *This, const GUID *const guid, const IUnknown *pData);
void (__stdcall *GetDesc)(ID3D11DepthStencilState *This, D3D11_DEPTH_STENCIL_DESC *pDesc);
}
;

struct D3D11_RASTERIZER_DESC
{
D3D11_FILL_MODE FillMode;
D3D11_CULL_MODE CullMode;
BOOL FrontCounterClockwise;
INT DepthBias;
FLOAT DepthBiasClamp;
FLOAT SlopeScaledDepthBias;
BOOL DepthClipEnable;
BOOL ScissorEnable;
BOOL MultisampleEnable;
BOOL AntialiasedLineEnable;
}
;

enum D3D11_FILL_MODE
{
D3D11_FILL_WIREFRAME = 0x2,
D3D11_FILL_SOLID = 0x3,
}
;

enum D3D11_CULL_MODE
{
D3D11_CULL_NONE = 0x1,
D3D11_CULL_FRONT = 0x2,
D3D11_CULL_BACK = 0x3,
}
;

typedef void ID3D11RasterizerState
;

struct ID3D11RasterizerStateVtbl
{
HRESULT (__stdcall *QueryInterface)(ID3D11RasterizerState *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ID3D11RasterizerState *This);
ULONG (__stdcall *Release)(ID3D11RasterizerState *This);
void (__stdcall *GetDevice)(ID3D11RasterizerState *This, ID3D11Device **ppDevice);
HRESULT (__stdcall *GetPrivateData)(ID3D11RasterizerState *This, const GUID *const guid, UINT *pDataSize, void *pData);
HRESULT (__stdcall *SetPrivateData)(ID3D11RasterizerState *This, const GUID *const guid, UINT DataSize, const void *pData);
HRESULT (__stdcall *SetPrivateDataInterface)(ID3D11RasterizerState *This, const GUID *const guid, const IUnknown *pData);
void (__stdcall *GetDesc)(ID3D11RasterizerState *This, D3D11_RASTERIZER_DESC *pDesc);
}
;

struct D3D11_SAMPLER_DESC
{
D3D11_FILTER Filter;
D3D11_TEXTURE_ADDRESS_MODE AddressU;
D3D11_TEXTURE_ADDRESS_MODE AddressV;
D3D11_TEXTURE_ADDRESS_MODE AddressW;
FLOAT MipLODBias;
UINT MaxAnisotropy;
D3D11_COMPARISON_FUNC ComparisonFunc;
FLOAT BorderColor[4];
FLOAT MinLOD;
FLOAT MaxLOD;
}
;

enum D3D11_FILTER
{
D3D11_FILTER_MIN_MAG_MIP_POINT = 0x0,
D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR = 0x1,
D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x4,
D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR = 0x5,
D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT = 0x10,
D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x11,
D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT = 0x14,
D3D11_FILTER_MIN_MAG_MIP_LINEAR = 0x15,
D3D11_FILTER_ANISOTROPIC = 0x55,
D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT = 0x80,
D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR = 0x81,
D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x84,
D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR = 0x85,
D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT = 0x90,
D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x91,
D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT = 0x94,
D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR = 0x95,
D3D11_FILTER_COMPARISON_ANISOTROPIC = 0xD5,
D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT = 0x100,
D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR = 0x101,
D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x104,
D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR = 0x105,
D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT = 0x110,
D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x111,
D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT = 0x114,
D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR = 0x115,
D3D11_FILTER_MINIMUM_ANISOTROPIC = 0x155,
D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT = 0x180,
D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR = 0x181,
D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x184,
D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR = 0x185,
D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT = 0x190,
D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x191,
D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT = 0x194,
D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR = 0x195,
D3D11_FILTER_MAXIMUM_ANISOTROPIC = 0x1D5,
}
;

enum D3D11_TEXTURE_ADDRESS_MODE
{
D3D11_TEXTURE_ADDRESS_WRAP = 0x1,
D3D11_TEXTURE_ADDRESS_MIRROR = 0x2,
D3D11_TEXTURE_ADDRESS_CLAMP = 0x3,
D3D11_TEXTURE_ADDRESS_BORDER = 0x4,
D3D11_TEXTURE_ADDRESS_MIRROR_ONCE = 0x5,
}
;

typedef void ID3D11SamplerState
;

struct ID3D11SamplerStateVtbl
{
HRESULT (__stdcall *QueryInterface)(ID3D11SamplerState *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ID3D11SamplerState *This);
ULONG (__stdcall *Release)(ID3D11SamplerState *This);
void (__stdcall *GetDevice)(ID3D11SamplerState *This, ID3D11Device **ppDevice);
HRESULT (__stdcall *GetPrivateData)(ID3D11SamplerState *This, const GUID *const guid, UINT *pDataSize, void *pData);
HRESULT (__stdcall *SetPrivateData)(ID3D11SamplerState *This, const GUID *const guid, UINT DataSize, const void *pData);
HRESULT (__stdcall *SetPrivateDataInterface)(ID3D11SamplerState *This, const GUID *const guid, const IUnknown *pData);
void (__stdcall *GetDesc)(ID3D11SamplerState *This, D3D11_SAMPLER_DESC *pDesc);
}
;

struct D3D11_QUERY_DESC
{
D3D11_QUERY Query;
UINT MiscFlags;
}
;

enum D3D11_QUERY
{
D3D11_QUERY_EVENT = 0x0,
D3D11_QUERY_OCCLUSION = 0x1,
D3D11_QUERY_TIMESTAMP = 0x2,
D3D11_QUERY_TIMESTAMP_DISJOINT = 0x3,
D3D11_QUERY_PIPELINE_STATISTICS = 0x4,
D3D11_QUERY_OCCLUSION_PREDICATE = 0x5,
D3D11_QUERY_SO_STATISTICS = 0x6,
D3D11_QUERY_SO_OVERFLOW_PREDICATE = 0x7,
D3D11_QUERY_SO_STATISTICS_STREAM0 = 0x8,
D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM0 = 0x9,
D3D11_QUERY_SO_STATISTICS_STREAM1 = 0xA,
D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM1 = 0xB,
D3D11_QUERY_SO_STATISTICS_STREAM2 = 0xC,
D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM2 = 0xD,
D3D11_QUERY_SO_STATISTICS_STREAM3 = 0xE,
D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM3 = 0xF,
}
;

typedef void ID3D11Query
;

struct ID3D11QueryVtbl
{
HRESULT (__stdcall *QueryInterface)(ID3D11Query *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ID3D11Query *This);
ULONG (__stdcall *Release)(ID3D11Query *This);
void (__stdcall *GetDevice)(ID3D11Query *This, ID3D11Device **ppDevice);
HRESULT (__stdcall *GetPrivateData)(ID3D11Query *This, const GUID *const guid, UINT *pDataSize, void *pData);
HRESULT (__stdcall *SetPrivateData)(ID3D11Query *This, const GUID *const guid, UINT DataSize, const void *pData);
HRESULT (__stdcall *SetPrivateDataInterface)(ID3D11Query *This, const GUID *const guid, const IUnknown *pData);
UINT (__stdcall *GetDataSize)(ID3D11Query *This);
void (__stdcall *GetDesc)(ID3D11Query *This, D3D11_QUERY_DESC *pDesc);
}
;

struct ID3D11Predicate
{
ID3D11PredicateVtbl *lpVtbl;
}
;

struct ID3D11PredicateVtbl
{
HRESULT (__stdcall *QueryInterface)(ID3D11Predicate *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ID3D11Predicate *This);
ULONG (__stdcall *Release)(ID3D11Predicate *This);
void (__stdcall *GetDevice)(ID3D11Predicate *This, ID3D11Device **ppDevice);
HRESULT (__stdcall *GetPrivateData)(ID3D11Predicate *This, const GUID *const guid, UINT *pDataSize, void *pData);
HRESULT (__stdcall *SetPrivateData)(ID3D11Predicate *This, const GUID *const guid, UINT DataSize, const void *pData);
HRESULT (__stdcall *SetPrivateDataInterface)(ID3D11Predicate *This, const GUID *const guid, const IUnknown *pData);
UINT (__stdcall *GetDataSize)(ID3D11Predicate *This);
void (__stdcall *GetDesc)(ID3D11Predicate *This, D3D11_QUERY_DESC *pDesc);
}
;

struct D3D11_COUNTER_DESC
{
D3D11_COUNTER Counter;
UINT MiscFlags;
}
;

enum __bitmask D3D11_COUNTER
{
D3D11_COUNTER_DEVICE_DEPENDENT_0 = 0x40000000,
}
;

struct ID3D11Counter
{
ID3D11CounterVtbl *lpVtbl;
}
;

struct ID3D11CounterVtbl
{
HRESULT (__stdcall *QueryInterface)(ID3D11Counter *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ID3D11Counter *This);
ULONG (__stdcall *Release)(ID3D11Counter *This);
void (__stdcall *GetDevice)(ID3D11Counter *This, ID3D11Device **ppDevice);
HRESULT (__stdcall *GetPrivateData)(ID3D11Counter *This, const GUID *const guid, UINT *pDataSize, void *pData);
HRESULT (__stdcall *SetPrivateData)(ID3D11Counter *This, const GUID *const guid, UINT DataSize, const void *pData);
HRESULT (__stdcall *SetPrivateDataInterface)(ID3D11Counter *This, const GUID *const guid, const IUnknown *pData);
UINT (__stdcall *GetDataSize)(ID3D11Counter *This);
void (__stdcall *GetDesc)(ID3D11Counter *This, D3D11_COUNTER_DESC *pDesc);
}
;

struct ID3D11DeviceContext
{
ID3D11DeviceContextVtbl *lpVtbl;
}
;

struct ID3D11DeviceContextVtbl
{
HRESULT (__stdcall *QueryInterface)(ID3D11DeviceContext *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ID3D11DeviceContext *This);
ULONG (__stdcall *Release)(ID3D11DeviceContext *This);
void (__stdcall *GetDevice)(ID3D11DeviceContext *This, ID3D11Device **ppDevice);
HRESULT (__stdcall *GetPrivateData)(ID3D11DeviceContext *This, const GUID *const guid, UINT *pDataSize, void *pData);
HRESULT (__stdcall *SetPrivateData)(ID3D11DeviceContext *This, const GUID *const guid, UINT DataSize, const void *pData);
HRESULT (__stdcall *SetPrivateDataInterface)(ID3D11DeviceContext *This, const GUID *const guid, const IUnknown *pData);
void (__stdcall *VSSetConstantBuffers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers);
void (__stdcall *PSSetShaderResources)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews);
void (__stdcall *PSSetShader)(ID3D11DeviceContext *This, ID3D11PixelShader *pPixelShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances);
void (__stdcall *PSSetSamplers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers);
void (__stdcall *VSSetShader)(ID3D11DeviceContext *This, ID3D11VertexShader *pVertexShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances);
void (__stdcall *DrawIndexed)(ID3D11DeviceContext *This, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
void (__stdcall *Draw)(ID3D11DeviceContext *This, UINT VertexCount, UINT StartVertexLocation);
HRESULT (__stdcall *Map)(ID3D11DeviceContext *This, ID3D11Resource *pResource, UINT Subresource, D3D11_MAP MapType, UINT MapFlags, D3D11_MAPPED_SUBRESOURCE *pMappedResource);
void (__stdcall *Unmap)(ID3D11DeviceContext *This, ID3D11Resource *pResource, UINT Subresource);
void (__stdcall *PSSetConstantBuffers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers);
void (__stdcall *IASetInputLayout)(ID3D11DeviceContext *This, ID3D11InputLayout *pInputLayout);
void (__stdcall *IASetVertexBuffers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppVertexBuffers, const UINT *pStrides, const UINT *pOffsets);
void (__stdcall *IASetIndexBuffer)(ID3D11DeviceContext *This, ID3D11Buffer *pIndexBuffer, DXGI_FORMAT Format, UINT Offset);
void (__stdcall *DrawIndexedInstanced)(ID3D11DeviceContext *This, UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation);
void (__stdcall *DrawInstanced)(ID3D11DeviceContext *This, UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation);
void (__stdcall *GSSetConstantBuffers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers);
void (__stdcall *GSSetShader)(ID3D11DeviceContext *This, ID3D11GeometryShader *pShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances);
void (__stdcall *IASetPrimitiveTopology)(ID3D11DeviceContext *This, D3D11_PRIMITIVE_TOPOLOGY Topology);
void (__stdcall *VSSetShaderResources)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews);
void (__stdcall *VSSetSamplers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers);
void (__stdcall *Begin)(ID3D11DeviceContext *This, ID3D11Asynchronous *pAsync);
void (__stdcall *End)(ID3D11DeviceContext *This, ID3D11Asynchronous *pAsync);
HRESULT (__stdcall *GetData)(ID3D11DeviceContext *This, ID3D11Asynchronous *pAsync, void *pData, UINT DataSize, UINT GetDataFlags);
void (__stdcall *SetPredication)(ID3D11DeviceContext *This, ID3D11Predicate *pPredicate, BOOL PredicateValue);
void (__stdcall *GSSetShaderResources)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews);
void (__stdcall *GSSetSamplers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers);
void (__stdcall *OMSetRenderTargets)(ID3D11DeviceContext *This, UINT NumViews, ID3D11RenderTargetView *const *ppRenderTargetViews, ID3D11DepthStencilView *pDepthStencilView);
void (__stdcall *OMSetRenderTargetsAndUnorderedAccessViews)(ID3D11DeviceContext *This, UINT NumRTVs, ID3D11RenderTargetView *const *ppRenderTargetViews, ID3D11DepthStencilView *pDepthStencilView, UINT UAVStartSlot, UINT NumUAVs, ID3D11UnorderedAccessView *const *ppUnorderedAccessViews, const UINT *pUAVInitialCounts);
void (__stdcall *OMSetBlendState)(ID3D11DeviceContext *This, ID3D11BlendState *pBlendState, const FLOAT BlendFactor[4], UINT SampleMask);
void (__stdcall *OMSetDepthStencilState)(ID3D11DeviceContext *This, ID3D11DepthStencilState *pDepthStencilState, UINT StencilRef);
void (__stdcall *SOSetTargets)(ID3D11DeviceContext *This, UINT NumBuffers, ID3D11Buffer *const *ppSOTargets, const UINT *pOffsets);
void (__stdcall *DrawAuto)(ID3D11DeviceContext *This);
void (__stdcall *DrawIndexedInstancedIndirect)(ID3D11DeviceContext *This, ID3D11Buffer *pBufferForArgs, UINT AlignedByteOffsetForArgs);
void (__stdcall *DrawInstancedIndirect)(ID3D11DeviceContext *This, ID3D11Buffer *pBufferForArgs, UINT AlignedByteOffsetForArgs);
void (__stdcall *Dispatch)(ID3D11DeviceContext *This, UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ);
void (__stdcall *DispatchIndirect)(ID3D11DeviceContext *This, ID3D11Buffer *pBufferForArgs, UINT AlignedByteOffsetForArgs);
void (__stdcall *RSSetState)(ID3D11DeviceContext *This, ID3D11RasterizerState *pRasterizerState);
void (__stdcall *RSSetViewports)(ID3D11DeviceContext *This, UINT NumViewports, const D3D11_VIEWPORT *pViewports);
void (__stdcall *RSSetScissorRects)(ID3D11DeviceContext *This, UINT NumRects, const D3D11_RECT *pRects);
void (__stdcall *CopySubresourceRegion)(ID3D11DeviceContext *This, ID3D11Resource *pDstResource, UINT DstSubresource, UINT DstX, UINT DstY, UINT DstZ, ID3D11Resource *pSrcResource, UINT SrcSubresource, const D3D11_BOX *pSrcBox);
void (__stdcall *CopyResource)(ID3D11DeviceContext *This, ID3D11Resource *pDstResource, ID3D11Resource *pSrcResource);
void (__stdcall *UpdateSubresource)(ID3D11DeviceContext *This, ID3D11Resource *pDstResource, UINT DstSubresource, const D3D11_BOX *pDstBox, const void *pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch);
void (__stdcall *CopyStructureCount)(ID3D11DeviceContext *This, ID3D11Buffer *pDstBuffer, UINT DstAlignedByteOffset, ID3D11UnorderedAccessView *pSrcView);
void (__stdcall *ClearRenderTargetView)(ID3D11DeviceContext *This, ID3D11RenderTargetView *pRenderTargetView, const FLOAT ColorRGBA[4]);
void (__stdcall *ClearUnorderedAccessViewUint)(ID3D11DeviceContext *This, ID3D11UnorderedAccessView *pUnorderedAccessView, const UINT Values[4]);
void (__stdcall *ClearUnorderedAccessViewFloat)(ID3D11DeviceContext *This, ID3D11UnorderedAccessView *pUnorderedAccessView, const FLOAT Values[4]);
void (__stdcall *ClearDepthStencilView)(ID3D11DeviceContext *This, ID3D11DepthStencilView *pDepthStencilView, UINT ClearFlags, FLOAT Depth, UINT8 Stencil);
void (__stdcall *GenerateMips)(ID3D11DeviceContext *This, ID3D11ShaderResourceView *pShaderResourceView);
void (__stdcall *SetResourceMinLOD)(ID3D11DeviceContext *This, ID3D11Resource *pResource, FLOAT MinLOD);
FLOAT (__stdcall *GetResourceMinLOD)(ID3D11DeviceContext *This, ID3D11Resource *pResource);
void (__stdcall *ResolveSubresource)(ID3D11DeviceContext *This, ID3D11Resource *pDstResource, UINT DstSubresource, ID3D11Resource *pSrcResource, UINT SrcSubresource, DXGI_FORMAT Format);
void (__stdcall *ExecuteCommandList)(ID3D11DeviceContext *This, ID3D11CommandList *pCommandList, BOOL RestoreContextState);
void (__stdcall *HSSetShaderResources)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews);
void (__stdcall *HSSetShader)(ID3D11DeviceContext *This, ID3D11HullShader *pHullShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances);
void (__stdcall *HSSetSamplers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers);
void (__stdcall *HSSetConstantBuffers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers);
void (__stdcall *DSSetShaderResources)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews);
void (__stdcall *DSSetShader)(ID3D11DeviceContext *This, ID3D11DomainShader *pDomainShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances);
void (__stdcall *DSSetSamplers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers);
void (__stdcall *DSSetConstantBuffers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers);
void (__stdcall *CSSetShaderResources)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews);
void (__stdcall *CSSetUnorderedAccessViews)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumUAVs, ID3D11UnorderedAccessView *const *ppUnorderedAccessViews, const UINT *pUAVInitialCounts);
void (__stdcall *CSSetShader)(ID3D11DeviceContext *This, ID3D11ComputeShader *pComputeShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances);
void (__stdcall *CSSetSamplers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers);
void (__stdcall *CSSetConstantBuffers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers);
void (__stdcall *VSGetConstantBuffers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumBuffers, ID3D11Buffer **ppConstantBuffers);
void (__stdcall *PSGetShaderResources)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView **ppShaderResourceViews);
void (__stdcall *PSGetShader)(ID3D11DeviceContext *This, ID3D11PixelShader **ppPixelShader, ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances);
void (__stdcall *PSGetSamplers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumSamplers, ID3D11SamplerState **ppSamplers);
void (__stdcall *VSGetShader)(ID3D11DeviceContext *This, ID3D11VertexShader **ppVertexShader, ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances);
void (__stdcall *PSGetConstantBuffers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumBuffers, ID3D11Buffer **ppConstantBuffers);
void (__stdcall *IAGetInputLayout)(ID3D11DeviceContext *This, ID3D11InputLayout **ppInputLayout);
void (__stdcall *IAGetVertexBuffers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumBuffers, ID3D11Buffer **ppVertexBuffers, UINT *pStrides, UINT *pOffsets);
void (__stdcall *IAGetIndexBuffer)(ID3D11DeviceContext *This, ID3D11Buffer **pIndexBuffer, DXGI_FORMAT *Format, UINT *Offset);
void (__stdcall *GSGetConstantBuffers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumBuffers, ID3D11Buffer **ppConstantBuffers);
void (__stdcall *GSGetShader)(ID3D11DeviceContext *This, ID3D11GeometryShader **ppGeometryShader, ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances);
void (__stdcall *IAGetPrimitiveTopology)(ID3D11DeviceContext *This, D3D11_PRIMITIVE_TOPOLOGY *pTopology);
void (__stdcall *VSGetShaderResources)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView **ppShaderResourceViews);
void (__stdcall *VSGetSamplers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumSamplers, ID3D11SamplerState **ppSamplers);
void (__stdcall *GetPredication)(ID3D11DeviceContext *This, ID3D11Predicate **ppPredicate, BOOL *pPredicateValue);
void (__stdcall *GSGetShaderResources)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView **ppShaderResourceViews);
void (__stdcall *GSGetSamplers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumSamplers, ID3D11SamplerState **ppSamplers);
void (__stdcall *OMGetRenderTargets)(ID3D11DeviceContext *This, UINT NumViews, ID3D11RenderTargetView **ppRenderTargetViews, ID3D11DepthStencilView **ppDepthStencilView);
void (__stdcall *OMGetRenderTargetsAndUnorderedAccessViews)(ID3D11DeviceContext *This, UINT NumRTVs, ID3D11RenderTargetView **ppRenderTargetViews, ID3D11DepthStencilView **ppDepthStencilView, UINT UAVStartSlot, UINT NumUAVs, ID3D11UnorderedAccessView **ppUnorderedAccessViews);
void (__stdcall *OMGetBlendState)(ID3D11DeviceContext *This, ID3D11BlendState **ppBlendState, FLOAT BlendFactor[4], UINT *pSampleMask);
void (__stdcall *OMGetDepthStencilState)(ID3D11DeviceContext *This, ID3D11DepthStencilState **ppDepthStencilState, UINT *pStencilRef);
void (__stdcall *SOGetTargets)(ID3D11DeviceContext *This, UINT NumBuffers, ID3D11Buffer **ppSOTargets);
void (__stdcall *RSGetState)(ID3D11DeviceContext *This, ID3D11RasterizerState **ppRasterizerState);
void (__stdcall *RSGetViewports)(ID3D11DeviceContext *This, UINT *pNumViewports, D3D11_VIEWPORT *pViewports);
void (__stdcall *RSGetScissorRects)(ID3D11DeviceContext *This, UINT *pNumRects, D3D11_RECT *pRects);
void (__stdcall *HSGetShaderResources)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView **ppShaderResourceViews);
void (__stdcall *HSGetShader)(ID3D11DeviceContext *This, ID3D11HullShader **ppHullShader, ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances);
void (__stdcall *HSGetSamplers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumSamplers, ID3D11SamplerState **ppSamplers);
void (__stdcall *HSGetConstantBuffers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumBuffers, ID3D11Buffer **ppConstantBuffers);
void (__stdcall *DSGetShaderResources)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView **ppShaderResourceViews);
void (__stdcall *DSGetShader)(ID3D11DeviceContext *This, ID3D11DomainShader **ppDomainShader, ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances);
void (__stdcall *DSGetSamplers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumSamplers, ID3D11SamplerState **ppSamplers);
void (__stdcall *DSGetConstantBuffers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumBuffers, ID3D11Buffer **ppConstantBuffers);
void (__stdcall *CSGetShaderResources)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView **ppShaderResourceViews);
void (__stdcall *CSGetUnorderedAccessViews)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumUAVs, ID3D11UnorderedAccessView **ppUnorderedAccessViews);
void (__stdcall *CSGetShader)(ID3D11DeviceContext *This, ID3D11ComputeShader **ppComputeShader, ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances);
void (__stdcall *CSGetSamplers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumSamplers, ID3D11SamplerState **ppSamplers);
void (__stdcall *CSGetConstantBuffers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumBuffers, ID3D11Buffer **ppConstantBuffers);
void (__stdcall *ClearState)(ID3D11DeviceContext *This);
void (__stdcall *Flush)(ID3D11DeviceContext *This);
D3D11_DEVICE_CONTEXT_TYPE (__stdcall *GetType)(ID3D11DeviceContext *This);
UINT (__stdcall *GetContextFlags)(ID3D11DeviceContext *This);
HRESULT (__stdcall *FinishCommandList)(ID3D11DeviceContext *This, BOOL RestoreDeferredContextState, ID3D11CommandList **ppCommandList);
}
;

enum D3D11_MAP
{
D3D11_MAP_READ = 0x1,
D3D11_MAP_WRITE = 0x2,
D3D11_MAP_READ_WRITE = 0x3,
D3D11_MAP_WRITE_DISCARD = 0x4,
D3D11_MAP_WRITE_NO_OVERWRITE = 0x5,
}
;

struct D3D11_MAPPED_SUBRESOURCE
{
void *pData;
UINT RowPitch;
UINT DepthPitch;
}
;

typedef D3D_PRIMITIVE_TOPOLOGY D3D11_PRIMITIVE_TOPOLOGY
;

enum D3D_PRIMITIVE_TOPOLOGY
{
D3D_PRIMITIVE_TOPOLOGY_UNDEFINED = 0x0,
D3D_PRIMITIVE_TOPOLOGY_POINTLIST = 0x1,
D3D_PRIMITIVE_TOPOLOGY_LINELIST = 0x2,
D3D_PRIMITIVE_TOPOLOGY_LINESTRIP = 0x3,
D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST = 0x4,
D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP = 0x5,
D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ = 0xA,
D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ = 0xB,
D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ = 0xC,
D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ = 0xD,
D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST = 0x21,
D3D_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST = 0x22,
D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST = 0x23,
D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST = 0x24,
D3D_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST = 0x25,
D3D_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST = 0x26,
D3D_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST = 0x27,
D3D_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST = 0x28,
D3D_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST = 0x29,
D3D_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST = 0x2A,
D3D_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST = 0x2B,
D3D_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST = 0x2C,
D3D_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST = 0x2D,
D3D_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST = 0x2E,
D3D_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST = 0x2F,
D3D_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST = 0x30,
D3D_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST = 0x31,
D3D_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST = 0x32,
D3D_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST = 0x33,
D3D_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST = 0x34,
D3D_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST = 0x35,
D3D_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST = 0x36,
D3D_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST = 0x37,
D3D_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST = 0x38,
D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST = 0x39,
D3D_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST = 0x3A,
D3D_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST = 0x3B,
D3D_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST = 0x3C,
D3D_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST = 0x3D,
D3D_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST = 0x3E,
D3D_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST = 0x3F,
D3D_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST = 0x40,
D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED = 0x0,
D3D10_PRIMITIVE_TOPOLOGY_POINTLIST = 0x1,
D3D10_PRIMITIVE_TOPOLOGY_LINELIST = 0x2,
D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP = 0x3,
D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST = 0x4,
D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP = 0x5,
D3D10_PRIMITIVE_TOPOLOGY_LINELIST_ADJ = 0xA,
D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ = 0xB,
D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ = 0xC,
D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ = 0xD,
D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED = 0x0,
D3D11_PRIMITIVE_TOPOLOGY_POINTLIST = 0x1,
D3D11_PRIMITIVE_TOPOLOGY_LINELIST = 0x2,
D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP = 0x3,
D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST = 0x4,
D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP = 0x5,
D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ = 0xA,
D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ = 0xB,
D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ = 0xC,
D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ = 0xD,
D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST = 0x21,
D3D11_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST = 0x22,
D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST = 0x23,
D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST = 0x24,
D3D11_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST = 0x25,
D3D11_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST = 0x26,
D3D11_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST = 0x27,
D3D11_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST = 0x28,
D3D11_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST = 0x29,
D3D11_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST = 0x2A,
D3D11_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST = 0x2B,
D3D11_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST = 0x2C,
D3D11_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST = 0x2D,
D3D11_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST = 0x2E,
D3D11_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST = 0x2F,
D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST = 0x30,
D3D11_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST = 0x31,
D3D11_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST = 0x32,
D3D11_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST = 0x33,
D3D11_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST = 0x34,
D3D11_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST = 0x35,
D3D11_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST = 0x36,
D3D11_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST = 0x37,
D3D11_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST = 0x38,
D3D11_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST = 0x39,
D3D11_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST = 0x3A,
D3D11_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST = 0x3B,
D3D11_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST = 0x3C,
D3D11_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST = 0x3D,
D3D11_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST = 0x3E,
D3D11_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST = 0x3F,
D3D11_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST = 0x40,
}
;

struct ID3D11Asynchronous
{
ID3D11AsynchronousVtbl *lpVtbl;
}
;

struct ID3D11AsynchronousVtbl
{
HRESULT (__stdcall *QueryInterface)(ID3D11Asynchronous *This, const IID *const riid, void **ppvObject);
ULONG (__stdcall *AddRef)(ID3D11Asynchronous *This);
ULONG (__stdcall *Release)(ID3D11Asynchronous *This);
void (__stdcall *GetDevice)(ID3D11Asynchronous *This, ID3D11Device **ppDevice);
HRESULT (__stdcall *GetPrivateData)(ID3D11Asynchronous *This, const GUID *const guid, UINT *pDataSize, void *pData);
HRESULT (__stdcall *SetPrivateData)(ID3D11Asynchronous *This, const GUID *const guid, UINT DataSize, const void *pData);
HRESULT (__stdcall *SetPrivateDataInterface)(ID3D11Asynchronous *This, const GUID *const guid, const IUnknown *pData);
UINT (__stdcall *GetDataSize)(ID3D11Asynchronous *This);
}
;

struct D3D11_COUNTER_INFO
{
D3D11_COUNTER LastDeviceDependentCounter;
UINT NumSimultaneousCounters;
UINT8 NumDetectableParallelUnits;
}
;

enum D3D11_COUNTER_TYPE
{
D3D11_COUNTER_TYPE_FLOAT32 = 0x0,
D3D11_COUNTER_TYPE_UINT16 = 0x1,
D3D11_COUNTER_TYPE_UINT32 = 0x2,
D3D11_COUNTER_TYPE_UINT64 = 0x3,
}
;

enum D3D11_FEATURE
{
D3D11_FEATURE_THREADING = 0x0,
D3D11_FEATURE_DOUBLES = 0x1,
D3D11_FEATURE_FORMAT_SUPPORT = 0x2,
D3D11_FEATURE_FORMAT_SUPPORT2 = 0x3,
D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS = 0x4,
D3D11_FEATURE_D3D11_OPTIONS = 0x5,
D3D11_FEATURE_ARCHITECTURE_INFO = 0x6,
D3D11_FEATURE_D3D9_OPTIONS = 0x7,
D3D11_FEATURE_SHADER_MIN_PRECISION_SUPPORT = 0x8,
D3D11_FEATURE_D3D9_SHADOW_SUPPORT = 0x9,
D3D11_FEATURE_D3D11_OPTIONS1 = 0xA,
D3D11_FEATURE_D3D9_SIMPLE_INSTANCING_SUPPORT = 0xB,
D3D11_FEATURE_MARKER_SUPPORT = 0xC,
D3D11_FEATURE_D3D9_OPTIONS1 = 0xD,
D3D11_FEATURE_D3D11_OPTIONS2 = 0xE,
D3D11_FEATURE_D3D11_OPTIONS3 = 0xF,
D3D11_FEATURE_GPU_VIRTUAL_ADDRESS_SUPPORT = 0x10,
D3D11_FEATURE_D3D11_OPTIONS4 = 0x11,
D3D11_FEATURE_SHADER_CACHE = 0x12,
D3D11_FEATURE_D3D11_OPTIONS5 = 0x13,
}
;

enum D3D_FEATURE_LEVEL
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

struct ansel
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

typedef const CHAR *LPCCH
;

typedef BOOL *LPBOOL
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

typedef ULONG_PTR HCRYPTPROV
;

typedef ULONG_PTR HCRYPTKEY
;

typedef ULONG_PTR HCRYPTHASH
;

typedef unsigned int ALG_ID
;

typedef unsigned int u_long
;

typedef struct addrinfo *PADDRINFOA
;

typedef DWORD *LPDWORD
;

typedef struct _OVERLAPPED *LPWSAOVERLAPPED
;

typedef void (__stdcall *LPWSAOVERLAPPED_COMPLETION_ROUTINE)(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
;

typedef struct _FILETIME *LPFILETIME
;

typedef unsigned __int64 uintptr_t
;

typedef unsigned int fexcept_t
;

typedef size_t rsize_t
;

typedef struct _STARTUPINFOW *LPSTARTUPINFOW
;

typedef struct _PROCESS_INFORMATION *LPPROCESS_INFORMATION
;

typedef WORD *LPWORD
;

typedef __crt_locale_pointers *_locale_t
;

typedef const WCHAR *PCNZWCH
;

typedef struct _OVERLAPPED *LPOVERLAPPED
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

typedef int (__cdecl *_CoreCrtNonSecureSearchSortCompareFunction)(const void *, const void *)
;

typedef char *va_list
;

typedef unsigned __int64 *PULONG64
;

typedef struct _MINIDUMP_EXCEPTION_INFORMATION *PMINIDUMP_EXCEPTION_INFORMATION
;

typedef struct _MINIDUMP_USER_STREAM_INFORMATION *PMINIDUMP_USER_STREAM_INFORMATION
;

struct _MINIDUMP_USER_STREAM_INFORMATION
{
ULONG UserStreamCount;
PMINIDUMP_USER_STREAM UserStreamArray;
}
;

typedef struct _MINIDUMP_USER_STREAM *PMINIDUMP_USER_STREAM
;

struct _MINIDUMP_USER_STREAM
{
ULONG32 Type;
ULONG BufferSize;
PVOID Buffer;
}
;

typedef struct _MINIDUMP_CALLBACK_INFORMATION *PMINIDUMP_CALLBACK_INFORMATION
;

typedef BYTE *PBYTE
;

typedef DWORD *PDWORD
;

typedef PVOID HDEVINFO
;

typedef ACCESS_MASK REGSAM
;

typedef DWORD ACCESS_MASK
;

typedef struct _SP_DEVINFO_DATA *PSP_DEVINFO_DATA
;

typedef DWORD DEVNODE
;

typedef struct _SP_DEVICE_INTERFACE_DATA *PSP_DEVICE_INTERFACE_DATA
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

typedef HANDLE HLOCAL
;

typedef const WCHAR *PCWSTR
;

typedef IUnknown *LPUNKNOWN
;

typedef unsigned __int8 *RPC_CSTR
;

typedef unsigned int GROUP
;

typedef int *LPINT
;

typedef UCHAR *PUCHAR
;

typedef struct _FILETIME *PFILETIME
;

typedef LPVOID HINTERNET
;

typedef void (__stdcall *LPOVERLAPPED_COMPLETION_ROUTINE)(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
;

typedef PRTL_CRITICAL_SECTION LPCRITICAL_SECTION
;

typedef struct _RTL_CRITICAL_SECTION *PRTL_CRITICAL_SECTION
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

typedef struct tagRECT *LPRECT
;

typedef RTL_SRWLOCK *PSRWLOCK
;

typedef HSTRING__ *HSTRING
;

struct HSTRING__
{
int unused;
}
;

typedef WCHAR *PWSTR
;

typedef struct HIMC__ *HIMC
;

struct HIMC__
{
int unused;
}
;

typedef const CHAR *PCNZCH
;

typedef ULONG *PULONG
;

typedef void *HGDIOBJ
;

typedef struct _TOKEN_PRIVILEGES *PTOKEN_PRIVILEGES
;

typedef struct _CERT_CHAIN_PARA *PCERT_CHAIN_PARA
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

typedef PVOID PSECURITY_DESCRIPTOR
;

typedef PRTL_CRITICAL_SECTION PCRITICAL_SECTION
;

typedef const CHAR *PCSTR
;

typedef struct _SECURITY_ATTRIBUTES *LPSECURITY_ATTRIBUTES
;

typedef HANDLE HGLOBAL
;

struct DataPtr
{
unsigned int m_offset;
}
;

struct SerializedTreeData
{
unsigned int m_nodeCount_mapWidth;
DataPtr m_treeData;
DataPtr m_map;
unsigned int m_numSplitValues;
DataPtr m_splitValues;
}
;

struct tmat33_t::$049CA76E17B6074ED35B0F8855160E1C::$3DAD10731B4F1208DC49FB5F6BE4AE83
{
vec3_t row0;
vec3_t row1;
vec3_t row2;
}
;

union tmat33_t::$049CA76E17B6074ED35B0F8855160E1C
{
vec3_t m[3];
struct
{
vec3_t row0;
vec3_t row1;
vec3_t row2;
};
}
;

struct tmat33_t
{
union
{
vec3_t m[3];
};
}
;

typedef int std::int32_t
;

struct MaterialVertexShader
{
const char *name;
const char *debugName;
MaterialVertexShaderProgram prog;
}
;

struct MaterialHullShader
{
const char *name;
const char *debugName;
MaterialHullShaderProgram prog;
}
;

struct MaterialDomainShader
{
const char *name;
const char *debugName;
MaterialDomainShaderProgram prog;
}
;

struct MaterialPixelShader
{
const char *name;
const char *debugName;
MaterialPixelShaderProgram prog;
}
;

struct MaterialVertexDeclaration
{
const char *name;
unsigned __int8 streamCount;
bool hasOptionalSource;
MaterialVertexStreamRouting routing;
}
;

struct FxEffectDef
{
const char *name;
int flags;
int totalSize;
int msecLoopingLife;
int elemDefCountLooping;
int elemDefCountOneShot;
int elemDefCountEmission;
float elemMaxRadius;
float occlusionQueryDepthBias;
int occlusionQueryFadeIn;
int occlusionQueryFadeOut;
FxFloatRange occlusionQueryScaleRange;
FxElemDef *elemDefs;
}
;

struct LeaderBoardDef
;

struct StructuredDataDefSet
{
const char *name;
unsigned int defCount;
StructuredDataDef *defs;
}
;

struct XAnimArcheType
;

struct GfxLightMap
{
const char *name;
GfxImage *textures[3];
}
;

union FxCombinedUnion
{
FxEffectDef *fx;
ParticleSystemDef *vfx;
void *data;
}
;

struct unk_1453E1188
{
char __pad0[68];
}
;

struct unk_1453E1068
{
int numUnk;
unk_1453E1188 *unk;
}
;

typedef __int16 int16_t
;

typedef int int32_t
;

typedef __int64 int64_t
;

typedef unsigned int uint32_t
;

struct tmat33_t::$::$3DAD10731B4F1208DC49FB5F6BE4AE83
{
vec3_t row0;
vec3_t row1;
vec3_t row2;
}
;

enum XAnimPartsFlags
{
ANIM_LOOP = 0x1,
ANIM_DELTA = 0x2,
ANIM_DELTA_3D = 0x4,
ANIM_DEFAULT = 0x8,
ANIM_SHMEM = 0x10,
}
;

struct XSurfaceCollisionAabb
{
unsigned __int16 mins[3];
unsigned __int16 maxs[3];
}
;

struct XSurfaceCollisionNode
{
XSurfaceCollisionAabb aabb;
unsigned __int16 childBeginIndex;
unsigned __int16 childCount;
}
;

struct XSurfaceCollisionLeaf
{
unsigned __int16 triangleBeginIndex;
}
;

struct XSurfaceCollisionTree
{
float trans[3];
float scale[3];
unsigned int nodeCount;
XSurfaceCollisionNode *nodes;
unsigned int leafCount;
XSurfaceCollisionLeaf *leafs;
}
;

union PackedUnitVec
{
unsigned int packed;
unsigned __int8 array[4];
}
;

union PackedTexCoords
{
unsigned int packed;
unsigned __int8 array[4];
}
;

struct PackedSelfVisibility
{
unsigned int packed;
}
;

struct GfxPackedVertex
{
float xyz[3];
PackedSelfVisibility selfVisibility;
GfxColor color;
PackedTexCoords texCoord;
PackedUnitVec normal;
PackedUnitVec tangent;
}
;

struct GfxPackedMotionVertex
{
float xyz[3];
float binormalSignAndHeight;
GfxColor pieceIndex;
PackedTexCoords texCoord;
PackedUnitVec normal;
PackedUnitVec tangent;
}
;

union GfxVertexUnion0
{
GfxPackedVertex *packedVerts0;
GfxPackedMotionVertex *packedMotionVerts0;
void *verts0;
}
;

struct Face
{
unsigned __int16 v1;
unsigned __int16 v2;
unsigned __int16 v3;
}
;

struct SHProbeSimplexData2
{
char __pad0[24];
}
;

struct SHProbeSimplexData1
{
char __pad0[8];
}
;

struct SHProbeSimplexData0
{
SHProbeSimplexData2 a;
SHProbeSimplexData1 b;
}
;

union SHProbeSimplexDataUnion
{
void *data;
SHProbeSimplexData0 *data0;
SHProbeSimplexData1 *data1;
SHProbeSimplexData2 *data2;
}
;

typedef unsigned __int16 XBlendInfo
;

enum XSurfaceFlagBits
{
SURF_FLAG_BIT_VERTCOL_GREY = 0x0,
SURF_FLAG_BIT_VERTCOL_NONE = 0x1,
SURF_FLAG_BIT_SKINNED = 0x2,
SURF_FLAG_BIT_REACTIVE_MOTION = 0x3,
SURF_FLAG_BIT_LIGHTMAP_COORDS = 0x4,
SURF_FLAG_BIT_SUBDIV = 0x5,
SURF_FLAG_BIT_TENSION = 0x6,
SURF_FLAG_BIT_SELF_VISIBILITY = 0x7,
SURF_FLAG_BIT_SECONDUV = 0x8,
SURF_FLAG_BIT_MAYHEM_CUSTOM_CHANNELS = 0x9,
SURF_FLAG_BIT_MAYHEM_SELFVIS = 0xA,
SURF_FLAG_BIT_COUNT = 0xB,
}
;

enum XSurfaceFlags
{
SURF_FLAG_NONE = 0x0,
SURF_FLAG_VERTCOL_GREY = 0x1,
SURF_FLAG_VERTCOL_NONE = 0x2,
SURF_FLAG_SKINNED = 0x4,
SURF_FLAG_REACTIVE_MOTION = 0x8,
SURF_FLAG_LIGHTMAP_COORDS = 0x10,
SURF_FLAG_SUBDIV = 0x20,
SURF_FLAG_TENSION = 0x40,
SURF_FLAG_SELF_VISIBILITY = 0x80,
SURF_FLAG_SECONDUV = 0x100,
SURF_FLAG_MAYHEM_CUSTOM_CHANNELS = 0x200,
SURF_FLAG_MAYHEM_SELFVIS = 0x400,
SURF_FLAG_UNK800 = 0x800,
}
;

struct XModelAngle
{
__int16 x;
__int16 y;
__int16 z;
__int16 base;
}
;

struct XModelTagPos
{
float x;
float y;
float z;
}
;

struct ReactiveMotionModelTweaks
{
float scale[4];
}
;

enum XModelLodInfoFlags
{
XMODEL_LODINFO_FLAG_NONE = 0x0,
XMODEL_LODINFO_FLAG_SUBDIV = 0x1,
XMODEL_LODINFO_FLAG_SUBDIV_NON_ADAPTIVE = 0x2,
}
;

enum SurfaceType
{
SURFACE_TYPE_NONE = 0x0,
SURFACE_TYPE_BARK = 0x1,
SURFACE_TYPE_BRICK = 0x2,
SURFACE_TYPE_CARPET_SOLID = 0x3,
SURFACE_TYPE_CLOTH = 0x4,
SURFACE_TYPE_CONCRETE_DRY = 0x5,
SURFACE_TYPE_DIRT = 0x6,
SURFACE_TYPE_FLESH = 0x7,
SURFACE_TYPE_FOLIAGE_DRY = 0x8,
SURFACE_TYPE_GLASS_PANE = 0x9,
SURFACE_TYPE_GRASS_SHORT = 0xA,
SURFACE_TYPE_GRAVEL = 0xB,
SURFACE_TYPE_ICE_SOLID = 0xC,
SURFACE_TYPE_METAL_THICK = 0xD,
SURFACE_TYPE_METAL_GRATE = 0xE,
SURFACE_TYPE_MUD = 0xF,
SURFACE_TYPE_PAPER = 0x10,
SURFACE_TYPE_PLASTER = 0x11,
SURFACE_TYPE_ROCK = 0x12,
SURFACE_TYPE_SAND = 0x13,
SURFACE_TYPE_SNOW = 0x14,
SURFACE_TYPE_WATER = 0x15,
SURFACE_TYPE_WOOD_SOLID = 0x16,
SURFACE_TYPE_ASPHALT_DRY = 0x17,
SURFACE_TYPE_CERAMIC = 0x18,
SURFACE_TYPE_PLASTIC = 0x19,
SURFACE_TYPE_RUBBER = 0x1A,
SURFACE_TYPE_FRUIT = 0x1B,
SURFACE_TYPE_METAL_PAINTED = 0x1C,
SURFACE_TYPE_RIOTSHIELD = 0x1D,
SURFACE_TYPE_SLUSH = 0x1E,
SURFACE_TYPE_ASPHALT_WET = 0x1F,
SURFACE_TYPE_CARPET_WOOD = 0x20,
SURFACE_TYPE_CARPET_METAL = 0x21,
SURFACE_TYPE_CONCRETE_WET = 0x22,
SURFACE_TYPE_CUSHION = 0x23,
SURFACE_TYPE_DEFAULT = 0x24,
SURFACE_TYPE_FOLIAGE_WET = 0x25,
SURFACE_TYPE_GLASS_BROKEN = 0x26,
SURFACE_TYPE_GLASS_SOLID = 0x27,
SURFACE_TYPE_GLASS_VEHICLE = 0x28,
SURFACE_TYPE_GRASS_TALL = 0x29,
SURFACE_TYPE_ICE_THIN = 0x2A,
SURFACE_TYPE_METAL_THIN = 0x2B,
SURFACE_TYPE_ROBOT_HEAD = 0x2C,
SURFACE_TYPE_ROBOT_TORSO = 0x2D,
SURFACE_TYPE_ROBOT_LIMB = 0x2E,
SURFACE_TYPE_ROBOT_ARMOR = 0x2F,
SURFACE_TYPE_BIG_ROBOT_HEAD = 0x30,
SURFACE_TYPE_BIG_ROBOT_LIMB = 0x31,
SURFACE_TYPE_WATER_KNEE = 0x32,
SURFACE_TYPE_WATER_WAIST = 0x33,
SURFACE_TYPE_WOOD_FLOOR = 0x34,
SURFACE_TYPE_USER_TERRAIN_1 = 0x35,
SURFACE_TYPE_USER_TERRAIN_2 = 0x36,
SURFACE_TYPE_USER_TERRAIN_3 = 0x37,
SURFACE_TYPE_USER_TERRAIN_4 = 0x38,
SURFACE_TYPE_USER_TERRAIN_5 = 0x39,
SURFACE_TYPE_USER_TERRAIN_6 = 0x3A,
SURFACE_TYPE_USER_TERRAIN_7 = 0x3B,
SURFACE_TYPE_USER_TERRAIN_8 = 0x3C,
SURFACE_TYPE_USER_TERRAIN_9 = 0x3D,
SURFACE_TYPE_USER_TERRAIN_10 = 0x3E,
SURFACE_TYPE_CODE_RESERVED = 0x3F,
SURFACE_TYPE_COUNT = 0x40,
}
;

struct XModelCollSurf_s
{
Bounds bounds;
int boneIdx;
int contents;
int surfFlags;
}
;

union XBoneInfo::$3F5B938DF5E4F97F8E1F05029085C3EA
{
float radiusSquared;
unsigned int radiusSquaredAsInt;
}
;

struct unk_1453E14D8
{
Bounds bounds;
}
;

enum XModelFlags
{
XMODEL_FLAG_NONE = 0x0,
XMODEL_FLAG_ANIMATED_VERTS = 0x1,
XMODEL_FLAG_REACTIVE_MOTION = 0x8,
XMODEL_FLAG_COMPOSITE = 0x400,
XMODEL_FLAG_HAS_DEFAULT_MODEL = 0x8000,
XMODEL_FLAG_HAS_ANY_DEFAULT_SURFS = 0x10000,
XMODEL_FLAG_HAS_ALL_DEFAULT_SURFS = 0x20000,
XMODEL_FLAG_HAS_RIGID_VERT_LIST = 0x80000,
XMODEL_FLAG_HAS_MAYHEM_SELFVIS = 0x8000000,
}
;

typedef signed __int8 std::int8_t
;

typedef __int16 std::int16_t
;

typedef __int64 std::int64_t
;

typedef unsigned __int8 std::uint8_t
;

typedef unsigned __int16 std::uint16_t
;

typedef unsigned int std::uint32_t
;

typedef unsigned __int64 std::uint64_t
;

struct std
{
}
;

struct Vector3
{
float x;
float y;
float z;
}
;

struct ImpTome
{
unsigned int m_versionMagic;
unsigned int m_crc32;
unsigned int m_size;
float m_lodBaseDistance;
unsigned int m_flags;
Vector3 m_treeMin;
Vector3 m_treeMax;
SerializedTreeData m_tileTree;
int m_numObjects;
DataPtr m_objBounds;
DataPtr m_objDistances;
DataPtr m_userIDStarts;
DataPtr m_userIDs;
unsigned int m_listWidths;
DataPtr m_objectLists;
int m_objectListSize;
DataPtr m_clusterLists;
int m_clusterListSize;
int m_numGates;
DataPtr m_gateIndexMap;
DataPtr m_gateVertices;
int m_numGateVertices;
DataPtr m_gateIndices;
int m_numClusters;
DataPtr m_clusters;
DataPtr m_clusterPortals;
DataPtr m_cellStarts;
int m_numLeafTiles;
int m_numTiles;
int m_bitsPerSlotPath;
DataPtr m_slotPaths;
DataPtr m_tileLodLevels;
DataPtr m_tiles;
DataPtr m_tileMatchingData;
DataPtr m_matchingTrees;
int m_numMatchingTrees;
int m_numTomes;
DataPtr m_tomeClusterStarts;
DataPtr m_tomeClusterPortalStarts;
char m_computationString[128];
DataPtr m_objectDepthmaps;
DataPtr m_depthmapFaces;
DataPtr m_depthmapPalettes;
int m_numFaces;
DataPtr m_tilePortalExpands;
Vector3 m_boundsMin;
Vector3 m_boundsMax;
float m_clusterCoordScale;
int m_pad[1];
}
;

enum FxCombinedType : unsigned __int8
{
FX_COMBINED_FX = 0x0,
FX_COMBINED_VFX = 0x1,
}
;

enum SurfaceFlags : unsigned __int32
{
SURFACE_FLAG_NONE = 0x0,
SURFACE_FLAG_BARK = 0x80000,
SURFACE_FLAG_BRICK = 0x100000,
SURFACE_FLAG_CARPET_SOLID = 0x180000,
SURFACE_FLAG_CLOTH = 0x200000,
SURFACE_FLAG_CONCRETE_DRY = 0x280000,
SURFACE_FLAG_DIRT = 0x300000,
SURFACE_FLAG_FLESH = 0x380000,
SURFACE_FLAG_FOLIAGE_DRY = 0x400000,
SURFACE_FLAG_GLASS_PANE = 0x480000,
SURFACE_FLAG_GRASS_SHORT = 0x500000,
SURFACE_FLAG_GRAVEL = 0x580000,
SURFACE_FLAG_ICE_SOLID = 0x600000,
SURFACE_FLAG_METAL_THICK = 0x680000,
SURFACE_FLAG_METAL_GRATE = 0x700000,
SURFACE_FLAG_MUD = 0x780000,
SURFACE_FLAG_PAPER = 0x800000,
SURFACE_FLAG_PLASTER = 0x880000,
SURFACE_FLAG_ROCK = 0x900000,
SURFACE_FLAG_SAND = 0x980000,
SURFACE_FLAG_SNOW = 0xA00000,
SURFACE_FLAG_WATER = 0xA80000,
SURFACE_FLAG_WOOD_SOLID = 0xB00000,
SURFACE_FLAG_ASPHALT_DRY = 0xB80000,
SURFACE_FLAG_CERAMIC = 0xC00000,
SURFACE_FLAG_PLASTIC = 0xC80000,
SURFACE_FLAG_RUBBER = 0xD00000,
SURFACE_FLAG_FRUIT = 0xD80000,
SURFACE_FLAG_METAL_PAINTED = 0xE00000,
SURFACE_FLAG_RIOTSHIELD = 0xE80000,
SURFACE_FLAG_SLUSH = 0xF00000,
SURFACE_FLAG_ASPHALT_WET = 0xF80000,
SURFACE_FLAG_CARPET_WOOD = 0x1000000,
SURFACE_FLAG_CARPET_METAL = 0x1080000,
SURFACE_FLAG_CONCRETE_WET = 0x1100000,
SURFACE_FLAG_CUSHION = 0x1180000,
SURFACE_FLAG_DEFAULT = 0x1200000,
SURFACE_FLAG_FOLIAGE_WET = 0x1280000,
SURFACE_FLAG_GLASS_BROKEN = 0x1300000,
SURFACE_FLAG_GLASS_SOLID = 0x1380000,
SURFACE_FLAG_GLASS_VEHICLE = 0x1400000,
SURFACE_FLAG_GRASS_TALL = 0x1480000,
SURFACE_FLAG_ICE_THIN = 0x1500000,
SURFACE_FLAG_METAL_THIN = 0x1580000,
SURFACE_FLAG_ROBOT_HEAD = 0x1600000,
SURFACE_FLAG_ROBOT_TORSO = 0x1680000,
SURFACE_FLAG_ROBOT_LIMB = 0x1700000,
SURFACE_FLAG_ROBOT_ARMOR = 0x1780000,
SURFACE_FLAG_BIG_ROBOT_HEAD = 0x1800000,
SURFACE_FLAG_BIG_ROBOT_LIMB = 0x1880000,
SURFACE_FLAG_WATER_KNEE = 0x1900000,
SURFACE_FLAG_WATER_WAIST = 0x1980000,
SURFACE_FLAG_WOOD_FLOOR = 0x1A00000,
SURFACE_FLAG_USER_TERRAIN_1 = 0x1A80000,
SURFACE_FLAG_USER_TERRAIN_2 = 0x1B00000,
SURFACE_FLAG_USER_TERRAIN_3 = 0x1B80000,
SURFACE_FLAG_USER_TERRAIN_4 = 0x1C00000,
SURFACE_FLAG_USER_TERRAIN_5 = 0x1C80000,
SURFACE_FLAG_USER_TERRAIN_6 = 0x1D00000,
SURFACE_FLAG_USER_TERRAIN_7 = 0x1D80000,
SURFACE_FLAG_USER_TERRAIN_8 = 0x1E00000,
SURFACE_FLAG_USER_TERRAIN_9 = 0x1E80000,
SURFACE_FLAG_USER_TERRAIN_10 = 0x1F00000,
SURFACE_FLAG_CODE_RESERVED = 0x1F80000,
SURFACE_FLAG_OPAQUEGLASS = 0x1380000,
SURFACE_FLAG_CLIPMISSILE = 0x0,
SURFACE_FLAG_AI_NOSIGHT = 0x0,
SURFACE_FLAG_CLIPSHOT = 0x0,
SURFACE_FLAG_PLAYERCLIP = 0x0,
SURFACE_FLAG_MONSTERCLIP = 0x0,
SURFACE_FLAG_AICLIPALLOWDEATH = 0x0,
SURFACE_FLAG_VEHICLECLIP = 0x0,
SURFACE_FLAG_ITEMCLIP = 0x0,
SURFACE_FLAG_NODROP = 0x0,
SURFACE_FLAG_NONSOLID = 0x4000,
SURFACE_FLAG_DETAIL = 0x0,
SURFACE_FLAG_STRUCTURAL = 0x0,
SURFACE_FLAG_PORTAL = 0x80000000,
SURFACE_FLAG_CANSHOOTCLIP = 0x0,
SURFACE_FLAG_ORIGIN = 0x0,
SURFACE_FLAG_SKY = 0x4,
SURFACE_FLAG_NOCASTSHADOW = 0x40000,
SURFACE_FLAG_PHYSICSGEOM = 0x0,
SURFACE_FLAG_LIGHTPORTAL = 0x0,
SURFACE_FLAG_OUTDOORBOUNDS = 0x0,
SURFACE_FLAG_SLICK = 0x2,
SURFACE_FLAG_NOIMPACT = 0x10,
SURFACE_FLAG_NOMARKS = 0x20,
SURFACE_FLAG_NOPENETRATE = 0x100,
SURFACE_FLAG_LADDER = 0x8,
SURFACE_FLAG_NODAMAGE = 0x1,
SURFACE_FLAG_MANTLEON = 0x2000000,
SURFACE_FLAG_MANTLEOVER = 0x4000000,
SURFACE_FLAG_STAIRS = 0x200,
SURFACE_FLAG_SOFT = 0x1000,
SURFACE_FLAG_NOSTEPS = 0x2000,
SURFACE_FLAG_NODRAW = 0x80,
SURFACE_FLAG_NOLIGHTMAP = 0x400,
SURFACE_FLAG_NODLIGHT = 0x20000,
SURFACE_FLAG_NAVINCLUSIONVOLUME = 0x0,
SURFACE_FLAG_NAVEXCLUSIONVOLUME = 0x0,
SURFACE_FLAG_NAV3DVOLUME = 0x0,
}
;

enum SurfaceTypeBits : unsigned __int64
{
SURFACE_TYPE_BIT_NONE = 0x0LL,
SURFACE_TYPE_BIT_BARK = 0x1LL,
SURFACE_TYPE_BIT_BRICK = 0x2LL,
SURFACE_TYPE_BIT_CARPET_SOLID = 0x4LL,
SURFACE_TYPE_BIT_CLOTH = 0x8LL,
SURFACE_TYPE_BIT_CONCRETE_DRY = 0x10LL,
SURFACE_TYPE_BIT_DIRT = 0x20LL,
SURFACE_TYPE_BIT_FLESH = 0x40LL,
SURFACE_TYPE_BIT_FOLIAGE_DRY = 0x80LL,
SURFACE_TYPE_BIT_GLASS_PANE = 0x100LL,
SURFACE_TYPE_BIT_GRASS_SHORT = 0x200LL,
SURFACE_TYPE_BIT_GRAVEL = 0x400LL,
SURFACE_TYPE_BIT_ICE_SOLID = 0x800LL,
SURFACE_TYPE_BIT_METAL_THICK = 0x1000LL,
SURFACE_TYPE_BIT_METAL_GRATE = 0x2000LL,
SURFACE_TYPE_BIT_MUD = 0x4000LL,
SURFACE_TYPE_BIT_PAPER = 0x8000LL,
SURFACE_TYPE_BIT_PLASTER = 0x10000LL,
SURFACE_TYPE_BIT_ROCK = 0x20000LL,
SURFACE_TYPE_BIT_SAND = 0x40000LL,
SURFACE_TYPE_BIT_SNOW = 0x80000LL,
SURFACE_TYPE_BIT_WATER = 0x100000LL,
SURFACE_TYPE_BIT_WOOD_SOLID = 0x200000LL,
SURFACE_TYPE_BIT_ASPHALT_DRY = 0x400000LL,
SURFACE_TYPE_BIT_CERAMIC = 0x800000LL,
SURFACE_TYPE_BIT_PLASTIC = 0x1000000LL,
SURFACE_TYPE_BIT_RUBBER = 0x2000000LL,
SURFACE_TYPE_BIT_FRUIT = 0x4000000LL,
SURFACE_TYPE_BIT_METAL_PAINTED = 0x8000000LL,
SURFACE_TYPE_BIT_RIOTSHIELD = 0x10000000LL,
SURFACE_TYPE_BIT_SLUSH = 0x20000000LL,
SURFACE_TYPE_BIT_ASPHALT_WET = 0x40000000LL,
SURFACE_TYPE_BIT_CARPET_WOOD = 0x80000000LL,
SURFACE_TYPE_BIT_CARPET_METAL = 0x100000000LL,
SURFACE_TYPE_BIT_CONCRETE_WET = 0x200000000LL,
SURFACE_TYPE_BIT_CUSHION = 0x400000000LL,
SURFACE_TYPE_BIT_DEFAULT = 0x800000000LL,
SURFACE_TYPE_BIT_FOLIAGE_WET = 0x1000000000LL,
SURFACE_TYPE_BIT_GLASS_BROKEN = 0x2000000000LL,
SURFACE_TYPE_BIT_GLASS_SOLID = 0x4000000000LL,
SURFACE_TYPE_BIT_GLASS_VEHICLE = 0x8000000000LL,
SURFACE_TYPE_BIT_GRASS_TALL = 0x10000000000LL,
SURFACE_TYPE_BIT_ICE_THIN = 0x20000000000LL,
SURFACE_TYPE_BIT_METAL_THIN = 0x40000000000LL,
SURFACE_TYPE_BIT_ROBOT_HEAD = 0x80000000000LL,
SURFACE_TYPE_BIT_ROBOT_TORSO = 0x100000000000LL,
SURFACE_TYPE_BIT_ROBOT_LIMB = 0x200000000000LL,
SURFACE_TYPE_BIT_ROBOT_ARMOR = 0x400000000000LL,
SURFACE_TYPE_BIT_BIG_ROBOT_HEAD = 0x800000000000LL,
SURFACE_TYPE_BIT_BIG_ROBOT_LIMB = 0x1000000000000LL,
SURFACE_TYPE_BIT_WATER_KNEE = 0x2000000000000LL,
SURFACE_TYPE_BIT_WATER_WAIST = 0x4000000000000LL,
SURFACE_TYPE_BIT_WOOD_FLOOR = 0x8000000000000LL,
SURFACE_TYPE_BIT_USER_TERRAIN_1 = 0x10000000000000LL,
SURFACE_TYPE_BIT_USER_TERRAIN_2 = 0x20000000000000LL,
SURFACE_TYPE_BIT_USER_TERRAIN_3 = 0x40000000000000LL,
SURFACE_TYPE_BIT_USER_TERRAIN_4 = 0x80000000000000LL,
SURFACE_TYPE_BIT_USER_TERRAIN_5 = 0x100000000000000LL,
SURFACE_TYPE_BIT_USER_TERRAIN_6 = 0x200000000000000LL,
SURFACE_TYPE_BIT_USER_TERRAIN_7 = 0x400000000000000LL,
SURFACE_TYPE_BIT_USER_TERRAIN_8 = 0x800000000000000LL,
SURFACE_TYPE_BIT_USER_TERRAIN_9 = 0x1000000000000000LL,
SURFACE_TYPE_BIT_USER_TERRAIN_10 = 0x2000000000000000LL,
SURFACE_TYPE_BIT_CODE_RESERVED = 0x4000000000000000LL,
SURFACE_TYPE_BIT_OPAQUEGLASS = 0x8000000000000000LL,
}
;

enum XModelCharCollBoundsType : unsigned __int8
{
CharCollBoundsType_None = 0x0,
CharCollBoundsType_Human = 0x1,
CharCollBoundsType_C12 = 0x2,
CharCollBoundsType_C8 = 0x3,
CharCollBoundsType_Seeker = 0x4,
CharCollBoundsType_Zombie = 0x5,
CharCollBoundsType_LargeZombie = 0x6,
CharCollBoundsType_Alien = 0x7,
CharCollBoundsType_LargeAlien = 0x8,
CharCollBoundsType_ExtraLargeZombie = 0x9,
CharCollBoundsType_Num = 0xA,
}
;

enum TechsetdefRenderFlags : unsigned __int8
{
TECHSETDEF_RENDER_FLAGS_NONE = 0x0,
TECHSETDEF_RENDER_FLAGS_2D = 0x1,
TECHSETDEF_RENDER_FLAGS_CASTS_SHADOWS = 0x2,
}
;

struct MaterialInfo
{
const char *name;
unsigned __int8 gameFlags;
unsigned __int8 unkFlags;
unsigned __int8 sortKey;
unsigned __int8 textureAtlasRowCount;
unsigned __int8 textureAtlasColumnCount;
unsigned __int8 textureAtlasFrameBlend;
unsigned __int8 textureAtlasAsArray;
unsigned __int8 renderFlags;
GfxDrawSurf drawSurf;
SurfaceTypeBits surfaceTypeBits;
unsigned int hashIndex;
}
;

union Material::$A6AEF759CECCEA8408FA9431437CADFF
{
const char *name;
MaterialInfo info;
}
;

struct GfxPixelShaderLoadDef
{
unsigned __int8 *program;
unsigned int programSize;
unsigned int microCodeCrc;
}
;

struct MaterialPixelShaderProgram
{
ID3D11PixelShader *ps;
GfxPixelShaderLoadDef loadDef;
}
;

struct GfxVertexShaderLoadDef
{
unsigned __int8 *program;
unsigned int programSize;
unsigned int microCodeCrc;
}
;

struct MaterialVertexShaderProgram
{
ID3D11VertexShader *vs;
GfxVertexShaderLoadDef loadDef;
}
;

struct GfxHullShaderLoadDef
{
unsigned __int8 *program;
unsigned int programSize;
}
;

struct MaterialHullShaderProgram
{
ID3D11HullShader *hs;
GfxHullShaderLoadDef loadDef;
}
;

struct GfxDomainShaderLoadDef
{
unsigned __int8 *program;
unsigned int programSize;
}
;

struct MaterialDomainShaderProgram
{
ID3D11DomainShader *ds;
GfxDomainShaderLoadDef loadDef;
}
;

struct MaterialStreamRouting
{
unsigned __int8 source;
unsigned __int8 dest;
unsigned __int8 mask;
}
;

struct MaterialVertexStreamRouting
{
MaterialStreamRouting data[34];
ID3D11InputLayout *decl[270];
}
;

struct MaterialArgumentCodeConst
{
unsigned __int16 index;
unsigned __int8 firstRow;
unsigned __int8 rowCount;
}
;

union MaterialArgumentDef
{
const float *literalConst;
MaterialArgumentCodeConst codeConst;
unsigned int codeSampler;
unsigned int nameHash;
}
;

enum MaterialShaderArgumentType : unsigned __int8
{
}
;

struct MaterialPass
{
MaterialVertexShader *vertexShader;
MaterialVertexDeclaration *vertexDecl;
MaterialHullShader *hullShader;
MaterialDomainShader *domainShader;
MaterialPixelShader *pixelShader;
unsigned __int8 pixelOutputMask;
unsigned __int8 perPrimArgCount;
unsigned __int8 perObjArgCount;
unsigned __int8 stableArgCount;
unsigned __int16 perPrimArgSize;
unsigned __int16 perObjArgSize;
unsigned __int16 stableArgSize;
unsigned __int16 zone;
unsigned __int8 perPrimConstantBuffer;
unsigned __int8 perObjConstantBuffer;
unsigned __int8 stableConstantBuffer;
unsigned int customBufferFlags;
unsigned __int8 customSamplerFlags;
unsigned __int8 precompiledIndex;
unsigned __int8 stageConfig;
char __pad0[33];
MaterialShaderArgument *args;
}
;

struct MaterialTechniqueHeader
{
const char *name;
unsigned __int16 flags;
unsigned __int16 passCount;
}
;

enum MapType : unsigned __int8
{
MAPTYPE_NONE = 0x0,
MAPTYPE_INVALID1 = 0x1,
MAPTYPE_1D = 0x2,
MAPTYPE_2D = 0x3,
MAPTYPE_3D = 0x4,
MAPTYPE_CUBE = 0x5,
MAPTYPE_ARRAY = 0x6,
MAPTYPE_CUBE_ARRAY = 0x7,
MAPTYPE_COUNT = 0x8,
}
;

enum TextureSemantic : unsigned __int8
{
TS_2D = 0x0,
TS_FUNCTION = 0x1,
TS_COLOR_MAP = 0x2,
TS_DETAIL_MAP = 0x3,
TS_UNUSED_4 = 0x4,
TS_NORMAL_MAP = 0x5,
TS_UNUSED_6 = 0x6,
TS_UNUSED_7 = 0x7,
TS_SPECULAR_MAP = 0x8,
TS_SPECULAR_OCCLUSION_MAP = 0x9,
TS_UNUSED_10 = 0xA,
TS_THINFILM_MAP = 0xB,
TS_DISPLACEMENT_MAP = 0xC,
TS_PARALLAX_MAP = 0xD,
TS_COLOR_SPECULAR_MAP = 0xE,
TS_NORMAL_OCCLUSSION_GLOSS_MAP = 0xF,
TS_ALPHA_REVEAL_THICKNESS_MAP = 0x10,
TS_COUNT = 0x11,
}
;

enum GfxImageCategory : unsigned __int8
{
IMG_CATEGORY_UNKNOWN = 0x0,
IMG_CATEGORY_AUTO_GENERATED = 0x1,
IMG_CATEGORY_LIGHTMAP = 0x2,
IMG_CATEGORY_LOAD_FROM_FILE = 0x3,
IMG_CATEGORY_RAW = 0x4,
IMG_CATEGORY_FIRST_UNMANAGED = 0x5,
IMG_CATEGORY_RENDERTARGET = 0x5,
IMG_CATEGORY_TEMP = 0x6,
}
;

struct GfxImageLoadDef
{
char levelCount;
char numElements;
char pad[2];
int flags;
int format;
int resourceSize;
char data[1];
}
;

union GfxTexture::$47DFA868630A633A4A90C602B4D45932
{
ID3D11Texture1D *linemap;
ID3D11Texture2D *map;
ID3D11Texture3D *volmap;
ID3D11Texture2D *cubemap;
GfxImageLoadDef *loadDef;
}
;

struct GfxTexture
{
union
{
ID3D11Texture1D *linemap;
ID3D11Texture2D *map;
ID3D11Texture3D *volmap;
ID3D11Texture2D *cubemap;
GfxImageLoadDef *loadDef;
};
ID3D11ShaderResourceView *shaderView;
ID3D11ShaderResourceView *shaderViewAlternate;
}
;

struct PicMip
{
unsigned __int8 platform[2];
}
;

struct CardMemory
{
unsigned __int8 platform[2];
}
;

typedef uint32_t SndStringHash
;

enum SndVolMod
{
SND_VOLMOD_HUD = 0x0,
SND_VOLMOD_INTERFACE = 0x1,
SND_VOLMOD_INTERFACE_MUSIC = 0x2,
SND_VOLMOD_MUSIC = 0x3,
SND_VOLMOD_MUSIC_EMITTER = 0x4,
SND_VOLMOD_MUSIC_EMITTER_PA = 0x5,
SND_VOLMOD_AMBIENCE = 0x6,
SND_VOLMOD_AMBIENCE_DIST = 0x7,
SND_VOLMOD_ELEMENT = 0x8,
SND_VOLMOD_EMITTER = 0x9,
SND_VOLMOD_PHYSICS = 0xA,
SND_VOLMOD_BODYFALL = 0xB,
SND_VOLMOD_FOLEY_PLR = 0xC,
SND_VOLMOD_FOLEYMP_PLR = 0xD,
SND_VOLMOD_FOLEY_NPC = 0xE,
SND_VOLMOD_FOLEYMP_NPC = 0xF,
SND_VOLMOD_FOLEY_WPN_PLR = 0x10,
SND_VOLMOD_FOLEY_WPN_NPC = 0x11,
SND_VOLMOD_FOOTSTEP_PLR = 0x12,
SND_VOLMOD_FOOTSTEP_NPC = 0x13,
SND_VOLMOD_FOOTSTEPMP_PLR = 0x14,
SND_VOLMOD_FOOTSTEPMP_NPC = 0x15,
SND_VOLMOD_MELEE_PLR = 0x16,
SND_VOLMOD_MELEE_NPC = 0x17,
SND_VOLMOD_CHATTERAL = 0x18,
SND_VOLMOD_CHATTERAX = 0x19,
SND_VOLMOD_REACTIONAL = 0x1A,
SND_VOLMOD_REACTIONAX = 0x1B,
SND_VOLMOD_VOICEOVER = 0x1C,
SND_VOLMOD_VOICEOVER_RADIO = 0x1D,
SND_VOLMOD_VOICEOVER_RADIO_3D = 0x1E,
SND_VOLMOD_VOICEOVER_CRITICAL = 0x1F,
SND_VOLMOD_VOICEOVER_AMB = 0x20,
SND_VOLMOD_VOICEOVER_PLR = 0x21,
SND_VOLMOD_DESTRUCT = 0x22,
SND_VOLMOD_EXPLOSION = 0x23,
SND_VOLMOD_EXPLOSION_GRENADE = 0x24,
SND_VOLMOD_EXPLOSION_FLASHBANG = 0x25,
SND_VOLMOD_EXPLOSION_ROCKET = 0x26,
SND_VOLMOD_EXPLOSION_CAR = 0x27,
SND_VOLMOD_REX_EMITTERS = 0x28,
SND_VOLMOD_IMPACT = 0x29,
SND_VOLMOD_IMPACT_PLR = 0x2A,
SND_VOLMOD_IMPACT_NPC = 0x2B,
SND_VOLMOD_IMPACT_PLR_TO_NPC = 0x2C,
SND_VOLMOD_IMPACTMP = 0x2D,
SND_VOLMOD_IMPACTMP_PLR = 0x2E,
SND_VOLMOD_IMPACTMP_NPC = 0x2F,
SND_VOLMOD_IMPACTMP_PLR_TO_NPC = 0x30,
SND_VOLMOD_WHIZBY = 0x31,
SND_VOLMOD_WHIZBYMP = 0x32,
SND_VOLMOD_VEHICLE_PLR = 0x33,
SND_VOLMOD_VEHICLE_NPC = 0x34,
SND_VOLMOD_VEHICLE_WPN_PLR = 0x35,
SND_VOLMOD_VEHICLE_WPN_NPC = 0x36,
SND_VOLMOD_VEHICLE = 0x37,
SND_VOLMOD_GRENADEBOUNCE = 0x38,
SND_VOLMOD_GRENADEBOUNCEMP = 0x39,
SND_VOLMOD_SHELLCASINGS = 0x3A,
SND_VOLMOD_SHELLCASINGSMP = 0x3B,
SND_VOLMOD_WPN_PLR = 0x3C,
SND_VOLMOD_WPNMP_PLR = 0x3D,
SND_VOLMOD_WPN_NPC = 0x3E,
SND_VOLMOD_WPNMP_NPC = 0x3F,
SND_VOLMOD_WPN_PROJECTILE = 0x40,
SND_VOLMOD_WPNMP_PROJECTILE = 0x41,
SND_VOLMOD_WPN_REFLECTIONS = 0x42,
SND_VOLMOD_WPN_REFLECTIONS_MID = 0x43,
SND_VOLMOD_WPN_REFLECTIONS_FAR = 0x44,
SND_VOLMOD_NA = 0x45,
SND_VOLMOD_MAX = 0x46,
SND_VOLMOD_SCRIPTED1 = 0x47,
SND_VOLMOD_SCRIPTED2 = 0x48,
SND_VOLMOD_SCRIPTED3 = 0x49,
SND_VOLMOD_SCRIPTED4 = 0x4A,
SND_VOLMOD_SCRIPTED5 = 0x4B,
SND_VOLMOD_FULLVOLUME = 0x4C,
SND_VOLMOD_PERKMP_QUIET = 0x4D,
SND_VOLMOD_DEATHSDOOR = 0x4E,
SND_VOLMOD_DEFAULT = 0x4F,
}
;

enum SndChannel
{
SND_CHANNEL_SCN_FX_UNRES_3D = 0x0,
SND_CHANNEL_SCN_LFE_UNRES_3D = 0x1,
SND_CHANNEL_SCN_FX_UNRES_2D = 0x2,
SND_CHANNEL_SCN_LFE_UNRES_2D = 0x3,
SND_CHANNEL_SCN_FX_SPECIAL_UNRES_3D = 0x4,
SND_CHANNEL_SCN_FX_SPECIAL_UNRES_2D = 0x5,
SND_CHANNEL_SCN_FX_UNRES_3D_LIM = 0x6,
SND_CHANNEL_SCN_FX_UNRES_2D_LIM = 0x7,
SND_CHANNEL_SCN_FX_RES_3D = 0x8,
SND_CHANNEL_SCN_FX_RES_2D = 0x9,
SND_CHANNEL_MELEE_PLR_2D = 0xA,
SND_CHANNEL_WEAP_PLR_FIRE_1_2D = 0xB,
SND_CHANNEL_WEAP_PLR_FIRE_2_2D = 0xC,
SND_CHANNEL_WEAP_PLR_FIRE_3_2D = 0xD,
SND_CHANNEL_WEAP_PLR_FIRE_4_2D = 0xE,
SND_CHANNEL_WEAP_PLR_FIRE_OVERLAP_2D = 0xF,
SND_CHANNEL_WEAP_PLR_FIRE_LFE_2D = 0x10,
SND_CHANNEL_WEAP_PLR_FIRE_ALT_1_2D = 0x11,
SND_CHANNEL_WEAP_PLR_FIRE_ALT_2_2D = 0x12,
SND_CHANNEL_WEAP_PLR_FIRE_ALT_3_2D = 0x13,
SND_CHANNEL_WEAP_PLR_FIRE_ALT_4_2D = 0x14,
SND_CHANNEL_WEAP_PLR_FIRE_ALT_OVERLAP_2D = 0x15,
SND_CHANNEL_WEAP_PLR_FIRE_ALT_LFE_2D = 0x16,
SND_CHANNEL_FOLEY_PLR_STEP_2D = 0x17,
SND_CHANNEL_FOLEY_PLR_STEP_UNRES_2D = 0x18,
SND_CHANNEL_FOLEY_PLR_MVMT_UNRES_2D_LIM = 0x19,
SND_CHANNEL_FOLEY_PLR_WEAP_2D_LIM = 0x1A,
SND_CHANNEL_RELOAD_PLR_RES_2D = 0x1B,
SND_CHANNEL_RELOAD_PLR_UNRES_2D = 0x1C,
SND_CHANNEL_RELOAD_PLR_AKIMBO_RES_2D = 0x1D,
SND_CHANNEL_RELOAD_PLR_AKIMBO_UNRES_2D = 0x1E,
SND_CHANNEL_SPEAR_REFL_CLOSE_UNRES_3D_LIM = 0x1F,
SND_CHANNEL_SPEAR_REFL_UNRES_3D_LIM = 0x20,
SND_CHANNEL_WEAP_NPC_MAIN_3D = 0x21,
SND_CHANNEL_WEAP_NPC_MECH_3D = 0x22,
SND_CHANNEL_WEAP_NPC_MID_3D = 0x23,
SND_CHANNEL_WEAP_NPC_LFE_3D = 0x24,
SND_CHANNEL_WEAP_NPC_DIST_3D = 0x25,
SND_CHANNEL_WEAP_NPC_LO_3D = 0x26,
SND_CHANNEL_MELEE_NPC_3D = 0x27,
SND_CHANNEL_PLR_USE_MISC_UNRES_2D = 0x28,
SND_CHANNEL_PLR_UI_INGAME_UNRES_2D = 0x29,
SND_CHANNEL_PLR_USE_PS4SPKR_RES_2D = 0x2A,
SND_CHANNEL_EQUIP_USE_UNRES_3D = 0x2B,
SND_CHANNEL_EQUIP_UI_INGAME_UNRES_3D = 0x2C,
SND_CHANNEL_ABILITY_PLR_UNRES_2D = 0x2D,
SND_CHANNEL_ABILITY_NPC_UNRES_3D = 0x2E,
SND_CHANNEL_EXPLO_1_3D = 0x2F,
SND_CHANNEL_EXPLO_2_3D = 0x30,
SND_CHANNEL_EXPLO_3_3D = 0x31,
SND_CHANNEL_EXPLO_4_3D = 0x32,
SND_CHANNEL_EXPLO_5_3D = 0x33,
SND_CHANNEL_EXPLO_LFE_3D = 0x34,
SND_CHANNEL_EXPLO_DIST_1_3D = 0x35,
SND_CHANNEL_EXPLO_DIST_2_3D = 0x36,
SND_CHANNEL_FOLEY_C12_STEP_3D = 0x37,
SND_CHANNEL_FOLEY_C12_LFE_3D = 0x38,
SND_CHANNEL_PROJECTILE_LOOP_CLOSE = 0x39,
SND_CHANNEL_PROJECTILE_LOOP_MID = 0x3A,
SND_CHANNEL_PROJECTILE_LOOP_DIST = 0x3B,
SND_CHANNEL_WHIZBY_IN_UNRES_3D_LIM = 0x3C,
SND_CHANNEL_WHIZBY_OUT_UNRES_3D_LIM = 0x3D,
SND_CHANNEL_BULLETIMPACT_UNRES_3D_LIM = 0x3E,
SND_CHANNEL_BEAMIMPACT_UNRES_3D_LIM = 0x3F,
SND_CHANNEL_BULLETIMPACT_LO_UNRES_3D_LIM = 0x40,
SND_CHANNEL_BULLET_RICOCHETS_UNRES_3D_LIM = 0x41,
SND_CHANNEL_BULLET_RICO_REFLECT_UNRES_3D_LIM = 0x42,
SND_CHANNEL_BULLETFLESH_1_UNRES_3D_LIM = 0x43,
SND_CHANNEL_BULLETFLESH_2_UNRES_3D_LIM = 0x44,
SND_CHANNEL_BULLETFLESH_LFE_UNRES_2D_LIM = 0x45,
SND_CHANNEL_BULLETFLESH_NPC_1_UNRES_3D_LIM = 0x46,
SND_CHANNEL_BULLETFLESH_NPC_2_UNRES_3D_LIM = 0x47,
SND_CHANNEL_BULLETFLESH_NPCNPC1_UNRES_3D_LIM = 0x48,
SND_CHANNEL_BULLETFLESH_NPCNPC2_UNRES_3D_LIM = 0x49,
SND_CHANNEL_MELEE_IMPACT_3D = 0x4A,
SND_CHANNEL_FOLEY_NPC_STEP_3D = 0x4B,
SND_CHANNEL_FOLEY_NPC_STEP_UNRES_3D = 0x4C,
SND_CHANNEL_FOLEY_NPC_ZMB_STEP_3D_LIM = 0x4D,
SND_CHANNEL_FOLEY_NPC_MVMT_UNRES_3D_LIM = 0x4E,
SND_CHANNEL_FOLEY_NPC_WEAP_UNRES_3D_LIM = 0x4F,
SND_CHANNEL_RELOAD_NPC_RES_3D = 0x50,
SND_CHANNEL_RELOAD_NPC_UNRES_3D = 0x51,
SND_CHANNEL_VOICE_AIR_3D = 0x52,
SND_CHANNEL_VOICE_RADIO_3D = 0x53,
SND_CHANNEL_VOICE_RADIO_2D = 0x54,
SND_CHANNEL_VOICE_NARRATION_2D = 0x55,
SND_CHANNEL_VOICE_SPECIAL_2D = 0x56,
SND_CHANNEL_VOICE_WALLA_1_3D = 0x57,
SND_CHANNEL_VOICE_BCHATTER_1_3D = 0x58,
SND_CHANNEL_VOICE_BCHATTER_LO_UNRES_3D_LIM = 0x59,
SND_CHANNEL_VOICE_PLR_2D = 0x5A,
SND_CHANNEL_VOICE_PLR_EFFORTS_2D = 0x5B,
SND_CHANNEL_VOICE_PLR_BREATH_2D = 0x5C,
SND_CHANNEL_VOICE_ANIMAL_1_3D = 0x5D,
SND_CHANNEL_AMB_BED_2D = 0x5E,
SND_CHANNEL_AMB_ELM_UNRES_3D = 0x5F,
SND_CHANNEL_AMB_ELM_INT_UNRES_3D = 0x60,
SND_CHANNEL_AMB_ELM_EXT_UNRES_3D = 0x61,
SND_CHANNEL_AMB_ELM_UNRES_2D_LIM = 0x62,
SND_CHANNEL_AMB_ELM_UNRES_2D = 0x63,
SND_CHANNEL_AMB_ELM_INT_SPECIAL_UNRES_3D = 0x64,
SND_CHANNEL_AMB_ELM_EXT_SPECIAL_UNRES_3D = 0x65,
SND_CHANNEL_BODYFALLS_HI_UNRES_3D_LIM = 0x66,
SND_CHANNEL_VEHICLE_AIR_FLYBY_CLOSE_3D_LIM = 0x67,
SND_CHANNEL_VEHICLE_AIR_LOOPS_3D_LIM = 0x68,
SND_CHANNEL_VEHICLE_AIR_FLYBY_MIDFAR_3D_LIM = 0x69,
SND_CHANNEL_VEHICLE_WEAP_CLOSE_3D_LIM = 0x6A,
SND_CHANNEL_VEHICLE_HI_RES_3D = 0x6B,
SND_CHANNEL_VEHICLE_HI_UNRES_3D = 0x6C,
SND_CHANNEL_VEHICLE_LFE_3D = 0x6D,
SND_CHANNEL_VEHICLE_LO_3D_LIM = 0x6E,
SND_CHANNEL_VEHICLE_UNRES_2D = 0x6F,
SND_CHANNEL_VEHICLE_UNRES_2D_LIM = 0x70,
SND_CHANNEL_SHOCK_NOFILTER_2D = 0x71,
SND_CHANNEL_HURT_NOFILTER_2D = 0x72,
SND_CHANNEL_SHOCK1_NOFILTER_3D = 0x73,
SND_CHANNEL_SHOCK2_NOFILTER_3D = 0x74,
SND_CHANNEL_MUSIC_LR = 0x75,
SND_CHANNEL_MUSIC_LSRS = 0x76,
SND_CHANNEL_MUSICNOPAUSE_LR = 0x77,
SND_CHANNEL_MUSICNOPAUSE_LSRS = 0x78,
SND_CHANNEL_MUS_EMITTER_3D = 0x79,
SND_CHANNEL_MENU_UNRES_2D = 0x7A,
SND_CHANNEL_MENU_1_2D_LIM = 0x7B,
SND_CHANNEL_MENU_2_2D_LIM = 0x7C,
SND_CHANNEL_PHYSICS_HI_UNRES_3D_LIM = 0x7D,
SND_CHANNEL_PHYSICS_LO_UNRES_3D_LIM = 0x7E,
SND_CHANNEL_BODYFALLS_LO_UNRES_3D_LIM = 0x7F,
SND_CHANNEL_SPECIAL_HI_UNRES_1_3D = 0x80,
SND_CHANNEL_SPECIAL_HI_UNRES_1_2D = 0x81,
SND_CHANNEL_SPECIAL_LO_UNRES_1_3D = 0x82,
SND_CHANNEL_SPECIAL_LO_UNRES_1_2D = 0x83,
SND_CHANNEL_PA_SPEAKER = 0x84,
SND_CHANNEL_PA_VOICE = 0x85,
}
;

enum SndAliasType
{
SAT_LOADED = 0x0,
SAT_HYBRID_PCM = 0x1,
SAT_STREAMED = 0x2,
SAT_PRIMED = 0x3,
SAT_COUNT = 0x4,
}
;

struct SndAliasFlags
{
unsigned __int32 looping : 3;
unsigned __int32 reverb : 1;
unsigned __int32 shape : 3;
unsigned __int32 type : 2;
unsigned __int32 channel : 8;
unsigned __int32 unused : 1;
unsigned __int32 ignoreDistanceCheck : 1;
unsigned __int32 precached : 1;
unsigned __int32 MusicContent : 2;
unsigned __int32 GPadOutput : 2;
unsigned __int32 ForceSubtitle : 1;
unsigned __int32 pad : 7;
}
;

struct unk_1453E2FD8
{
SndStringHash id;
const char *name;
float unk3;
float unk4;
float unk5;
float unk6;
int unk7;
int unk8;
int unk9;
float unk10;
float unk11;
int unk12;
}
;

enum SndMusicAssetType
{
SND_MUSIC_INTRO = 0x0,
SND_MUSIC_LOOP = 0x1,
SND_MUSIC_EXIT = 0x2,
}
;

struct cplane_s
{
float normal[3];
float dist;
unsigned __int8 type;
unsigned __int8 pad[3];
}
;

struct ClipInfo
{
int planeCount;
cplane_s *planes;
}
;

struct cStaticModel_s
{
XModel *xmodel;
float origin[3];
float invScaledAxis[3][3];
bool unk1;
bool unk2;
bool hasTransientModel;
bool hasTransientPhysicsAsset;
int pad;
}
;

enum GfxLightType : unsigned __int8
{
GFX_LIGHT_TYPE_NONE = 0x0,
GFX_LIGHT_TYPE_DIR = 0x1,
GFX_LIGHT_TYPE_SPOT = 0x2,
GFX_LIGHT_TYPE_OMNI = 0x3,
GFX_LIGHT_TYPE_COUNT = 0x4,
GFX_LIGHT_TYPE_DIR_SHADOWMAP = 0x4,
GFX_LIGHT_TYPE_SPOT_SHADOWMAP = 0x5,
GFX_LIGHT_TYPE_OMNI_SHADOWMAP = 0x6,
GFX_LIGHT_TYPE_COUNT_WITH_SHADOWMAP_VERSIONS = 0x7,
GFX_LIGHT_TYPE_SPOT_SHADOWMAP_CUCOLORIS = 0x7,
GFX_LIGHT_TYPE_COUNT_WITH_ALL_VERSIONS = 0x8,
}
;

struct ComPrimaryLightEnv
{
unsigned __int16 primaryLightIndices[4];
unsigned __int8 numIndices;
}
;

union PathNodeOrientationUnion
{
pathnode_yaworient_t yaw_orient;
vec3_t angles;
}
;

enum nodeType : unsigned __int16
{
NODE_ERROR = 0x0,
NODE_PATHNODE = 0x1,
NODE_COVER_STAND = 0x2,
NODE_COVER_CROUCH = 0x3,
NODE_COVER_CROUCH_WINDOW = 0x4,
NODE_COVER_PRONE = 0x5,
NODE_COVER_RIGHT = 0x6,
NODE_COVER_LEFT = 0x7,
NODE_AMBUSH = 0x8,
NODE_EXPOSED = 0x9,
NODE_CONCEALMENT_STAND = 0xA,
NODE_CONCEALMENT_CROUCH = 0xB,
NODE_CONCEALMENT_PRONE = 0xC,
NODE_DOOR = 0xD,
NODE_DOOR_INTERIOR = 0xE,
NODE_SCRIPTED = 0xF,
NODE_NEGOTIATION_BEGIN = 0x10,
NODE_NEGOTIATION_END = 0x11,
NODE_TURRET = 0x12,
NODE_GUARD = 0x13,
NODE_PATHNODE_3D = 0x14,
NODE_COVER_3D = 0x15,
NODE_COVER_STAND_3D = 0x16,
UNUSED_0 = 0x17,
UNUSED_1 = 0x18,
NODE_EXPOSED_3D = 0x19,
NODE_SCRIPTED_3D = 0x1A,
NODE_NEGOTIATION_BEGIN_3D = 0x1B,
NODE_NEGOTIATION_END_3D = 0x1C,
NODE_JUMP = 0x1D,
NODE_JUMP_ATTACK = 0x1E,
NODE_COVER_MULTI = 0x1F,
NODE_NUMTYPES = 0x20,
}
;

enum DynEntityType : __int8
{
DYNENT_TYPE_INVALID = 0x0,
DYNENT_TYPE_CLUTTER = 0x1,
DYNENT_TYPE_CLUTTER_NOSHADOW = 0x2,
DYNENT_TYPE_HINGE = 0x3,
DYNENT_TYPE_SCRIPTABLEINST = 0x4,
DYNENT_TYPE_SCRIPTABLEPHYSICS = 0x5,
DYNENT_TYPE_LINKED = 0x6,
DYNENT_TYPE_LINKED_NOSHADOW = 0x7,
DYNENT_TYPE_COUNT = 0x8,
}
;

enum DynEntityBasis
{
DYNENT_BASIS_MODEL = 0x0,
DYNENT_BASIS_BRUSH = 0x1,
DYNENT_BASIS_COUNT = 0x2,
}
;

struct DynEntityLinkToDef
{
int anchorIndex;
float originOffset[3];
float angleOffset[3];
}
;

struct GpuLightGridRequestRecord
{
unsigned int lgvFrame;
unsigned int lgvHistoryEntry;
unsigned int lgvNumProbes;
}
;

struct DynEntityPose
{
GfxPlacement pose;
float radius;
GpuLightGridRequestRecord lastGpuLightGridRequest;
unsigned int numPoses;
GfxPlacement *poses;
char *unk;
char __pad1[1];
bool cachedActive;
char __pad2[6];
}
;

struct DynEntityClient
{
unsigned __int16 flags;
char __pad0[6];
XModel *activeModel;
int physicsSystemId;
int physicsSystemDetailId;
char numPhysicsBodies;
int singlePhysicsBody;
int detailBoundBody;
int unkId;
int dynEntDefId;
int locked;
char __pad3[8];
}
;

struct DynEntityGlobalId
{
char basis;
unsigned int id;
}
;

struct unk_1453E4280
{
char __pad0[8];
}
;

struct unk_1453E4298
{
char __pad0[28];
int unkIndex;
unsigned int unk01Count;
unk_1453E4280 *unk01;
}
;

struct unk_1453E42A8
{
int unkIndex;
int unk;
}
;

struct unk_1453E1130
{
char unk;
}
;

struct unk_1453E24B0
{
XModel *model;
bool neverMoves;
bool dynamicSimulation;
char __pad0[6];
}
;

struct unk_1453E2510
{
ScriptableDef *def;
char __pad0[52];
__int16 entityId;
char __pad1[10];
unk_1453E24B0 unk01;
unsigned int eventStreamBufferSize;
char *eventStreamBuffer;
char __pad2[4];
}
;

struct unk_1453E2520
{
unk_1453E2510 unk01;
char __pad0[16];
}
;

struct unk_1453E2530
{
unk_1453E2510 unk01;
char __pad0[40];
}
;

struct unk_1453E2558
{
int id;
}
;

struct unk_1453E2560
{
int unk01Count;
unk_1453E2558 *unk01;
int unk02Count;
unk_1453E2558 *unk02_1;
unk_1453E2558 *unk02_2;
}
;

enum SpawnerFieldType : unsigned __int8
{
SF_TYPE_UNDEFINED = 0x0,
SF_TYPE_STRING = 0x2,
SF_TYPE_ISTRING = 0x3,
SF_TYPE_VECTOR = 0x4,
SF_TYPE_FLOAT = 0x5,
SF_TYPE_INTEGER = 0x6,
}
;

struct $03A8A7B39FA20F64B5AB79125E07CD62
{
FxSpatialFrame frame;
float radius;
}
;

struct GfxSky
{
int skySurfCount;
int *skyStartSurfs;
GfxImage *skyImage;
unsigned __int8 skySamplerState;
}
;

struct __cppobj GfxRawTexture : GfxTexture
{
}
;

struct GfxReflectionProbeSampleData
{
float unk[8];
}
;

struct GfxLightGridTree
{
unsigned __int8 maxDepth;
int nodeCount;
int leafCount;
int coordMinGridSpace[3];
int coordMaxGridSpace[3];
int coordHalfSizeGridSpace[3];
int defaultColorIndexBitCount;
int defaultLightIndexBitCount;
unsigned int *p_nodeTable;
int leafTableSize;
unsigned __int8 *p_leafTable;
}
;

struct GfxLightGridColorsHDR
{
float rgb[56][3];
}
;

struct GfxProbeData
{
unsigned int data[16];
}
;

struct GfxGpuLightGridProbePosition
{
float origin[3];
}
;

struct GfxLightGridProbeData
{
unsigned int gpuVisibleProbesCount;
GfxGpuLightGridProbePosition *gpuVisibleProbePositions;
GfxProbeData *gpuVisibleProbesData;
void *gpuVisibleProbesBuffer;
void *gpuVisibleProbesView;
void *gpuVisibleProbesRWView;
unsigned int probeCount;
GfxProbeData *probes;
void *probesBuffer;
void *probesView;
void *probesRWView;
GfxGpuLightGridProbePosition *probePositions;
void *probePositionsBuffer;
void *probePositionsView;
unsigned int zoneCount;
GfxGpuLightGridZone *zones;
unsigned int tetrahedronCount;
unsigned int tetrahedronCountVisible;
GfxGpuLightGridTetrahedron *tetrahedrons;
void *tetrahedronBuffer;
void *tetrahedronView;
GfxGpuLightGridTetrahedronNeighbors *tetrahedronNeighbors;
void *tetrahedronNeighborsBuffer;
void *tetrahedronNeighborsView;
GfxGpuLightGridTetrahedronVisibility *tetrahedronVisibility;
void *tetrahedronVisibilityBuffer;
void *tetrahedronVisibilityView;
unsigned int voxelStartTetrahedronCount;
GfxGpuLightGridVoxelStartTetrahedron *voxelStartTetrahedron;
void *voxelStartTetrahedronBuffer;
void *voxelStartTetrahedronView;
}
;

struct GfxLightGrid
{
int unk[9];
int tableVersion;
int paletteVersion;
char rangeExponent8BitsEncoding;
char rangeExponent12BitsEncoding;
char rangeExponent16BitsEncoding;
unsigned __int8 stageCount;
float *stageLightingContrastGain;
unsigned int paletteEntryCount;
int *paletteEntryAddress;
unsigned int paletteBitstreamSize;
unsigned __int8 *paletteBitstream;
GfxLightGridColorsHDR skyLightGridColors;
GfxLightGridColorsHDR defaultLightGridColors;
GfxLightGridTree tree;
GfxLightGridProbeData probeData;
}
;

struct unk_1453E4B08
{
unsigned int unk01Count;
unsigned int *unk01;
unsigned int unk02Count;
unsigned int *unk02;
unsigned int unk03Count;
unsigned __int16 *unk03;
}
;

struct MaterialMemory
{
Material *material;
int memory;
}
;

struct GfxLightRegionAxis
{
float dir[3];
float midPoint;
float halfSize;
}
;

struct GfxLightRegionHull
{
float kdopMidPoint[9];
float kdopHalfSize[9];
unsigned int axisCount;
GfxLightRegionAxis *axis;
}
;

struct GfxLightRegion
{
unsigned int hullCount;
GfxLightRegionHull *hulls;
}
;

struct GfxStaticModelInst
{
Bounds bounds;
float lightingOrigin[3];
}
;

struct GfxPackedPlacement
{
float origin[3];
float axis[3][3];
float scale;
}
;

struct GfxStaticModelVertexLighting
{
unsigned __int8 visibility[4];
unsigned __int16 ambientColorFloat16[4];
unsigned __int16 highlightColorFloat16[4];
}
;

struct GfxStaticModelVertexLightingInfo
{
GfxStaticModelVertexLighting *lightingValues;
ID3D11Buffer *smodelLightingVb;
ID3D11Buffer *lightingValuesVb;
GfxSubdivCache cache;
int flags;
unsigned int numLightingValues;
}
;

struct GfxStaticModelLightmapInfo
{
float offset[2];
float scale[2];
int lightmapIndex;
}
;

enum StaticModelFlag : __int32
{
STATIC_MODEL_FLAG_SCALE_0 = 0x0,
STATIC_MODEL_FLAG_SCALE_1 = 0x1,
STATIC_MODEL_FLAG_SCALE_2 = 0x2,
STATIC_MODEL_FLAG_SCALE_3 = 0x3,
STATIC_MODEL_FLAG_SCALE_4 = 0x4,
STATIC_MODEL_FLAG_SCALE_5 = 0x5,
STATIC_MODEL_FLAG_SCALE_6 = 0x6,
STATIC_MODEL_FLAG_SCALE_7 = 0x7,
STATIC_MODEL_FLAG_SCALE_8 = 0x8,
STATIC_MODEL_FLAG_SCALE_9 = 0x9,
STATIC_MODEL_FLAG_SCALE_10 = 0xA,
STATIC_MODEL_FLAG_SCALE_11 = 0xB,
STATIC_MODEL_FLAG_SCALE_12 = 0xC,
STATIC_MODEL_FLAG_SCALE_13 = 0xD,
STATIC_MODEL_FLAG_SCALE_14 = 0xE,
STATIC_MODEL_FLAG_SCALE_15 = 0xF,
STATIC_MODEL_FLAG_SCALE_MODIFIER_MASK = 0xF,
STATIC_MODEL_FLAG_NO_CAST_SHADOW = 0x10,
STATIC_MODEL_FLAG_GROUND_LIGHTING = 0x20,
STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING = 0x40,
STATIC_MODEL_FLAG_VERTEXLIT_LIGHTING = 0x80,
STATIC_MODEL_FLAG_LIGHTMAP_LIGHTING = 0x100,
STATIC_MODEL_FLAG_ALLOW_FXMARK = 0x200,
STATIC_MODEL_FLAG_REACTIVEMOTION = 0x400,
STATIC_MODEL_FLAG_ANIMATED_VERTS = 0x800,
}
;

struct GfxStaticModelDrawInst
{
GfxPackedPlacement placement;
XModel *model;
GfxStaticModelLightmapInfo modelLightmapInfo;
unsigned __int16 unk0;
unsigned __int16 unk1;
unsigned __int16 unk2;
unsigned __int16 unk3;
unsigned __int16 lightingHandle;
unsigned __int16 unk5;
unsigned __int16 unk6[4];
unsigned __int16 cullDist;
unsigned __int16 flags;
unsigned __int16 unk9;
unsigned __int16 unk10;
unsigned __int16 unk11;
unsigned __int8 unk13;
unsigned __int8 unk14;
unsigned __int8 unk15[4];
unsigned __int16 primaryLightEnvIndex;
unsigned __int8 reflectionProbeIndex;
unsigned __int8 firstMtlSkinIndex;
unsigned __int8 sunShadowFlags;
unsigned __int8 transientZone;
unsigned __int8 unk21;
}
;

struct GfxHeroOnlyLight
{
unsigned __int8 type;
unsigned __int8 unused[3];
float color[3];
float dir[3];
float up[3];
float origin[3];
float radius;
float cosHalfFovOuter;
float cosHalfFovInner;
int exponent;
}
;

typedef void *umbraTomePtr_t
;

struct UmbraGate2
{
char __pad0[40];
}
;

struct GfxWorldVertex
{
float xyz[3];
float binormalSign;
GfxColor color;
float texCoord[2];
float lmapCoord[2];
PackedUnitVec normal;
PackedUnitVec tangent;
}
;

union GfxWorldVertex0Union
{
GfxWorldVertex *vertices;
}
;

struct GfxWorldVertexData
{
GfxWorldVertex *vertices;
ID3D11Buffer *worldVb;
}
;

struct GfxWorldVertexLayerData
{
unsigned __int8 *data;
ID3D11Buffer *layerVb;
}
;

struct unk_1453E3788
{
char __pad0[48];
}
;

enum GestureDirectionalAssetIndex
{
GESTURE_ASSET_LEFT_CENTER = 0x0,
GESTURE_ASSET_LEFT_DOWN = 0x1,
GESTURE_ASSET_LEFT_LEFT = 0x2,
GESTURE_ASSET_LEFT_RIGHT = 0x3,
GESTURE_ASSET_LEFT_UP = 0x4,
GESTURE_ASSET_RIGHT_CENTER = 0x5,
GESTURE_ASSET_RIGHT_DOWN = 0x6,
GESTURE_ASSET_RIGHT_LEFT = 0x7,
GESTURE_ASSET_RIGHT_RIGHT = 0x8,
GESTURE_ASSET_RIGHT_UP = 0x9,
GESTURE_ASSET_IN = 0xA,
GESTURE_ASSET_OUT = 0xB,
GESTURE_ASSET_IN_ADDITIVE = 0xC,
GESTURE_ASSET_OUT_ADDITIVE = 0xD,
GESTURE_ASSET_PITCH = 0xE,
GESTURE_ASSET_YAW = 0xF,
GESTURE_ASSET_WEAPON_CHECK = 0x10,
GESTURE_ASSET_WALK_ADDITIVE = 0x11,
GESTURE_ASSET_NUM = 0x12,
}
;

enum OffhandGestureTypes
{
OHGT_WEAPON = 0x0,
OHGT_PULLBACK = 0x1,
OHGT_THROW = 0x2,
OHGT_DETONATE = 0x3,
OHGT_SHIELD_DEPLOY = 0x4,
OHGT_SHIELD_DEPLOY_WHILE_FIRING = 0x5,
OHGT_SHIELD_FIRE_WEAPON = 0x6,
OHGT_SHIELD_RETRACT_WHILE_FIRING = 0x7,
OHGT_SHIELD_BASH = 0x8,
OHGT_NUM_TYPES = 0x9,
OHGT_ANY_TYPE = 0xA,
}
;

enum ImpactType : __int32
{
IMPACT_TYPE_NONE = 0x0,
IMPACT_TYPE_COUNT = 0x25,
}
;

enum playerAnimType_t : __int32
{
PLAYERANIMTYPE_NONE = 0x0,
PLAYERANIMTYPE_OTHER = 0x1,
PLAYERANIMTYPE_PISTOL = 0x2,
PLAYERANIMTYPE_SMG = 0x3,
PLAYERANIMTYPE_AUTORIFLE = 0x4,
PLAYERANIMTYPE_MG = 0x5,
PLAYERANIMTYPE_SNIPER = 0x6,
PLAYERANIMTYPE_ROCKETLAUNCHER = 0x7,
PLAYERANIMTYPE_EXPLOSIVE = 0x8,
PLAYERANIMTYPE_GRENADE = 0x9,
PLAYERANIMTYPE_TURRET = 0xA,
PLAYERANIMTYPE_C4 = 0xB,
PLAYERANIMTYPE_M203 = 0xC,
PLAYERANIMTYPE_HOLD = 0xD,
PLAYERANIMTYPE_BRIEFCASE = 0xE,
PLAYERANIMTYPE_RIOTSHIELD = 0xF,
PLAYERANIMTYPE_LAPTOP = 0x10,
PLAYERANIMTYPE_THROWINGKNIFE = 0x11,
PLAYERANIMTYPE_MORTAR = 0x12,
PLAYERANIMTYPE_KNIFE = 0x13,
PLAYERANIMTYPE_CLAYMORE = 0x14,
PLAYERANIMTYPE_MINIGUN = 0x15,
PLAYERANIMTYPE_DRILL = 0x16,
PLAYERANIMTYPE_KILLSTREAKTRIGGER = 0x17,
PLAYERANIMTYPE_TROPHYSYSTEM = 0x18,
PLAYERANIMTYPE_GRENADE_TROPHY = 0x19,
PLAYERANIMTYPE_GRENADE_FRISBEE = 0x1A,
PLAYERANIMTYPE_GRENADE_LARGE = 0x1B,
PLAYERANIMTYPE_GRENADE_UNDERHAND = 0x1C,
PLAYERANIMTYPE_GRENADE_CHUCK = 0x1D,
PLAYERANIMTYPE_GRENADE_BASKETBALL = 0x1E,
PLAYERANIMTYPE_GRENADE_SKEEBALL = 0x1F,
PLAYERANIMTYPE_GRENADE_BASEBALL = 0x20,
PLAYERANIMTYPE_CHARGESHIELD = 0x21,
PLAYERANIMTYPE_AXE = 0x22,
PLAYERANIMTYPE_CLAW = 0x23,
PLAYERANIMTYPE_SMG_ENERGY = 0x24,
PLAYERANIMTYPE_ERAD = 0x25,
PLAYERANIMTYPE_AR57 = 0x26,
PLAYERANIMTYPE_KBS = 0x27,
PLAYERANIMTYPE_CRB = 0x28,
PLAYERANIMTYPE_M4 = 0x29,
PLAYERANIMTYPE_ACR = 0x2A,
PLAYERANIMTYPE_M1 = 0x2B,
PLAYERANIMTYPE_SPAS = 0x2C,
PLAYERANIMTYPE_DEVASTATOR = 0x2D,
PLAYERANIMTYPE_NRG = 0x2E,
PLAYERANIMTYPE_GAUNTLET = 0x2F,
PLAYERANIMTYPE_BALL = 0x30,
PLAYERANIMTYPE_FHR = 0x31,
PLAYERANIMTYPE_REVOLVER = 0x32,
PLAYERANIMTYPE_AKE = 0x33,
PLAYERANIMTYPE_RIPPER = 0x34,
PLAYERANIMTYPE_FMG = 0x35,
PLAYERANIMTYPE_SDFLMG = 0x36,
PLAYERANIMTYPE_LMGTURRET = 0x37,
PLAYERANIMTYPE_SDFSHOTTY = 0x38,
PLAYERANIMTYPE_SONIC = 0x39,
PLAYERANIMTYPE_LONGBOW = 0x3A,
PLAYERANIMTYPE_M200 = 0x3B,
PLAYERANIMTYPE_M8 = 0x3C,
PLAYERANIMTYPE_CHARGESHOT = 0x3D,
PLAYERANIMTYPE_ROLLERCOASTERSEATED = 0x3E,
PLAYERANIMTYPE_CANDYBOX = 0x3F,
PLAYERANIMTYPE_C6_SUPER = 0x40,
PLAYERANIMTYPE_UMP45 = 0x41,
PLAYERANIMTYPE_SDFAR = 0x42,
PLAYERANIMTYPE_SDFAR_LEGENDARY = 0x43,
PLAYERANIMTYPE_REVOLVER_LEGENDARY = 0x44,
PLAYERANIMTYPE_ACR_LEGENDARY = 0x45,
PLAYERANIMTYPE_AR57_LEGENDARY = 0x46,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_01 = 0x47,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_02 = 0x48,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_03 = 0x49,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_04 = 0x4A,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_05 = 0x4B,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_06 = 0x4C,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_07 = 0x4D,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_08 = 0x4E,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_09 = 0x4F,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_10 = 0x50,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_11 = 0x51,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_12 = 0x52,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_13 = 0x53,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_14 = 0x54,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_15 = 0x55,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_16 = 0x56,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_17 = 0x57,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_18 = 0x58,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_19 = 0x59,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_20 = 0x5A,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_21 = 0x5B,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_22 = 0x5C,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_23 = 0x5D,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_24 = 0x5E,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_25 = 0x5F,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_26 = 0x60,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_27 = 0x61,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_28 = 0x62,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_29 = 0x63,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_30 = 0x64,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_31 = 0x65,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_32 = 0x66,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_33 = 0x67,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_34 = 0x68,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_35 = 0x69,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_36 = 0x6A,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_37 = 0x6B,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_38 = 0x6C,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_39 = 0x6D,
PLAYERANIMTYPE_ADDITIONAL_WEAPON_40 = 0x6E,
PLAYERANIMTYPE_NUM = 0x6F,
}
;

enum scriptedAnimType_t : __int32
{
SCRIPTEDANIMTYPE_NONE = 0x0,
SCRIPTEDANIMTYPE_OVERCHARGE = 0x1,
SCRIPTEDANIMTYPE_ADRENALINE = 0x2,
SCRIPTEDANIMTYPE_MULTIVISOR = 0x3,
SCRIPTEDANIMTYPE_SURGE = 0x4,
SCRIPTEDANIMTYPE_KINETICWAVE = 0x5,
SCRIPTEDANIMTYPE_PHASESHIFT = 0x6,
SCRIPTEDANIMTYPE_OPTICWAVE = 0x7,
SCRIPTEDANIMTYPE_CAMO = 0x8,
SCRIPTEDANIMTYPE_TELEPORT = 0x9,
SCRIPTEDANIMTYPE_REWIND = 0xA,
SCRIPTEDANIMTYPE_KILLSTREAKCALLOUT = 0xB,
SCRIPTEDANIMTYPE_GESTURE000 = 0xC,
SCRIPTEDANIMTYPE_GESTURE001 = 0xD,
SCRIPTEDANIMTYPE_GESTURE002 = 0xE,
SCRIPTEDANIMTYPE_GESTURE003 = 0xF,
SCRIPTEDANIMTYPE_GESTURE004 = 0x10,
SCRIPTEDANIMTYPE_GESTURE005 = 0x11,
SCRIPTEDANIMTYPE_GESTURE006 = 0x12,
SCRIPTEDANIMTYPE_GESTURE007 = 0x13,
SCRIPTEDANIMTYPE_GESTURE008 = 0x14,
SCRIPTEDANIMTYPE_GESTURE009 = 0x15,
SCRIPTEDANIMTYPE_GESTURE010 = 0x16,
SCRIPTEDANIMTYPE_GESTURE011 = 0x17,
SCRIPTEDANIMTYPE_GESTURE012 = 0x18,
SCRIPTEDANIMTYPE_GESTURE013 = 0x19,
SCRIPTEDANIMTYPE_GESTURE014 = 0x1A,
SCRIPTEDANIMTYPE_GESTURE015 = 0x1B,
SCRIPTEDANIMTYPE_GESTURE016 = 0x1C,
SCRIPTEDANIMTYPE_GESTURE017 = 0x1D,
SCRIPTEDANIMTYPE_GESTURE018 = 0x1E,
SCRIPTEDANIMTYPE_GESTURE019 = 0x1F,
SCRIPTEDANIMTYPE_GESTURE020 = 0x20,
SCRIPTEDANIMTYPE_GESTURE021 = 0x21,
SCRIPTEDANIMTYPE_GESTURE022 = 0x22,
SCRIPTEDANIMTYPE_GESTURE023 = 0x23,
SCRIPTEDANIMTYPE_GESTURE024 = 0x24,
SCRIPTEDANIMTYPE_GESTURE025 = 0x25,
SCRIPTEDANIMTYPE_GESTURE026 = 0x26,
SCRIPTEDANIMTYPE_GESTURE027 = 0x27,
SCRIPTEDANIMTYPE_GESTURE028 = 0x28,
SCRIPTEDANIMTYPE_GESTURE029 = 0x29,
SCRIPTEDANIMTYPE_GESTURE030 = 0x2A,
SCRIPTEDANIMTYPE_GESTURE031 = 0x2B,
SCRIPTEDANIMTYPE_GESTURE032 = 0x2C,
SCRIPTEDANIMTYPE_GESTURE033 = 0x2D,
SCRIPTEDANIMTYPE_GESTURE034 = 0x2E,
SCRIPTEDANIMTYPE_GESTURE035 = 0x2F,
SCRIPTEDANIMTYPE_GESTURE036 = 0x30,
SCRIPTEDANIMTYPE_GESTURE037 = 0x31,
SCRIPTEDANIMTYPE_GESTURE038 = 0x32,
SCRIPTEDANIMTYPE_GESTURE039 = 0x33,
SCRIPTEDANIMTYPE_GESTURE040 = 0x34,
SCRIPTEDANIMTYPE_GESTURE041 = 0x35,
SCRIPTEDANIMTYPE_GESTURE042 = 0x36,
SCRIPTEDANIMTYPE_GESTURE043 = 0x37,
SCRIPTEDANIMTYPE_GESTURE044 = 0x38,
SCRIPTEDANIMTYPE_GESTURE045 = 0x39,
SCRIPTEDANIMTYPE_GESTURE046 = 0x3A,
SCRIPTEDANIMTYPE_GESTURE047 = 0x3B,
SCRIPTEDANIMTYPE_GESTURE048 = 0x3C,
SCRIPTEDANIMTYPE_GESTURE049 = 0x3D,
SCRIPTEDANIMTYPE_GESTURE050 = 0x3E,
SCRIPTEDANIMTYPE_GESTURE051 = 0x3F,
SCRIPTEDANIMTYPE_GESTURE052 = 0x40,
SCRIPTEDANIMTYPE_GESTURE053 = 0x41,
SCRIPTEDANIMTYPE_GESTURE054 = 0x42,
SCRIPTEDANIMTYPE_GESTURE055 = 0x43,
SCRIPTEDANIMTYPE_GESTURE056 = 0x44,
SCRIPTEDANIMTYPE_GESTURE057 = 0x45,
SCRIPTEDANIMTYPE_GESTURE058 = 0x46,
SCRIPTEDANIMTYPE_GESTURE059 = 0x47,
SCRIPTEDANIMTYPE_GESTURE060 = 0x48,
SCRIPTEDANIMTYPE_GESTURE061 = 0x49,
SCRIPTEDANIMTYPE_GESTURE062 = 0x4A,
SCRIPTEDANIMTYPE_GESTURE063 = 0x4B,
SCRIPTEDANIMTYPE_GESTURE064 = 0x4C,
SCRIPTEDANIMTYPE_GESTURE065 = 0x4D,
SCRIPTEDANIMTYPE_GESTURE066 = 0x4E,
SCRIPTEDANIMTYPE_GESTURE067 = 0x4F,
SCRIPTEDANIMTYPE_GESTURE068 = 0x50,
SCRIPTEDANIMTYPE_GESTURE069 = 0x51,
SCRIPTEDANIMTYPE_GESTURE070 = 0x52,
SCRIPTEDANIMTYPE_GESTURE071 = 0x53,
SCRIPTEDANIMTYPE_GESTURE072 = 0x54,
SCRIPTEDANIMTYPE_GESTURE073 = 0x55,
SCRIPTEDANIMTYPE_GESTURE074 = 0x56,
SCRIPTEDANIMTYPE_GESTURE075 = 0x57,
SCRIPTEDANIMTYPE_GESTURE076 = 0x58,
SCRIPTEDANIMTYPE_GESTURE077 = 0x59,
SCRIPTEDANIMTYPE_GESTURE078 = 0x5A,
SCRIPTEDANIMTYPE_GESTURE079 = 0x5B,
SCRIPTEDANIMTYPE_GESTURE080 = 0x5C,
SCRIPTEDANIMTYPE_GESTURE081 = 0x5D,
SCRIPTEDANIMTYPE_GESTURE082 = 0x5E,
SCRIPTEDANIMTYPE_GESTURE083 = 0x5F,
SCRIPTEDANIMTYPE_GESTURE084 = 0x60,
SCRIPTEDANIMTYPE_GESTURE085 = 0x61,
SCRIPTEDANIMTYPE_GESTURE086 = 0x62,
SCRIPTEDANIMTYPE_GESTURE087 = 0x63,
SCRIPTEDANIMTYPE_GESTURE088 = 0x64,
SCRIPTEDANIMTYPE_GESTURE089 = 0x65,
SCRIPTEDANIMTYPE_GESTURE090 = 0x66,
SCRIPTEDANIMTYPE_GESTURE091 = 0x67,
SCRIPTEDANIMTYPE_GESTURE092 = 0x68,
SCRIPTEDANIMTYPE_GESTURE093 = 0x69,
SCRIPTEDANIMTYPE_GESTURE094 = 0x6A,
SCRIPTEDANIMTYPE_GESTURE095 = 0x6B,
SCRIPTEDANIMTYPE_GESTURE096 = 0x6C,
SCRIPTEDANIMTYPE_GESTURE097 = 0x6D,
SCRIPTEDANIMTYPE_GESTURE098 = 0x6E,
SCRIPTEDANIMTYPE_GESTURE099 = 0x6F,
SCRIPTEDANIMTYPE_GESTURE100 = 0x70,
SCRIPTEDANIMTYPE_GESTURE101 = 0x71,
SCRIPTEDANIMTYPE_GESTURE102 = 0x72,
SCRIPTEDANIMTYPE_GESTURE103 = 0x73,
SCRIPTEDANIMTYPE_GESTURE104 = 0x74,
SCRIPTEDANIMTYPE_GESTURE105 = 0x75,
SCRIPTEDANIMTYPE_GESTURE106 = 0x76,
SCRIPTEDANIMTYPE_GESTURE107 = 0x77,
SCRIPTEDANIMTYPE_GESTURE108 = 0x78,
SCRIPTEDANIMTYPE_GESTURE109 = 0x79,
SCRIPTEDANIMTYPE_GESTURE110 = 0x7A,
SCRIPTEDANIMTYPE_GESTURE111 = 0x7B,
SCRIPTEDANIMTYPE_GESTURE112 = 0x7C,
SCRIPTEDANIMTYPE_GESTURE113 = 0x7D,
SCRIPTEDANIMTYPE_GESTURE114 = 0x7E,
SCRIPTEDANIMTYPE_GESTURE115 = 0x7F,
SCRIPTEDANIMTYPE_NUM = 0x80,
}
;

enum weapAnimFiles_t
{
WEAP_ANIM_ROOT = 0x0,
WEAP_ANIM_INNER_ROOT = 0x1,
WEAP_ANIM_GESTURE_IK_TARGET_1 = 0x2,
WEAP_ANIM_GESTURE_IK_TARGET_2 = 0x3,
WEAP_ANIM_GESTURE_IK_TARGET_3 = 0x4,
WEAP_ANIM_GESTURE_IK_TARGET_4 = 0x5,
WEAP_ANIM_IK_NODE = 0x6,
WEAP_ANIM_PROC_NODE = 0x7,
WEAP_ANIM_IK_NODE_VOID = 0x8,
WEAP_ANIM_PROC_NODE_VOID = 0x9,
WEAP_ANIM_RELATIVE_ROOT = 0xA,
WEAP_ANIM_ADDITIVE_ADS_ROOT = 0xB,
WEAP_ANIM_ADDITIVE_ADS_UP = 0xC,
WEAP_ANIM_ADDITIVE_DRAG_LEFT_ROOT = 0xD,
WEAP_ANIM_ADDITIVE_DRAG_LEFT = 0xE,
WEAP_ANIM_ADDITIVE_DRAG_RIGHT_ROOT = 0xF,
WEAP_ANIM_ADDITIVE_DRAG_RIGHT = 0x10,
WEAP_ANIM_ADDITIVE_DRAG_UP_ROOT = 0x11,
WEAP_ANIM_ADDITIVE_DRAG_UP = 0x12,
WEAP_ANIM_ADDITIVE_DRAG_DOWN_ROOT = 0x13,
WEAP_ANIM_ADDITIVE_DRAG_DOWN = 0x14,
WEAP_ANIM_ADDITIVE_SWIM_FORWARD_ROOT = 0x15,
WEAP_ANIM_ADDITIVE_SWIM_FORWARD = 0x16,
WEAP_ANIM_ADDITIVE_SWIM_BACKWARD_ROOT = 0x17,
WEAP_ANIM_ADDITIVE_SWIM_BACKWARD = 0x18,
WEAP_ANIM_ADDITIVE_JUMP_ROOT = 0x19,
WEAP_ANIM_ADDITIVE_JUMP = 0x1A,
WEAP_ANIM_ADDITIVE_JUMP_BOOST = 0x1B,
WEAP_ANIM_ADDITIVE_JUMP_LAND_ROOT = 0x1C,
WEAP_ANIM_ADDITIVE_JUMP_LAND = 0x1D,
WEAP_ANIM_ADDITIVE_WALK_ROOT = 0x1E,
WEAP_ANIM_ADDITIVE_WALK = 0x1F,
WEAP_ANIM_ADDITIVE_WALK_GESTURE_1 = 0x20,
WEAP_ANIM_ADDITIVE_WALK_GESTURE_2 = 0x21,
WEAP_ANIM_ADDITIVE_SPRINT_ROOT = 0x22,
WEAP_ANIM_ADDITIVE_SPRINT_IN = 0x23,
WEAP_ANIM_ADDITIVE_SPRINT_LOOP = 0x24,
WEAP_ANIM_ADDITIVE_SUPER_SPRINT_LOOP = 0x25,
WEAP_ANIM_ADDITIVE_SPRINT_OUT = 0x26,
WEAP_ANIM_ADDITIVE_SPRINT_OFFSET_ROOT = 0x27,
WEAP_ANIM_ADDITIVE_SPRINT_OFFSET = 0x28,
WEAP_ANIM_ADDITIVE_CRAWL_IN_ROOT = 0x29,
WEAP_ANIM_ADDITIVE_CRAWL_IN = 0x2A,
WEAP_ANIM_ADDITIVE_CRAWL_LOOP_ROOT = 0x2B,
WEAP_ANIM_ADDITIVE_CRAWL_LOOP = 0x2C,
WEAP_ANIM_ADDITIVE_CRAWL_LOOP_LEFT_ROOT = 0x2D,
WEAP_ANIM_ADDITIVE_CRAWL_LOOP_LEFT = 0x2E,
WEAP_ANIM_ADDITIVE_CRAWL_LOOP_RIGHT_ROOT = 0x2F,
WEAP_ANIM_ADDITIVE_CRAWL_LOOP_RIGHT = 0x30,
WEAP_ANIM_ADDITIVE_PRONE_DROP_ROOT = 0x31,
WEAP_ANIM_ADDITIVE_PRONE_DROP = 0x32,
WEAP_ANIM_ADDITIVE_FINGER_POSE_LEFT_ROOT = 0x33,
WEAP_ANIM_ADDITIVE_FINGER_POSE_LEFT = 0x34,
WEAP_ANIM_ADDITIVE_FINGER_POSE_RIGHT_ROOT = 0x35,
WEAP_ANIM_ADDITIVE_FINGER_POSE_RIGHT = 0x36,
WEAP_ANIM_ADDITIVE_DEMEANOR_OFFSET_ROOT = 0x37,
WEAP_ANIM_ADDITIVE_DEMEANOR_OFFSET_SAFE = 0x38,
WEAP_ANIM_ADDITIVE_GESTURE_1 = 0x39,
WEAP_ANIM_ADDITIVE_GESTURE_2 = 0x3A,
WEAP_ANIM_ADDITIVE_GESTURE_3 = 0x3B,
WEAP_ANIM_ADDITIVE_GESTURE_4 = 0x3C,
WEAP_ANIM_ADDITIVE_GESTURE_5 = 0x3D,
WEAP_ANIM_ADDITIVE_GESTURE_6 = 0x3E,
WEAP_ANIM_ADDITIVE_GESTURE_7 = 0x3F,
WEAP_ANIM_ADDITIVE_GESTURE_8 = 0x40,
WEAP_ANIM_ADDITIVE_GESTURE_9 = 0x41,
WEAP_ANIM_ADDITIVE_GESTURE_10 = 0x42,
WEAP_ANIM_ADDITIVE_GESTURE_11 = 0x43,
WEAP_ANIM_ADDITIVE_GESTURE_12 = 0x44,
WEAP_ANIM_ADDITIVE_GESTURE_13 = 0x45,
WEAP_ANIM_ADDITIVE_GESTURE_14 = 0x46,
WEAP_ANIM_ADDITIVE_GESTURE_15 = 0x47,
WEAP_ANIM_ADDITIVE_GESTURE_16 = 0x48,
WEAP_ANIM_ADDITIVE_GESTURE_17 = 0x49,
WEAP_ANIM_ADDITIVE_GESTURE_18 = 0x4A,
WEAP_ANIM_ADDITIVE_GESTURE_19 = 0x4B,
WEAP_ANIM_ADDITIVE_GESTURE_20 = 0x4C,
WEAP_ANIM_ADDITIVE_GESTURE_21 = 0x4D,
WEAP_ANIM_ADDITIVE_GESTURE_22 = 0x4E,
WEAP_ANIM_ADDITIVE_GESTURE_23 = 0x4F,
WEAP_ANIM_ADDITIVE_GESTURE_24 = 0x50,
WEAP_ANIM_ADDITIVE_GESTURE_25 = 0x51,
WEAP_ANIM_ADDITIVE_GESTURE_26 = 0x52,
WEAP_ANIM_ADDITIVE_GESTURE_27 = 0x53,
WEAP_ANIM_ADDITIVE_GESTURE_28 = 0x54,
WEAP_ANIM_ADDITIVE_GESTURE_29 = 0x55,
WEAP_ANIM_ADDITIVE_GESTURE_30 = 0x56,
WEAP_ANIM_ADDITIVE_GESTURE_31 = 0x57,
WEAP_ANIM_ADDITIVE_GESTURE_32 = 0x58,
WEAP_ANIM_ADDITIVE_GESTURE_33 = 0x59,
WEAP_ANIM_ADDITIVE_GESTURE_34 = 0x5A,
WEAP_ANIM_ADDITIVE_GESTURE_35 = 0x5B,
WEAP_ANIM_ADDITIVE_GESTURE_36 = 0x5C,
WEAP_ANIM_ADDITIVE_GESTURE_37 = 0x5D,
WEAP_ANIM_ADDITIVE_GESTURE_38 = 0x5E,
WEAP_ANIM_ADDITIVE_GESTURE_39 = 0x5F,
WEAP_ANIM_ADDITIVE_GESTURE_40 = 0x60,
WEAP_ANIM_ADDITIVE_SETTLE_ROOT = 0x61,
WEAP_ANIM_ADDITIVE_SETTLE_ADS = 0x62,
WEAP_ANIM_ADDITIVE_REGENERATE_ROOT = 0x63,
WEAP_ANIM_ADDITIVE_REGENERATE = 0x64,
WEAP_ANIM_ADDITIVE_MAGAZINE_BULLETS_ROOT = 0x65,
WEAP_ANIM_ADDITIVE_MAGAZINE_BULLETS = 0x66,
WEAP_ANIM_ADDITIVE_RECOIL_ROOT = 0x67,
WEAP_ANIM_ADDITIVE_RECOIL = 0x68,
WEAP_ANIM_ADDITIVE_EMPTY_ROOT = 0x69,
WEAP_ANIM_ADDITIVE_EMPTY = 0x6A,
WEAP_ANIM_IDLE = 0x6B,
WEAP_ANIM_EMPTY_IDLE = 0x6C,
WEAP_ANIM_FIRE = 0x6D,
WEAP_ANIM_HOLD_FIRE = 0x6E,
WEAP_ANIM_LASTSHOT = 0x6F,
WEAP_ANIM_RECHAMBER = 0x70,
WEAP_ANIM_GRENADE_PRIME = 0x71,
WEAP_ANIM_GRENADE_PRIME_READY_TO_THROW = 0x72,
WEAP_ANIM_MELEE_FATAL = 0x73,
WEAP_ANIM_MELEE_HIT = 0x74,
WEAP_ANIM_MELEE_MISS = 0x75,
WEAP_ANIM_MELEE_SWIPE = 0x76,
WEAP_ANIM_MELEE_VICTIM_CROUCHING_FATAL = 0x77,
WEAP_ANIM_MELEE_VICTIM_CROUCHING_HIT = 0x78,
WEAP_ANIM_MELEE_VICTIM_CROUCHING_MISS = 0x79,
WEAP_ANIM_MELEE_FATAL_2 = 0x7A,
WEAP_ANIM_MELEE_HIT_2 = 0x7B,
WEAP_ANIM_MELEE_MISS_2 = 0x7C,
WEAP_ANIM_MELEE_SWIPE_2 = 0x7D,
WEAP_ANIM_MELEE_VICTIM_CROUCHING_FATAL_2 = 0x7E,
WEAP_ANIM_MELEE_VICTIM_CROUCHING_HIT_2 = 0x7F,
WEAP_ANIM_MELEE_VICTIM_CROUCHING_MISS_2 = 0x80,
WEAP_ANIM_MELEE_FATAL_3 = 0x81,
WEAP_ANIM_MELEE_HIT_3 = 0x82,
WEAP_ANIM_MELEE_MISS_3 = 0x83,
WEAP_ANIM_MELEE_SWIPE_3 = 0x84,
WEAP_ANIM_MELEE_VICTIM_CROUCHING_FATAL_3 = 0x85,
WEAP_ANIM_MELEE_VICTIM_CROUCHING_HIT_3 = 0x86,
WEAP_ANIM_MELEE_VICTIM_CROUCHING_MISS_3 = 0x87,
WEAP_ANIM_MELEE_FATAL_4 = 0x88,
WEAP_ANIM_MELEE_HIT_4 = 0x89,
WEAP_ANIM_MELEE_MISS_4 = 0x8A,
WEAP_ANIM_MELEE_SWIPE_4 = 0x8B,
WEAP_ANIM_MELEE_VICTIM_CROUCHING_FATAL_4 = 0x8C,
WEAP_ANIM_MELEE_VICTIM_CROUCHING_HIT_4 = 0x8D,
WEAP_ANIM_MELEE_VICTIM_CROUCHING_MISS_4 = 0x8E,
WEAP_ANIM_ALT_MELEE_FATAL = 0x8F,
WEAP_ANIM_ALT_MELEE_HIT = 0x90,
WEAP_ANIM_ALT_MELEE_MISS = 0x91,
WEAP_ANIM_ALT_MELEE_SWIPE = 0x92,
WEAP_ANIM_ALT_MELEE_VICTIM_CROUCHING_FATAL = 0x93,
WEAP_ANIM_ALT_MELEE_VICTIM_CROUCHING_HIT = 0x94,
WEAP_ANIM_ALT_MELEE_VICTIM_CROUCHING_MISS = 0x95,
WEAP_ANIM_ALT_MELEE_FATAL_2 = 0x96,
WEAP_ANIM_ALT_MELEE_HIT_2 = 0x97,
WEAP_ANIM_ALT_MELEE_MISS_2 = 0x98,
WEAP_ANIM_ALT_MELEE_SWIPE_2 = 0x99,
WEAP_ANIM_ALT_MELEE_VICTIM_CROUCHING_FATAL_2 = 0x9A,
WEAP_ANIM_ALT_MELEE_VICTIM_CROUCHING_HIT_2 = 0x9B,
WEAP_ANIM_ALT_MELEE_VICTIM_CROUCHING_MISS_2 = 0x9C,
WEAP_ANIM_ALT_MELEE_FATAL_3 = 0x9D,
WEAP_ANIM_ALT_MELEE_HIT_3 = 0x9E,
WEAP_ANIM_ALT_MELEE_MISS_3 = 0x9F,
WEAP_ANIM_ALT_MELEE_SWIPE_3 = 0xA0,
WEAP_ANIM_ALT_MELEE_VICTIM_CROUCHING_FATAL_3 = 0xA1,
WEAP_ANIM_ALT_MELEE_VICTIM_CROUCHING_HIT_3 = 0xA2,
WEAP_ANIM_ALT_MELEE_VICTIM_CROUCHING_MISS_3 = 0xA3,
WEAP_ANIM_ALT_MELEE_FATAL_4 = 0xA4,
WEAP_ANIM_ALT_MELEE_HIT_4 = 0xA5,
WEAP_ANIM_ALT_MELEE_MISS_4 = 0xA6,
WEAP_ANIM_ALT_MELEE_SWIPE_4 = 0xA7,
WEAP_ANIM_ALT_MELEE_VICTIM_CROUCHING_FATAL_4 = 0xA8,
WEAP_ANIM_ALT_MELEE_VICTIM_CROUCHING_HIT_4 = 0xA9,
WEAP_ANIM_ALT_MELEE_VICTIM_CROUCHING_MISS_4 = 0xAA,
WEAP_ANIM_RELOAD = 0xAB,
WEAP_ANIM_RELOAD_EMPTY = 0xAC,
WEAP_ANIM_RELOAD_START = 0xAD,
WEAP_ANIM_RELOAD_END = 0xAE,
WEAP_ANIM_FAST_RELOAD = 0xAF,
WEAP_ANIM_FAST_RELOAD_EMPTY = 0xB0,
WEAP_ANIM_FAST_RELOAD_START = 0xB1,
WEAP_ANIM_FAST_RELOAD_END = 0xB2,
WEAP_ANIM_RAISE = 0xB3,
WEAP_ANIM_FIRST_RAISE = 0xB4,
WEAP_ANIM_BREACH_RAISE = 0xB5,
WEAP_ANIM_DROP = 0xB6,
WEAP_ANIM_ALT_RAISE = 0xB7,
WEAP_ANIM_ALT_RAISE_ADS = 0xB8,
WEAP_ANIM_ALT_RAISE_AKIMBO = 0xB9,
WEAP_ANIM_ALT_DROP = 0xBA,
WEAP_ANIM_ALT_DROP_ADS = 0xBB,
WEAP_ANIM_ALT_DROP_AKIMBO = 0xBC,
WEAP_ANIM_ALT_OVERRIDE = 0xBD,
WEAP_ANIM_TOGGLE_OVERRIDE = 0xBE,
WEAP_ANIM_SCOPE_TOGGLE_ON = 0xBF,
WEAP_ANIM_SCOPE_TOGGLE_OFF = 0xC0,
WEAP_ANIM_QUICK_RAISE = 0xC1,
WEAP_ANIM_QUICK_DROP = 0xC2,
WEAP_ANIM_EMPTY_RAISE = 0xC3,
WEAP_ANIM_EMPTY_DROP = 0xC4,
WEAP_ANIM_SPRINT_IN = 0xC5,
WEAP_ANIM_SPRINT_IN_CANCEL = 0xC6,
WEAP_ANIM_SPRINT_LOOP = 0xC7,
WEAP_ANIM_SPRINT_OUT = 0xC8,
WEAP_ANIM_STUNNED_START = 0xC9,
WEAP_ANIM_STUNNED_LOOP = 0xCA,
WEAP_ANIM_STUNNED_END = 0xCB,
WEAP_ANIM_DETONATE = 0xCC,
WEAP_ANIM_NIGHTVISION_WEAR = 0xCD,
WEAP_ANIM_NIGHTVISION_REMOVE = 0xCE,
WEAP_ANIM_ADS_FIRE = 0xCF,
WEAP_ANIM_ADS_LASTSHOT = 0xD0,
WEAP_ANIM_ADS_RECHAMBER = 0xD1,
WEAP_ANIM_BLAST_FRONT = 0xD2,
WEAP_ANIM_BLAST_RIGHT = 0xD3,
WEAP_ANIM_BLAST_BACK = 0xD4,
WEAP_ANIM_BLAST_LEFT = 0xD5,
WEAP_ANIM_DODGE_GROUND_BACK = 0xD6,
WEAP_ANIM_DODGE_GROUND_LEFT = 0xD7,
WEAP_ANIM_DODGE_GROUND_RIGHT = 0xD8,
WEAP_ANIM_DODGE_AIR_FORWARD = 0xD9,
WEAP_ANIM_DODGE_AIR_BACK = 0xDA,
WEAP_ANIM_DODGE_AIR_LEFT = 0xDB,
WEAP_ANIM_DODGE_AIR_RIGHT = 0xDC,
WEAP_ANIM_LEAP_IN = 0xDD,
WEAP_ANIM_LEAP_LOOP = 0xDE,
WEAP_ANIM_LEAP_CANCEL = 0xDF,
WEAP_ANIM_LEAP_OUT = 0xE0,
WEAP_ANIM_CHARGE_IN = 0xE1,
WEAP_ANIM_CHARGE_LOOP = 0xE2,
WEAP_ANIM_CHARGE_OUT = 0xE3,
WEAP_ANIM_ADS_CHARGE_IN = 0xE4,
WEAP_ANIM_ADS_CHARGE_LOOP = 0xE5,
WEAP_ANIM_ADS_CHARGE_OUT = 0xE6,
WEAP_ANIM_RECOIL_SETTLE = 0xE7,
WEAP_ANIM_SWIM_LOOP = 0xE8,
WEAP_ANIM_RELOAD_MULTIPLE_1 = 0xE9,
WEAP_ANIM_RELOAD_MULTIPLE_2 = 0xEA,
WEAP_ANIM_RELOAD_MULTIPLE_3 = 0xEB,
WEAP_ANIM_RELOAD_MULTIPLE_4 = 0xEC,
WEAP_ANIM_RELOAD_MULTIPLE_5 = 0xED,
WEAP_ANIM_RELOAD_MULTIPLE_6 = 0xEE,
WEAP_ANIM_RELOAD_MULTIPLE_7 = 0xEF,
WEAP_ANIM_RELOAD_MULTIPLE_8 = 0xF0,
WEAP_ANIM_RELOAD_MULTIPLE_FAST_1 = 0xF1,
WEAP_ANIM_RELOAD_MULTIPLE_FAST_2 = 0xF2,
WEAP_ANIM_RELOAD_MULTIPLE_FAST_3 = 0xF3,
WEAP_ANIM_RELOAD_MULTIPLE_FAST_4 = 0xF4,
WEAP_ANIM_RELOAD_MULTIPLE_FAST_5 = 0xF5,
WEAP_ANIM_RELOAD_MULTIPLE_FAST_6 = 0xF6,
WEAP_ANIM_RELOAD_MULTIPLE_FAST_7 = 0xF7,
WEAP_ANIM_RELOAD_MULTIPLE_FAST_8 = 0xF8,
WEAP_ANIM_ADS_UP = 0xF9,
WEAP_ANIM_ADS_DOWN = 0xFA,
WEAP_ANIM_SHIELD_UP = 0xFB,
WEAP_ANIM_SHIELD_IDLE = 0xFC,
WEAP_ANIM_SHIELD_DOWN = 0xFD,
WEAP_ANIM_RECOIL = 0xFE,
WEAP_ALT_ANIM_ADJUST = 0xFF,
WEAP_ANIM_UNUSED = 0x100,
WEAP_ANIM_3P_FINGER_POSE_LEFT = 0x101,
WEAP_ANIM_3P_FINGER_POSE_RIGHT = 0x102,
WEAP_ANIM_3P_ADDITIVE_ALT_OVERRIDE = 0x103,
NUM_WEAP_ANIMS = 0x104,
}
;

struct AttUnknown01
{
Material *material;
float unk_0;
int unk_1;
int unk_2;
float unk_3;
float unk_4;
float unk_5;
int unk_6;
float unk_7;
float unk_8;
int unk_9;
int unk_10;
int unk_11;
int unk_12;
float unk_13;
float unk_14;
float unk_15;
float unk_16;
float unk_17;
float unk_18;
float unk_19;
float unk_20;
float unk_21;
float unk_22;
float unk_23;
float unk_24;
float unk_25;
float unk_26;
float unk_27;
float unk_28;
float unk_29;
float unk_30;
float unk_31;
float unk_32;
float unk_33;
float unk_34;
float unk_35;
float unk_36;
float unk_37;
float unk_38;
int unk_39;
int unk_40;
int unk_41;
int unk_42;
int unk_43;
int unk_44;
int unk_45;
int unk_46;
bool unk_47;
bool unk_48;
bool pad0;
bool pad1;
}
;

enum WeaponChargeType : __int32
{
}
;

union WeaponAttachment::$8C9EC49DA4939354795B66A02F37984A
{
const char *szInternalName;
const char *name;
}
;

enum hitLocation_t
{
HITLOC_NONE = 0x0,
HITLOC_HELMET = 0x1,
HITLOC_HEAD = 0x2,
HITLOC_NECK = 0x3,
HITLOC_TORSO_UPR = 0x4,
HITLOC_TORSO_LWR = 0x5,
HITLOC_R_ARM_UPR = 0x6,
HITLOC_L_ARM_UPR = 0x7,
HITLOC_R_ARM_LWR = 0x8,
HITLOC_L_ARM_LWR = 0x9,
HITLOC_R_HAND = 0xA,
HITLOC_L_HAND = 0xB,
HITLOC_R_LEG_UPR = 0xC,
HITLOC_L_LEG_UPR = 0xD,
HITLOC_R_LEG_LWR = 0xE,
HITLOC_L_LEG_LWR = 0xF,
HITLOC_R_FOOT = 0x10,
HITLOC_L_FOOT = 0x11,
HITLOC_GUN = 0x12,
HITLOC_SHIELD = 0x13,
HITLOC_ARMOR = 0x14,
HITLOC_SOFT = 0x15,
HITLOC_NUM = 0x16,
HITLOC_LIMB_START = 0x6,
HITLOC_LIMB_END = 0x11,
}
;

union WeaponCompleteDef::$8C9EC49DA4939354795B66A02F37984A
{
const char *szInternalName;
const char *name;
}
;

enum PARTICLE_MODULE_FLAG : unsigned __int32
{
PARTICLE_MODULE_FLAG_DISABLED = 0x1,
PARTICLE_MODULE_FLAG_HAS_CURVES = 0x2,
PARTICLE_MODULE_FLAG_USE_EMITTER_LIFE = 0x4,
PARTICLE_MODULE_FLAG_HAS_ASSETS = 0x8,
PARTICLE_MODULE_FLAG_RANDOMIZE_BETWEEN_CURVES = 0x10,
PARTICLE_MODULE_FLAG_USE_NON_UNIFORM_INTERPOLATION = 0x20,
PARTICLE_MODULE_FLAG_NO_LERP = 0x40,
PARTICLE_MODULE_FLAG_USE_WORLD_SPACE = 0x80,
PARTICLE_MODULE_FLAG_GRAPH_BY_LENGTH = 0x100,
PARTICLE_MODULE_FLAG_UPDATE_EXTERNAL = 0x200,
PARTICLE_MODULE_FLAG_GRAPH_ADDITIVE = 0x400,
PARTICLE_MODULE_FLAG_HAS_LIGHT_DEFS = 0x800,
PARTICLE_MODULE_FLAG_GRAPH_IS_LINEAR = 0x1000,
PARTICLE_MODULE_FLAG_MODEL_IGNORE_EMITTER_ORIENTATION = 0x2000,
PARTICLE_MODULE_FLAG_USE_OCCLUSION_QUERY_WORLD_SIZE = 0x4000,
PARTICLE_MODULE_FLAG_EMISSION_CURVE_SCRUBBED_BY_GAME = 0x8000,
PARTICLE_MODULE_FLAG_ON_IMPACT_BOLT_TO_ENTITY = 0x10000,
PARTICLE_MODULE_FLAG_RAYCAST_COLLIDE_WITH_WZ_TRAIN = 0x20000,
PARTICLE_MODULE_FLAGS_ALL = 0xFFFFFFFF,
}
;

enum PARTICLE_STATE_DEF_FLAG : unsigned __int64
{
PARTICLE_STATE_DEF_FLAG_DISABLED = 0x1LL,
PARTICLE_STATE_DEF_FLAG_HAS_CAMERA_OFFSET = 0x2LL,
PARTICLE_STATE_DEF_FLAG_HAS_SPAWN_SHAPE = 0x4LL,
PARTICLE_STATE_DEF_FLAG_HAS_POSITION_CURVE = 0x8LL,
PARTICLE_STATE_DEF_FLAG_HAS_ROTATION_1D_CURVE = 0x10LL,
PARTICLE_STATE_DEF_FLAG_HAS_ROTATION_3D_CURVE = 0x20LL,
PARTICLE_STATE_DEF_FLAG_HAS_ROTATION_1D_INIT = 0x40LL,
PARTICLE_STATE_DEF_FLAG_HAS_ROTATION_3D_INIT = 0x80LL,
PARTICLE_STATE_DEF_FLAG_HAS_VELOCITY_CURVE_LOCAL1 = 0x100LL,
PARTICLE_STATE_DEF_FLAG_HAS_VELOCITY_CURVE_WORLD1 = 0x200LL,
PARTICLE_STATE_DEF_FLAG_HAS_VELOCITY_CURVE_LOCAL2 = 0x400LL,
PARTICLE_STATE_DEF_FLAG_HAS_VELOCITY_CURVE_WORLD2 = 0x800LL,
PARTICLE_STATE_DEF_FLAG_USE_PHYSICS = 0x1000LL,
PARTICLE_STATE_DEF_FLAG_MIRROR_TEXTURE_HORIZONTALLY = 0x2000LL,
PARTICLE_STATE_DEF_FLAG_MIRROR_TEXTURE_HORIZONTALLY_RANDOM = 0x4000LL,
PARTICLE_STATE_DEF_FLAG_MIRROR_TEXTURE_VERTICALLY = 0x8000LL,
PARTICLE_STATE_DEF_FLAG_MIRROR_TEXTURE_VERTICALLY_RANDOM = 0x10000LL,
PARTICLE_STATE_DEF_FLAG_SORT_PARTICLES = 0x20000LL,
PARTICLE_STATE_DEF_FLAG_HANDLE_ON_IMPACT = 0x40000LL,
PARTICLE_STATE_DEF_FLAG_PLAYER_FACING = 0x80000LL,
PARTICLE_STATE_DEF_FLAG_PLAYER_FACING_LOCK_UP_VECTOR = 0x100000LL,
PARTICLE_STATE_DEF_FLAG_USE_OCCLUSION_QUERY = 0x200000LL,
PARTICLE_STATE_DEF_FLAG_HAS_COLOR = 0x400000LL,
PARTICLE_STATE_DEF_FLAG_HAS_RAY_CAST_PHYSICS = 0x800000LL,
PARTICLE_STATE_DEF_FLAG_0x1000000 = 0x1000000LL,
PARTICLE_STATE_DEF_FLAG_HAS_EMISSIVE_CURVE = 0x2000000LL,
PARTICLE_STATE_DEF_FLAG_HAS_INTENSITY_CURVE = 0x4000000LL,
PARTICLE_STATE_DEF_FLAG_USE_VECTOR_FIELDS = 0x8000000LL,
PARTICLE_STATE_DEF_FLAG_INHERIT_PARENT_VELOCITY = 0x10000000LL,
PARTICLE_STATE_DEF_FLAG_DRAW_WITH_VIEW_MODEL = 0x20000000LL,
PARTICLE_STATE_DEF_FLAG_PLAY_SOUNDS = 0x40000000LL,
PARTICLE_STATE_DEF_FLAG_HAS_CAMERA_OFFSET_POSITION_ONLY = 0x40000000LL,
PARTICLE_STATE_DEF_FLAG_ON_IMPACT_USE_SURFACE_TYPE = 0x80000000LL,
PARTICLE_STATE_DEF_FLAG_IS_SPRITE = 0x100000000LL,
PARTICLE_STATE_DEF_FLAG_HAS_TRANS_SHADOWS = 0x200000000LL,
PARTICLE_STATE_DEF_FLAG_HAS_CHILD_EFFECTS = 0x400000000LL,
PARTICLE_STATE_DEF_FLAG_BLOCKS_SIGHT = 0x800000000LL,
PARTICLE_STATE_DEF_FLAG_HANDLE_TIME_IN_STATE = 0x0LL,
PARTICLE_STATE_DEF_FLAG_SCALE_BY_DISTANCE = 0x0LL,
PARTICLE_STATE_DEF_FLAG_HAS_VECTOR_FIELD_CURVE = 0x0LL,
PARTICLE_STATE_DEF_FLAG_USE_LOCAL_VECTOR_FIELDS_ONLY = 0x0LL,
PARTICLE_STATE_DEF_FLAG_HAS_SHADER_CURVE = 0x0LL,
PARTICLE_STATE_DEF_FLAG_HAS_SIZE_CURVE = 0x0LL,
PARTICLE_STATE_DEF_FLAG_HAS_SIZE_LERP = 0x0LL,
PARTICLE_STATE_DEF_FLAG_HAS_TEMPERATURE_CURVE = 0x0LL,
PARTICLE_STATE_DEF_FLAG_HAS_LIGHTING_FRACTION_CURVE = 0x0LL,
PARTICLE_STATE_DEF_FLAG_HAS_ROTATION_CURVE = 0x30LL,
PARTICLE_STATE_DEF_FLAG_HAS_ROTATION_1D = 0x50LL,
PARTICLE_STATE_DEF_FLAG_HAS_ROTATION_3D = 0xA0LL,
PARTICLE_STATE_DEF_FLAG_HAS_ROTATION = 0xF0LL,
PARTICLE_STATE_DEF_FLAG_HAS_MIRROR_TEXTURE = 0x1E000LL,
PARTICLE_STATE_DEF_FLAG_HAS_VELOCITY_CURVE = 0x300LL,
PARTICLE_STATE_DEF_FLAG_REQUIRES_WORLD_COLLISION = 0x801000LL,
}
;

enum PARTICLE_EMITTER_DEF_FLAG : unsigned __int32
{
PARTICLE_EMITTER_DEF_FLAG_DISABLED = 0x1,
PARTICLE_EMITTER_DEF_FLAG_USE_BURST_MODE = 0x2,
PARTICLE_EMITTER_DEF_FLAG_DRAW_PAST_FOG = 0x4,
PARTICLE_EMITTER_DEF_FLAG_INFINITE_PARTICLE_LIFE = 0x8,
PARTICLE_EMITTER_DEF_FLAG_LOOP_INFINITE_PARTICLES = 0x10,
PARTICLE_EMITTER_DEF_FLAG_HAS_PHYSICS_HEAVY = 0x20,
PARTICLE_EMITTER_DEF_FLAG_HAS_SCRIPTED_INPUTS = 0x40,
PARTICLE_EMITTER_DEF_FLAG_HAS_LIGHTS = 0x80,
PARTICLE_EMITTER_DEF_FLAG_USE_SCRIPTED_VELOCITY = 0x100,
PARTICLE_EMITTER_DEF_FLAG_EMIT_BY_DISTANCE = 0x200,
PARTICLE_EMITTER_DEF_FLAG_USE_OCCLUSION_QUERY = 0x400,
PARTICLE_EMITTER_DEF_FLAG_HAS_TRANS_SHADOWS = 0x800,
PARTICLE_EMITTER_DEF_FLAG_GROUP_ONLY_WHEN_NVG_ON = 0x1000,
PARTICLE_EMITTER_DEF_FLAG_GROUP_ONLY_WHEN_NVG_OFF = 0x2000,
PARTICLE_EMITTER_DEF_FLAG_GROUP_ONLY_WHEN_THERMAL_ON = 0x4000,
PARTICLE_EMITTER_DEF_FLAG_GROUP_ONLY_WHEN_THERMAL_OFF = 0x8000,
PARTICLE_EMITTER_DEF_FLAG_DISABLE_IN_SP = 0x10000,
PARTICLE_EMITTER_DEF_FLAG_DISABLE_IN_MP_LPC = 0x20000,
PARTICLE_EMITTER_DEF_FLAG_DISABLE_IN_MP_HPC = 0x40000,
PARTICLE_EMITTER_DEF_FLAG_DISABLE_IN_CP = 0x80000,
PARTICLE_EMITTER_DEF_FLAG_HAS_INSTANCE_POOL = 0x100000,
PARTICLE_EMITTER_DEF_FLAG_FORCE_HALF_RES = 0x200000,
PARTICLE_EMITTER_DEF_FLAG_FORCE_FULL_RES = 0x400000,
PARTICLE_EMITTER_DEF_FLAG_IGNORE_MODIFIERS = 0x800000,
PARTICLE_EMITTER_DEF_FLAG_HAS_BARREL_HEAT_SCALE = 0x1000000,
PARTICLE_EMITTER_DEF_FLAG_IGNORE_FOV_SCALE = 0x2000000,
}
;

enum PARTICLE_SYSTEM_DEF_FLAG : unsigned __int32
{
PARTICLE_SYSTEM_DEF_FLAG_HAS_SPRITES = 0x1,
PARTICLE_SYSTEM_DEF_FLAG_HAS_NON_SPRITES = 0x2,
PARTICLE_SYSTEM_DEF_FLAG_HAS_LIGHTS = 0x4,
PARTICLE_SYSTEM_DEF_FLAG_HAS_PHYSICS_HEAVY = 0x8,
PARTICLE_SYSTEM_DEF_FLAG_HAS_PHYSICS_LIGHT = 0x10,
PARTICLE_SYSTEM_DEF_FLAG_HAS_SCRIPTED_INPUTS = 0x20,
PARTICLE_SYSTEM_DEF_FLAG_USE_OCCLUSION_QUERY = 0x40,
PARTICLE_SYSTEM_DEF_FLAG_USE_OCCLUSION_QUERY_OVERRIDE_POS = 0x80,
PARTICLE_SYSTEM_DEF_FLAG_KILL_ON_OWNER_ENTITY_DEATH = 0x100,
PARTICLE_SYSTEM_DEF_FLAG_HAS_TRANS_SHADOWS = 0x200,
PARTICLE_SYSTEM_DEF_FLAG_ALIGN_TO_SUN = 0x400,
PARTICLE_SYSTEM_DEF_FLAG_KILL_ON_KILLCAM_TRANSITION = 0x800,
PARTICLE_SYSTEM_DEF_FLAG_CANNOT_PRE_ROLL = 0x1000,
PARTICLE_SYSTEM_DEF_FLAG_ALWAYS_UPDATE_BONE_INDEX = 0x2000,
PARTICLE_SYSTEM_DEF_FLAG_AFFECTS_GAMEPLAY = 0x4000,
PARTICLE_SYSTEM_DEF_FLAG_UPDATE_CHILDREN_AFTER_BOLTING = 0x8000,
PARTICLE_SYSTEM_DEF_FLAG_PRE_ROLL_CHILD_EFFECTS = 0x10000,
PARTICLE_SYSTEM_DEF_FLAG_KILL_ON_KILLCAM_ENTITY_TRANSITION = 0x20000,
PARTICLE_SYSTEM_DEF_FLAG_KILL_STOPPED_INFINITE_EFFECTS = 0x40000,
PARTICLE_SYSTEM_DEF_FLAG_PARENT_UPDATES_CHILD = 0x80000,
PARTICLE_SYSTEM_DEF_FLAG_STOP_ON_DYNAMIC_BOLT_DEATH = 0x100000,
PARTICLE_SYSTEM_DEF_FLAG_DISABLE_IN_SPLIT_SCREEN = 0x200000,
PARTICLE_SYSTEM_DEF_FLAG_HAS_EMITTER_GROUP_IDS = 0x400000,
PARTICLE_SYSTEM_DEF_FLAG_HAS_EMITTER_NVG_OR_THERMAL = 0x800000,
PARTICLE_SYSTEM_DEF_FLAG_HAS_INSTANCE_POOL = 0x1000000,
PARTICLE_SYSTEM_DEF_FLAG_HIDE_IF_BONE_IS_HIDDEN = 0x2000000,
PARTICLE_SYSTEM_DEF_FLAG_FOV_CULL = 0x4000000,
PARTICLE_SYSTEM_DEF_FLAG_USE_OCCLUSION_QUERY_OVERRIDE_LOC = 0x8000000,
}
;

enum ParticleModuleType : unsigned __int16
{
PARTICLE_MODULE_INIT_ATLAS = 0x0,
PARTICLE_MODULE_INIT_ATTRIBUTES = 0x1,
PARTICLE_MODULE_INIT_BEAM = 0x2,
PARTICLE_MODULE_INIT_CAMERA_OFFSET = 0x3,
PARTICLE_MODULE_INIT_CLOUD = 0x4,
PARTICLE_MODULE_INIT_DECAL = 0x5,
PARTICLE_MODULE_INIT_FLARE = 0x6,
PARTICLE_MODULE_INIT_GEO_TRAIL = 0x7,
PARTICLE_MODULE_INIT_LIGHT_OMNI = 0x8,
PARTICLE_MODULE_INIT_LIGHT_SPOT = 0x9,
PARTICLE_MODULE_INIT_MATERIAL = 0xA,
PARTICLE_MODULE_INIT_MIRROR_TEXTURE = 0xB,
PARTICLE_MODULE_INIT_MODEL = 0xC,
PARTICLE_MODULE_INIT_OCCLUSION_QUERY = 0xD,
PARTICLE_MODULE_INIT_ORIENTED_SPRITE = 0xE,
PARTICLE_MODULE_INIT_PARTICLE_SIM = 0xF,
PARTICLE_MODULE_INIT_PLAYER_FACING = 0x10,
PARTICLE_MODULE_INIT_RELATIVE_VELOCITY = 0x11,
PARTICLE_MODULE_INIT_ROTATION = 0x12,
PARTICLE_MODULE_INIT_ROTATION_3D = 0x13,
PARTICLE_MODULE_INIT_RUNNER = 0x14,
PARTICLE_MODULE_INIT_SOUND = 0x15,
PARTICLE_MODULE_INIT_SPAWN = 0x16,
PARTICLE_MODULE_INIT_SPAWN_SHAPE_BOX = 0x17,
PARTICLE_MODULE_INIT_SPAWN_SHAPE_CYLINDER = 0x18,
PARTICLE_MODULE_INIT_SPAWN_SHAPE_ELLIPSOID = 0x19,
PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH = 0x1A,
PARTICLE_MODULE_INIT_SPAWN_SHAPE_SPHERE = 0x1B,
PARTICLE_MODULE_INIT_TAIL = 0x1C,
PARTICLE_MODULE_INIT_VECTOR_FIELD = 0x1D,
PARTICLE_MODULE_INIT_VOLUMETRIC = 0x1E,
PARTICLE_MODULE_ATTRACTOR = 0x1F,
PARTICLE_MODULE_COLOR_GRAPH = 0x20,
PARTICLE_MODULE_COLOR_LERP = 0x21,
PARTICLE_MODULE_EMISSION_GRAPH = 0x22,
PARTICLE_MODULE_EMISSIVE_GRAPH = 0x23,
PARTICLE_MODULE_FORCE = 0x24,
PARTICLE_MODULE_GRAVITY = 0x25,
PARTICLE_MODULE_INTENSITY_GRAPH = 0x26,
PARTICLE_MODULE_PARENT_VELOCITY_GRAPH = 0x27,
PARTICLE_MODULE_PHYSICS_LIGHT = 0x28,
PARTICLE_MODULE_PHYSICS_RAY_CAST = 0x29,
PARTICLE_MODULE_POSITION_GRAPH = 0x2A,
PARTICLE_MODULE_ROTATION_GRAPH = 0x2B,
PARTICLE_MODULE_ROTATION_GRAPH_3D = 0x2C,
PARTICLE_MODULE_SIZE_GRAPH = 0x2D,
PARTICLE_MODULE_SIZE_LERP = 0x2E,
PARTICLE_MODULE_VELOCITY_GRAPH = 0x2F,
PARTICLE_MODULE_TEST_AGE = 0x30,
PARTICLE_MODULE_TEST_BIRTH = 0x31,
PARTICLE_MODULE_TEST_DEATH = 0x32,
PARTICLE_MODULE_TEST_GRAVITY = 0x33,
PARTICLE_MODULE_TEST_IMPACT = 0x34,
PARTICLE_MODULE_TEST_POS = 0x35,
PARTICLE_MODULE_COUNT = 0x36,
PARTICLE_MODULE_INVALID = 0xFF,
}
;

enum PARTICLE_ELEMENT_TYPE : unsigned __int32
{
PARTICLE_ELEMENT_TYPE_BILLBOARD_SPRITE = 0x0,
PARTICLE_ELEMENT_TYPE_BEAM = 0x1,
PARTICLE_ELEMENT_TYPE_CLOUD = 0x2,
PARTICLE_ELEMENT_TYPE_SPARK_CLOUD = 0x3,
PARTICLE_ELEMENT_TYPE_DECAL = 0x4,
PARTICLE_ELEMENT_TYPE_FLARE = 0x5,
PARTICLE_ELEMENT_TYPE_GEO_TRAIL = 0x6,
PARTICLE_ELEMENT_TYPE_LIGHT_OMNI = 0x7,
PARTICLE_ELEMENT_TYPE_LIGHT_SPOT = 0x8,
PARTICLE_ELEMENT_TYPE_MODEL = 0x9,
PARTICLE_ELEMENT_TYPE_ORIENTED_SPRITE = 0xA,
PARTICLE_ELEMENT_TYPE_RUNNER = 0xB,
PARTICLE_ELEMENT_TYPE_TAIL = 0xC,
PARTICLE_ELEMENT_TYPE_VECTOR_FIELD = 0xD,
PARTICLE_ELEMENT_TYPE_VOLUMETRIC = 0xE,
}
;

enum ParticleModuleAxesFlags
{
PARTICLE_MODULE_AXES_FLAG_POS_X = 0x1,
PARTICLE_MODULE_AXES_FLAG_NEG_X = 0x2,
PARTICLE_MODULE_AXES_FLAG_POS_Y = 0x4,
PARTICLE_MODULE_AXES_FLAG_NEG_Y = 0x8,
PARTICLE_MODULE_AXES_FLAG_POS_Z = 0x10,
PARTICLE_MODULE_AXES_FLAG_NEG_Z = 0x20,
PARTICLE_MODULE_AXES_FLAG_ALL_X = 0x3,
PARTICLE_MODULE_AXES_FLAG_ALL_Y = 0xC,
PARTICLE_MODULE_AXES_FLAG_ALL_Z = 0x30,
PARTICLE_MODULE_AXES_FLAG_ALL_POS = 0x15,
PARTICLE_MODULE_AXES_FLAG_ALL_NEG = 0x2A,
PARTICLE_MODULE_AXES_FLAG_ALL = 0x3F,
}
;

enum ParticleSpawnFlags
{
PARTICLE_SPAWN_ONLY_ON_SURFACE = 0x1,
PARTICLE_SPAWN_USE_CURVE_VALUES = 0x2,
PARTICLE_SPAWN_USE_MODIFIER_SCALE = 0x4,
PARTICLE_SPAWN_IS_DETERMINISTIC = 0x8,
PARTICLE_SPAWN_UNIFORM = 0x10,
}
;

enum ParticleUseOrientationOptions
{
PARTICLE_USE_ORIENTATION_OPTION_IDENTITY = 0x0,
PARTICLE_USE_ORIENTATION_OPTION_ROTATION = 0x1,
PARTICLE_USE_ORIENTATION_OPTION_RELATIVE_ROTATION = 0x2,
PARTICLE_USE_ORIENTATION_OPTION_VELOCITY_Y = 0x3,
PARTICLE_USE_ORIENTATION_OPTION_VELOCITY_X = 0x4,
PARTICLE_USE_ORIENTATION_OPTION_VELOCITY_Z = 0x5,
PARTICLE_USE_ORIENTATION_OPTION_RELATIVE = 0x6,
PARTICLE_USE_ORIENTATION_OPTION_COUNT = 0x7,
}
;

enum ParticleUseScaleOptions
{
PARTICLE_USE_SCALE_OPTION_NONE = 0x0,
PARTICLE_USE_SCALE_OPTION_MULT = 0x1,
PARTICLE_USE_SCALE_OPTION_OVERRIDE = 0x2,
PARTICLE_USE_SCALE_OPTION_COUNT = 0x3,
}
;

enum ParticleUseVelocityOptions
{
PARTICLE_USE_VELOCITY_OPTION_NONE = 0x0,
PARTICLE_USE_VELOCITY_OPTION_MULT = 0x1,
PARTICLE_USE_VELOCITY_OPTION_OVERRIDE = 0x2,
PARTICLE_USE_VELOCITY_OPTION_COUNT = 0x3,
}
;

enum ParticleModuleGroup
{
PARTICLE_MODULE_GROUP_INIT = 0x0,
PARTICLE_MODULE_GROUP_UPDATE = 0x1,
PARTICLE_MODULE_GROUP_TEST = 0x2,
PARTICLE_MODULE_GROUP_COUNT = 0x3,
}
;

struct FxFloatRange
{
float base;
float amplitude;
}
;

struct FxIntRange
{
int base;
int amplitude;
}
;

struct FxElemVec3Range
{
float base[3];
float amplitude[3];
}
;

struct FxSpawnDefLooping
{
int intervalMsec;
int count;
}
;

struct FxSpawnDefOneShot
{
FxIntRange count;
}
;

union FxSpawnDef
{
FxSpawnDefLooping looping;
FxSpawnDefOneShot oneShot;
}
;

enum FXElemAtlasBehavior : unsigned __int8
{
FX_ATLAS_START_FIXED = 0x0,
FX_ATLAS_START_RANDOM = 0x1,
FX_ATLAS_START_INDEXED = 0x2,
FX_ATLAS_START_MASK = 0x3,
FX_ATLAS_PLAY_OVER_LIFE = 0x4,
FX_ATLAS_LOOP_ONLY_N_TIMES = 0x8,
}
;

struct FxElemAtlas
{
unsigned __int8 behavior;
unsigned __int8 index;
unsigned __int8 fps;
unsigned __int8 unk;
unsigned __int8 loopCount;
unsigned __int8 colIndexBits;
unsigned __int8 rowIndexBits;
__int16 entryCount;
}
;

struct FxElemVelStateInFrame
{
FxElemVec3Range velocity;
FxElemVec3Range totalDelta;
}
;

struct FxElemVelStateSample
{
FxElemVelStateInFrame local;
FxElemVelStateInFrame world;
}
;

struct FxElemVisualState
{
float color[4];
float rotationDelta;
float rotationTotal;
float size[2];
float scale;
}
;

struct FxElemVisStateSample
{
FxElemVisualState base;
FxElemVisualState amplitude;
}
;

struct FxElemMarkVisuals
{
Material *materials[3];
}
;

union FxEffectDefRef
{
FxEffectDef *handle;
const char *name;
}
;

union FxElemVisuals
{
const void *anonymous;
Material *material;
XModel *model;
FxEffectDefRef effectDef;
const char *soundName;
const char *vectorFieldName;
GfxLightDef *lightDef;
FxParticleSimAnimation *particleSimAnimation;
}
;

union FxElemDefVisuals
{
FxElemMarkVisuals *markArray;
FxElemVisuals *array;
FxElemVisuals instance;
}
;

struct FxTrailVertex
{
vec2_t pos;
vec2_t normal;
vec2_t texCoord;
}
;

struct FxTrailDef
{
int scrollTimeMsec;
int repeatDist;
float invSplitDist;
float invSplitArcDist;
float invSplitTime;
int vertCount;
FxTrailVertex *verts;
int indCount;
unsigned __int16 *inds;
}
;

struct FxSparkFountainDef
{
float gravity;
float bounceFrac;
float bounceRand;
float sparkSpacing;
float sparkLength;
int sparkCount;
float loopTime;
float velMin;
float velMax;
float velConeFrac;
float restSpeed;
float boostTime;
float boostFactor;
}
;

struct FxSpotLightDef
{
float fovInnerFraction;
float startRadius;
float endRadius;
float brightness;
float maxLength;
int exponent;
}
;

struct FxFlareDef
{
float position;
int angularRotCount;
int flags;
FxFloatRange depthScaleRange;
FxFloatRange depthScaleValue;
FxFloatRange radialRot;
FxFloatRange radialScaleX;
FxFloatRange radialScaleY;
vec3_t dir;
int intensityXIntervalCount;
int intensityYIntervalCount;
int srcCosIntensityIntervalCount;
int srcCosScaleIntervalCount;
float *intensityX;
float *intensityY;
float *srcCosIntensity;
float *srcCosScale;
}
;

union FxElemExtendedDefPtr
{
FxTrailDef *trailDef;
FxSparkFountainDef *sparkFountainDef;
FxSpotLightDef *spotLightDef;
FxFlareDef *flareDef;
char *unknownDef;
}
;

enum FxElemType : unsigned __int8
{
FX_ELEM_TYPE_SPRITE_BILLBOARD = 0x0,
FX_ELEM_TYPE_SPRITE_ORIENTED = 0x1,
FX_ELEM_TYPE_TAIL = 0x2,
FX_ELEM_TYPE_TRAIL = 0x3,
FX_ELEM_TYPE_FLARE = 0x4,
FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION = 0x5,
FX_ELEM_TYPE_CLOUD = 0x6,
FX_ELEM_TYPE_SPARK_CLOUD = 0x7,
FX_ELEM_TYPE_SPARK_FOUNTAIN = 0x8,
FX_ELEM_TYPE_MODEL = 0x9,
FX_ELEM_TYPE_OMNI_LIGHT = 0xA,
FX_ELEM_TYPE_SPOT_LIGHT = 0xB,
FX_ELEM_TYPE_SOUND = 0xC,
FX_ELEM_TYPE_DECAL = 0xD,
FX_ELEM_TYPE_RUNNER = 0xE,
FX_ELEM_TYPE_VECTORFIELD = 0xF,
FX_ELEM_TYPE_COUNT = 0x10,
FX_ELEM_TYPE_LAST_SPRITE = 0x5,
FX_ELEM_TYPE_LAST_DRAWN = 0xB,
}
;

enum FxElemDefFlags : unsigned __int32
{
FX_ELEM_SPAWN_RELATIVE_TO_EFFECT = 0x2,
FX_ELEM_SPAWN_FRUSTUM_CULL = 0x4,
FX_ELEM_RUNNER_USES_RAND_ROT = 0x8,
FX_ELEM_SPAWN_OFFSET_NONE = 0x0,
FX_ELEM_SPAWN_OFFSET_SPHERE = 0x10,
FX_ELEM_SPAWN_OFFSET_CYLINDER = 0x20,
FX_ELEM_SPAWN_OFFSET_MASK = 0x30,
FX_ELEM_RUN_RELATIVE_TO_WORLD = 0x0,
FX_ELEM_RUN_RELATIVE_TO_SPAWN = 0x40,
FX_ELEM_RUN_RELATIVE_TO_EFFECT = 0x80,
FX_ELEM_RUN_RELATIVE_TO_OFFSET = 0xC0,
FX_ELEM_RUN_RELATIVE_TO_CAMERA = 0x100,
FX_ELEM_RUN_MASK = 0x1C0,
FX_ELEM_DIE_ON_TOUCH = 0x200,
FX_ELEM_DRAW_PAST_FOG = 0x400,
FX_ELEM_DRAW_WITH_VIEWMODEL = 0x800,
FX_ELEM_BLOCK_SIGHT = 0x1000,
FX_ELEM_DRAW_IN_THERMAL_VIEW_ONLY = 0x2000,
FX_ELEM_TRAIL_ORIENT_BY_VELOCITY = 0x4000,
FX_ELEM_EMIT_BOLT = 0x80000000,
FX_ELEM_EMIT_ORIENT_BY_ELEM = 0x8000,
FX_ELEM_USE_OCCLUSION_QUERY = 0x10000,
FX_ELEM_NODRAW_IN_THERMAL_VIEW = 0x20000,
FX_ELEM_THERMAL_MASK = 0x22000,
FX_ELEM_SPAWN_IMPACT_FX_WITH_SURFACE_NAME = 0x40000,
FX_ELEM_DEPRECATED = 0x80000,
FX_ELEM_VOLUMETRIC_TRAIL = 0x100000,
FX_ELEM_USE_COLLISION = 0x200000,
FX_ELEM_USE_VECTORFIELDS = 0x400000,
FX_ELEM_HAS_VELOCITY_GRAPH_LOCAL = 0x1000000,
FX_ELEM_HAS_VELOCITY_GRAPH_WORLD = 0x2000000,
FX_ELEM_HAS_GRAVITY = 0x4000000,
FX_ELEM_USE_MODEL_PHYSICS = 0x8000000,
FX_ELEM_NONUNIFORM_SCALE = 0x10000000,
FX_ELEM_FOUNTAIN_DISABLE_COLLISION = 0x80000000,
}
;

enum FxElemDefExtraFlags : unsigned __int32
{
FX_ELEM2_BILLBOARD_FACING_CAMERA_PERPENDICULAR = 0x1,
FX_ELEM2_BILLBOARD_FACING_PLAYER = 0x2,
FX_ELEM2_BILLBOARD_FACING_MASK = 0x3,
FX_ELEM2_EMIT_TRAILS = 0x4,
FX_ELEM2_REACTIVE_TURBULANCE = 0x8,
FX_ELEM2_GPU_LIGHTING = 0x10,
FX_ELEM2_GPU_LIGHTING_SH = 0x20,
FX_ELEM2_COLOR_OR_ALPHA_CURVE = 0x40,
}
;

struct FxElemDef
{
int flags;
int flags2;
FxSpawnDef spawn;
FxFloatRange spawnRange;
FxFloatRange fadeInRange;
FxFloatRange fadeOutRange;
float spawnFrustumCullRadius;
FxIntRange spawnDelayMsec;
FxIntRange lifeSpanMsec;
FxFloatRange spawnOrigin[3];
FxFloatRange spawnOffsetRadius;
FxFloatRange spawnOffsetHeight;
FxFloatRange spawnAngles[3];
FxFloatRange angularVelocity[3];
FxFloatRange initialRotation;
FxFloatRange gravity;
FxFloatRange reflectionFactor;
FxElemAtlas atlas;
FxElemType elemType;
unsigned __int8 visualCount;
unsigned __int8 velIntervalCount;
unsigned __int8 visStateIntervalCount;
FxElemVelStateSample *velSamples;
FxElemVisStateSample *visSamples;
FxElemDefVisuals visuals;
Bounds collBounds;
FxEffectDefRef effectOnImpact;
FxEffectDefRef effectOnDeath;
FxEffectDefRef effectEmitted;
FxFloatRange emitDist;
FxFloatRange emitDistVariance;
FxElemExtendedDefPtr extended;
unsigned __int8 sortOrder;
unsigned __int8 lightingFrac;
unsigned __int8 useItemClip;
unsigned __int8 fadeInfo;
int randomSeed;
float litUnlitBlendFactor;
float pad;
}
;

enum FxEffectDefFlags : unsigned __int32
{
FX_EFFECT_NEEDS_LIGHTING_AT_SPAWN = 0x1,
FX_EFFECT_NEEDS_LIGHTING_PER_FRAME_AT_ORIGIN = 0x2,
FX_EFFECT_RECEIVES_DYNAMIC_LIGHTING_PER_FRAME_AT_ORIGIN = 0x4,
FX_EFFECT_USE_VECTORFIELDS = 0x8,
FX_EFFECT_UNK = 0x10,
FX_EFFECT_NEEDS_LIGHTING_AT_SPAWN_2 = 0x20,
}
;

enum FX_FleshType
{
FX_FLESHTYPE_BODY_NONFATAL = 0x0,
FX_FLESHTYPE_BODY_FATAL = 0x1,
FX_FLESHTYPE_HEAD_NONFATAL = 0x2,
FX_FLESHTYPE_HEAD_FATAL = 0x3,
FX_FLESHTYPE_HELMET_NONFATAL = 0x4,
FX_FLESHTYPE_HELMET_FATAL = 0x5,
FX_FLESHTYPE_ALIEN = 0x6,
FX_FLESHTYPE_ALIEN_FATAL = 0x7,
FX_FLESHTYPE_SOFT = 0x8,
FX_FLESHTYPE_SM_ROBOT_HEAD = 0x9,
FX_FLESHTYPE_SM_ROBOT_HEAD_FATAL = 0xA,
FX_FLESHTYPE_SM_ROBOT_LIMB = 0xB,
FX_FLESHTYPE_SM_ROBOT_LIMB_FATAL = 0xC,
FX_FLESHTYPE_SM_ROBOT_TORSO = 0xD,
FX_FLESHTYPE_SM_ROBOT_TORSO_FATAL = 0xE,
FX_FLESHTYPE_SM_ROBOT_ARMOR = 0xF,
FX_FLESHTYPE_LG_ROBOT_HEAD = 0x10,
FX_FLESHTYPE_LG_ROBOT_HEAD_FATAL = 0x11,
FX_FLESHTYPE_LG_ROBOT_LIMB = 0x12,
FX_FLESHTYPE_LG_ROBOT_LIMB_FATAL = 0x13,
FX_FLESHTYPE_LG_ROBOT_TORSO = 0x14,
FX_FLESHTYPE_LG_ROBOT_TORSO_FATAL = 0x15,
FX_FLESHTYPE_LG_ROBOT_ARMOR = 0x16,
FX_FLESHTYPE_COUNT = 0x17,
}
;

struct StringTableCell
{
const char *string;
int hash;
}
;

struct StructuredDataEnumEntry
{
scr_string_t string;
unsigned __int16 index;
}
;

struct StructuredDataEnum
{
int entryCount;
int reservedEntryCount;
StructuredDataEnumEntry *entries;
}
;

enum StructuredDataTypeCategory
{
DATA_INT = 0x0,
DATA_BYTE = 0x1,
DATA_BOOL = 0x2,
DATA_STRING = 0x3,
DATA_ENUM = 0x4,
DATA_STRUCT = 0x5,
DATA_INDEXED_ARRAY = 0x6,
DATA_ENUM_ARRAY = 0x7,
DATA_FLOAT = 0x8,
DATA_SHORT = 0x9,
DATA_COUNT = 0xA,
}
;

union StructuredDataTypeUnion
{
unsigned int stringDataLength;
int enumIndex;
int structIndex;
int indexedArrayIndex;
int enumedArrayIndex;
int index;
}
;

struct StructuredDataType
{
StructuredDataTypeCategory type;
StructuredDataTypeUnion u;
}
;

enum StructuredDataValidationType
{
VALIDATION_NONE = 0x0,
}
;

struct StructuredDataStructProperty
{
scr_string_t name;
StructuredDataType type;
unsigned int offset;
StructuredDataValidationType validation;
}
;

struct StructuredDataStruct
{
int propertyCount;
StructuredDataStructProperty *properties;
int size;
unsigned int bitOffset;
}
;

struct StructuredDataIndexedArray
{
int arraySize;
StructuredDataType elementType;
unsigned int elementSize;
}
;

struct StructuredDataEnumedArray
{
int enumIndex;
StructuredDataType elementType;
unsigned int elementSize;
}
;

struct StructuredDataDef
{
int version;
unsigned int formatChecksum;
int enumCount;
StructuredDataEnum *enums;
int structCount;
StructuredDataStruct *structs;
int indexedArrayCount;
StructuredDataIndexedArray *indexedArrays;
int enumedArrayCount;
StructuredDataEnumedArray *enumedArrays;
StructuredDataType rootType;
unsigned int size;
}
;

enum DDLType : __int32
{
DDL_BYTE_TYPE = 0x0,
DDL_SHORT_TYPE = 0x1,
DDL_UINT_TYPE = 0x2,
DDL_INT_TYPE = 0x3,
DDL_UINT64_TYPE = 0x4,
DDL_FLOAT_TYPE = 0x5,
DDL_FIXEDPOINT_TYPE = 0x6,
DDL_STRING_TYPE = 0x7,
DDL_STRUCT_TYPE = 0x8,
DDL_ENUM_TYPE = 0x9,
DDL_PAD_TYPE = 0xA,
}
;

enum DDLFlags : unsigned __int8
{
DDL_FLAG_DIRTY = 0x0,
DDL_FLAG_CHECKSUM = 0x1,
DDL_FLAG_CODE_VERSION = 0x2,
DDL_FLAG_USER_FLAGS = 0x4,
DDL_FLAG_NO_PADDING = 0x8,
DDL_FLAG_RESERVE = 0x10,
DDL_FLAG_DDL_CHECKSUM = 0x20,
}
;

struct unk_1453E1B90
{
const char *name;
int flags;
char __pad0[4];
}
;

struct unk_1453E1BC0
{
unk_1453E1B90 *base;
}
;

struct unk_1453E1C00
{
unk_1453E1B90 *base;
}
;

struct unk_1453E1C20
{
unk_1453E1B90 *base;
char __pad0[8];
}
;

struct unk_1453E1C70
{
unk_1453E1B90 *base;
unsigned int count;
unsigned int *val;
}
;

union unk_1453E1C80
{
unk_1453E1BC0 __0;
unk_1453E1C00 __1;
unk_1453E1C20 __2;
unk_1453E1C70 __3;
}
;

enum ScriptablePartReferenceType
{
type_1453E1BC0 = 0x0,
type_1453E1C00 = 0x1,
type_1453E1C20 = 0x2,
type_1453E1C70 = 0x3,
unk_1453E1CC8_type_count = 0x4,
}
;

enum RumbleGraphs
{
RUMBLEGRAPH_HIGH = 0x0,
RUMBLEGRAPH_LOW = 0x1,
RUMBLEGRAPH_COUNT = 0x2,
}
;

struct FootstepAnims
{
FootstepAnim footstep[9];
}
;

struct unk_1453E1E68
{
scr_string_t unk01;
char __pad0[92];
scr_string_t unk02;
char __pad1[4];
scr_string_t unk03;
char __pad2[20];
}
;

struct unk_1453E1E30
{
scr_string_t unk01;
char __pad0[8];
}
;

struct unk_1453E1EA8
{
char __pad0[32];
}
;

struct XAssetEntry
{
XAssetHeader header;
int nextHash;
int nextOverride;
char inuseMask;
unsigned __int8 type;
unsigned __int16 zoneIndex;
}
;

struct ScriptStringList
{
int count;
const char **strings;
}
;

union GfxZoneTableEntry
{
char *dataPtr;
void *voidPtr;
ID3D11Buffer *buffer;
ID3D11DepthStencilState *depthStencilState;
ID3D11BlendState *blendState;
}
;

typedef std::uint32_t GfxBlendStateBits[4]
;

struct XGfxGlobals
{
unsigned int depthStencilStateCount;
unsigned int blendStateCount;
std::uint64_t *depthStencilStateBits;
GfxBlendStateBits *blendStateBits;
GfxZoneTableEntry *depthStencilStates;
GfxZoneTableEntry *blendStates;
unsigned int perPrimConstantBufferCount;
unsigned int perObjConstantBufferCount;
unsigned int stableConstantBufferCount;
unsigned int *perPrimConstantBufferSizes;
unsigned int *perObjConstantBufferSizes;
unsigned int *stableConstantBufferSizes;
GfxZoneTableEntry *perPrimConstantBuffers;
GfxZoneTableEntry *perObjConstantBuffers;
GfxZoneTableEntry *stableConstantBuffers;
}
;

struct XGlobals
{
XGfxGlobals *gfxGlobals;
}
;

struct XAssetList
{
ScriptStringList stringList;
int assetCount;
XAsset *assets;
XGlobals *globals;
}
;

enum DBSyncMode : __int8
{
DB_LOAD_SYNC = 0x1,
DB_LOAD_ASYNC = 0x2,
}
;

enum DBAllocFlags : __int32
{
DB_ZONE_PERMANENT = 0x1,
DB_ZONE_GLOBAL_TIER1 = 0x2,
DB_ZONE_GLOBAL_TIER2 = 0x4,
DB_ZONE_GAMEMODE_TIER1 = 0x8,
DB_ZONE_GAMEMODE_TIER2 = 0x10,
DB_ZONE_GAMEMODE_TIER3 = 0x20,
DB_ZONE_UI = 0x40,
DB_ZONE_UI_SCENE = 0x80,
DB_ZONE_GAME = 0x100,
DB_ZONE_LOAD = 0x200,
DB_ZONE_DEV = 0x400,
DB_ZONE_BASEMAP = 0x800,
DB_ZONE_TRANSIENT_GAMEMODE_SP = 0x1000,
DB_ZONE_TRANSIENT_GLOBAL_GROUP = 0x2000,
DB_ZONE_TRANSIENT_WORLD_SP = 0x4000,
DB_ZONE_TRANSIENT_WORLD_MP = 0x8000,
DB_ZONE_TRANSIENT_PHYSICS_MP = 0x10000,
DB_ZONE_TRANSIENT_INFIL_COMMON_MP = 0x20000,
DB_ZONE_TRANSIENT_INFIL_LEVEL_MP = 0x40000,
DB_ZONE_TRANSIENT_CUSTOMIZATION = 0x80000,
DB_ZONE_PRELOAD_FRONTEND = 0x100000,
DB_ZONE_PRELOAD_LEVEL_SP = 0x200000,
DB_ZONE_PRELOAD_TRANSIENT_SP = 0x400000,
DB_ZONE_PRELOAD_TRANSIENT_WORLD_MP = 0x800000,
DB_ZONE_PRELOAD_TRANSIENT_PHYSICS_MP = 0x1000000,
DB_ZONE_PRELOAD_TRANSIENT_INFIL_MP = 0x2000000,
DB_ZONE_COMMS = 0x4000000,
DB_ZONE_CUSTOM = 0x8000000,
}
;

struct XZoneInfo
{
const char *name;
int allocFlags;
}
;

enum DB_FastFileFailureMode
{
REQUIRED = 0x0,
ALLOW_MISSING = 0x1,
}
;

struct DB_FastfileInfo
{
const char *name;
unsigned int zoneFlags;
DB_FastFileFailureMode failureMode;
unsigned int priority;
}
;

struct DBFile
{
char name[64];
void *handle;
unsigned __int64 memoryBufferSize;
unsigned __int8 *memoryBuffer;
void *handle_dcache;
char __pad1[22];
bool isSecure;
char __pad2[1];
int some_check;
char __pad3[44];
}
;

struct db_internal_state
{
int dummy;
}
;

struct db_z_stream_s
{
unsigned __int8 *next_in;
unsigned int avail_in;
unsigned int total_in;
unsigned __int8 *next_out;
unsigned int avail_out;
unsigned int total_out;
char *msg;
db_internal_state *state;
unsigned __int8 *(__fastcall *zalloc)(unsigned __int8 *, unsigned int, unsigned int);
void (__fastcall *zfree)(unsigned __int8 *, unsigned __int8 *);
unsigned __int8 *opaque;
int data_type;
unsigned int adler;
unsigned int reserved;
}
;

enum DB_CompressorType : unsigned __int8
{
DB_COMPRESSOR_INVALID = 0x0,
DB_COMPRESSOR_PASSTHROUGH = 0x1,
DB_COMPRESSOR_BLOCK = 0x2,
}
;

struct DB_ReadStream
{
unsigned __int8 *next_in;
unsigned __int64 avail_in;
unsigned __int64 total_in;
unsigned __int8 *next_out;
unsigned __int64 avail_out;
unsigned __int64 total_out;
DB_CompressorType compressor;
}
;

struct DB_LoadData
{
DBFile *file;
void *unk1;
unsigned __int8 *fileBuffer;
unsigned __int64 readSize;
unsigned __int64 completedReadSize;
unsigned __int64 offset;
unsigned __int8 *start_in;
unsigned int readError;
int readingResident;
DB_ReadStream stream;
__int64 read_size_;
__int64 offset_;
int unk4;
int unk5;
int unk6;
int unk7;
int unk8;
int unk9;
int unk10;
int unk11;
}
;

struct DB_Zone
{
char name[64];
char __pad0[1080];
}
;

enum XFileBlock
{
XFILE_BLOCK_TEMP = 0x0,
XFILE_BLOCK_TEMP_PRELOAD = 0x1,
XFILE_BLOCK_TEMP_POSTLOAD = 0x2,
XFILE_BLOCK_IMAGE_STREAM = 0x3,
XFILE_BLOCK_SHARED_STREAM = 0x4,
XFILE_BLOCK_CALLBACK = 0x5,
XFILE_BLOCK_RUNTIME = 0x6,
XFILE_BLOCK_UNK7 = 0x7,
XFILE_BLOCK_VIRTUAL = 0x8,
XFILE_BLOCK_SCRIPT = 0x9,
XFILE_BLOCK_VIRTUAL_IW8REPLAY = 0x5,
MAX_XFILE_COUNT = 0xA,
}
;

struct XBlock
{
char *alloc;
unsigned __int64 size;
}
;

struct XStreamFile
{
std::uint64_t offset;
std::uint64_t offsetEnd;
std::uint16_t fileIndex;
bool isLocalized;
}
;

struct XZoneMemory
{
XBlock blocks[10];
unsigned __int64 callbackPos;
char __pad0[4][24];
void *unk1;
XStreamFile *shared_ff_data;
unsigned int shared_ff_count;
unsigned int shared_ff_count2;
void *unknown;
XStreamFile *streamed_images;
unsigned int streamed_image_count;
int streamed_image_index;
}
;

struct DB_AuthHash
{
unsigned __int8 bytes[32];
}
;

struct DB_AuthSignature
{
unsigned __int8 bytes[256];
}
;

struct DB_MasterBlock
{
DB_AuthHash chunkHashes[512];
}
;

struct DB_AuthSubHeader
{
char fastfileName[32];
unsigned int reserved;
DB_AuthHash masterBlockHashes[192];
}
;

struct DB_AuthHeader
{
char magic[8];
unsigned int reserved;
DB_AuthHash subheaderHash;
DB_AuthSignature signedSubheaderHash;
DB_AuthSubHeader subheader;
char padding[9904];
}
;

struct XPakHeader
{
char magic[8];
std::uint32_t version;
char unknown[20];
unsigned __int8 iv[16];
DB_AuthHash hash;
DB_AuthSignature signedhash;
}
;

struct XFileStreamData
{
std::uint64_t size;
std::uint64_t unk1;
std::uint64_t unk2;
std::uint64_t block_size[10];
std::uint64_t unk_arr[8];
}
;

struct XFileHeader
{
char magic[8];
std::uint32_t version;
std::uint8_t unused;
std::uint8_t has_no_image_fastfile;
std::uint8_t has_no_shared_fastfile;
std::uint8_t unk1;
std::uint32_t fileTimeHigh;
std::uint32_t fileTimeLow;
XFileStreamData stream_data;
std::uint32_t shared_ff_hash;
std::uint32_t shared_ff_count;
std::uint32_t image_ff_hash;
std::uint32_t image_ff_count;
std::uint64_t fileLen;
std::uint64_t fileLenUnk1;
std::uint64_t fileLenUnk2;
}
;

struct XFileCompressorHeader
{
DB_CompressorType compressor;
char magic[3];
}
;

struct XBlockCompressionBlockHeader
{
unsigned int compressedSize;
std::uint64_t uncompressedSize;
}
;

struct XBlockCompressionBlockSizeAndCompressionType
{
unsigned __int32 blockSize : 24;
unsigned __int32 compressionType : 8;
}
;

struct XBlockCompressionDataHeader
{
std::uint64_t uncompressedSize;
XBlockCompressionBlockSizeAndCompressionType blockSizeAndType;
}
;

enum XBlockCompressionType
{
XBLOCK_COMPRESSION_INVALID = 0x0,
XBLOCK_COMPRESSION_ZLIB_SIZE = 0x1,
XBLOCK_COMPRESSION_ZLIB_SPEED = 0x2,
XBLOCK_COMPRESSION_LZ4HC = 0x3,
XBLOCK_COMPRESSION_LZ4 = 0x4,
XBLOCK_COMPRESSION_NONE = 0x5,
}
;

struct XFileReadData
{
bool header_parsed;
std::uint8_t has_no_image_fastfile;
std::uint8_t has_no_shared_fastfile;
std::uint8_t unk1;
char __pad0[4];
XFileStreamData stream_data;
std::uint32_t version;
std::uint32_t shared_ff_hash;
std::uint32_t shared_ff_count;
std::uint64_t fileLenUnk1;
std::uint32_t image_ff_hash;
std::uint32_t image_ff_count;
std::uint64_t fileLen;
std::uint64_t fileLenUnk2;
}
;

enum snd_asset_format : __int32
{
SND_ASSET_FORMAT_PCMS16 = 0x0,
SND_ASSET_FORMAT_PCMS24 = 0x1,
SND_ASSET_FORMAT_PCMS32 = 0x2,
SND_ASSET_FORMAT_IEEE = 0x3,
SND_ASSET_FORMAT_XMA4 = 0x4,
SND_ASSET_FORMAT_MP3 = 0x5,
SND_ASSET_FORMAT_MSADPCM = 0x6,
SND_ASSET_FORMAT_WMA = 0x7,
SND_ASSET_FORMAT_FLAC = 0x8,
SND_ASSET_FORMAT_WIIUADPCM = 0x9,
SND_ASSET_FORMAT_MPC = 0xA,
SND_ASSET_FORMAT_COUNT = 0xB,
}
;

struct SndAssetBankHeader
{
unsigned int magic;
unsigned int version;
unsigned int entrySize;
unsigned int checksumSize;
unsigned int dependencySize;
unsigned int entryCount;
unsigned int dependencyCount;
unsigned int buildVersion;
std::uint64_t fileSize;
std::uint64_t entryOffset;
std::uint64_t checksumOffset;
char checksumChecksum[16];
char dependencies[512];
std::uint64_t SourceChecksumOffset;
std::uint64_t AssetNameOffset;
char zoneName[64];
char platform[8];
char language[3];
unsigned int convertedAssetVersion;
unsigned int assetSectionSize;
char padding[5];
}
;

struct SndAssetBankEntry
{
unsigned int id;
unsigned int size;
unsigned int seekTableSize;
unsigned int frameCount;
unsigned int hybridPcmSize;
unsigned __int64 offset;
int frameRate;
char channelCount;
char looping;
char format;
unsigned __int16 EnvelopeTime1;
unsigned __int16 EnvelopeTime2;
char EnvelopeLoudness0;
char EnvelopeLoudness1;
char EnvelopeLoudness2;
char EnvelopeLoudness3;
char BlocksizeKB;
}
;

struct tmat33_t::$::data
{
vec3_t row0;
vec3_t row1;
vec3_t row2;
}
;

union tmat33_t::$04A31BCE4F65D1EEE6CD3F7AEE9A08C8
{
vec3_t m[3];
}
;

enum XSurfaceSharedFlags : __int32
{
NONE = 0x0,
STREAMER_HANDLE = 0x1,
}
;

union $21B5B2324A2AFD50A7F974C00D42E0F0
{
XAnimDynamicBinding dynamic;
XAnimConstant constant;
}
;

union $F8E94A00F93B437FA06178475B4E431B
{
XAnimParts *parts;
XAnimParent animParent;
}
;

union $D6340DA7F7093F0D1EBCA111EB9B691A
{
pathnode_yaworient_t yaw_orient;
vec3_t angles;
}
;

