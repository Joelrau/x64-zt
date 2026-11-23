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

struct tagPOINT
{
LONG x;
LONG y;
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

typedef unsigned int UINT
;

typedef tagPOINT POINT
;

typedef tagRECT RECT
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

typedef GUID IID
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

typedef _RTL_SRWLOCK RTL_SRWLOCK
;

struct _RTL_SRWLOCK
{
PVOID Ptr;
}
;

typedef void *PVOID
;

struct _FILETIME
{
DWORD dwLowDateTime;
DWORD dwHighDateTime;
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

typedef unsigned __int64 ULONGLONG
;

struct _IP_ADAPTER_ADDRESSES_LH::$88EAB68B38CC03D4ADFF2CEE83F7F421::$540D896092981D96F2B14DC684EC72A2
{
ULONG Length;
IF_INDEX IfIndex;
}
;

typedef unsigned int ULONG
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

typedef WCHAR *PWCHAR
;

typedef wchar_t WCHAR
;

typedef unsigned __int16 wchar_t
;

typedef unsigned __int8 BYTE
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

typedef unsigned __int64 ULONG64
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

typedef unsigned __int64 UINT_PTR
;

typedef LONG_PTR LPARAM
;

typedef __int64 LONG_PTR
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

typedef const WCHAR *LPCWSTR
;

typedef GUID UUID
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

struct timeval
{
int tv_sec;
int tv_usec;
}
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

struct IDispatch
{
IDispatchVtbl *lpVtbl;
}
;

typedef int HRESULT
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

typedef WCHAR OLECHAR
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

typedef unsigned __int16 VARTYPE
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

typedef LONG SCODE
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

typedef float FLOAT
;

typedef double DOUBLE
;

typedef __int16 VARIANT_BOOL
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

typedef CHAR *LPSTR
;

typedef WCHAR *LPWSTR
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

struct _SP_DEVICE_INTERFACE_DATA
{
DWORD cbSize;
GUID InterfaceClassGuid;
DWORD Flags;
ULONG_PTR Reserved;
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

typedef BYTE *LPBYTE
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

struct _PROCESS_INFORMATION
{
HANDLE hProcess;
HANDLE hThread;
DWORD dwProcessId;
DWORD dwThreadId;
}
;

struct _cpinfo
{
UINT MaxCharSize;
BYTE DefaultChar[2];
BYTE LeadByte[12];
}
;

struct _SECURITY_ATTRIBUTES
{
DWORD nLength;
LPVOID lpSecurityDescriptor;
BOOL bInheritHandle;
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

struct C_SCOPE_TABLE
{
_UNKNOWN *__ptr32 Begin __offset(OFF64|RVAOFF);
_UNKNOWN *__ptr32 End __offset(OFF64|RVAOFF|PASTEND);
_UNKNOWN *__ptr32 Handler __offset(OFF64|RVAOFF);
_UNKNOWN *__ptr32 Target __offset(OFF64|RVAOFF);
}
;

struct __crt_locale_pointers
{
struct __crt_locale_data *locinfo;
struct __crt_multibyte_data *mbcinfo;
}
;

struct bdChannelInfo
;

struct hks::Preprocessor::PreprocessorStateProxy
;

struct bdDTLSCookieAck
;

struct MatchMakingInfo
;

struct bdEndpoint
;

struct PublicProfileInfo
;

struct bdCommonAddr
;

struct bdPreCopyResult
;

struct bdUCDPublicKeyInfo
;

struct bdProfileInfo
;

struct bdMatchMakingInfo
;

struct bdPacketBufferInternal
;

struct bdSAckChunk
;

struct bdFileMetaData
;

struct bdFileInfo
;

struct bdContextUserStorageFileInfo
;

struct bdUserInfo
;

struct GfxCmdBufState
;

struct hks::CodeGenerator
;

struct bdBandwidthTestClient
;

struct bdStatsInfo
;

struct bdConnection
;

struct bdRunnable
;

struct bdDMLInfo
;

struct bdContentStreaming
;

struct bdSessionInvite
;

struct bdRemoteTask
;

struct bdSocketRouter
;

struct bdSummaryMetaHandler
;

struct bdConnectionStatistics
;

struct bdURL
;

struct bdQoSReplyPacket
;

struct hks::Preprocessor
;

struct hks
;

struct hks::Visitor::Node
;

struct hks::StructProto
;

struct hks::Parser
;

struct hks::StringPinner
;

struct hks::BytecodeReader::Reader
;

struct hks::HashTable
;

struct hks::SimpleCompilerState
;

struct bdAddr
;

struct bdSecurityID
;

struct bdString
;

struct bdInetAddr
;

struct bdRandom
;

struct bdLobbyConnection
;

struct bdTaskByteBuffer
;

struct bdUPnP
;

struct bdSockAddr
;

struct bdServiceBandwidthArbitrator
;

struct bdStopwatch
;

struct bdThread
;

struct bdStreamSocket
;

struct bdECCKey
;

struct bdSequenceNumber
;

struct dxGeom
;

struct bdSecurityKeyMapListener
;

struct bdCypher
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

struct _OFSTRUCT
{
BYTE cBytes;
BYTE fFixedDisk;
WORD nErrCode;
WORD Reserved1;
WORD Reserved2;
CHAR szPathName[128];
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

struct bdDTLSInitAck
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

struct BgAntiLag
;

struct MarketingCommsManager
;

struct DB_AssetEntryPool
;

struct bdTag
;

struct DB_AssetEntryTable
;

struct bdJSONDeserializer
;

struct dwLogonHSM_xb3
;

struct ClConfigStringsSP
;

struct bdStats
;

struct CgGlobalsSP
;

struct CgWeaponSystemSP
;

struct ClGameModeApplication
;

struct ClGameModeApplicationMP
;

struct ClGameModeApplicationSP
;

struct DWServicesAccess
;

struct DWLobbyService
;

struct bdTitleUtilities
;

struct GAntiLagSP
;

struct LUITraceRunner
;

struct LargeLocal
;

struct CachedContentCrypto
;

struct OnlineMatchmakerOmniscient
;

struct OnlineMgr
;

struct Online_PatchStreamer
;

struct ComGameModeApplication
;

struct CgSoundSystem
;

struct ComGameModeAppMP
;

struct ComGameModeAppSP
;

struct SvGameModeAppMP
;

struct SvGameModeAppSP
;

struct TaskManager
;

struct hkString
;

struct hkStringBuf
;

struct bdVoteRank
;

struct bdNetImpl
;

struct bdGetHostByName
;

struct bdBitBuffer
;

struct bdNATTravListener
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

struct PhysicsSFXEventAssetRule
{
PhysicsSFXEventAssetRuleType type;
PhysicsSFXEventAssetRuleUnion u;
}
;

struct PhysicsSFXEventAssetRules
{
int numrules;
PhysicsSFXEventAssetRule *rules;
}
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

struct __declspec(align(8)) PhysicsSFXEventAssetSoundRule
{
char *hitSoundAlias;
char *scrapeSoundAlias;
float scrapeSoundVolumeSpeedThresholdMin;
float scrapeSoundVolumeSpeedThresholdMax;
float scrapeSoundVolumeBlendSpeed;
}
;

struct PhysicsSFXEventAssetMaterialEntry
{
int surfaceType;
PhysicsSFXEventAssetRules rules;
}
;

struct PhysicsSFXEventAssetMaterialRule
{
int numentries;
PhysicsSFXEventAssetMaterialEntry *entries;
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

struct PhysicsSFXEventAssetAngleRule
{
int numentries;
PhysicsSFXEventAssetAngleEntry *entries;
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

struct PhysicsSFXEventAssetSpeedRule
{
int numentries;
PhysicsSFXEventAssetSpeedEntry *entries;
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

struct PhysicsSFXEventAssetMassRule
{
int numentries;
PhysicsSFXEventAssetMassEntry *entries;
}
;

struct PhysicsSFXEventAssetGravityEntry
{
bool gravity;
PhysicsSFXEventAssetRules rules;
}
;

struct PhysicsSFXEventAssetGravityRule
{
int numentries;
PhysicsSFXEventAssetGravityEntry *entries;
}
;

struct PhysicsSFXEventAssetAtmosphereEntry
{
bool atmosphere;
PhysicsSFXEventAssetRules rules;
}
;

struct PhysicsSFXEventAssetAtmosphereRule
{
int numentries;
PhysicsSFXEventAssetAtmosphereEntry *entries;
}
;

struct PhysicsSFXEventAssetCameraDistanceEntry
{
float distance;
PhysicsSFXEventAssetRules rules;
}
;

struct PhysicsSFXEventAssetCameraDistanceRule
{
int numentries;
PhysicsSFXEventAssetCameraDistanceEntry *entries;
}
;

struct PhysicsSFXEventAssetFrequencyEntry
{
int maxEvents;
PhysicsSFXEventAssetRules rules;
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

struct PhysicsSFXEventAsset
{
const char *name;
int priority;
PhysicsSFXEventAssetRules rules;
}
;

struct PhysicsVFXEventAssetRule
{
PhysicsVFXEventAssetRuleType type;
PhysicsVFXEventAssetRuleUnion u;
}
;

struct PhysicsVFXEventAssetRules
{
int numrules;
PhysicsVFXEventAssetRule *rules;
}
;

enum PhysicsVFXEventAssetRuleType : __int32
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

struct PhysicsVFXEventAssetMaterialEntry
{
int surfaceType;
PhysicsVFXEventAssetRules rules;
}
;

struct PhysicsVFXEventAssetMaterialRule
{
int numentries;
PhysicsVFXEventAssetMaterialEntry *entries;
}
;

struct PhysicsVFXEventAssetAngleEntry
{
float angle;
PhysicsVFXEventAssetRules rules;
}
;

struct PhysicsVFXEventAssetAngleRule
{
int numentries;
PhysicsVFXEventAssetAngleEntry *entries;
}
;

struct PhysicsVFXEventAssetSpeedEntry
{
float speedThreshold;
PhysicsVFXEventAssetRules rules;
}
;

struct PhysicsVFXEventAssetSpeedRule
{
int numentries;
PhysicsVFXEventAssetSpeedEntry *entries;
}
;

struct PhysicsVFXEventAssetMassEntry
{
float massThreshold;
PhysicsVFXEventAssetRules rules;
}
;

struct PhysicsVFXEventAssetMassRule
{
int numentries;
PhysicsVFXEventAssetMassEntry *entries;
}
;

struct PhysicsVFXEventAssetGravityEntry
{
bool gravity;
PhysicsVFXEventAssetRules rules;
}
;

struct PhysicsVFXEventAssetGravityRule
{
int numentries;
PhysicsVFXEventAssetGravityEntry *entries;
}
;

struct PhysicsVFXEventAssetAtmosphereEntry
{
bool atmosphere;
PhysicsVFXEventAssetRules rules;
}
;

struct PhysicsVFXEventAssetAtmosphereRule
{
int numentries;
PhysicsVFXEventAssetAtmosphereEntry *entries;
}
;

struct PhysicsVFXEventAssetCameraDistanceEntry
{
float distance;
PhysicsVFXEventAssetRules rules;
}
;

struct PhysicsVFXEventAssetCameraDistanceRule
{
int numentries;
PhysicsVFXEventAssetCameraDistanceEntry *entries;
}
;

struct PhysicsVFXEventAssetFrequencyEntry
{
int maxEvents;
PhysicsVFXEventAssetRules rules;
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
char __pad0[8];
int numSFXEventAssets;
void **sfxEventAssets;
int numVFXEventAssets;
void **vfxEventAssets;
char __pad1[16];
}
;

struct PhysicsFXPipeline
{
const char *name;
char __pad0[24];
}
;

struct PhysicsFXShape
{
const char *name;
unsigned int size;
vec3_t *vecs;
}
;

union XAnimIndices
{
char *_1;
unsigned __int16 *_2;
void *data;
}
;

union XAnimDynamicFrames
{
char (*_1)[3];
unsigned __int16 (*_2)[3];
}
;

union XAnimDynamicIndices
{
char _1[1];
unsigned __int16 _2[1];
}
;

struct __declspec(align(8)) XAnimDeltaPartQuatDataFrames2
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

struct XAnimDeltaPartQuat2
{
unsigned __int16 size;
XAnimDeltaPartQuatData2 u;
}
;

struct __declspec(align(8)) XAnimDeltaPartQuatDataFrames
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

struct XAnimDeltaPartQuat
{
unsigned __int16 size;
XAnimDeltaPartQuatData u;
}
;

enum scr_string_t : __int32
{
}
;

struct XAnimNotifyInfo
{
scr_string_t name;
float time;
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
char flags;
char boneCount[10];
char notifyCount;
char assetType;
char ikType;
}
;

typedef float vec_t
;

typedef vec_t vec2_t[2]
;

typedef vec_t vec3_t[3]
;

typedef vec_t vec4_t[4]
;

struct __declspec(align(8)) XAnimPartTransFrames
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

struct XAnimPartTrans
{
unsigned __int16 size;
char smallTrans;
XAnimPartTransData u;
}
;

struct XAnimDeltaPart
{
XAnimPartTrans *trans;
XAnimDeltaPartQuat2 *quat2;
XAnimDeltaPartQuat *quat;
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

struct LocalizeEntry
{
const char *name;
const char *value;
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

struct StringTableCell
{
const char *string;
int hash;
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

struct LuaFile
{
const char *name;
int len;
char strippingType;
const char *buffer;
}
;

enum NetConstStringType : __int32
{
}
;

enum NetConstStringSource : __int32
{
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

struct DObjAnimMat
{
float quat[4];
float trans[3];
float transWeight;
}
;

struct ReactiveMotionModelPart
{
float center[3];
float stiffness;
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

struct XRigidVertList
{
unsigned __int16 boneOffset;
unsigned __int16 vertCount;
unsigned __int16 triOffset;
unsigned __int16 triCount;
XSurfaceCollisionTree *collisionTree;
}
;

union PackedUnitVec
{
unsigned int packed;
}
;

union PackedTexCoords
{
unsigned int packed;
}
;

union GfxColor
{
unsigned __int8 array[4];
unsigned int packed;
}
;

struct GfxPackedVertex
{
float xyz[3];
float binormalSign;
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

struct XSurface
{
unsigned __int16 flags;
unsigned __int16 vertCount;
unsigned __int16 triCount;
char rigidVertListCount;
char subdivLevelCount;
__int16 blendVertCounts[8];
int unkBlendCount;
char __pad0[4];
GfxVertexUnion0 verts0;
Face *triIndices;
ID3D11Buffer *vb0;
ID3D11ShaderResourceView *vb0View;
ID3D11Buffer *indexBuffer;
XRigidVertList *rigidVertLists;
char *unkBlend;
ID3D11Buffer *blendVertsBuffer;
ID3D11ShaderResourceView *blendVertsView;
__int64 *lmapUnwrap;
ID3D11Buffer *vblmapBuffer;
ID3D11ShaderResourceView *vblmapView;
XSurfaceSubdivInfo *subdiv;
XBlendInfo *blendVerts;
TensionData *tensionData;
void *tensionAccumTable;
ID3D11Buffer *tensionAccumTableBuffer;
ID3D11ShaderResourceView *tensionAccumTableView;
ID3D11Buffer *tensionDataBuffer;
ID3D11ShaderResourceView *tensionDataView;
ID3D11ShaderResourceView *indexBufferView;
SHProbeSimplexDataUnion *shProbeSimplexVertData;
ID3D11Buffer *shProbeSimplexVertBuffer;
ID3D11ShaderResourceView *shProbeSimplexVertBufferView;
int partBits[8];
}
;

struct XModelSurfs
{
const char *name;
XSurface *surfs;
unsigned __int16 numsurfs;
int partBits[8];
}
;

struct XModel
{
const char *name;
char __pad0[12];
unsigned __int8 numBones;
unsigned __int8 numRootBones;
unsigned __int8 numReactiveMotionParts;
char b;
unsigned __int16 numClientBones;
unsigned __int8 numsurfs;
char __pad1[5];
unsigned __int16 numCollSurfs;
char __pad2[38];
unsigned __int8 unk1Count;
char __pad3[15];
void *scriptableMoverDef;
void *proceduralBones;
scr_string_t *aimAssistBones;
unsigned __int8 numAimAssistBones;
char __pad4[39];
scr_string_t *boneNames;
unsigned __int8 *parentList;
XModelAngle *tagAngles;
XModelTagPos *tagPositions;
unsigned __int8 *partClassification;
DObjAnimMat *baseMat;
ReactiveMotionModelPart *reactiveMotionParts;
ReactiveMotionModelTweaks *reactiveMotionTweaks;
Material **materialHandles;
XModelLodInfo lodInfo[6];
XModelCollSurf_s *collSurfs;
XBoneInfo *boneInfo;
unsigned __int16 *invHighMipRadius;
PhysicsAsset *physAsset;
PhysicsFXShape *physFxShape;
char *unk1;
unsigned __int16 unkNamesCount;
char __pad5[6];
scr_string_t *unkNames;
char __pad6[16];
int unk2Count;
char __pad8[4];
char *unk2;
char __pad9[1];
unsigned __int8 unkVec3Count;
char __pad10[6];
vec3_t *unkVec3;
char __pad11[1];
char unk3Count;
char __pad12[6];
void *unk3;
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

struct GfxWrappedBaseBuffer
{
void *buffer;
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

union MayhemFramesUnion
{
void *data;
MayhemAnimFramesSplineCompressed *splineCompressedFrames;
MayhemAnimFramesUncompressed *uncompressedFrames;
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

union MayhemDataChannelsUnion
{
void *data;
MayhemDataKeysSplineCompressed *splineCompressedKeys;
MayhemDataKeysUncompressed *uncompressedKeys;
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

struct Material
{
const char *name;
char __pad0[272];
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

struct GfxImageStreamData
{
unsigned __int16 width;
unsigned __int16 height;
unsigned int pixelSize;
}
;

struct GfxImage
{
char __pad0[24];
DXGI_FORMAT imageFormat;
char flags;
char __pad1[3];
MapType mapType;
char sematic;
char category;
unsigned __int8 platform[2];
char __pad2[3];
unsigned int dataLen1;
unsigned int dataLen2;
unsigned __int16 width;
unsigned __int16 height;
unsigned __int16 depth;
unsigned __int16 numElements;
unsigned __int8 mipmapCount;
bool streamed;
char __pad3[6];
char *pixelData;
GfxImageStreamData streams[4];
const char *name;
}
;

enum __shex DXGI_FORMAT : __int32
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
DXGI_FORMAT_FORCE_UINT = -0x1,
}
;

struct cmd_function_s
{
cmd_function_s *next;
const char *name;
void (__fastcall *function)();
}
;

enum DvarSetSource : unsigned __int32
{
DVAR_SOURCE_INTERNAL = 0x0,
DVAR_SOURCE_EXTERNAL = 0x1,
DVAR_SOURCE_SCRIPT = 0x2,
DVAR_SOURCE_UISCRIPT = 0x3,
DVAR_SOURCE_SERVERCMD = 0x4,
DVAR_SOURCE_NUM = 0x5,
}
;

enum DvarFlags : unsigned __int32
{
DVAR_FLAG_NONE = 0x0,
DVAR_FLAG_SAVED = 0x1,
DVAR_FLAG_LATCHED = 0x2,
DVAR_FLAG_CHEAT = 0x4,
DVAR_FLAG_REPLICATED = 0x8,
DVAR_FLAG_WRITE = 0x800,
DVAR_FLAG_READ = 0x2000,
}
;

enum DvarType : unsigned __int8
{
DVAR_TYPE_BOOL = 0x0,
boolean = 0x0,
DVAR_TYPE_FLOAT = 0x1,
value = 0x1,
DVAR_TYPE_FLOAT_2 = 0x2,
vec2 = 0x2,
DVAR_TYPE_FLOAT_3 = 0x3,
vec3 = 0x3,
DVAR_TYPE_FLOAT_4 = 0x4,
vec4 = 0x4,
DVAR_TYPE_INT = 0x5,
integer = 0x5,
DVAR_TYPE_ENUM = 0x6,
enumeration = 0x6,
DVAR_TYPE_STRING = 0x7,
string = 0x7,
DVAR_TYPE_COLOR = 0x8,
color = 0x8,
DVAR_TYPE_FLOAT_3_COLOR = 0x9,
rgb = 0x9,
DVAR_TYPE_COUNT = 0xA,
}
;

union DvarValue
{
bool enabled;
int integer;
unsigned int unsignedInt;
float value;
float vector[4];
const char *string;
char color[4];
}
;

struct $A37BA207B3DDD6345C554D4661813EDD
{
int stringCount;
const char *const *strings;
}
;

struct $9CA192F9DB66A3CB7E01DE78A0DEA53D
{
int min;
int max;
}
;

struct $251C2428A496074035CACA7AAF3D55BD
{
float min;
float max;
}
;

union DvarLimits
{
$A37BA207B3DDD6345C554D4661813EDD enumeration;
$9CA192F9DB66A3CB7E01DE78A0DEA53D integer;
$251C2428A496074035CACA7AAF3D55BD value;
$251C2428A496074035CACA7AAF3D55BD vector;
}
;

struct dvar_t
{
unsigned int checksum;
unsigned int flags;
char level;
DvarType type;
bool modified;
DvarValue current;
DvarValue latched;
DvarValue reset;
DvarLimits domain;
bool (__fastcall *domainFunc)(dvar_t *, DvarValue);
dvar_t *hashNext;
}
;

struct GMovingPlatforms
;

struct xbox::services::marketplace::catalog_item
;

struct bdRewardGameEventTimeStamp
;

struct bdLobbyService
;

struct bdByteBuffer
;

struct ntl::nxheap
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

struct ID3D11Buffer
{
ID3D11BufferVtbl *lpVtbl;
}
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

struct ID3D11Texture1D
{
ID3D11Texture1DVtbl *lpVtbl;
}
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

struct ID3D11Texture2D
{
ID3D11Texture2DVtbl *lpVtbl;
}
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

struct ID3D11Texture3D
{
ID3D11Texture3DVtbl *lpVtbl;
}
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

struct ID3D11Resource
{
ID3D11ResourceVtbl *lpVtbl;
}
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

struct ID3D11ShaderResourceView
{
ID3D11ShaderResourceViewVtbl *lpVtbl;
}
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

struct ID3D11UnorderedAccessView
{
ID3D11UnorderedAccessViewVtbl *lpVtbl;
}
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

struct ID3D11RenderTargetView
{
ID3D11RenderTargetViewVtbl *lpVtbl;
}
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

struct ID3D11DepthStencilView
{
ID3D11DepthStencilViewVtbl *lpVtbl;
}
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

struct ID3D11InputLayout
{
ID3D11InputLayoutVtbl *lpVtbl;
}
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

struct ID3D11VertexShader
{
ID3D11VertexShaderVtbl *lpVtbl;
}
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

struct ID3D11GeometryShader
{
ID3D11GeometryShaderVtbl *lpVtbl;
}
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

struct ID3D11PixelShader
{
ID3D11PixelShaderVtbl *lpVtbl;
}
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

struct ID3D11HullShader
{
ID3D11HullShaderVtbl *lpVtbl;
}
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

struct ID3D11DomainShader
{
ID3D11DomainShaderVtbl *lpVtbl;
}
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

struct ID3D11ComputeShader
{
ID3D11ComputeShaderVtbl *lpVtbl;
}
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

struct ID3D11BlendState
{
ID3D11BlendStateVtbl *lpVtbl;
}
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

struct ID3D11DepthStencilState
{
ID3D11DepthStencilStateVtbl *lpVtbl;
}
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

struct ID3D11RasterizerState
{
ID3D11RasterizerStateVtbl *lpVtbl;
}
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

struct ID3D11SamplerState
{
ID3D11SamplerStateVtbl *lpVtbl;
}
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

struct ID3D11Query
{
ID3D11QueryVtbl *lpVtbl;
}
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

struct ComFastFileLoadModule
;

struct SvCommandInfo
{
const char *name;
void (__fastcall *function)();
cmd_function_s svvar;
}
;

struct BgVehiclePhysicsWaterBuoyancy
;

struct SvServerInitSettings
{
char mapName[64];
char gameType[64];
char serverHostName[64];
bool hardcoreMode;
unsigned int maxClientCount;
unsigned int maxAgentCount;
bool isMapPreloaded;
bool isSaveGame;
bool isRestart;
bool isFrontEnd;
char __pad0[2];
bool serverThreadStartup;
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

struct DDL
;

struct Online_Friends
;

struct NetConnection
;

struct hclWorldEx
;

enum XAssetType : __int32
{
ASSET_TYPE_PHYSICSLIBRARY = 0x0,
ASSET_TYPE_PHYSICS_SFX_EVENT_ASSET = 0x1,
ASSET_TYPE_PHYSICS_VFX_EVENT_ASSET = 0x2,
ASSET_TYPE_PHYSICSASSET = 0x3,
ASSET_TYPE_PHYSICS_FX_PIPELINE = 0x4,
ASSET_TYPE_PHYSICS_FX_SHAPE = 0x5,
ASSET_TYPE_XANIMPARTS = 0x6,
ASSET_TYPE_XMODEL_SURFS = 0x7,
ASSET_TYPE_XMODEL = 0x8,
ASSET_TYPE_MAYHEM = 0x9,
ASSET_TYPE_MATERIAL = 0xA,
ASSET_TYPE_COMPUTESHADER = 0xB,
ASSET_TYPE_VERTEXSHADER = 0xC,
ASSET_TYPE_HULLSHADER = 0xD,
ASSET_TYPE_DOMAINSHADER = 0xE,
ASSET_TYPE_PIXELSHADER = 0xF,
ASSET_TYPE_VERTEXDECL = 0x10,
ASSET_TYPE_TECHNIQUE_SET = 0x11,
ASSET_TYPE_IMAGE = 0x12,
ASSET_TYPE_SOUND_GLOBALS = 0x13,
ASSET_TYPE_SOUND_BANK = 0x14,
ASSET_TYPE_SOUND_BANK_TRANSIENT = 0x15,
ASSET_TYPE_CLIPMAP = 0x16,
ASSET_TYPE_COMWORLD = 0x17,
ASSET_TYPE_GLASSWORLD = 0x18,
ASSET_TYPE_PATHDATA = 0x19,
ASSET_TYPE_NAVMESH = 0x1A,
ASSET_TYPE_MAP_ENTS = 0x1B,
ASSET_TYPE_FXWORLD = 0x1C,
ASSET_TYPE_GFXWORLD = 0x1D,
ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE = 0x1E,
ASSET_TYPE_IESPROFILE = 0x1F,
ASSET_TYPE_LIGHT_DEF = 0x20,
ASSET_TYPE_UI_MAP = 0x21,
ASSET_TYPE_ANIMCLASS = 0x22,
ASSET_TYPE_PLAYERANIM = 0x23,
ASSET_TYPE_GESTURE = 0x24,
ASSET_TYPE_LOCALIZE_ENTRY = 0x25,
ASSET_TYPE_ATTACHMENT = 0x26,
ASSET_TYPE_WEAPON = 0x27,
ASSET_TYPE_VFX = 0x28,
ASSET_TYPE_IMPACT_FX = 0x29,
ASSET_TYPE_SURFACE_FX = 0x2A,
ASSET_TYPE_AITYPE = 0x2B,
ASSET_TYPE_MPTYPE = 0x2C,
ASSET_TYPE_CHARACTER = 0x2D,
ASSET_TYPE_XMODELALIAS = 0x2E,
ASSET_TYPE_UNKNOWN = 0x2F,
ASSET_TYPE_RAWFILE = 0x30,
ASSET_TYPE_SCRIPTFILE = 0x31,
ASSET_TYPE_STRINGTABLE = 0x32,
ASSET_TYPE_LEADERBOARD = 0x33,
ASSET_TYPE_VIRTUAL_LEADERBOARD = 0x34,
ASSET_TYPE_STRUCTUREDDATADEF = 0x35,
ASSET_TYPE_DDL = 0x36,
ASSET_TYPE_TRACER = 0x37,
ASSET_TYPE_VEHICLE = 0x38,
ASSET_TYPE_ADDON_MAP_ENTS = 0x39,
ASSET_TYPE_NET_CONST_STRINGS = 0x3A,
ASSET_TYPE_LUA_FILE = 0x3B,
ASSET_TYPE_SCRIPTABLE = 0x3C,
ASSET_TYPE_EQUIPMENT_SND_TABLE = 0x3D,
ASSET_TYPE_VECTORFIELD = 0x3E,
ASSET_TYPE_PARTICLE_SIM_ANIMATION = 0x3F,
ASSET_TYPE_STREAMING_INFO = 0x40,
ASSET_TYPE_LASER = 0x41,
ASSET_TYPE_TTF = 0x42,
ASSET_TYPE_SUIT = 0x43,
ASSET_TYPE_SUITANIMPACKAGE = 0x44,
ASSET_TYPE_SPACESHIPTARGET = 0x45,
ASSET_TYPE_RUMBLE = 0x46,
ASSET_TYPE_RUMBLE_GRAPH = 0x47,
ASSET_TYPE_ANIM_PACKAGE = 0x48,
ASSET_TYPE_SFX_PACKAGE = 0x49,
ASSET_TYPE_VFX_PACKAGE = 0x4A,
ASSET_TYPE_BEHAVIOR_TREE = 0x4B,
ASSET_TYPE_XANIM_ARCHETYPE = 0x4C,
ASSET_TYPE_XANIM_PROCEDURALBONES = 0x4D,
ASSET_TYPE_RETICLE = 0x4E,
ASSET_TYPE_GFXLIGHTMAP = 0x4F,
ASSET_TYPE_COUNT = 0x50,
}
;

union XAssetHeader
{
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
void *data;
}
;

struct XAsset
{
XAssetType type;
XAssetHeader header;
}
;

typedef unsigned int XBlendInfo
;

struct XSubdivRigidVertList
{
unsigned int firstFace;
unsigned int faceCount;
unsigned int firstRegularPatch;
unsigned int regularPatchCount;
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
ID3D11Buffer *regularPatchIndexBuffer;
ID3D11Buffer *faceIndexBuffer;
ID3D11ShaderResourceView *regularPatchIndexBufferView;
ID3D11ShaderResourceView *regularPatchFlagsView;
ID3D11ShaderResourceView *facePointsView;
ID3D11ShaderResourceView *edgePointsView;
ID3D11ShaderResourceView *vertexPointsView;
ID3D11ShaderResourceView *normalsView;
ID3D11ShaderResourceView *transitionPointsView;
ID3D11ShaderResourceView *regularPatchConesView;
}
;

struct GfxSubdivCache
{
unsigned int size;
ID3D11Buffer *subdivCacheBuffer;
ID3D11ShaderResourceView *subdivCacheView;
}
;

struct XSurfaceSubdivInfo
{
XSurfaceSubdivLevel *levels;
char __pad0[24];
GfxSubdivCache cache;
}
;

struct TensionData
{
char __pad0[32];
}
;

struct SHProbeSimplexData0
{
char __pad0[32];
}
;

struct SHProbeSimplexData1
{
char __pad0[8];
}
;

struct SHProbeSimplexData2
{
char __pad0[24];
}
;

union SHProbeSimplexDataUnion
{
SHProbeSimplexData0 data0;
SHProbeSimplexData1 data1;
SHProbeSimplexData2 data2;
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

struct XModelLodInfo
{
float dist;
unsigned __int16 numsurfs;
unsigned __int16 surfIndex;
XModelSurfs *modelSurfs;
int partBits[8];
XSurface *surfs;
int unk;
char flags;
char pad[3];
}
;

union $3F5B938DF5E4F97F8E1F05029085C3EA
{
float radiusSquared;
unsigned int radiusSquaredAsInt;
}
;

struct Bounds
{
vec3_t midPoint;
vec3_t halfSize;
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

struct bfx::ComponentManager
;

struct bfx::Matrix
;

struct bfx::Timer
;

struct bfx
;

struct bfx::AreaHandle
;

struct hkcdStaticAabbTree
;

struct ProtoBufSerializer
;

struct bdSocket
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

struct __cppobj lua_State : ChunkHeader
{
HksGlobal *m_global;
CallStack m_callStack;
ApiStack m_apistack;
UpValue *pending;
HksObject globals;
HksObject m_cEnv;
CallSite *m_callsites;
int m_numberOfCCalls;
void *m_context;
InternString *m_name;
lua_State *m_nextState;
lua_State *m_nextStateStack;
Status m_status;
HksError m_error;
}
;

struct __cppobj HashTable : ChunkHeader
{
Metatable *m_meta;
unsigned int m_version;
unsigned int m_mask;
Node *m_hashPart;
HksObject *m_arrayPart;
unsigned int m_arraySize;
Node *m_freeNode;
}
;

struct __cppobj cclosure : ChunkHeader
{
lua_function m_function;
HashTable *m_env;
__int16 m_numUpvalues;
__int16 m_flags;
InternString *m_name;
HksObject m_upvalues[1];
}
;

struct GenericChunkHeader
{
unsigned __int64 m_flags;
}
;

struct __cppobj ChunkHeader : GenericChunkHeader
{
ChunkHeader *m_next;
}
;

struct __cppobj UserData : ChunkHeader
{
unsigned __int64 m_envAndSizeOffsetHighBits;
unsigned __int64 m_metaAndSizeOffsetLowBits;
char m_data[8];
}
;

struct InternString
{
unsigned __int64 m_flags;
unsigned __int64 m_lengthbits;
unsigned int m_hash;
char m_data[30];
}
;

union HksValue
{
cclosure *cClosure;
void *closure;
UserData *userData;
HashTable *table;
void *tstruct;
InternString *str;
void *thread;
void *ptr;
float number;
__int64 i64;
unsigned __int64 ui64;
unsigned int native;
bool boolean;
}
;

enum __shex HksObjectType : __int32
{
TNIL = 0x0,
TBOOLEAN = 0x1,
TLIGHTUSERDATA = 0x2,
TNUMBER = 0x3,
TSTRING = 0x4,
TTABLE = 0x5,
TFUNCTION = 0x6,
TUSERDATA = 0x7,
TTHREAD = 0x8,
TIFUNCTION = 0x9,
TCFUNCTION = 0xA,
TUI64 = 0xB,
TSTRUCT = 0xC,
NUM_TYPE_OBJECTS = 0xE,
TANY = -0x2,
TNONE = -0x1,
}
;

struct HksObject
{
HksObjectType t;
HksValue v;
}
;

struct hksInstruction
{
unsigned int code;
}
;

struct ActivationRecord
{
HksObject *m_base;
const hksInstruction *m_returnAddress;
__int16 m_tailCallDepth;
__int16 m_numVarargs;
int m_numExpectedReturns;
}
;

struct CallStack
{
ActivationRecord *m_records;
ActivationRecord *m_lastrecord;
ActivationRecord *m_current;
const hksInstruction *m_current_lua_pc;
const hksInstruction *m_hook_return_addr;
int m_hook_level;
}
;

struct ApiStack
{
HksObject *top;
HksObject *base;
HksObject *alloc_top;
HksObject *bottom;
}
;

struct __cppobj UpValue : ChunkHeader
{
HksObject m_storage;
HksObject *loc;
UpValue *m_next;
}
;

struct CallSite
{
_SETJMP_FLOAT128 m_jumpBuffer[16];
CallSite *m_prev;
}
;

enum Status : __int32
{
NEW = 0x1,
RUNNING = 0x2,
YIELDED = 0x3,
DEAD_ERROR = 0x4,
}
;

enum HksError : __int32
{
HKS_NO_ERROR = 0x0,
HKS_GC_YIELD = 0x1,
HKS_THROWING_ERROR = 0xFFFFFE0C,
HKS_ERRERR = 0xFFFFFED4,
HKS_ERRMEM = 0xFFFFFF38,
HKS_ERRRUN = 0xFFFFFF9C,
HKS_ERRFILE = 0xFFFFFFFB,
HKS_ERRSYNTAX = 0xFFFFFFFC,
}
;

struct lua_Debug
{
int event;
const char *name;
const char *namewhat;
const char *what;
const char *source;
int currentline;
int nups;
int nparams;
int ishksfunc;
int linedefined;
int lastlinedefined;
char short_src[512];
int callstack_level;
int is_tail_call;
}
;

struct Node
{
HksObject m_key;
HksObject m_value;
}
;

struct Metatable
{
}
;

enum HksCompilerSettings_BytecodeSharingFormat
{
BYTECODE_DEFAULT = 0x0,
BYTECODE_INPLACE = 0x1,
BYTECODE_REFERENCED = 0x2,
}
;

enum HksCompilerSettings_IntLiteralOptions
{
INT_LITERALS_NONE = 0x0,
INT_LITERALS_LUD = 0x1,
INT_LITERALS_32BIT = 0x1,
INT_LITERALS_UI64 = 0x2,
INT_LITERALS_64BIT = 0x2,
INT_LITERALS_ALL = 0x3,
}
;

struct HksCompilerSettings
{
int m_emitStructCode;
const char **m_stripNames;
int m_emitGlobalMemoization;
int _m_isHksGlobalMemoTestingMode;
HksCompilerSettings_BytecodeSharingFormat m_bytecodeSharingFormat;
HksCompilerSettings_IntLiteralOptions m_enableIntLiterals;
int (__fastcall *m_debugMap)(const char *, int);
}
;

enum HksBytecodeSharingMode
{
HKS_BYTECODE_SHARING_OFF = 0x0,
HKS_BYTECODE_SHARING_ON = 0x1,
HKS_BYTECODE_SHARING_SECURE = 0x2,
}
;

struct HksGcWeights
{
int m_removeString;
int m_finalizeUserdataNoMM;
int m_finalizeUserdataGcMM;
int m_cleanCoroutine;
int m_removeWeak;
int m_markObject;
int m_traverseString;
int m_traverseUserdata;
int m_traverseCoroutine;
int m_traverseWeakTable;
int m_freeChunk;
int m_sweepTraverse;
}
;

struct GarbageCollector_Stack
{
void *m_storage;
unsigned int m_numEntries;
unsigned int m_numAllocated;
}
;

struct ProtoList
{
void **m_protoList;
unsigned __int16 m_protoSize;
unsigned __int16 m_protoAllocSize;
}
;

struct GarbageCollector
{
int m_target;
int m_stepsLeft;
int m_stepLimit;
HksGcWeights m_costs;
int m_unit;
_SETJMP_FLOAT128 (*m_jumpPoint)[16];
lua_State *m_mainState;
lua_State *m_finalizerState;
void *m_memory;
int m_phase;
GarbageCollector_Stack m_resumeStack;
GarbageCollector_Stack m_greyStack;
GarbageCollector_Stack m_remarkStack;
GarbageCollector_Stack m_weakStack;
int m_finalizing;
HksObject m_safeTableValue;
lua_State *m_startOfStateStackList;
lua_State *m_endOfStateStackList;
lua_State *m_currentState;
HksObject m_safeValue;
void *m_compiler;
void *m_bytecodeReader;
void *m_bytecodeWriter;
int m_pauseMultiplier;
int m_stepMultiplier;
bool m_stopped;
int (__fastcall *m_gcPolicy)(lua_State *);
unsigned __int64 m_pauseTriggerMemoryUsage;
int m_stepTriggerCountdown;
unsigned int m_stringTableIndex;
unsigned int m_stringTableSize;
UserData *m_lastBlackUD;
UserData *m_activeUD;
}
;

enum MemoryManager_ChunkColor
{
RED = 0x0,
BLACK = 0x1,
}
;

struct ChunkList
{
ChunkHeader m_prevToStart;
}
;

enum Hks_DeleteCheckingMode
{
HKS_DELETE_CHECKING_OFF = 0x0,
HKS_DELETE_CHECKING_ACCURATE = 0x1,
HKS_DELETE_CHECKING_SAFE = 0x2,
}
;

struct MemoryManager
{
void *(__fastcall *m_allocator)(void *, void *, unsigned __int64, unsigned __int64);
void *m_allocatorUd;
MemoryManager_ChunkColor m_chunkColor;
unsigned __int64 m_used;
unsigned __int64 m_highwatermark;
ChunkList m_allocationList;
ChunkList m_sweepList;
ChunkHeader *m_lastKeptChunk;
lua_State *m_state;
ChunkList m_deletedList;
int m_deleteMode;
Hks_DeleteCheckingMode m_deleteCheckingMode;
}
;

struct StaticStringCache
{
HksObject m_objects[41];
}
;

enum HksBytecodeEndianness
{
HKS_BYTECODE_DEFAULT_ENDIAN = 0x0,
HKS_BYTECODE_BIG_ENDIAN = 0x1,
HKS_BYTECODE_LITTLE_ENDIAN = 0x2,
}
;

struct RuntimeProfileData_Stats
{
unsigned __int64 hksTime;
unsigned __int64 callbackTime;
unsigned __int64 gcTime;
unsigned __int64 cFinalizerTime;
unsigned __int64 compilerTime;
unsigned int hkssTimeSamples;
unsigned int callbackTimeSamples;
unsigned int gcTimeSamples;
unsigned int compilerTimeSamples;
unsigned int num_newuserdata;
unsigned int num_tablerehash;
unsigned int num_pushstring;
unsigned int num_pushcfunction;
unsigned int num_newtables;
}
;

struct RuntimeProfileData
{
__int64 stackDepth;
__int64 callbackDepth;
unsigned __int64 lastTimer;
RuntimeProfileData_Stats frameStats;
unsigned __int64 gcStartTime;
unsigned __int64 finalizerStartTime;
unsigned __int64 compilerStartTime;
unsigned __int64 compilerStartGCTime;
unsigned __int64 compilerStartGCFinalizerTime;
unsigned __int64 compilerCallbackStartTime;
__int64 compilerDepth;
void *outFile;
lua_State *rootState;
}
;

struct HksGlobal
{
MemoryManager m_memory;
GarbageCollector m_collector;
StringTable m_stringTable;
HksBytecodeSharingMode m_bytecodeSharingMode;
unsigned int m_tableVersionInitializer;
HksObject m_registry;
ProtoList m_protoList;
HashTable *m_structProtoByName;
ChunkList m_userDataList;
lua_State *m_root;
StaticStringCache m_staticStringCache;
void *m_debugger;
void *m_profiler;
RuntimeProfileData m_runProfilerData;
HksCompilerSettings m_compilerSettings;
int (__fastcall *m_panicFunction)(lua_State *);
void *m_luaplusObjectList;
int m_heapAssertionFrequency;
int m_heapAssertionCount;
void (*m_logFunction)(lua_State *, const char *, ...);
HksBytecodeEndianness m_bytecodeDumpEndianness;
}
;

typedef int (__fastcall *lua_function)(lua_State *)
;

struct luaL_Reg
{
const char *name;
lua_function function;
}
;

enum netadrtype_t
{
NA_BOT = 0x0,
NA_BAD = 0x1,
NA_LOOPBACK = 0x2,
NA_BROADCAST = 0x3,
NA_IP = 0x4,
}
;

enum netsrc_t
{
NS_CLIENT1 = 0x0,
NS_MAXCLIENTS = 0x1,
NS_SERVER = 0x2,
NS_PACKET = 0x3,
NS_INVALID_NETSRC = 0x4,
}
;

struct netadr_s
{
netadrtype_t type;
unsigned __int8 ip[4];
unsigned __int16 port;
netsrc_t localNetID;
unsigned int addrHandleIndex;
}
;

struct SessionStaticData
{
const char *sessionName;
bool registerUsersWithVoice;
}
;

struct ClientInfo
{
bool registered;
bool voiceRegistered;
unsigned __int64 xuid;
int natType;
netadr_s addr;
int usrVoiceConnectivityBits;
int nextConnectivityTestTime[1];
bool muted;
bool privateSlot;
}
;

struct RegisteredUser
{
bool active;
unsigned __int64 xuid;
bool privateSlot;
}
;

struct SessionDynamicData
{
bool sessionHandle;
char __pad0[47];
bool keysGenerated;
bool sessionStartCalled;
unsigned __int64 sessionNonce;
int privateSlots;
int publicSlots;
int flags;
bool qosListenEnabled;
ClientInfo users[18];
int localVoiceConnectivityBits;
int sessionCreateController;
int sessionDeleteTime;
bool allowJoining;
RegisteredUser internalRegisteredUsers[18];
}
;

struct SessionData
{
SessionStaticData staticData;
SessionDynamicData dyn;
}
;

enum PartyPreloadMapStage : unsigned __int32
{
PRELOAD_MAP_IDLE = 0x0,
PRELOAD_MAP_INITIATED = 0x1,
PRELOAD_MAP_STARTED = 0x2,
PRELOAD_MAP_COUNT = 0x3,
}
;

struct PartyData
{
SessionData *session;
char __pad0[11436];
PartyPreloadMapStage preloadingMapStage;
char __pad0_0[28];
PartyType partyId;
char __pad0_1[24];
int gameStartTime;
char __pad0_2[20];
int areWeHost;
int unk;
int inParty;
int party_systemActive;
char __pad1_2[5];
bool m_gameStartSkipCountdown;
char __pad2[110];
int lobbyFlags;
bool gameStartRequested;
}
;

enum PartyType : __int32
{
GAME_LOBBY_ID = 0x0,
PRIVATE_PARTY_ID = 0x1,
ANY_PARTY_ID = 0x2,
}
;

struct __declspec(align(16)) _SETJMP_FLOAT128
{
unsigned __int64 Part[2];
}
;

typedef ULONG_PTR HCRYPTPROV
;

typedef DWORD *LPDWORD
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

typedef struct _OVERLAPPED *LPWSAOVERLAPPED
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

typedef void (__stdcall *LPWSAOVERLAPPED_COMPLETION_ROUTINE)(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
;

typedef struct _FILETIME *LPFILETIME
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

typedef WORD *LPWORD
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

typedef const WCHAR *PCNZWCH
;

typedef unsigned __int64 *PULONG64
;

typedef struct _OVERLAPPED *LPOVERLAPPED
;

typedef BYTE *PBYTE
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

typedef DWORD *PDWORD
;

typedef struct _SP_DEVICE_INTERFACE_DETAIL_DATA_W *PSP_DEVICE_INTERFACE_DETAIL_DATA_W
;

struct _SP_DEVICE_INTERFACE_DETAIL_DATA_W
{
DWORD cbSize;
WCHAR DevicePath[1];
}
;

typedef struct _SP_DEVICE_INTERFACE_DATA *PSP_DEVICE_INTERFACE_DATA
;

typedef const WCHAR *PCWSTR
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

typedef IUnknown *LPUNKNOWN
;

typedef HANDLE HLOCAL
;

typedef CHAR *PSTR
;

typedef unsigned __int8 *RPC_CSTR
;

typedef ULONG_PTR HCRYPTHASH
;

typedef struct _MEMORY_BASIC_INFORMATION *PMEMORY_BASIC_INFORMATION
;

typedef const void *LPCVOID
;

typedef struct addrinfo *PADDRINFOA
;

typedef struct _FILETIME *PFILETIME
;

typedef ULONG *PULONG
;

typedef unsigned int GROUP
;

typedef const CHAR *PCSTR
;

typedef struct HMENU__ *HMENU
;

struct HMENU__
{
int unused;
}
;

typedef int RPC_STATUS
;

typedef const WCHAR *LPCWCH
;

typedef unsigned __int64 uintptr_t
;

typedef PRTL_CRITICAL_SECTION LPCRITICAL_SECTION
;

