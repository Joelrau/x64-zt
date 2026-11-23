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

union __declspec(align(16)) _SLIST_HEADER
{
struct
{
ULONGLONG Alignment;
ULONGLONG Region;
};
_SLIST_HEADER::$F9F9EB832D628D73E611400623F67F2B HeaderX64;
}
;

struct _SLIST_HEADER::$2AAD3A9E0F86A5BF9BE50654CA710F62
{
ULONGLONG Alignment;
ULONGLONG Region;
}
;

typedef unsigned __int64 ULONGLONG
;

struct _SLIST_HEADER::$F9F9EB832D628D73E611400623F67F2B
{
unsigned __int64 Depth : 16;
unsigned __int64 Sequence : 48;
unsigned __int64 Reserved : 4;
unsigned __int64 NextEntry : 60;
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

typedef tagRECT RECT
;

struct tagRECT
{
LONG left;
LONG top;
LONG right;
LONG bottom;
}
;

typedef GUID IID
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

typedef char CHAR
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

typedef __int64 __time64_t
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

typedef unsigned int _fsize_t
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

struct tagPOINT
{
LONG x;
LONG y;
}
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

typedef const WCHAR *LPCWSTR
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

typedef int BOOL
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

struct _WIN32_FIND_DATAA
{
DWORD dwFileAttributes;
FILETIME ftCreationTime;
FILETIME ftLastAccessTime;
FILETIME ftLastWriteTime;
DWORD nFileSizeHigh;
DWORD nFileSizeLow;
DWORD dwReserved0;
DWORD dwReserved1;
CHAR cFileName[260];
CHAR cAlternateFileName[14];
}
;

typedef _FILETIME FILETIME
;

struct _WIN_CERTIFICATE
{
DWORD dwLength;
WORD wRevision;
WORD wCertificateType;
BYTE bCertificate[1];
}
;

struct _CRYPT_VERIFY_MESSAGE_PARA
{
DWORD cbSize;
DWORD dwMsgAndCertEncodingType;
HCRYPTPROV_LEGACY hCryptProv;
PFN_CRYPT_GET_SIGNER_CERTIFICATE pfnGetSignerCertificate;
void *pvGetArg;
}
;

typedef ULONG_PTR HCRYPTPROV_LEGACY
;

typedef PCCERT_CONTEXT (__stdcall *PFN_CRYPT_GET_SIGNER_CERTIFICATE)(void *pvGetArg, DWORD dwCertEncodingType, PCERT_INFO pSignerId, HCERTSTORE hMsgCertStore)
;

typedef _CERT_INFO *PCERT_INFO
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

typedef _CRYPTOAPI_BLOB CRYPT_INTEGER_BLOB
;

struct _CRYPTOAPI_BLOB
{
DWORD cbData;
BYTE *pbData;
}
;

typedef _CRYPT_ALGORITHM_IDENTIFIER CRYPT_ALGORITHM_IDENTIFIER
;

struct _CRYPT_ALGORITHM_IDENTIFIER
{
LPSTR pszObjId;
CRYPT_OBJID_BLOB Parameters;
}
;

typedef CHAR *LPSTR
;

typedef _CRYPTOAPI_BLOB CRYPT_OBJID_BLOB
;

typedef _CRYPTOAPI_BLOB CERT_NAME_BLOB
;

typedef _CERT_PUBLIC_KEY_INFO CERT_PUBLIC_KEY_INFO
;

struct _CERT_PUBLIC_KEY_INFO
{
CRYPT_ALGORITHM_IDENTIFIER Algorithm;
CRYPT_BIT_BLOB PublicKey;
}
;

typedef _CRYPT_BIT_BLOB CRYPT_BIT_BLOB
;

struct _CRYPT_BIT_BLOB
{
DWORD cbData;
BYTE *pbData;
DWORD cUnusedBits;
}
;

typedef _CERT_EXTENSION *PCERT_EXTENSION
;

struct _CERT_EXTENSION
{
LPSTR pszObjId;
BOOL fCritical;
CRYPT_OBJID_BLOB Value;
}
;

typedef void *HCERTSTORE
;

typedef const CERT_CONTEXT *PCCERT_CONTEXT
;

typedef _CERT_CONTEXT CERT_CONTEXT
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

typedef _RTL_SRWLOCK RTL_SRWLOCK
;

struct _RTL_SRWLOCK
{
PVOID Ptr;
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

struct _EVENT_DATA_DESCRIPTOR
{
ULONGLONG Ptr;
ULONG Size;
union
{
ULONG Reserved;
struct
{
UCHAR Type;
UCHAR Reserved1;
USHORT Reserved2;
};
};
}
;

union _EVENT_DATA_DESCRIPTOR::$535316677C6A15A6ECBA40D88E1D787B
{
ULONG Reserved;
struct
{
UCHAR Type;
UCHAR Reserved1;
USHORT Reserved2;
};
}
;

struct _EVENT_DATA_DESCRIPTOR::$535316677C6A15A6ECBA40D88E1D787B::$AE424CE1AA1E88F41D16EB4A36C24CA2
{
UCHAR Type;
UCHAR Reserved1;
USHORT Reserved2;
}
;

typedef unsigned __int8 UCHAR
;

typedef _EVENT_DESCRIPTOR EVENT_DESCRIPTOR
;

struct _EVENT_DESCRIPTOR
{
USHORT Id;
UCHAR Version;
UCHAR Channel;
UCHAR Level;
UCHAR Opcode;
USHORT Task;
ULONGLONG Keyword;
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

typedef unsigned __int16 VARTYPE
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

typedef int INT
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

typedef OLECHAR *BSTR
;

typedef WCHAR OLECHAR
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

typedef WCHAR *LPWSTR
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

typedef int HRESULT
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

typedef GUID UUID
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

struct tagMONITORINFO
{
DWORD cbSize;
RECT rcMonitor;
RECT rcWork;
DWORD dwFlags;
}
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

struct _onexit_table_t
{
_PVFV *_first;
_PVFV *_last;
_PVFV *_end;
}
;

typedef void (__cdecl *_PVFV)()
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

typedef BYTE *LPBYTE
;

struct __crt_locale_pointers
{
struct __crt_locale_data *locinfo;
struct __crt_multibyte_data *mbcinfo;
}
;

struct _RTC_framedesc
{
int varCount;
_RTC_vardesc *variables;
}
;

struct _RTC_vardesc
{
int addr;
int size;
char *name;
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

struct charNode
;

struct __crt_strtox
;

struct std::ios_base
;

struct _LocaleUpdate
;

struct __FrameHandler3::TryBlockMap
;

struct _anonymous_namespace_
;

struct std::_Ref_count_base
;

struct std::bad_function_call
;

struct std::error_category
;

struct _lambda_9a32fed5bf61b6b509b2d3f6003082a1_
;

struct _lambda_525d7225a58b45f091abf5754dc0c700_
;

struct __crt_deferred_errno_cache
;

struct std::mutex
;

struct FH4
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

struct std::codecvt_base
;

struct std::_Lockit
;

typedef unsigned __int64 *PULONG64
;

typedef BYTE *PBYTE
;

typedef DWORD *PDWORD
;

typedef PVOID HDEVINFO
;

typedef HINSTANCE HMODULE
;

typedef char *va_list
;

typedef ACCESS_MASK REGSAM
;

typedef DWORD ACCESS_MASK
;

typedef struct HKEY__ *HKEY
;

struct HKEY__
{
int unused;
}
;

typedef IUnknown *LPUNKNOWN
;

typedef HANDLE HLOCAL
;

typedef __crt_locale_pointers *_locale_t
;

typedef struct _SP_DEVINFO_DATA *PSP_DEVINFO_DATA
;

typedef DWORD DEVNODE
;

typedef struct _SP_DEVICE_INTERFACE_DETAIL_DATA_W *PSP_DEVICE_INTERFACE_DETAIL_DATA_W
;

struct _SP_DEVICE_INTERFACE_DETAIL_DATA_W
{
DWORD cbSize;
WCHAR DevicePath[1];
}
;

typedef const WCHAR *PCWSTR
;

typedef DWORD *LPDWORD
;

typedef HKEY *PHKEY
;

typedef struct _OVERLAPPED *LPOVERLAPPED
;

typedef CHAR *PSTR
;

typedef unsigned __int8 *RPC_CSTR
;

typedef ULONG_PTR HCRYPTHASH
;

typedef ULONG_PTR HCRYPTPROV
;

typedef unsigned int u_long
;

typedef struct _OVERLAPPED *LPWSAOVERLAPPED
;

typedef void (__stdcall *LPWSAOVERLAPPED_COMPLETION_ROUTINE)(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
;

typedef struct addrinfo *PADDRINFOA
;

typedef UCHAR *PUCHAR
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

struct _CTL_USAGE
{
DWORD cUsageIdentifier;
LPSTR *rgpszUsageIdentifier;
}
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

typedef const CHAR *LPCCH
;

typedef BOOL *LPBOOL
;

typedef _Mtx_imp_t _Mtx_t
;

typedef struct _Mtx_internal_imp_t *_Mtx_imp_t
;

typedef struct _SP_DEVICE_INTERFACE_DATA *PSP_DEVICE_INTERFACE_DATA
;

typedef struct _HIDP_PREPARSED_DATA *PHIDP_PREPARSED_DATA
;

typedef __int64 fpos_t
;

typedef struct _FILETIME *PFILETIME
;

typedef struct HMENU__ *HMENU
;

struct HMENU__
{
int unused;
}
;

typedef struct _STARTUPINFOA *LPSTARTUPINFOA
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

typedef struct _PROCESS_INFORMATION *LPPROCESS_INFORMATION
;

typedef PVOID BCRYPT_HASH_HANDLE
;

typedef void (__stdcall *LPOVERLAPPED_COMPLETION_ROUTINE)(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
;

typedef struct _SECURITY_ATTRIBUTES *LPSECURITY_ATTRIBUTES
;

struct _SECURITY_ATTRIBUTES
{
DWORD nLength;
LPVOID lpSecurityDescriptor;
BOOL bInheritHandle;
}
;

typedef PRTL_CRITICAL_SECTION LPCRITICAL_SECTION
;

typedef struct _RTL_CRITICAL_SECTION *PRTL_CRITICAL_SECTION
;

typedef WORD LANGID
;

typedef int (__cdecl *_CoreCrtNonSecureSearchSortCompareFunction)(const void *, const void *)
;

typedef unsigned __int64 uintptr_t
;

typedef struct _EVENT_FILTER_DESCRIPTOR *PEVENT_FILTER_DESCRIPTOR
;

struct _EVENT_FILTER_DESCRIPTOR
{
ULONGLONG Ptr;
ULONG Size;
ULONG Type;
}
;

