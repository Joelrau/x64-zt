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

typedef char CHAR
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

struct _FILETIME
{
DWORD dwLowDateTime;
DWORD dwHighDateTime;
}
;

typedef GUID IID
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

typedef const CHAR *LPCSTR
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

struct _SP_DEVICE_INTERFACE_DATA
{
DWORD cbSize;
GUID InterfaceClassGuid;
DWORD Flags;
ULONG_PTR Reserved;
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

typedef unsigned __int8 UCHAR
;

typedef __int16 SHORT
;

typedef unsigned int ULONG
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

typedef unsigned __int64 ULONGLONG
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
unsigned int Lo;
int Hi;
};
LONGLONG int64;
}
;

struct tagCY::$983F061E0B702DD8AE0B4B955735A6A3
{
unsigned int Lo;
int Hi;
}
;

typedef double DATE
;

typedef _FILETIME FILETIME
;

typedef GUID CLSID
;

typedef tagCLIPDATA CLIPDATA
;

struct tagCLIPDATA
{
ULONG cbSize;
int ulClipFmt;
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

typedef CHAR *LPSTR
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

typedef void *PVOID
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

struct _PROCESS_INFORMATION
{
HANDLE hProcess;
HANDLE hThread;
DWORD dwProcessId;
DWORD dwThreadId;
}
;

struct _SECURITY_ATTRIBUTES
{
DWORD nLength;
LPVOID lpSecurityDescriptor;
BOOL bInheritHandle;
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

struct RUNTIME_FUNCTION
{
_UNKNOWN *__ptr32 FunctionStart __offset(OFF64|RVAOFF);
_UNKNOWN *__ptr32 FunctionEnd __offset(OFF64|RVAOFF|PASTEND);
_UNKNOWN *__ptr32 UnwindInfo __offset(OFF64|RVAOFF);
}
;

struct UNWIND_INFO
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

struct C_SCOPE_TABLE
{
_UNKNOWN *__ptr32 Begin __offset(OFF64|RVAOFF);
_UNKNOWN *__ptr32 End __offset(OFF64|RVAOFF|PASTEND);
_UNKNOWN *__ptr32 Handler __offset(OFF64|RVAOFF);
_UNKNOWN *__ptr32 Target __offset(OFF64|RVAOFF);
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

struct ESTypeList
{
int nCount __dec __tabform(NODUPS,1);
_UNKNOWN *__ptr32 pTypeArray __offset(OFF64|RVAOFF);
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
}
;

struct TypeDescriptor
{
_UNKNOWN *pVFTable;
_UNKNOWN *spare;
char name[] __strlit(C,"windows-1252");
}
;

struct IPtoStateMap
{
_UNKNOWN *__ptr32 pc __offset(OFF64|RVAOFF);
int state __dec __tabform(NODUPS,1);
}
;

struct hostent
{
char *h_name;
char **h_aliases;
__int16 h_addrtype;
__int16 h_length;
char **h_addr_list;
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

struct localeinfo_struct
{
pthreadlocinfo locinfo;
pthreadmbcinfo mbcinfo;
}
;

typedef threadlocaleinfostruct *pthreadlocinfo
;

struct threadlocaleinfostruct
{
int refcount;
unsigned int lc_codepage;
unsigned int lc_collate_cp;
unsigned int lc_handle[6];
LC_ID lc_id[6];
threadlocaleinfostruct::$F0551D0CB09E7A078CAEF7CAC43D74C7 lc_category[6];
int lc_clike;
int mb_cur_max;
int *lconv_intl_refcount;
int *lconv_num_refcount;
int *lconv_mon_refcount;
lconv *lconv;
int *ctype1_refcount;
unsigned __int16 *ctype1;
const unsigned __int16 *pctype;
const unsigned __int8 *pclmap;
const unsigned __int8 *pcumap;
__lc_time_data *lc_time_curr;
}
;

typedef tagLC_ID LC_ID
;

struct tagLC_ID
{
unsigned __int16 wLanguage;
unsigned __int16 wCountry;
unsigned __int16 wCodePage;
}
;

struct threadlocaleinfostruct::$F0551D0CB09E7A078CAEF7CAC43D74C7
{
char *locale;
wchar_t *wlocale;
int *refcount;
int *wrefcount;
}
;

struct lconv
{
char *decimal_point;
char *thousands_sep;
char *grouping;
char *int_curr_symbol;
char *currency_symbol;
char *mon_decimal_point;
char *mon_thousands_sep;
char *mon_grouping;
char *positive_sign;
char *negative_sign;
char int_frac_digits;
char frac_digits;
char p_cs_precedes;
char p_sep_by_space;
char n_cs_precedes;
char n_sep_by_space;
char p_sign_posn;
char n_sign_posn;
wchar_t *_W_decimal_point;
wchar_t *_W_thousands_sep;
wchar_t *_W_int_curr_symbol;
wchar_t *_W_currency_symbol;
wchar_t *_W_mon_decimal_point;
wchar_t *_W_mon_thousands_sep;
wchar_t *_W_positive_sign;
wchar_t *_W_negative_sign;
}
;

struct __lc_time_data
;

typedef threadmbcinfostruct *pthreadmbcinfo
;

struct threadmbcinfostruct
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

struct bdInetAddr
;

struct bdAddr
;

struct hks::Preprocessor::PreprocessorStateProxy
;

struct bdQoSReplyPacket
;

struct bdCommonAddr
;

struct hks
;

struct hks::CodeGenerator
;

struct hks::HeapVisualizer
;

struct hks::Preprocessor
;

struct hks::StringReader
;

struct hks::StructProto
;

struct hks::CompilerReader
;

struct hks::CompilerLexer
;

struct hks::GarbageCollector
;

struct hks::Visitor::Node
;

struct hks::HashTable
;

struct hks::Serializer
;

struct hks::BytecodeWriter
;

struct hks::StructInst
;

struct hks::CodeGenerator::FunctionGenerationState
;

struct hks::StructUtil
;

struct hks::CallStack
;

struct hks::StringTable
;

struct hks::Parser
;

struct hks::StringPinner
;

struct hks::MemoryAllocationsByType
;

struct hks::BytecodeReader
;

struct hks::BytecodeReader::Reader
;

struct hks::Visitor
;

struct hks::GarbageInspector
;

struct hks::MemoryBuffer
;

struct MatchMakingInfo
;

struct bdPendingBufferTransfer
;

struct hks::SimpleCompilerState
;

struct CustomSearchInfo_FindPath
;

struct CustomSearchInfo_FindPathWithinRadius
;

struct pathnode_t
{
pathnode_constant_t constant;
pathnode_dynamic_t dynamic;
pathnode_transient_t transient;
}
;

struct bdHTTPWrapper
;

struct dwQoSMultiProbeListener
;

struct bdUnicastConnection
;

struct bdTaskByteBuffer
;

struct LargeLocal
;

struct bdMessage
;

struct bdCypher3Des
;

struct bdSAckChunk
;

struct bdNetStartParams
;

struct std::error_category
;

struct Leaderboard
;

struct MemoryHashInfo
;

struct bdAntiCheatChallenge
;

struct std::exception
;

struct bdGetHostByName
;

struct bdSecurityKey
;

struct GfxCmdBufState
;

struct bdString
;

struct bdByteBuffer
;

struct bdBitBuffer
;

struct bdTag
;

struct bdDTLSCookieAck
;

struct bdRandom
;

struct bdEndpoint
;

struct bdEndpointHashingClass
;

struct bdHashTiger192
;

struct bdUserInfo
;

struct bdSecurityKeyMap
;

struct bdRemoteTask
;

struct PublicProfileInfo
;

struct bdLobbyService
;

struct bdAntiCheatResponses
;

struct bdDTLSInitAck
;

struct bdCounterValue
;

struct bdPerformanceValue
;

struct bdBandwidthTestResults
;

struct bdBandwidthTestClient
;

struct bdLobbyEventHandler
;

struct bdStats
;

struct bdStorage
;

struct bdRemoteTaskManager
;

struct bdSessionID
;

struct bdAntiCheatChallenges
;

struct bdMatchMakingInfo
;

struct bdStatsInfo
;

struct bdLobbyConnectionListener
;

struct bdLobbyConnection
;

struct bdAntiCheatChallengeParam
;

struct bdNetImpl
;

struct bdDTLSAssociation
;

struct bdGetHostByNameConfig
;

struct bdUPnP
;

struct bdUPnPDevice
;

struct bdSockAddr
;

struct bdQoSProbe::bdQoSProbeEntryWrapper
;

struct bdQoSProbe
;

struct bdAddrHandle
;

struct bdPacketInterceptor
;

struct bdSocketRouter
;

struct bdFileMetaData
;

struct bdQoSProbeListener
;

struct bdNATTypeDiscoveryConfig
;

struct bdNATTypeDiscoveryClient
;

struct bdIPDiscoveryClient
;

struct bdNATTravListener
;

struct bdRunnable
;

struct bdDTLSHeader
;

struct bdQoSRequestPacket
;

struct bdServiceBandwidthArbitrator
;

struct bdNATTravClient::CachedTraversal
;

struct bdNATTravClient
;

struct bdNATTravClientData
;

struct bdDTLSError
;

struct bdNATTraversalPacket
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

struct bdIPDiscoveryPacketReply
;

struct bdIPDiscoveryPacket
;

struct bdDTLSData
;

struct bdStopwatch
;

struct bdVoteRank
;

struct bdContextUserStorageFileInfo
;

struct bdSemaphore
;

struct bdThread
;

struct bdSocket
;

struct bdHMacSHA1
;

struct bdStreamSocket
;

struct bdECCKey
;

struct bdSequenceNumber
;

struct bdSequenceNumberStore
;

struct bdDMLInfo
;

struct bdCypherAES
;

struct dxGeom
;

struct bdContentStreaming
;

struct bdSessionInvite
;

struct bdSummaryMetaHandler
;

struct bdURL
;

struct bdPreCopyResult
;

struct bdUCDPublicKeyInfo
;

struct dxSpace
;

struct bdConnection
;

struct bdConnectionStore
;

struct bdPacket
;

struct bdDispatcher
;

struct bdLoopbackConnection
;

struct bdConnectionStatistics
;

struct bdUnreliableSendWindow
;

struct bdChunk
;

struct bdDataChunk
;

struct bdReliableSendWindow::bdMessageFrame
;

struct bdReliableSendWindow
;

struct bdReliableReceiveWindow
;

struct bdInitChunk
;

struct bdInitAckChunk
;

struct bdCookie
;

struct bdCookieEchoChunk
;

struct bdHeartbeatChunk
;

struct bdHeartbeatAckChunk
;

struct bdShutdownChunk
;

struct bdShutdownAckChunk
;

struct bdCookieAckChunk
;

struct bdAddrHandleRefWrapper
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

struct _LocaleUpdate
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
lua_CFunction m_function;
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

typedef int lua_function
;

struct luaL_Reg
{
const char *name;
lua_CFunction function;
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

typedef int (__fastcall *lua_CFunction)(lua_State *)
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

typedef float vec_t
;

typedef vec_t vec3_t[3]
;

typedef vec_t vec4_t[4]
;

struct dummy
{
}
;

enum scr_string_t : __int32
{
}
;

enum XAssetType : __int32
{
ASSET_TYPE_PHYSPRESET = 0x0,
ASSET_TYPE_PHYSCOLLMAP = 0x1,
ASSET_TYPE_PHYSWATERPRESET = 0x2,
ASSET_TYPE_PHYSWORLDMAP = 0x3,
ASSET_TYPE_PHYSCONSTRAINT = 0x4,
ASSET_TYPE_XANIM = 0x5,
ASSET_TYPE_XMODEL_SURFS = 0x6,
ASSET_TYPE_XMODEL = 0x7,
ASSET_TYPE_MATERIAL = 0x8,
ASSET_TYPE_COMPUTESHADER = 0x9,
ASSET_TYPE_VERTEXSHADER = 0xA,
ASSET_TYPE_HULLSHADER = 0xB,
ASSET_TYPE_DOMAINSHADER = 0xC,
ASSET_TYPE_PIXELSHADER = 0xD,
ASSET_TYPE_VERTEXDECL = 0xE,
ASSET_TYPE_TECHNIQUE_SET = 0xF,
ASSET_TYPE_IMAGE = 0x10,
ASSET_TYPE_SOUND = 0x11,
ASSET_TYPE_SOUNDSUBMIX = 0x12,
ASSET_TYPE_SOUND_CURVE = 0x13,
ASSET_TYPE_LPF_CURVE = 0x14,
ASSET_TYPE_REVERB_CURVE = 0x15,
ASSET_TYPE_SOUND_CONTEXT = 0x16,
ASSET_TYPE_LOADED_SOUND = 0x17,
ASSET_TYPE_COL_MAP_MP = 0x18,
ASSET_TYPE_COM_MAP = 0x19,
ASSET_TYPE_GLASS_MAP = 0x1A,
ASSET_TYPE_AIPATHS = 0x1B,
ASSET_TYPE_VEHICLE_TRACK = 0x1C,
ASSET_TYPE_MAP_ENTS = 0x1D,
ASSET_TYPE_FX_MAP = 0x1E,
ASSET_TYPE_GFX_MAP = 0x1F,
ASSET_TYPE_LIGHT_DEF = 0x20,
ASSET_TYPE_UI_MAP = 0x21,
ASSET_TYPE_MENULIST = 0x22,
ASSET_TYPE_MENU = 0x23,
ASSET_TYPE_ANIMCLASS = 0x24,
ASSET_TYPE_LOCALIZE_ENTRY = 0x25,
ASSET_TYPE_ATTACHMENT = 0x26,
ASSET_TYPE_WEAPON = 0x27,
ASSET_TYPE_SNDDRIVERGLOBALS = 0x28,
ASSET_TYPE_FX = 0x29,
ASSET_TYPE_IMPACT_FX = 0x2A,
ASSET_TYPE_SURFACE_FX = 0x2B,
ASSET_TYPE_AITYPE = 0x2C,
ASSET_TYPE_MPTYPE = 0x2D,
ASSET_TYPE_CHARACTER = 0x2E,
ASSET_TYPE_XMODELALIAS = 0x2F,
ASSET_TYPE_RAWFILE = 0x30,
ASSET_TYPE_SCRIPTFILE = 0x31,
ASSET_TYPE_STRINGTABLE = 0x32,
ASSET_TYPE_LEADERBOARD = 0x33,
ASSET_TYPE_VIRTUAL_LEADERBOARD = 0x34,
ASSET_TYPE_STRUCTUREDDATADEF = 0x35,
ASSET_TYPE_DDL = 0x36,
ASSET_TYPE_PROTO = 0x37,
ASSET_TYPE_TRACER = 0x38,
ASSET_TYPE_VEHICLE = 0x39,
ASSET_TYPE_ADDON_MAP_ENTS = 0x3A,
ASSET_TYPE_NET_CONST_STRINGS = 0x3B,
ASSET_TYPE_REVERB_PRESET = 0x3C,
ASSET_TYPE_LUA_FILE = 0x3D,
ASSET_TYPE_SCRIPTABLE = 0x3E,
ASSET_TYPE_EQUIPMENT_SND_TABLE = 0x3F,
ASSET_TYPE_VECTORFIELD = 0x40,
ASSET_TYPE_DOPPLER_PRESET = 0x41,
ASSET_TYPE_PARTICLE_SIM_ANIMATION = 0x42,
ASSET_TYPE_LASER = 0x43,
ASSET_TYPE_SKELETONSCRIPT = 0x44,
ASSET_TYPE_CLUT = 0x45,
ASSET_TYPE_TTF = 0x46,
ASSET_TYPE_COUNT = 0x47,
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

struct GfxImage
{
GfxTexture texture;
DXGI_FORMAT imageFormat;
MapType mapType;
unsigned __int8 semantic;
unsigned __int8 category;
unsigned __int8 flags;
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
char __pad1[2];
unsigned __int8 *pixelData;
GfxImageStreamData streams[4];
const char *name;
}
;

struct XModel
{
const char *name;
unsigned __int8 numBones;
unsigned __int8 numRootBones;
unsigned __int8 numsurfs;
unsigned __int8 numReactiveMotionParts;
unsigned __int8 lodRampType;
unsigned __int8 numBonePhysics;
char __pad0[2];
float scale;
unsigned int noScalePartBits[8];
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
char numLods;
char collLod;
char numCompositeModels;
char u1;
__int16 flags;
__int16 numCollSurfs;
XModelCollSurf_s *collSurfs;
int contents;
XBoneInfo *boneInfo;
float radius;
Bounds bounds;
unsigned __int16 *invHighMipRadius;
int memUsage;
bool bad;
char pad;
unsigned __int16 targetCount;
unsigned __int16 numberOfWeights;
unsigned __int16 numberOfWeightMaps;
char __pad2[4];
scr_string_t *weightNames;
BlendShapeWeightMap *blendShapeWeightMap;
PhysPreset *physPreset;
PhysCollmap *physCollmap;
unsigned __int16 mdaoVolumeCount;
__int16 u3;
float quantization;
MdaoVolume *mdaoVolumes;
int u4;
int u5;
SkeletonScript *skeletonScript;
XModel **compositeModels;
XPhysBoneInfo *bonePhysics;
}
;

struct __declspec(align(8)) Statement_s
{
int numEntries;
expressionEntry *entries;
ExpressionSupportingData *supportingData;
ExpressionPersistentState persistentState;
}
;

struct MenuEventHandlerSet
{
int eventHandlerCount;
MenuEventHandler **eventHandlers;
}
;

struct menuDef_t
{
menuData_t *data;
windowDef_t window;
int itemCount;
itemDef_t **items;
}
;

struct GfxLightDef
{
const char *name;
GfxLightImage attenuation;
GfxLightImage cucoloris;
int lmapLookupStart;
}
;

struct Bounds
{
float midPoint[3];
float halfSize[3];
}
;

struct GfxColorFloat
{
float array[4];
}
;

enum PhysPresetScaling : __int32
{
PHYSPRESET_SCALING_LINEAR = 0x0,
PHYSPRESET_SCALING_QUADRATIC = 0x1,
PHYSPRESET_SCALING_COUNT = 0x2,
}
;

struct PhysPreset
{
const char *name;
int type;
float mass;
float bounce;
float friction;
float bulletForceScale;
float explosiveForceScale;
char __pad0[8];
const char *sndAliasPrefix;
float piecesSpreadFraction;
float piecesUpwardVelocity;
float minMomentum;
float maxMomentum;
float minVolume;
float maxVolume;
float minPitch;
float maxPitch;
PhysPresetScaling volumeType;
PhysPresetScaling pitchType;
bool tempDefaultToCylinder;
bool perSurfaceSndAlias;
}
;

struct dmMeshNode_array_t::$0D085F009AF9235A3A6155F9CF71DEF7::_
{
int pad[4];
}
;

union dmMeshNode_array_t::$0D085F009AF9235A3A6155F9CF71DEF7
{
struct dmMeshNode_array_t::$0D085F009AF9235A3A6155F9CF71DEF7::_ a;
char __pad0[16];
}
;

struct dmMeshNode_array_t
{
union
{
struct dmMeshNode_array_t::$18507B7D6B1B539713A3B628F57D29C1::_ a;
char __pad0[16];
};
}
;

struct dmMeshTriangle::$21D83A6700F80B579A50802DEA10F56D::_
{
int indexes[8];
}
;

union dmMeshTriangle::$21D83A6700F80B579A50802DEA10F56D
{
struct dmMeshTriangle::$21D83A6700F80B579A50802DEA10F56D::_ a;
char __pad0[32];
}
;

struct dmMeshTriangle
{
union
{
struct dmMeshTriangle::$7CEA4073BB1700D75BE24620F7B5136A::_ a;
char __pad0[32];
};
}
;

struct dmMeshData
{
dmMeshNode_array_t *meshNodes;
vec4_t *vec4_array0;
dmMeshTriangle *meshTriangles;
Bounds bounds;
float unk0[3];
unsigned int meshNodeCount;
unsigned int vec4_array0_count;
unsigned int meshTriangleCount;
int unk1;
int contents;
}
;

union dmSubEdge::$F4DEBE766222AFDFF30B8BB9B4AB60A3
{
int value;
char __pad0[4];
}
;

struct dmSubEdge
{
union dmSubEdge::$F4DEBE766222AFDFF30B8BB9B4AB60A3 a;
}
;

struct dmFloat4::$ADB640233938C0613F2112E96A930156::_
{
float normal[3];
float dist;
}
;

union dmFloat4::$ADB640233938C0613F2112E96A930156
{
struct dmFloat4::$ADB640233938C0613F2112E96A930156::_ a;
char __pad0[16];
}
;

struct dmFloat4
{
union
{
struct dmFloat4::$A2DC37B8DB498AD66C1AEB3032955749::_ a;
char __pad0[16];
};
}
;

struct dmPolytopeData
{
vec4_t *__ptr64 vec4_array0;
vec4_t *__ptr64 vec4_array1;
unsigned __int16 *__ptr64 uint16_array0;
unsigned __int16 *__ptr64 uint16_array1;
dmSubEdge *__ptr64 edges;
unsigned __int8 *__ptr64 uint8_array0;
char __pad0[12];
unsigned int count0;
unsigned int count1;
unsigned int count2;
float pad1[8];
int contents;
int pad2;
}
;

struct PhysGeomInfo
{
dmPolytopeData *data;
}
;

struct PhysMass
{
float centerOfMass[3];
float momentsOfInertia[3];
float productsOfInertia[3];
}
;

struct PhysCollmap
{
const char *name;
unsigned int count;
PhysGeomInfo *geoms;
PhysMass mass;
Bounds bounds;
}
;

struct PhysWaterPreset
{
const char *name;
char __pad0[64];
FxEffectDef *fx0;
FxEffectDef *fx1;
FxEffectDef *fx2;
FxEffectDef *fx3;
FxEffectDef *fx4;
FxEffectDef *fx5;
FxEffectDef *fx6;
}
;

struct PhysWaterVolumeDef
{
PhysWaterPreset *physWaterPreset;
char __pad0[12];
scr_string_t string;
char __pad1[8];
}
;

struct PhysBrushModelPacked
{
uint64_t p;
}
;

struct PhysBrushModelFields
{
__int16 polytopeIndex;
__int16 unk;
__int16 worldIndex;
__int16 meshIndex;
}
;

union PhysBrushModel
{
PhysBrushModelPacked packed;
PhysBrushModelFields fields;
}
;

struct PhysWorld
{
const char *name;
PhysBrushModel *models;
dmPolytopeData *polytopeDatas;
dmMeshData *meshDatas;
PhysWaterVolumeDef *waterVolumes;
unsigned int modelsCount;
unsigned int polytopeDatasCount;
unsigned int meshDatasCount;
unsigned int waterVolumesCount;
}
;

struct PhysConstraint
{
const char *name;
char __pad0[32];
}
;

struct Packed128
{
uint64_t p0;
uint64_t p1;
}
;

struct GfxDrawSurfFields
{
unsigned __int64 objectId : 16;
unsigned __int64 pad0 : 20;
unsigned __int64 reflectionProbeIndex : 8;
unsigned __int64 hasGfxEntIndex : 1;
unsigned __int64 customIndex : 5;
unsigned __int64 materialSortedIndex : 14;
unsigned __int64 tessellation : 2;
unsigned __int64 prepass : 2;
unsigned __int64 pad1 : 4;
unsigned __int64 useHeroLighting : 1;
unsigned __int64 sceneLightEnvIndex : 16;
unsigned __int64 viewModelRender : 1;
unsigned __int64 surfType : 4;
unsigned __int64 primarySortKey : 6;
unsigned __int64 unused : 28;
}
;

union GfxDrawSurf
{
GfxDrawSurfFields fields;
Packed128 packed;
}
;

enum MaterialTechniqueType : __int32
{
TECHNIQUE_STANDARD_BEGIN = 0x0,
TECHNIQUE_ZPREPASS = 0x0,
TECHNIQUE_ZPREPASS_VELOCITY_RIGID = 0x1,
TECHNIQUE_ZPREPASS_VELOCITY_SKINNED = 0x2,
TECHNIQUE_ZPREPASS_HIDIR = 0x3,
TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_RIGID = 0x4,
TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_SKINNED = 0x5,
TECHNIQUE_BUILD_SHADOWMAP_DEPTH = 0x6,
TECHNIQUE_BUILD_SHADOWMAP_COLOR = 0x7,
TECHNIQUE_UNLIT = 0x8,
TECHNIQUE_EMISSIVE = 0x9,
TECHNIQUE_EMISSIVE_DFOG = 0xA,
TECHNIQUE_EMISSIVE_SHADOW = 0xB,
TECHNIQUE_EMISSIVE_SHADOW_DFOG = 0xC,
TECHNIQUE_LIT = 0xD,
TECHNIQUE_LIT_DIR = 0xE,
TECHNIQUE_LIT_DIR_SHADOW = 0xF,
TECHNIQUE_LIT_SPOT = 0x10,
TECHNIQUE_LIT_SPOT_SHADOW = 0x11,
TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS = 0x12,
TECHNIQUE_LIT_OMNI = 0x13,
TECHNIQUE_LIT_OMNI_SHADOW = 0x14,
TECHNIQUE_LIT_DYNAMIC_BRANCHING_CUCOLORIS = 0x15,
TECHNIQUE_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS = 0x16,
TECHNIQUE_LIT_DYNAMIC_BRANCHING = 0x17,
TECHNIQUE_LIT_SUN_DYNAMIC_BRANCHING = 0x18,
TECHNIQUE_LIT_DFOG = 0x19,
TECHNIQUE_LIT_DIR_DFOG = 0x1A,
TECHNIQUE_LIT_DIR_SHADOW_DFOG = 0x1B,
TECHNIQUE_LIT_SPOT_DFOG = 0x1C,
TECHNIQUE_LIT_SPOT_SHADOW_DFOG = 0x1D,
TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_DFOG = 0x1E,
TECHNIQUE_LIT_OMNI_DFOG = 0x1F,
TECHNIQUE_LIT_OMNI_SHADOW_DFOG = 0x20,
TECHNIQUE_LIT_DYNAMIC_BRANCHING_CUCOLORIS_DFOG = 0x21,
TECHNIQUE_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS_DFOG = 0x22,
TECHNIQUE_LIT_DYNAMIC_BRANCHING_DFOG = 0x23,
TECHNIQUE_LIT_SUN_DYNAMIC_BRANCHING_DFOG = 0x24,
TECHNIQUE_LIGHT_SPOT = 0x25,
TECHNIQUE_LIGHT_OMNI = 0x26,
TECHNIQUE_LIGHT_SPOT_SHADOW = 0x27,
TECHNIQUE_LIGHT_SPOT_SHADOW_CUCOLORIS = 0x28,
TECHNIQUE_LIGHT_SPOT_DFOG = 0x2B,
TECHNIQUE_LIGHT_OMNI_DFOG = 0x2C,
TECHNIQUE_LIGHT_SPOT_SHADOW_DFOG = 0x2D,
TECHNIQUE_LIGHT_SPOT_SHADOW_CUCOLORIS_DFOG = 0x2E,
TECHNIQUE_FAKELIGHT_NORMAL = 0x31,
TECHNIQUE_FAKELIGHT_VIEW = 0x32,
TECHNIQUE_SUNLIGHT_PREVIEW = 0x33,
TECHNIQUE_CASE_TEXTURE = 0x34,
TECHNIQUE_WIREFRAME_SOLID = 0x35,
TECHNIQUE_WIREFRAME_SHADED = 0x36,
TECHNIQUE_THERMAL = 0x37,
TECHNIQUE_CTQ = 0x38,
TECHNIQUE_VELOCITY_RIGID = 0x39,
TECHNIQUE_VELOCITY_SKINNED = 0x3A,
TECHNIQUE_DEBUG_BUMPMAP = 0x3B,
TECHNIQUE_STANDARD_LAST = 0x3B,
TECHNIQUE_INSTANCED_BEGIN = 0x3C,
TECHNIQUE_INSTANCED_ZPREPASS = 0x3C,
TECHNIQUE_INSTANCED_ZPREPASS_VELOCITY_RIGID = 0x3D,
TECHNIQUE_INSTANCED_ZPREPASS_VELOCITY_SKINNED = 0x3E,
TECHNIQUE_INSTANCED_ZPREPASS_HIDIR = 0x3F,
TECHNIQUE_INSTANCED_ZPREPASS_HIDIR_VELOCITY_RIGID = 0x40,
TECHNIQUE_INSTANCED_ZPREPASS_HIDIR_VELOCITY_SKINNED = 0x41,
TECHNIQUE_INSTANCED_BUILD_SHADOWMAP_DEPTH = 0x42,
TECHNIQUE_INSTANCED_BUILD_SHADOWMAP_COLOR = 0x43,
TECHNIQUE_INSTANCED_UNLIT = 0x44,
TECHNIQUE_INSTANCED_EMISSIVE = 0x45,
TECHNIQUE_INSTANCED_EMISSIVE_DFOG = 0x46,
TECHNIQUE_INSTANCED_EMISSIVE_SHADOW = 0x47,
TECHNIQUE_INSTANCED_EMISSIVE_SHADOW_DFOG = 0x48,
TECHNIQUE_INSTANCED_LIT = 0x49,
TECHNIQUE_INSTANCED_LIT_DIR = 0x4A,
TECHNIQUE_INSTANCED_LIT_DIR_SHADOW = 0x4B,
TECHNIQUE_INSTANCED_LIT_SPOT = 0x4C,
TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW = 0x4D,
TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS = 0x4E,
TECHNIQUE_INSTANCED_LIT_OMNI = 0x4F,
TECHNIQUE_INSTANCED_LIT_OMNI_SHADOW = 0x50,
TECHNIQUE_INSTANCED_LIT_DYNAMIC_BRANCHING_CUCOLORIS = 0x51,
TECHNIQUE_INSTANCED_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS = 0x52,
TECHNIQUE_INSTANCED_LIT_DYNAMIC_BRANCHING = 0x53,
TECHNIQUE_INSTANCED_LIT_SUN_DYNAMIC_BRANCHING = 0x54,
TECHNIQUE_INSTANCED_LIT_DFOG = 0x55,
TECHNIQUE_INSTANCED_LIT_DIR_DFOG = 0x56,
TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_DFOG = 0x57,
TECHNIQUE_INSTANCED_LIT_SPOT_DFOG = 0x58,
TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_DFOG = 0x59,
TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS_DFOG = 0x5A,
TECHNIQUE_INSTANCED_LIT_OMNI_DFOG = 0x5B,
TECHNIQUE_INSTANCED_LIT_OMNI_SHADOW_DFOG = 0x5C,
TECHNIQUE_INSTANCED_LIT_DYNAMIC_BRANCHING_CUCOLORIS_DFOG = 0x5D,
TECHNIQUE_INSTANCED_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS_DFOG = 0x5E,
TECHNIQUE_INSTANCED_LIT_DYNAMIC_BRANCHING_DFOG = 0x5F,
TECHNIQUE_INSTANCED_LIT_SUN_DYNAMIC_BRANCHING_DFOG = 0x60,
TECHNIQUE_INSTANCED_LIGHT_SPOT = 0x61,
TECHNIQUE_INSTANCED_LIGHT_OMNI = 0x62,
TECHNIQUE_INSTANCED_LIGHT_SPOT_SHADOW = 0x63,
TECHNIQUE_INSTANCED_LIGHT_SPOT_SHADOW_CUCOLORIS = 0x64,
TECHNIQUE_INSTANCED_LIGHT_SPOT_DFOG = 0x67,
TECHNIQUE_INSTANCED_LIGHT_OMNI_DFOG = 0x68,
TECHNIQUE_INSTANCED_LIGHT_SPOT_SHADOW_DFOG = 0x69,
TECHNIQUE_INSTANCED_LIGHT_SPOT_SHADOW_CUCOLORIS_DFOG = 0x6A,
TECHNIQUE_INSTANCED_FAKELIGHT_NORMAL = 0x6D,
TECHNIQUE_INSTANCED_FAKELIGHT_VIEW = 0x6E,
TECHNIQUE_INSTANCED_SUNLIGHT_PREVIEW = 0x6F,
TECHNIQUE_INSTANCED_CASE_TEXTURE = 0x70,
TECHNIQUE_INSTANCED_WIREFRAME_SOLID = 0x71,
TECHNIQUE_INSTANCED_WIREFRAME_SHADED = 0x72,
TECHNIQUE_INSTANCED_THERMAL = 0x73,
TECHNIQUE_INSTANCED_CTQ = 0x74,
TECHNIQUE_INSTANCED_VELOCITY_RIGID = 0x75,
TECHNIQUE_INSTANCED_VELOCITY_SKINNED = 0x76,
TECHNIQUE_INSTANCED_DEBUG_BUMPMAP = 0x77,
TECHNIQUE_INSTANCED_LAST = 0x77,
TECHNIQUE_SUBDIV_PATCH_BEGIN = 0x78,
TECHNIQUE_SUBDIV_PATCH_ZPREPASS = 0x78,
TECHNIQUE_SUBDIV_PATCH_ZPREPASS_VELOCITY_RIGID = 0x79,
TECHNIQUE_SUBDIV_PATCH_ZPREPASS_VELOCITY_SKINNED = 0x7A,
TECHNIQUE_SUBDIV_PATCH_ZPREPASS_HIDIR = 0x7B,
TECHNIQUE_SUBDIV_PATCH_ZPREPASS_HIDIR_VELOCITY_RIGID = 0x7C,
TECHNIQUE_SUBDIV_PATCH_ZPREPASS_HIDIR_VELOCITY_SKINNED = 0x7D,
TECHNIQUE_SUBDIV_PATCH_BUILD_SHADOWMAP_DEPTH = 0x7E,
TECHNIQUE_SUBDIV_PATCH_BUILD_SHADOWMAP_COLOR = 0x7F,
TECHNIQUE_SUBDIV_PATCH_UNLIT = 0x80,
TECHNIQUE_SUBDIV_PATCH_EMISSIVE = 0x81,
TECHNIQUE_SUBDIV_PATCH_EMISSIVE_DFOG = 0x82,
TECHNIQUE_SUBDIV_PATCH_EMISSIVE_SHADOW = 0x83,
TECHNIQUE_SUBDIV_PATCH_EMISSIVE_SHADOW_DFOG = 0x84,
TECHNIQUE_SUBDIV_PATCH_LIT = 0x85,
TECHNIQUE_SUBDIV_PATCH_LIT_DIR = 0x86,
TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW = 0x87,
TECHNIQUE_SUBDIV_PATCH_LIT_SPOT = 0x88,
TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW = 0x89,
TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS = 0x8A,
TECHNIQUE_SUBDIV_PATCH_LIT_OMNI = 0x8B,
TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_SHADOW = 0x8C,
TECHNIQUE_SUBDIV_PATCH_LIT_DYNAMIC_BRANCHING_CUCOLORIS = 0x8D,
TECHNIQUE_SUBDIV_PATCH_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS = 0x8E,
TECHNIQUE_SUBDIV_PATCH_LIT_DYNAMIC_BRANCHING = 0x8F,
TECHNIQUE_SUBDIV_PATCH_LIT_SUN_DYNAMIC_BRANCHING = 0x90,
TECHNIQUE_SUBDIV_PATCH_LIT_DFOG = 0x91,
TECHNIQUE_SUBDIV_PATCH_LIT_DIR_DFOG = 0x92,
TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_DFOG = 0x93,
TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_DFOG = 0x94,
TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_DFOG = 0x95,
TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS_DFOG = 0x96,
TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_DFOG = 0x97,
TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_SHADOW_DFOG = 0x98,
TECHNIQUE_SUBDIV_PATCH_LIT_DYNAMIC_BRANCHING_CUCOLORIS_DFOG = 0x99,
TECHNIQUE_SUBDIV_PATCH_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS_DFOG = 0x9A,
TECHNIQUE_SUBDIV_PATCH_LIT_DYNAMIC_BRANCHING_DFOG = 0x9B,
TECHNIQUE_SUBDIV_PATCH_LIT_SUN_DYNAMIC_BRANCHING_DFOG = 0x9C,
TECHNIQUE_SUBDIV_PATCH_LIGHT_SPOT = 0x9D,
TECHNIQUE_SUBDIV_PATCH_LIGHT_OMNI = 0x9E,
TECHNIQUE_SUBDIV_PATCH_LIGHT_SPOT_SHADOW = 0x9F,
TECHNIQUE_SUBDIV_PATCH_LIGHT_SPOT_SHADOW_CUCOLORIS = 0xA0,
TECHNIQUE_SUBDIV_PATCH_LIGHT_SPOT_DFOG = 0xA3,
TECHNIQUE_SUBDIV_PATCH_LIGHT_OMNI_DFOG = 0xA4,
TECHNIQUE_SUBDIV_PATCH_LIGHT_SPOT_SHADOW_DFOG = 0xA5,
TECHNIQUE_SUBDIV_PATCH_LIGHT_SPOT_SHADOW_CUCOLORIS_DFOG = 0xA6,
TECHNIQUE_SUBDIV_PATCH_FAKELIGHT_NORMAL = 0xA9,
TECHNIQUE_SUBDIV_PATCH_FAKELIGHT_VIEW = 0xAA,
TECHNIQUE_SUBDIV_PATCH_SUNLIGHT_PREVIEW = 0xAB,
TECHNIQUE_SUBDIV_PATCH_CASE_TEXTURE = 0xAC,
TECHNIQUE_SUBDIV_PATCH_WIREFRAME_SOLID = 0xAD,
TECHNIQUE_SUBDIV_PATCH_WIREFRAME_SHADED = 0xAE,
TECHNIQUE_SUBDIV_PATCH_THERMAL = 0xAF,
TECHNIQUE_SUBDIV_PATCH_CTQ = 0xB0,
TECHNIQUE_SUBDIV_PATCH_VELOCITY_RIGID = 0xB1,
TECHNIQUE_SUBDIV_PATCH_VELOCITY_SKINNED = 0xB2,
TECHNIQUE_SUBDIV_PATCH_DEBUG_BUMPMAP = 0xB3,
TECHNIQUE_SUBDIV_PATCH_LAST = 0xB4,
TECHNIQUE_NO_DISPLACEMENT_BEGIN = 0xB4,
TECHNIQUE_NO_DISPLACEMENT_ZPREPASS = 0xB4,
TECHNIQUE_NO_DISPLACEMENT_ZPREPASS_VELOCITY_RIGID = 0xB5,
TECHNIQUE_NO_DISPLACEMENT_ZPREPASS_VELOCITY_SKINNED = 0xB6,
TECHNIQUE_NO_DISPLACEMENT_ZPREPASS_HIDIR = 0xB7,
TECHNIQUE_NO_DISPLACEMENT_ZPREPASS_HIDIR_VELOCITY_RIGID = 0xB8,
TECHNIQUE_NO_DISPLACEMENT_ZPREPASS_HIDIR_VELOCITY_SKINNED = 0xB9,
TECHNIQUE_NO_DISPLACEMENT_BUILD_SHADOWMAP_DEPTH = 0xBA,
TECHNIQUE_NO_DISPLACEMENT_BUILD_SHADOWMAP_COLOR = 0xBB,
TECHNIQUE_NO_DISPLACEMENT_UNLIT = 0xBC,
TECHNIQUE_NO_DISPLACEMENT_EMISSIVE = 0xBD,
TECHNIQUE_NO_DISPLACEMENT_EMISSIVE_DFOG = 0xBE,
TECHNIQUE_NO_DISPLACEMENT_EMISSIVE_SHADOW = 0xBF,
TECHNIQUE_NO_DISPLACEMENT_EMISSIVE_SHADOW_DFOG = 0xC0,
TECHNIQUE_NO_DISPLACEMENT_LIT = 0xC1,
TECHNIQUE_NO_DISPLACEMENT_LIT_DIR = 0xC2,
TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW = 0xC3,
TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT = 0xC4,
TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW = 0xC5,
TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS = 0xC6,
TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI = 0xC7,
TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_SHADOW = 0xC8,
TECHNIQUE_NO_DISPLACEMENT_LIT_DYNAMIC_BRANCHING_CUCOLORIS = 0xC9,
TECHNIQUE_NO_DISPLACEMENT_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS = 0xCA,
TECHNIQUE_NO_DISPLACEMENT_LIT_DYNAMIC_BRANCHING = 0xCB,
TECHNIQUE_NO_DISPLACEMENT_LIT_SUN_DYNAMIC_BRANCHING = 0xCC,
TECHNIQUE_NO_DISPLACEMENT_LIT_DFOG = 0xCD,
TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_DFOG = 0xCE,
TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_DFOG = 0xCF,
TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_DFOG = 0xD0,
TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_DFOG = 0xD1,
TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS_DFOG = 0xD2,
TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_DFOG = 0xD3,
TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_SHADOW_DFOG = 0xD4,
TECHNIQUE_NO_DISPLACEMENT_LIT_DYNAMIC_BRANCHING_CUCOLORIS_DFOG = 0xD5,
TECHNIQUE_NO_DISPLACEMENT_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS_DFOG = 0xD6,
TECHNIQUE_NO_DISPLACEMENT_LIT_DYNAMIC_BRANCHING_DFOG = 0xD7,
TECHNIQUE_NO_DISPLACEMENT_LIT_SUN_DYNAMIC_BRANCHING_DFOG = 0xD8,
TECHNIQUE_NO_DISPLACEMENT_LIGHT_SPOT = 0xD9,
TECHNIQUE_NO_DISPLACEMENT_LIGHT_OMNI = 0xDA,
TECHNIQUE_NO_DISPLACEMENT_LIGHT_SPOT_SHADOW = 0xDB,
TECHNIQUE_NO_DISPLACEMENT_LIGHT_SPOT_SHADOW_CUCOLORIS = 0xDC,
TECHNIQUE_NO_DISPLACEMENT_LIGHT_SPOT_DFOG = 0xDF,
TECHNIQUE_NO_DISPLACEMENT_LIGHT_OMNI_DFOG = 0xE0,
TECHNIQUE_NO_DISPLACEMENT_LIGHT_SPOT_SHADOW_DFOG = 0xE1,
TECHNIQUE_NO_DISPLACEMENT_LIGHT_SPOT_SHADOW_CUCOLORIS_DFOG = 0xE2,
TECHNIQUE_NO_DISPLACEMENT_FAKELIGHT_NORMAL = 0xE5,
TECHNIQUE_NO_DISPLACEMENT_FAKELIGHT_VIEW = 0xE6,
TECHNIQUE_NO_DISPLACEMENT_SUNLIGHT_PREVIEW = 0xE7,
TECHNIQUE_NO_DISPLACEMENT_CASE_TEXTURE = 0xE8,
TECHNIQUE_NO_DISPLACEMENT_WIREFRAME_SOLID = 0xE9,
TECHNIQUE_NO_DISPLACEMENT_WIREFRAME_SHADED = 0xEA,
TECHNIQUE_NO_DISPLACEMENT_THERMAL = 0xEB,
TECHNIQUE_NO_DISPLACEMENT_CTQ = 0xEC,
TECHNIQUE_NO_DISPLACEMENT_VELOCITY_RIGID = 0xED,
TECHNIQUE_NO_DISPLACEMENT_VELOCITY_SKINNED = 0xEE,
TECHNIQUE_NO_DISPLACEMENT_DEBUG_BUMPMAP = 0xEF,
TECHNIQUE_NO_DISPLACEMENT_LAST = 0xF0,
TECHNIQUE_COUNT = 0xF0,
}
;

struct GfxComputeShaderLoadDef
{
unsigned __int8 *program;
unsigned int programSize;
char __pad[4];
}
;

struct GfxVertexShaderLoadDef
{
unsigned __int8 *program;
unsigned int programSize;
unsigned int microCodeCrc;
}
;

struct GfxPixelShaderLoadDef
{
unsigned __int8 *program;
unsigned int programSize;
unsigned int microCodeCrc;
}
;

struct GfxHullShaderLoadDef
{
unsigned __int8 *program;
unsigned int programSize;
char __pad[4];
}
;

struct GfxDomainShaderLoadDef
{
unsigned __int8 *program;
unsigned int programSize;
char __pad[4];
}
;

typedef vec_t vec2_t[2]
;

struct dmMeshNode_array_t::$18507B7D6B1B539713A3B628F57D29C1::_
{
int pad[4];
}
;

union dmMeshNode_array_t::$18507B7D6B1B539713A3B628F57D29C1
{
struct dmMeshNode_array_t::$18507B7D6B1B539713A3B628F57D29C1::_ a;
char __pad0[16];
}
;

struct dmMeshTriangle::$7CEA4073BB1700D75BE24620F7B5136A::_
{
int indexes[8];
}
;

union dmMeshTriangle::$7CEA4073BB1700D75BE24620F7B5136A
{
struct dmMeshTriangle::$7CEA4073BB1700D75BE24620F7B5136A::_ a;
char __pad0[32];
}
;

struct dmFloat4::$A2DC37B8DB498AD66C1AEB3032955749::_
{
float normal[3];
float dist;
}
;

union dmFloat4::$A2DC37B8DB498AD66C1AEB3032955749
{
struct dmFloat4::$A2DC37B8DB498AD66C1AEB3032955749::_ a;
char __pad0[16];
}
;

struct ComputeShaderProgram
{
void *cs;
GfxComputeShaderLoadDef loadDef;
}
;

struct ComputeShader
{
const char *name;
ComputeShaderProgram prog;
}
;

struct MaterialVertexShaderProgram
{
void *vs;
GfxVertexShaderLoadDef loadDef;
}
;

struct MaterialVertexShader
{
const char *name;
MaterialVertexShaderProgram prog;
}
;

struct MaterialPixelShaderProgram
{
void *ps;
GfxPixelShaderLoadDef loadDef;
}
;

struct MaterialPixelShader
{
const char *name;
MaterialPixelShaderProgram prog;
}
;

struct MaterialHullShaderProgram
{
void *hs;
GfxHullShaderLoadDef loadDef;
}
;

struct MaterialHullShader
{
const char *name;
MaterialHullShaderProgram prog;
}
;

struct MaterialDomainShaderProgram
{
void *ds;
GfxDomainShaderLoadDef loadDef;
}
;

struct MaterialDomainShader
{
const char *name;
MaterialDomainShaderProgram prog;
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
MTL_ARG_CODE_CONST = 0x0,
MTL_ARG_CODE_TEXTURE = 0x1,
MTL_ARG_CODE_SAMPLER = 0x2,
MTL_ARG_MATERIAL_CONST = 0x3,
MTL_ARG_LITERAL_CONST = 0x4,
MTL_ARG_MATERIAL_TEXTURE = 0x5,
MTL_ARG_MATERIAL_SAMPLER = 0x6,
MTL_ARG_COUNT = 0x7,
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

struct MaterialStreamRouting
{
unsigned __int8 source;
unsigned __int8 dest;
unsigned __int8 mask;
}
;

struct MaterialVertexStreamRouting
{
MaterialStreamRouting data[32];
void *decl[250];
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
MaterialShaderArgument *args;
}
;

enum MaterialTechniqueFlags : unsigned __int16
{
MTL_TECHFLAG_NEEDS_RESOLVED_POST_SUN = 0x1,
MTL_TECHFLAG_NEEDS_RESOLVED_SCENE = 0x2,
}
;

struct MaterialTechniqueHeader
{
const char *name;
unsigned __int16 flags;
unsigned __int16 passCount;
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
MaterialTechnique *techniques[240];
}
;

struct WaterWritable
{
float floatTime;
}
;

struct water_t
{
WaterWritable writable;
float *H0X;
float *H0Y;
float *wTerm;
int M;
int N;
float Lx;
float Lz;
float gravity;
float windvel;
float winddir[2];
float amplitude;
GfxImage *image;
GfxImage *stagingImage;
}
;

union MaterialTextureDefInfo
{
GfxImage *image;
water_t *water;
}
;

enum MaterialSamplerState : unsigned __int8
{
SAMPLER_FILTER_SHIFT = 0x0,
SAMPLER_FILTER_NEAREST = 0x1,
SAMPLER_FILTER_LINEAR = 0x2,
SAMPLER_FILTER_ANISO2X = 0x3,
SAMPLER_FILTER_ANISO4X = 0x4,
SAMPLER_FILTER_CMP = 0x5,
SAMPLER_FILTER_ANISO16X = 0x6,
SAMPLER_FILTER_MASK = 0x7,
SAMPLER_MIPMAP_SHIFT = 0x3,
SAMPLER_MIPMAP_DISABLED = 0x0,
SAMPLER_MIPMAP_NEAREST = 0x8,
SAMPLER_MIPMAP_LINEAR = 0x10,
SAMPLER_MIPMAP_BESPOKE = 0x18,
SAMPLER_MIPMAP_COUNT = 0x3,
SAMPLER_MIPMAP_MASK = 0x18,
SAMPLER_CLAMP_U_SHIFT = 0x5,
SAMPLER_CLAMP_V_SHIFT = 0x6,
SAMPLER_CLAMP_W_SHIFT = 0x7,
SAMPLER_CLAMP_U = 0x20,
SAMPLER_CLAMP_V = 0x40,
SAMPLER_CLAMP_W = 0x80,
SAMPLER_CLAMP_MASK = 0xE0,
}
;

struct MaterialTextureDef
{
unsigned int nameHash;
char nameStart;
char nameEnd;
unsigned __int8 samplerState;
unsigned __int8 semantic;
MaterialTextureDefInfo u;
}
;

struct MaterialConstantDef
{
unsigned int nameHash;
char name[12];
float literal[4];
}
;

struct GfxStateBits
{
unsigned int loadBits[6];
unsigned __int16 zone;
unsigned __int8 depthStencilState[10];
unsigned __int8 blendState;
unsigned __int8 rasterizerState;
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
void *vsConstantBuffer;
void *hsConstantBuffer;
void *dsConstantBuffer;
void *psConstantBuffer;
}
;

enum MaterialGameFlags : unsigned __int8
{
MTL_GAMEFLAG_NONE = 0x0,
MTL_GAMEFLAG_CASTS_SHADOW_UNK1 = 0x1,
MTL_GAMEFLAG_CASTS_SHADOW_UNK2 = 0x2,
MTL_GAMEFLAG_CASTS_SHADOW_UNK3 = 0x3,
MTL_GAMEFLAG_CASTS_SHADOW_UNK4 = 0x4,
MTL_GAMEFLAG_CASTS_SHADOW_UNK5 = 0x5,
MTL_GAMEFLAG_CASTS_SHADOW_SHIFT = 0x6,
MTL_GAMEFLAG_CASTS_SHADOW_MASK = 0x1F,
MTL_GAMEFLAG_NO_MARK = 0x20,
MTL_GAMEFLAG_NO_DELAY = 0x40,
MTL_GAMEFLAG_STATIC_MATERIAL = 0x80,
}
;

enum MaterialSortKey : unsigned __int8
{
MTL_SORTKEY_NONE = 0x0,
MTL_SORTKEY_OPAQUE_AMBIENT = 0x1,
MTL_SORTKEY_OPAQUE = 0x2,
MTL_SORTKEY_SKY = 0x3,
MTL_SORTKEY_SUN = 0x4,
MTL_SORTKEY_UNKNOWN5 = 0x5,
MTL_SORTKEY_UNKNOWN6 = 0x6,
MTL_SORTKEY_LIT_DECAL = 0x7,
MTL_SORTKEY_UNKNOWN8 = 0x8,
MTL_SORTKEY_UNKNOWN9 = 0x9,
MTL_SORTKEY_UNKNOWN10 = 0xA,
MTL_SORTKEY_UNKNOWN11 = 0xB,
MTL_SORTKEY_UNKNOWN12 = 0xC,
MTL_SORTKEY_UNKNOWN13 = 0xD,
MTL_SORTKEY_WEAPON_IMPACT = 0xE,
MTL_SORTKEY_UNKNOWN15 = 0xF,
MTL_SORTKEY_UNKNOWN16 = 0x10,
MTL_SORTKEY_TOP_DECAL = 0x11,
MTL_SORTKEY_HAIR = 0x12,
MTL_SORTKEY_UNKNOWN19 = 0x13,
MTL_SORTKEY_UNKNOWN20 = 0x14,
MTL_SORTKEY_UNKNOWN21 = 0x15,
MTL_SORTKEY_UNKNOWN22 = 0x16,
MTL_SORTKEY_UNKNOWN23 = 0x17,
MTL_SORTKEY_UNKNOWN24 = 0x18,
MTL_SORTKEY_UNKNOWN25 = 0x19,
MTL_SORTKEY_WINDOW_INSIDE = 0x1A,
MTL_SORTKEY_WINDOW_OUTSIDE = 0x1B,
MTL_SORTKEY_UNKNOWN28 = 0x1C,
MTL_SORTKEY_UNKNOWN29 = 0x1D,
MTL_SORTKEY_UNKNOWN30 = 0x1E,
MTL_SORTKEY_UNKNOWN31 = 0x1F,
MTL_SORTKEY_UNKNOWN32 = 0x20,
MTL_SORTKEY_UNKNOWN33 = 0x21,
MTL_SORTKEY_UNKNOWN34 = 0x22,
MTL_SORTKEY_UNKNOWN35 = 0x23,
MTL_SORTKEY_UNKNOWN36 = 0x24,
MTL_SORTKEY_UNKNOWN37 = 0x25,
MTL_SORTKEY_UNKNOWN38 = 0x26,
MTL_SORTKEY_UNKNOWN39 = 0x27,
MTL_SORTKEY_UNKNOWN40 = 0x28,
MTL_SORTKEY_UNKNOWN41 = 0x29,
MTL_SORTKEY_UNKNOWN42 = 0x2A,
MTL_SORTKEY_EFFECT_DECAL = 0x2B,
MTL_SORTKEY_UNKNOWN44 = 0x2C,
MTL_SORTKEY_UNKNOWN45 = 0x2D,
MTL_SORTKEY_UNKNOWN46 = 0x2E,
MTL_SORTKEY_UNKNOWN47 = 0x2F,
MTL_SORTKEY_UNKNOWN48 = 0x30,
MTL_SORTKEY_UNKNOWN49 = 0x31,
MTL_SORTKEY_UNKNOWN50 = 0x32,
MTL_SORTKEY_UNKNOWN51 = 0x33,
MTL_SORTKEY_UNKNOWN52 = 0x34,
MTL_SORTKEY_EFFECT_AUTO = 0x35,
MTL_SORTKEY_UNKNOWN54 = 0x36,
MTL_SORTKEY_UNKNOWN55 = 0x37,
MTL_SORTKEY_UNKNOWN56 = 0x38,
MTL_SORT_PRIMARY_COUNT = 0x40,
}
;

enum SurfaceTypeBits : unsigned __int64
{
SURFTYPE_BITS_DEFAULT = 0x0LL,
SURFTYPE_BITS_BARK = 0x1LL,
SURFTYPE_BITS_BRICK = 0x2LL,
SURFTYPE_BITS_CARPET = 0x4LL,
SURFTYPE_BITS_CLOTH = 0x8LL,
SURFTYPE_BITS_CONCRETE = 0x10LL,
SURFTYPE_BITS_DIRT = 0x20LL,
SURFTYPE_BITS_FLESH = 0x40LL,
SURFTYPE_BITS_FOLIAGE_DEBRIS = 0x80LL,
SURFTYPE_BITS_GLASS = 0x100LL,
SURFTYPE_BITS_GRASS = 0x200LL,
SURFTYPE_BITS_GRAVEL = 0x400LL,
SURFTYPE_BITS_ICE = 0x800LL,
SURFTYPE_BITS_METAL_SOLID = 0x1000LL,
SURFTYPE_BITS_METAL_GRATE = 0x2000LL,
SURFTYPE_BITS_MUD = 0x4000LL,
SURFTYPE_BITS_PAPER = 0x8000LL,
SURFTYPE_BITS_PLASTER = 0x10000LL,
SURFTYPE_BITS_ROCK = 0x20000LL,
SURFTYPE_BITS_SAND = 0x40000LL,
SURFTYPE_BITS_SNOW = 0x80000LL,
SURFTYPE_BITS_WATER_WAIST = 0x100000LL,
SURFTYPE_BITS_WOOD_SOLID = 0x200000LL,
SURFTYPE_BITS_ASPHALT = 0x400000LL,
SURFTYPE_BITS_CERAMIC = 0x800000LL,
SURFTYPE_BITS_PLASTIC_SOLID = 0x1000000LL,
SURFTYPE_BITS_RUBBER = 0x2000000LL,
SURFTYPE_BITS_FRUIT = 0x4000000LL,
SURFTYPE_BITS_PAINTEDMETAL = 0x8000000LL,
SURFTYPE_BITS_RIOTSHIELD = 0x10000000LL,
SURFTYPE_BITS_SLUSH = 0x20000000LL,
SURFTYPE_BITS_ASPHALT_WET = 0x40000000LL,
SURFTYPE_BITS_ASPHALT_DEBRIS = 0x80000000LL,
SURFTYPE_BITS_CONCRETE_WET = 0x100000000LL,
SURFTYPE_BITS_CONCRETE_DEBRIS = 0x200000000LL,
SURFTYPE_BITS_FOLIAGE_VEGETATION = 0x400000000LL,
SURFTYPE_BITS_FOLIAGE_LEAVES = 0x800000000LL,
SURFTYPE_BITS_GRASS_TALL = 0x1000000000LL,
SURFTYPE_BITS_METAL_HOLLOW = 0x2000000000LL,
SURFTYPE_BITS_METAL_VEHICLE = 0x4000000000LL,
SURFTYPE_BITS_METAL_THIN = 0x8000000000LL,
SURFTYPE_BITS_METAL_WET = 0x10000000000LL,
SURFTYPE_BITS_METAL_DEBRIS = 0x20000000000LL,
SURFTYPE_BITS_PLASTIC_HOLLOW = 0x40000000000LL,
SURFTYPE_BITS_PLASTIC_TARP = 0x80000000000LL,
SURFTYPE_BITS_ROCK_WET = 0x100000000000LL,
SURFTYPE_BITS_ROCK_DEBRIS = 0x200000000000LL,
SURFTYPE_BITS_WATER_ANKLE = 0x400000000000LL,
SURFTYPE_BITS_WATER_KNEE = 0x800000000000LL,
SURFTYPE_BITS_WOOD_HOLLOW = 0x1000000000000LL,
SURFTYPE_BITS_WOOD_WET = 0x2000000000000LL,
SURFTYPE_BITS_WOOD_DEBRIS = 0x4000000000000LL,
SURFTYPE_BITS_CUSHION = 0x8000000000000LL,
}
;

struct MaterialInfo
{
const char *name;
unsigned __int8 gameFlags;
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

enum MaterialStateFlags : unsigned __int8
{
STATE_FLAG_CULL_BACK = 0x1,
STATE_FLAG_DECAL = 0x4,
STATE_FLAG_WRITES_DEPTH = 0x8,
STATE_FLAG_USES_DEPTH_BUFFER = 0x10,
STATE_FLAG_USES_STENCIL_BUFFER = 0x20,
}
;

enum GfxCameraRegionType : unsigned __int8
{
CAMERA_REGION_LIT_OPAQUE = 0x0,
CAMERA_REGION_LIT_OPAQUE_SSS = 0x1,
CAMERA_REGION_LIT_DECAL = 0x2,
CAMERA_REGION_LIT_TRANS = 0x3,
CAMERA_REGION_EMISSIVE = 0x4,
CAMERA_REGION_DEPTH_HACK = 0x5,
CAMERA_REGION_DEPTH_HACK_TRANS = 0x6,
CAMERA_REGION_DEPTH_HACK_SSS = 0x7,
CAMERA_REGION_PRE_DEPTH_HACK = 0x8,
CAMERA_REGION_PRE_DEPTH_HACK_SSS = 0x9,
CAMERA_REGION_PRE_OPAQUE = 0xA,
CAMERA_REGION_PRE_SSS = 0xB,
CAMERA_REGION_COUNT = 0xC,
CAMERA_REGION_NONE = 0xC,
}
;

enum MaterialType : unsigned __int8
{
MTL_TYPE_DEFAULT = 0x0,
MTL_TYPE_MODEL = 0x1,
MTL_TYPE_MODEL_GREY = 0x2,
MTL_TYPE_MODEL_VERTCOL = 0x3,
MTL_TYPE_MODEL_VERTCOL_GREY = 0x4,
MTL_TYPE_MODEL_VERTLIT = 0xD,
MTL_TYPE_MODEL_VERTLIT_VERTCOL = 0xE,
MTL_TYPE_MODEL_LMAP = 0x13,
MTL_TYPE_MODEL_LMAP_VERTCOL = 0x14,
MTL_TYPE_MODEL_SUBDIV = 0x19,
MTL_TYPE_MODEL_SUBDIV_VERTCOL = 0x1A,
MTL_TYPE_WORLD = 0x23,
MTL_TYPE_WORLD_VERTCOL = 0x24,
MTL_TYPE_COUNT = 0x24,
}
;

enum MaterialAssetFlags : unsigned __int8
{
MTL_ASSETFLAG_NONE = 0x0,
MTL_ASSETFLAG_CB_IS_WRITABLE = 0x1,
}
;

union Material::$47E784A74BB7900351251DE7BAB671D1
{
const char *name;
MaterialInfo info;
}
;

struct Material
{
union
{
const char *name;
MaterialInfo info;
};
unsigned __int8 stateBitsEntry[240];
unsigned __int8 textureCount;
unsigned __int8 constantCount;
unsigned __int8 stateBitsCount;
unsigned __int8 stateFlags;
unsigned __int8 cameraRegion;
unsigned __int8 materialType;
unsigned __int8 layerCount;
unsigned __int8 assetFlags;
MaterialTechniqueSet *techniqueSet;
MaterialTextureDef *textureTable;
MaterialConstantDef *constantTable;
GfxStateBits *stateBitsTable;
unsigned __int8 constantBufferIndex[240];
MaterialConstantBufferDef *constantBufferTable;
unsigned __int8 constantBufferCount;
const char **subMaterials;
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

union GfxTexture::$1E0C6F0D85BAC49B80DB1B34622CC66C
{
void *linemap;
void *map;
void *volmap;
void *cubemap;
GfxImageLoadDef *loadDef;
}
;

struct GfxTexture
{
union
{
void *linemap;
void *map;
void *volmap;
void *cubemap;
GfxImageLoadDef *loadDef;
};
void *shaderView;
void *shaderViewAlternate;
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

struct GfxImageStreamData
{
unsigned __int16 width;
unsigned __int16 height;
GfxImageStreamLevelCountAndSize levelCountAndSize;
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
MAPTYPE_COUNT = 0x7,
}
;

enum IMG_TS : unsigned __int8
{
TS_2D = 0x0,
TS_FUNCTION = 0x1,
TS_COLOR_MAP = 0x2,
TS_DETAIL_MAP = 0x3,
TS_UNUSED_2 = 0x4,
TS_NORMAL_MAP = 0x5,
TS_UNUSED_3 = 0x6,
TS_UNUSED_4 = 0x7,
TS_SPECULAR_MAP = 0x8,
TS_UNUSED_5 = 0x9,
TS_OCEANFLOW_DISPLACEMENT_MAP = 0xA,
TS_WATER_MAP = 0xB,
TS_OCEAN_DISPLACEMENT_MAP = 0xC,
TS_DISPLACEMENT_MAP = 0xD,
TS_PARALLAX_MAP = 0xE,
TS_COUNT = 0xF,
}
;

enum IMG_CATEGORY : unsigned __int8
{
IMG_CATEGORY_UNKNOWN = 0x0,
IMG_CATEGORY_AUTO_GENERATED = 0x1,
IMG_CATEGORY_LIGHTMAP = 0x2,
IMG_CATEGORY_LOAD_FROM_FILE = 0x3,
IMG_CATEGORY_RAW = 0x4,
IMG_CATEGORY_FIRST_UNMANAGED = 0x5,
IMG_CATEGORY_WATER = 0x5,
IMG_CATEGORY_RENDERTARGET = 0x6,
IMG_CATEGORY_TEMP = 0x7,
}
;

enum snd_alias_type_t : __int8
{
SAT_UNKNOWN = 0x0,
SAT_LOADED = 0x1,
SAT_STREAMED = 0x2,
SAT_PRIMED = 0x3,
SAT_COUNT = 0x4,
}
;

struct StreamFileNameRaw
{
const char *dir;
const char *name;
}
;

struct StreamFileNamePacked
{
unsigned __int64 offset;
unsigned __int64 length;
}
;

union StreamFileInfo
{
StreamFileNameRaw raw;
StreamFileNamePacked packed;
}
;

struct StreamFileName
{
bool isLocalized;
bool isStreamed;
unsigned __int16 fileIndex;
StreamFileInfo info;
}
;

struct StreamedSound
{
StreamFileName filename;
unsigned int totalMsec;
}
;

enum LoadedSoundFormat : __int16
{
SND_FORMAT_PCM = 0x1,
SND_FORMAT_FLAC = 0x6,
}
;

struct LoadedSoundInfo
{
char *data;
unsigned int sampleRate;
unsigned int dataByteCount;
unsigned int numSamples;
char channels;
char numBits;
char blockAlign;
__int16 format;
int loadedSize;
}
;

struct LoadedSound
{
const char *name;
StreamFileName filename;
LoadedSoundInfo info;
}
;

struct PrimedSound
{
LoadedSound *loadedPart;
StreamFileName streamedPart;
int dataOffset;
int totalSize;
}
;

union SoundFileRef
{
LoadedSound *loadSnd;
StreamedSound streamSnd;
PrimedSound primedSnd;
}
;

struct SoundFile
{
snd_alias_type_t type;
bool exists;
SoundFileRef u;
}
;

struct SndContext
{
const char *name;
char __pad0[8];
}
;

union SndCurve::$C30D40691AF2B33CB36727E3C47F4ED8
{
const char *filename;
const char *name;
}
;

struct SndCurve
{
bool isDefault;
union
{
const char *filename;
const char *name;
};
unsigned __int16 knotCount;
float knots[16][2];
}
;

struct SpeakerLevels
{
char speaker;
char numLevels;
float levels[2];
}
;

struct ChannelMap
{
int speakerCount;
SpeakerLevels speakers[6];
}
;

struct SpeakerMap
{
bool isDefault;
const char *name;
int unknown;
ChannelMap channelMaps[2][2];
}
;

struct DopplerPreset
{
const char *name;
float speedOfSound;
float playerVelocityScale;
float minPitch;
float maxPitch;
float smoothing;
}
;

struct SoundAliasFlags::packed
{
unsigned __int32 looping : 1;
unsigned __int32 isMaster : 1;
unsigned __int32 isSlave : 1;
unsigned __int32 fullDryLevel : 1;
unsigned __int32 noWetLevel : 1;
unsigned __int32 is3d : 1;
unsigned __int32 unk1 : 10;
unsigned __int32 type : 2;
unsigned __int32 unk2 : 14;
}
;

union SoundAliasFlags
{
struct SoundAliasFlags::packed packed;
unsigned int intValue;
}
;

enum SoundDspBus : __int8
{
SND_DSPBUS_DEFAULT = 0x0,
SND_DSPBUS_WEAPONS = 0x1,
SND_DSPBUS_WEAPONS_DIST = 0x2,
SND_DSPBUS_AMBIENCE = 0x3,
SND_DSPBUS_EMITTERS = 0x4,
SND_DSPBUS_QUADS = 0x5,
SND_DSPBUS_MUSIC = 0x6,
SND_DSPBUS_FOLEY = 0x7,
SND_DSPBUS_EXPLOSIONS = 0x8,
SND_DSPBUS_EXPLOSIONS_CRITICAL = 0x9,
SND_DSPBUS_TACTICAL = 0xA,
SND_DSPBUS_TACTICAL_SPECIAL = 0xB,
SND_DSPBUS_VEHICLES = 0xC,
SND_DSPBUS_AIR_VEHICLES = 0xD,
SND_DSPBUS_INT_VEHICLES_1 = 0xE,
SND_DSPBUS_INT_VEHICLES_2 = 0xF,
SND_DSPBUS_VOICES = 0x10,
SND_DSPBUS_VOICES_CRITICAL = 0x11,
SND_DSPBUS_VOICES_UNFILTERED = 0x12,
SND_DSPBUS_ANIMALS = 0x13,
SND_DSPBUS_PHYSICS = 0x14,
SND_DSPBUS_BATTLECHATTER = 0x15,
SND_DSPBUS_INTERFACE = 0x16,
SND_DSPBUS_SCRIPTED = 0x17,
SND_DSPBUS_SCRIPTED1 = 0x18,
SND_DSPBUS_SCRIPTED2 = 0x19,
SND_DSPBUS_SCRIPTED3 = 0x1A,
SND_DSPBUS_SCRIPTED4 = 0x1B,
SND_DSPBUS_SCRIPTED5 = 0x1C,
SND_DSPBUS_UNOCCLUDED = 0x1D,
SND_DSPBUS_WHIZBYS = 0x1E,
SND_DSPBUS_NOTIMESCALE = 0x1F,
SND_DSPBUS_COUNT = 0x20,
}
;

enum SoundVolMod : __int16
{
SND_VOLMOD_DEFAULT = 0x0,
SND_VOLMOD_HUD = 0x1,
SND_VOLMOD_FRONTEND_SFX = 0x2,
SND_VOLMOD_FRONTEND_MUSIC = 0x3,
SND_VOLMOD_SP_HIT_ALERT_NPC = 0x4,
SND_VOLMOD_MUSIC = 0x5,
SND_VOLMOD_MUSIC2 = 0x6,
SND_VOLMOD_MUSIC_BIG = 0x7,
SND_VOLMOD_MUSIC_EMITTER = 0x8,
SND_VOLMOD_AMB_QUAD = 0x9,
SND_VOLMOD_AMB_DIST = 0xA,
SND_VOLMOD_AMB_DAMB = 0xB,
SND_VOLMOD_AMB_DAMB_BIG = 0xC,
SND_VOLMOD_AMB_EMITTER = 0xD,
SND_VOLMOD_AMB_EMITTER_INT = 0xE,
SND_VOLMOD_AMB_EMITTER_EXT = 0xF,
SND_VOLMOD_PHYSICS = 0x10,
SND_VOLMOD_BODYFALL = 0x11,
SND_VOLMOD_BODYFALL_MECH = 0x12,
SND_VOLMOD_FOLEY_PLR = 0x13,
SND_VOLMOD_FOLEY_PLR_SPECIAL = 0x14,
SND_VOLMOD_MP_FOLEY_PLR = 0x15,
SND_VOLMOD_FOLEY_NPC = 0x16,
SND_VOLMOD_FOLEY_MECH = 0x17,
SND_VOLMOD_FOLEYMP_NPC = 0x18,
SND_VOLMOD_WPN_FOLEY_PLR = 0x19,
SND_VOLMOD_WPN_FOLEY_NPC = 0x1A,
SND_VOLMOD_WPN_FOLEY_MECH = 0x1B,
SND_VOLMOD_FOOTSTEP_PLR = 0x1C,
SND_VOLMOD_FOOTSTEP_PLR_SPECIAL = 0x1D,
SND_VOLMOD_FOOTSTEP_WATER_PLR = 0x1E,
SND_VOLMOD_FOOTSTEP_NPC = 0x1F,
SND_VOLMOD_FOOTSTEP_MECH = 0x20,
SND_VOLMOD_MP_FOOTSTEP_PLR = 0x21,
SND_VOLMOD_MP_FOOTSTEP_NPC = 0x22,
SND_VOLMOD_MELEE_PLR = 0x23,
SND_VOLMOD_MELEE_NPC = 0x24,
SND_VOLMOD_MELEE_HIT = 0x25,
SND_VOLMOD_PLR_INTERNAL = 0x26,
SND_VOLMOD_DEATHS_DOOR = 0x27,
SND_VOLMOD_CHATTER_FRIENDLY = 0x28,
SND_VOLMOD_CHATTER_ENEMY = 0x29,
SND_VOLMOD_CHATTER_ATLAS = 0x2A,
SND_VOLMOD_VOX_FRIENDLY = 0x2B,
SND_VOLMOD_VOX_ENEMY = 0x2C,
SND_VOLMOD_DIALOG = 0x2D,
SND_VOLMOD_DIALOG_RADIO = 0x2E,
SND_VOLMOD_DIALOG_CRITICAL = 0x2F,
SND_VOLMOD_DIALOG_AMB = 0x30,
SND_VOLMOD_DESTRUCT = 0x31,
SND_VOLMOD_EXPLOSION = 0x32,
SND_VOLMOD_EXPLOSION_GRENADE = 0x33,
SND_VOLMOD_EXPLOSION_FLASHBANG = 0x34,
SND_VOLMOD_EXPLOSION_ROCKET = 0x35,
SND_VOLMOD_EXPLOSION_CAR = 0x36,
SND_VOLMOD_EXPLOSION_CRITICAL = 0x37,
SND_VOLMOD_EXPLOSION_DEBRIS = 0x38,
SND_VOLMOD_IMPACT_CRITICAL = 0x39,
SND_VOLMOD_BULLET_IMPACT_GEO = 0x3A,
SND_VOLMOD_BULLET_IMPACT_PLR = 0x3B,
SND_VOLMOD_BULLET_IMPACT_NPC = 0x3C,
SND_VOLMOD_MP_BULLET_IMPACT_GEO = 0x3D,
SND_VOLMOD_MP_BULLET_IMPACT_PLR = 0x3E,
SND_VOLMOD_MP_BULLET_IMPACT_NPC = 0x3F,
SND_VOLMOD_BULLET_WHIZBY = 0x40,
SND_VOLMOD_MP_BULLET_WHIZBY = 0x41,
SND_VOLMOD_BULLET_IMPACT_SPECIAL = 0x42,
SND_VOLMOD_VEHICLE_PLR = 0x43,
SND_VOLMOD_VEHICLE_PLR_SPECIAL = 0x44,
SND_VOLMOD_VEHICLE_NPC = 0x45,
SND_VOLMOD_VEHICLE_NPC_SPECIAL = 0x46,
SND_VOLMOD_VEHICLE_AVM_PLR = 0x47,
SND_VOLMOD_VEHICLE_AVM_NPC = 0x48,
SND_VOLMOD_WPN_VEHICLE_PLR = 0x49,
SND_VOLMOD_WPN_VEHICLE_NPC = 0x4A,
SND_VOLMOD_VEHICLE = 0x4B,
SND_VOLMOD_VEHICLE_AIRCRAFT = 0x4C,
SND_VOLMOD_VEHICLE_PDRONE = 0x4D,
SND_VOLMOD_VEHICLE_DRONE_SWARM = 0x4E,
SND_VOLMOD_VEHICLE_AMB = 0x4F,
SND_VOLMOD_GRENADEBOUNCE = 0x50,
SND_VOLMOD_MP_GRENADEBOUNCE = 0x51,
SND_VOLMOD_SHELLCASINGS = 0x52,
SND_VOLMOD_MP_SHELLCASINGS = 0x53,
SND_VOLMOD_WPN_PLR = 0x54,
SND_VOLMOD_WPN_PLR_SPECIAL = 0x55,
SND_VOLMOD_WPN_NPC_SPECIAL = 0x56,
SND_VOLMOD_MP_WPN_PLR = 0x57,
SND_VOLMOD_WPN_NPC = 0x58,
SND_VOLMOD_MP_WPN_NPC = 0x59,
SND_VOLMOD_WPN_PROJECTILE = 0x5A,
SND_VOLMOD_MP_WPN_PROJECTILE = 0x5B,
SND_VOLMOD_WPN_PLR_SHOT_SPECIAL = 0x5C,
SND_VOLMOD_WPN_PLR_SHOT = 0x5D,
SND_VOLMOD_WPN_PLR_MECH = 0x5E,
SND_VOLMOD_WPN_PLR_SUB = 0x5F,
SND_VOLMOD_WPN_PLR_TAIL = 0x60,
SND_VOLMOD_WPN_NPC_SHOT = 0x61,
SND_VOLMOD_WPN_NPC_MECH = 0x62,
SND_VOLMOD_WPN_NPC_SUB = 0x63,
SND_VOLMOD_WPN_NPC_TAIL = 0x64,
SND_VOLMOD_WPN_NPC_DIST = 0x65,
SND_VOLMOD_TACTICAL_PLR = 0x66,
SND_VOLMOD_TACTICAL_NPC = 0x67,
SND_VOLMOD_TACTICAL_PLR_SPECIAL = 0x68,
SND_VOLMOD_TACTICAL_NPC_SPECIAL = 0x69,
SND_VOLMOD_ATTACK_DOG = 0x6A,
SND_VOLMOD_BIG_MOMENT = 0x6B,
SND_VOLMOD_SCRIPTED1 = 0x6C,
SND_VOLMOD_SCRIPTED2 = 0x6D,
SND_VOLMOD_SCRIPTED3 = 0x6E,
SND_VOLMOD_SCRIPTED4 = 0x6F,
SND_VOLMOD_SCRIPTED5 = 0x70,
SND_VOLMOD_SCRIPTED6 = 0x71,
SND_VOLMOD_SCRIPTED7 = 0x72,
SND_VOLMOD_SCRIPTED8 = 0x73,
SND_VOLMOD_SCRIPTED9 = 0x74,
SND_VOLMOD_SCRIPTED10 = 0x75,
SND_VOLMOD_FULLVOLUME = 0x76,
SND_VOLMOD_MP_PRE_EVENT = 0x77,
SND_VOLMOD_MP_POST_EVENT = 0x78,
SND_VOLMOD_MP_SPLASH_NOTIFY = 0x79,
SND_VOLMOD_MP_HIT_MARKER = 0x7A,
SND_VOLMOD_MP_PERK_QUIET = 0x7B,
SND_VOLMOD_MW3_AMB_DAMB = 0x7C,
SND_VOLMOD_MW3_AMB_EMITTER = 0x7D,
SND_VOLMOD_MW3_AMB_QUAD = 0x7E,
SND_VOLMOD_MW3_BODYFALL = 0x7F,
SND_VOLMOD_MW3_BULLET_IMPACT_GEO = 0x80,
SND_VOLMOD_MW3_BULLET_IMPACT_NPC = 0x81,
SND_VOLMOD_MW3_BULLET_IMPACT_PLR = 0x82,
SND_VOLMOD_MW3_BULLET_WHIZBY = 0x83,
SND_VOLMOD_MW3_CHATTER_ENEMY = 0x84,
SND_VOLMOD_MW3_CHATTER_FRIENDLY = 0x85,
SND_VOLMOD_MW3_DEPRECATED_DEFAULT = 0x86,
SND_VOLMOD_MW3_DEPRECATED_MAX = 0x87,
SND_VOLMOD_MW3_DEPRECATED_NA = 0x88,
SND_VOLMOD_MW3_DESTRUCT = 0x89,
SND_VOLMOD_MW3_DIALOG = 0x8A,
SND_VOLMOD_MW3_DIALOG_AMB = 0x8B,
SND_VOLMOD_MW3_DIALOG_CRITICAL = 0x8C,
SND_VOLMOD_MW3_DIALOG_RADIO = 0x8D,
SND_VOLMOD_MW3_EXPLOSION = 0x8E,
SND_VOLMOD_MW3_EXPLOSION_CAR = 0x8F,
SND_VOLMOD_MW3_EXPLOSION_FLASHBANG = 0x90,
SND_VOLMOD_MW3_EXPLOSION_GRENADE = 0x91,
SND_VOLMOD_MW3_EXPLOSION_ROCKET = 0x92,
SND_VOLMOD_MW3_FOLEY_NPC = 0x93,
SND_VOLMOD_MW3_FOOTSTEP_NPC = 0x94,
SND_VOLMOD_MW3_FRONTEND_SFX = 0x95,
SND_VOLMOD_MW3_FULLVOLUME = 0x96,
SND_VOLMOD_MW3_GRENADEBOUNCE = 0x97,
SND_VOLMOD_MW3_MELEE_NPC = 0x98,
SND_VOLMOD_MW3_MP_BULLET_IMPACT_GEO = 0x99,
SND_VOLMOD_MW3_MP_BULLET_IMPACT_NPC = 0x9A,
SND_VOLMOD_MW3_MP_FOLEY_NPC = 0x9B,
SND_VOLMOD_MW3_MP_FOOTSTEP_NPC = 0x9C,
SND_VOLMOD_MW3_MP_WPN_NPC = 0x9D,
SND_VOLMOD_MW3_MUSIC = 0x9E,
SND_VOLMOD_MW3_PHYSICS = 0x9F,
SND_VOLMOD_MW3_SCRIPTED1 = 0xA0,
SND_VOLMOD_MW3_SCRIPTED2 = 0xA1,
SND_VOLMOD_MW3_SCRIPTED3 = 0xA2,
SND_VOLMOD_MW3_SCRIPTED4 = 0xA3,
SND_VOLMOD_MW3_SCRIPTED5 = 0xA4,
SND_VOLMOD_MW3_SHELLCASINGS = 0xA5,
SND_VOLMOD_MW3_VEHICLE = 0xA6,
SND_VOLMOD_MW3_VEHICLE_PLR = 0xA7,
SND_VOLMOD_MW3_VEHICLE_NPC = 0xA8,
SND_VOLMOD_MW3_VOX_ENEMY = 0xA9,
SND_VOLMOD_MW3_VOX_FRIENDLY = 0xAA,
SND_VOLMOD_MW3_WPN_FOLEY_NPC = 0xAB,
SND_VOLMOD_MW3_WPN_FOLEY_PLR = 0xAC,
SND_VOLMOD_MW3_FOOTSTEP_PLR = 0xAD,
SND_VOLMOD_MW3_WPN_NPC = 0xAE,
SND_VOLMOD_MW3_WPN_PLR = 0xAF,
SND_VOLMOD_MW3_WPN_PROJECTILE = 0xB0,
SND_VOLMOD_MW3_WPN_VEHICLE_NPC = 0xB1,
SND_VOLMOD_MW3_WPN_VEHICLE_PLR = 0xB2,
SND_VOLMOD_COUNT = 0xB3,
}
;

struct snd_alias_t
{
const char *aliasName;
const char *subtitle;
const char *secondaryAliasName;
const char *chainAliasName;
SoundFile *soundFile;
const char *mixerGroup;
__int16 poly;
__int16 polyGlobal;
char polyEntityType;
char polyGlobalType;
char dspBusIndex;
char priority;
char __pad0[12];
float volMin;
float volMax;
__int16 volModIndex;
float pitchMin;
float pitchMax;
float distMin;
float distMax;
float velocityMin;
int flags;
char masterPriority;
float masterPercentage;
float slavePercentage;
char playbackPercentage;
float probability;
char u1;
SndContext *sndContext;
int sequence;
float lfePercentage;
float centerPercentage;
int startDelay;
SndCurve *sndCurve;
float envelopMin;
float envelopMax;
SndCurve *lpfCurve;
SndCurve *reverbSendCurve;
SpeakerMap *speakerMap;
float reverbWetMixOverride;
float reverbMultiplier;
float smartPanDistance2d;
float smartPanDistance3d;
float smartPanAttenuation3d;
float envelopPercentage;
__int16 stereo3dAngle;
float stereo3dStart;
float stereo3dEnd;
unsigned __int8 allowDoppler;
DopplerPreset *dopplerPreset;
float u2;
}
;

struct snd_alias_context_list
{
__int16 unk;
}
;

union snd_alias_list_t::$F3EACB80B5F54CCBD7595DC6FF1B9C51
{
const char *aliasName;
const char *name;
}
;

struct snd_alias_list_t
{
union
{
const char *aliasName;
const char *name;
};
snd_alias_t *head;
snd_alias_context_list *contextList;
unsigned __int8 count;
unsigned __int8 contextListCount;
}
;

struct LocalizeEntry
{
const char *value;
const char *name;
}
;

struct TriggerModel
{
int contents;
unsigned __int16 hullCount;
unsigned __int16 firstHull;
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
float dir[3];
float midPoint;
float halfSize;
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
}
;

struct ClientTriggerAabbNode
{
Bounds bounds;
unsigned __int16 firstChild;
unsigned __int16 childCount;
}
;

enum clientTriggerType_t : __int32
{
CLIENT_TRIGGER_NONE = 0x0,
CLIENT_TRIGGER_VISIONSET = 0x1,
CLIENT_TRIGGER_REVERB = 0x2,
CLIENT_TRIGGER_AUDIO = 0x4,
CLIENT_TRIGGER_BLEND_VISION = 0x8,
CLIENT_TRIGGER_BLEND_AUDIO = 0x10,
CLIENT_TRIGGER_BLEND_ALL = 0x12,
CLIENT_TRIGGER_NPC = 0x20,
}
;

struct ClientTriggers
{
MapTriggers trigger;
unsigned __int16 numClientTriggerNodes;
ClientTriggerAabbNode *clientTriggerAabbTree;
unsigned int triggerStringLength;
char *triggerString;
__int16 *visionSetTriggers;
__int16 *blendLookup;
__int16 *unk1;
__int16 *triggerType;
vec3_t *origins;
float *scriptDelay;
__int16 *audioTriggers;
__int16 *unk2;
__int16 *unk3;
__int16 *unk4;
__int16 *unk5;
__int16 *unk6;
}
;

struct ClientTriggerBlendNode
{
float pointA[3];
float pointB[3];
unsigned __int16 triggerA;
unsigned __int16 triggerB;
}
;

struct ClientTriggerBlend
{
unsigned __int16 numClientTriggerBlendNodes;
ClientTriggerBlendNode *blendNodes;
}
;

struct SpawnPointEntityRecord
{
unsigned __int16 index;
scr_string_t name;
scr_string_t target;
scr_string_t script_noteworthy;
scr_string_t unknown;
float origin[3];
float angles[3];
}
;

struct SpawnPointRecordList
{
unsigned __int16 spawnsCount;
SpawnPointEntityRecord *spawns;
}
;

struct SplinePointEntityRecord
{
int splineId;
int splineNodeId;
char *splineNodeLabel;
float splineNodeTension;
float origin[3];
float corridorDims[2];
float tangent[3];
float distToNextNode;
vec3_t *positionCubic;
vec3_t *tangentQuadratic;
}
;

struct SplinePointRecordList
{
unsigned __int16 splinePointCount;
float splineLength;
SplinePointEntityRecord *splinePoints;
}
;

struct SplineRecordList
{
unsigned __int16 splineCount;
SplinePointRecordList *splines;
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

struct StructuredDataDefSet
{
const char *name;
unsigned int defCount;
StructuredDataDef *defs;
}
;

enum NetConstStringType
{
}
;

enum NetConstStringSource
{
}
;

struct NetConstStrings
{
const char *name;
NetConstStringType stringType;
NetConstStringSource sourceType;
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

struct TTFDef
{
const char *name;
int fileLen;
const char *file;
void *ftFace;
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
bool evalVisStatePerParticle;
bool sortParticlesAtRuntime;
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
FxParticleSimAnimationFrame *frames;
GfxColorFloat *colorTable;
}
;

enum FxElemType : unsigned __int8
{
FX_ELEM_TYPE_SPRITE_BILLBOARD = 0x0,
FX_ELEM_TYPE_SPRITE_ORIENTED = 0x1,
FX_ELEM_TYPE_SPRITE_ROTATED = 0x2,
FX_ELEM_TYPE_TAIL = 0x3,
FX_ELEM_TYPE_LINE = 0x4,
FX_ELEM_TYPE_TRAIL = 0x5,
FX_ELEM_TYPE_FLARE = 0x6,
FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION = 0x7,
FX_ELEM_TYPE_CLOUD = 0x8,
FX_ELEM_TYPE_SPARK_CLOUD = 0x9,
FX_ELEM_TYPE_SPARK_FOUNTAIN = 0xA,
FX_ELEM_TYPE_MODEL = 0xB,
FX_ELEM_TYPE_OMNI_LIGHT = 0xC,
FX_ELEM_TYPE_SPOT_LIGHT = 0xD,
FX_ELEM_TYPE_SOUND = 0xE,
FX_ELEM_TYPE_DECAL = 0xF,
FX_ELEM_TYPE_RUNNER = 0x10,
FX_ELEM_TYPE_VECTORFIELD = 0x11,
}
;

enum FxElemLitType : unsigned __int8
{
FX_ELEM_LIT_TYPE_NONE = 0x0,
FX_ELEM_LIT_TYPE_LIGHTGRID_SPAWN_SINGLE = 0x1,
FX_ELEM_LIT_TYPE_LIGHTGRID_FRAME_SINGLE = 0x2,
FX_ELEM_LIT_TYPE_LIGHTGRID_FRAME_SPRITE = 0x3,
FX_ELEM_LIT_TYPE_LIGHTGRID_FRAME_VERTEX = 0x4,
FX_ELEM_LIT_TYPE_COUNT = 0x5,
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
FX_ELEM_RECEIVE_DYNAMIC_LIGHT = 0x80000,
FX_ELEM_VOLUMETRIC_TRAIL = 0x100000,
FX_ELEM_USE_COLLISION = 0x200000,
FX_ELEM_USE_VECTORFIELDS = 0x400000,
FX_ELEM_NO_SURFACE_HDR_SCALAR = 0x800000,
FX_ELEM_HAS_VELOCITY_GRAPH_LOCAL = 0x1000000,
FX_ELEM_HAS_VELOCITY_GRAPH_WORLD = 0x2000000,
FX_ELEM_HAS_GRAVITY = 0x4000000,
FX_ELEM_USE_MODEL_PHYSICS = 0x8000000,
FX_ELEM_NONUNIFORM_SCALE = 0x10000000,
FX_ELEM_CLOUD_SHAPE_CUBE = 0x0,
FX_ELEM_CLOUD_SHAPE_SPHERE_LARGE = 0x20000000,
FX_ELEM_CLOUD_SHAPE_SPHERE_MEDIUM = 0x40000000,
FX_ELEM_CLOUD_SHAPE_SPHERE_SMALL = 0x60000000,
FX_ELEM_CLOUD_SHAPE_MASK = 0x60000000,
FX_ELEM_FOUNTAIN_DISABLE_COLLISION = 0x80000000,
}
;

enum FxElemDefExtraFlags : unsigned __int32
{
}
;

struct FxFloatRange
{
float base;
float amplitude;
}
;

struct FxSpawnDefLooping
{
int intervalMsec;
int count;
}
;

struct FxIntRange
{
int base;
int amplitude;
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

struct FxElemAtlas
{
unsigned __int8 behavior;
unsigned __int8 index;
unsigned __int8 fps;
unsigned __int8 loopCount;
unsigned __int8 colIndexBits;
unsigned __int8 rowIndexBits;
__int16 entryCount;
}
;

union FxEffectDefRef
{
FxEffectDef *handle;
const char *name;
}
;

struct FxElemVec3Range
{
float base[3];
float amplitude[3];
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
float pad1[3];
float rotationDelta;
float rotationTotal;
float size[2];
float scale;
float pad2[2];
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
float pos[2];
float normal[2];
float texCoord[2];
char __pad0[8];
}
;

struct FxTrailDef
{
int scrollTimeMsec;
int repeatDist;
float invSplitDist;
float invSplitArcDist;
float invSplitTime;
char __pad0[8];
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
char __pad0[24];
}
;

struct FxOmniLightDef
{
char __pad0[16];
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
float dir[3];
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
char *unknownDef;
FxTrailDef *trailDef;
FxSparkFountainDef *sparkFountainDef;
FxSpotLightDef *spotLightDef;
FxOmniLightDef *omniLightDef;
FxFlareDef *flareDef;
}
;

struct FxElemDef
{
FxElemDefFlags flags;
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
FxElemLitType elemLitType;
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
float __pad0[6];
}
;

struct XModelIKData
{
unsigned __int8 charDataLen;
unsigned __int8 floatDataLen;
unsigned __int8 int32DataLen;
unsigned __int8 stringsCount;
char *charData;
float *floatData;
int *int32Data;
scr_string_t *strings;
}
;

struct SkeletonScriptCode
{
char __pad0[4];
}
;

struct SkeletonScript
{
const char *name;
XModelIKData ikData;
unsigned __int16 codeLen;
SkeletonScriptCode *code;
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
float mins[3];
float size[3];
XAnimDynamicFrames frames;
XAnimDynamicIndices indices;
}
;

union XAnimPartTransData
{
XAnimPartTransFrames frames;
float frame0[3];
}
;

struct XAnimPartTrans
{
unsigned __int16 size;
unsigned __int8 smallTrans;
XAnimPartTransData u;
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

struct XAnimDeltaPartQuat2
{
unsigned __int16 size;
XAnimDeltaPartQuatData2 u;
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

union XAnimIndices
{
unsigned __int8 *_1;
unsigned __int16 *_2;
void *data;
}
;

struct XAnimNotifyInfo
{
scr_string_t name;
float time;
}
;

enum XAnimPartsFlags : unsigned __int8
{
ANIM_NONE = 0x0,
ANIM_LOOP = 0x1,
ANIM_DELTA = 0x2,
ANIM_DELTA_3D = 0x4,
ANIM_DEFAULT = 0x8,
}
;

typedef float BlendShapeWeight
;

typedef char XAnimScriptedViewmodelAnimData
;

struct XAnimParts
{
const char *name;
unsigned __int16 dataByteCount;
unsigned __int16 dataShortCount;
unsigned __int16 dataIntCount;
unsigned __int16 numframes;
unsigned __int8 flags;
unsigned __int8 boneCount[10];
char u1;
char u2;
unsigned __int8 notifyCount;
unsigned __int8 assetType;
unsigned __int8 ikType;
unsigned int randomDataByteCount;
unsigned int randomDataShortCount;
unsigned int randomDataIntCount;
unsigned int indexCount;
float framerate;
float frequency;
scr_string_t *names;
char *dataByte;
__int16 *dataShort;
int *dataInt;
__int16 *randomDataShort;
unsigned __int8 *randomDataByte;
int *randomDataInt;
XAnimIndices indices;
XAnimNotifyInfo *notify;
XAnimDeltaPart *deltaPart;
const char *secondaryName;
__int16 u3;
unsigned __int16 blendShapeWeightCount;
__int16 u4;
scr_string_t *blendShapeWeightNames;
char (*blendShapeWeightUnknown1)[3];
unsigned __int16 *blendShapeWeightUnknown2;
unsigned __int16 *blendShapeWeightUnknown3;
unsigned __int16 *blendShapeWeightUnknown4;
BlendShapeWeight *blendShapeWeights;
uint64_t u5;
XAnimScriptedViewmodelAnimData *scriptedViewmodelAnimData;
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

struct UnknownXSurface0
{
float xyz[3];
PackedUnitVec normal;
}
;

struct BlendVertsUnknown
{
unsigned __int16 b[15];
unsigned __int16 blendVertCountIndex;
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
void *regularPatchIndexBuffer;
void *faceIndexBuffer;
void *regularPatchIndexBufferView;
void *regularPatchFlagsView;
void *facePointsView;
void *edgePointsView;
void *vertexPointsView;
void *normalsView;
void *transitionPointsView;
void *regularPatchConesView;
}
;

struct GfxSubdivCache
{
unsigned int size;
void *subdivCacheBuffer;
void *subdivCacheView;
}
;

struct XSurfaceSubdivInfo
{
XSurfaceSubdivLevel *levels;
char __pad0[24];
GfxSubdivCache cache;
}
;

struct BlendShapeVert
{
char __pad0[32];
}
;

struct BlendShape
{
unsigned int vertCount;
BlendShapeVert *verts;
void *blendShapeVertsBuffer;
void *blendShapeVertsView;
}
;

typedef char alignCompBufByte_t
;

typedef unsigned __int16 alignVertBufFloat16Vec2_t[2]
;

typedef unsigned __int16 alignCompBufUShort_t
;

typedef float alignCompBufFloat_t
;

typedef unsigned __int16 XBlendInfo
;

enum XSurfaceFlags : unsigned __int16
{
SURF_FLAG_VERTCOL_GREY = 0x1,
SURF_FLAG_VERTCOL_NONE = 0x2,
SURF_FLAG_SKINNED = 0x4,
SURF_FLAG_REACTIVE_MOTION = 0x8,
SURF_FLAG_LIGHTMAP_COORDS = 0x10,
SURF_FLAG_TENSION = 0x20,
}
;

struct XSurface
{
unsigned __int16 flags;
unsigned __int16 vertCount;
unsigned __int16 triCount;
unsigned __int8 rigidVertListCount;
unsigned __int8 subdivLevelCount;
__int16 blendVertCounts[8];
GfxVertexUnion0 verts0;
Face *triIndices;
Face *triIndices2;
void *vb0;
void *vb0View;
void *indexBuffer;
XRigidVertList *rigidVertLists;
UnknownXSurface0 *unknown0;
XBlendInfo *blendVerts;
BlendVertsUnknown *blendVertsTable;
void *blendVertsBuffer;
void *blendVertsView;
alignVertBufFloat16Vec2_t *lmapUnwrap;
void *vblmapBuffer;
void *vblmapView;
XSurfaceSubdivInfo *subdiv;
alignCompBufFloat_t *tensionData;
alignCompBufUShort_t *tensionAccumTable;
void *tensionAccumTableBuffer;
void *tensionAccumTableView;
void *tensionDataBuffer;
void *tensionDataView;
void *indexBufferView;
BlendShape *blendShapes;
unsigned int blendShapesCount;
unsigned int vertexLightingIndex;
char __pad0[4];
int partBits[8];
char __pad1[4];
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

struct ReactiveMotionModelTweaks
{
float scale[4];
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

struct BlendShapeWeightMap
{
unsigned __int16 weightIndex;
unsigned __int16 targetIndex;
float fullWeight;
}
;

struct ExtentBounds
{
vec3_t mins;
vec3_t maxs;
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

struct XPhysBoneInfo
{
PhysPreset *physPreset;
PhysConstraint *physContraint;
PhysCollmap *physCollmap;
char __pad0[8];
}
;

enum XModelFlags : unsigned __int16
{
XMODEL_FLAG_COMPOSITE = 0x400,
}
;

enum XModelLodRampType : unsigned __int8
{
XMODEL_LOD_RAMP_RIGID = 0x0,
XMODEL_LOD_RAMP_SKINNED = 0x1,
XMODEL_LOD_RAMP_COUNT = 0x2,
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

enum playerAnimType_t : unsigned __int8
{
PLAYERANIMTYPE_NONE = 0x0,
PLAYERANIMTYPE_OTHER = 0x1,
PLAYERANIMTYPE_PISTOL = 0x2,
PLAYERANIMTYPE_SMG = 0x3,
PLAYERANIMTYPE_AUTORIFLE = 0x4,
PLAYERANIMTYPE_SNIPER = 0x5,
PLAYERANIMTYPE_ROCKET_LAUNCHER = 0x6,
PLAYERANIMTYPE_GRENADE = 0x7,
PLAYERANIMTYPE_M203 = 0x8,
PLAYERANIMTYPE_HOLD = 0x9,
PLAYERANIMTYPE_BRIEFCASE = 0xA,
PLAYERANIMTYPE_RIOTSHIELD = 0xB,
PLAYERANIMTYPE_LAPTOP = 0xC,
PLAYERANIMTYPE_THROWINGKNIFE = 0xD,
PLAYERANIMTYPE_MINIGUN = 0xE,
PLAYERANIMTYPE_SMG_BULLPUP = 0x1F,
PLAYERANIMTYPE_AUTOFILE_BULLPUP = 0x10,
PLAYERANIMTYPE_SNIPER_BULLPUP = 0x11,
PLAYERANIMTYPE_KILLSTREAKTRIGGER = 0x12,
PLAYERANIMTYPE_TROPHYSYSTEM = 0x13,
PLAYERANIMTYPE_COMBATKNIFE = 0x14,
PLAYERANIMTYPE_NUM = 0x15,
}
;

enum weapType_t : __int32
{
WEAPCLASS_NONE = 0x0,
WEAPCLASS_BULLET = 0x1,
WEAPCLASS_GRENADE = 0x2,
WEAPCLASS_PROJECTILE = 0x3,
WEAPCLASS_RIOTSHIELD = 0x4,
WEAPCLASS_ENERGY = 0x5,
WEAPCLASS_NUM = 0x6,
}
;

enum weapClass_t : __int32
{
WEAPTYPE_RIFLE = 0x1,
WEAPTYPE_SNIPER = 0x2,
WEAPTYPE_MG = 0x3,
WEAPTYPE_SMG = 0x4,
WEAPTYPE_SPREAD = 0x5,
WEAPTYPE_PISTOL = 0x6,
WEAPTYPE_ROCKETLAUNCHER = 0x7,
WEAPTYPE_TURRET = 0x8,
WEAPTYPE_THROWINGKNIFE = 0x9,
WEAPTYPE_NON_PLAYER = 0xA,
WEAPTYPE_ITEM = 0xB,
WEAPTYPE_CONE = 0xC,
WEAPTYPE_BEAM = 0xD,
WEAPTYPE_SHIELD = 0xE,
WEAPTYPE_HOVER = 0xF,
WEAPTYPE_CLOAK = 0x10,
WEAPTYPE_PING = 0x11,
WEAPTYPE_REPULSOR = 0x12,
WEAPTYPE_ADRENALINE = 0x13,
WEAPTYPE_HEALTH = 0x14,
WEAPTYPE_MUTE = 0x15,
WEAPTYPE_UNDERWATER = 0x16,
WEAPTYPE_BALL = 0x17,
WEAPTYPE_NUM = 0x18,
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
WEAPINVENTORYCOUNT = 0x6,
}
;

enum weapFireType_t : __int32
{
WEAPON_FIRETYPE_FULLAUTO = 0x0,
WEAPON_FIRETYPE_SINGLESHOT = 0x1,
WEAPON_FIRETYPE_BURSTFIRE2 = 0x2,
WEAPON_FIRETYPE_BURSTFIRE3 = 0x3,
WEAPON_FIRETYPE_BURSTFIRE4 = 0x4,
WEAPON_FIRETYPE_DOUBLEBARREL = 0x5,
WEAPON_FIRETYPECOUNT = 0x6,
WEAPON_FIRETYPE_BURSTFIRE_FIRST = 0x2,
WEAPON_FIRETYPE_BURSTFIRE_LAST = 0x4,
}
;

enum PenetrateType : unsigned __int32
{
PENETRATE_TYPE_NONE = 0x0,
PENETRATE_TYPE_SMALL = 0x1,
PENETRATE_TYPE_MEDIUM = 0x2,
PENETRATE_TYPE_LARGE = 0x3,
PENETRATE_TYPE_COUNT = 0x4,
}
;

enum ImpactType : __int32
{
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

enum OffhandClass : unsigned __int32
{
OFFHAND_CLASS_NONE = 0x0,
OFFHAND_CLASS_FRAG_GRENADE = 0x1,
OFFHAND_CLASS_SMOKE_GRENADE = 0x2,
OFFHAND_CLASS_FLASH_GRENADE = 0x3,
OFFHAND_CLASS_OTHER = 0x9,
OFFHAND_CLASS_COUNT = 0xA,
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

enum __bitmask weapAdsFireMode_t : __int32
{
WEAPADSFIREMODE_NUM = 0x3,
}
;

enum weapGreebleType_t : __int32
{
WEAPGREEBLE_NONE = 0x0,
WEAPGREEBLE_ACCURACYLEVEL1 = 0x1,
WEAPGREEBLE_ACCURACYLEVEL2 = 0x2,
WEAPGREEBLE_FIRERATELEVEL1 = 0x3,
WEAPGREEBLE_FIRERATELEVEL2 = 0x4,
WEAPGREEBLE_NUM = 0x5,
}
;

enum weapAutoReloadType_t : __int32
{
}
;

enum weapFireBarrels_t : __int32
{
}
;

enum weapFireTimeInterpolation_t : __int32
{
}
;

enum WeaponSlotRestriction : __int32
{
SLOT_RESTRICTION_NONE = 0x0,
SLOT_RESTRICTION_OPEN = 0x1,
SLOT_RESTRICTION_PRESET = 0x2,
SLOT_RESTRICTION_COUNT = 0x3,
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

struct AttChargeInfo
{
char __pad0[28];
}
;

struct AttHybridSettings
{
char __pad0[72];
}
;

union WAFieldParm
{
char p_char;
bool p_bool;
int p_int;
float p_float;
const char *string;
}
;

enum WAFieldType : unsigned __int8
{
WAFIELD_TYPE_STRING = 0x0,
WAFIELD_TYPE_INT = 0x4,
WAFIELD_TYPE_BOOL = 0x6,
WAFIELD_TYPE_FLOAT = 0x7,
WAFIELD_TYPE_FLOAT32 = 0x9,
WAFIELD_TYPE_FX = 0xA,
WAFIELD_TYPE_MODEL = 0xB,
WAFIELD_TYPE_ANIM = 0xC,
WAFIELD_TYPE_MATERIAL = 0xD,
WAFIELD_TYPE_SOUND = 0xF,
WAFIELD_TYPE_TRACER = 0x10,
}
;

enum WAFieldCode : unsigned __int8
{
FIELD_OP_STRING_SET = 0x0,
FIELD_OP_STRING_REPLACE = 0x1,
FIELD_OP_STRING_APPEND = 0x2,
FIELD_OP_NUMBER_BEGIN = 0x3,
FIELD_OP_NUMBER_SET = 0x3,
FIELD_OP_NUMBER_END = 0x6,
FIELD_OP_COUNT = 0x7,
}
;

struct WAField
{
unsigned __int8 index;
unsigned __int8 fieldType;
unsigned __int8 code;
WAFieldParm parm;
}
;

union WeaponAttachment::$8C9EC49DA4939354795B66A02F37984A
{
const char *szInternalName;
const char *name;
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
AttachmentType type;
weapType_t weaponType;
weapClass_t weapClass;
XModel **worldModels;
XModel **viewModels;
XModel **reticleViewModels;
snd_alias_list_t **bounceSounds;
snd_alias_list_t **rollingSounds;
AttChargeInfo *chargeInfo;
AttHybridSettings *hybridSettings;
scr_string_t *stringArray1;
scr_string_t *stringArray2;
unsigned __int16 *waFieldOffsets;
WAField *waFields;
unsigned int waFieldsCount;
char __pad0[14];
bool riotShield;
char __pad1[5];
}
;

struct AnimOverrideEntry
{
unsigned __int16 attachment1;
unsigned __int16 attachment2;
XAnimParts *overrideAnim;
XAnimParts *altmodeAnim;
int animTime;
int altTime;
}
;

struct SoundOverrideEntry
{
unsigned __int16 attachment1;
unsigned __int16 attachment2;
snd_alias_list_t *overrideSound;
snd_alias_list_t *altmodeSound;
}
;

struct FXOverrideEntry
{
unsigned __int16 attachment1;
unsigned __int16 attachment2;
FxEffectDef *overrideFX;
FxEffectDef *altmodeFX;
}
;

struct ReloadStateTimerEntry
{
int attachment;
int reloadAddTime;
int reloadEmptyAddTime;
int reloadStartAddTime;
}
;

struct NoteTrackToSoundEntry
{
int attachment;
scr_string_t *notetrackSoundMapKeys;
scr_string_t *notetrackSoundMapValues;
}
;

struct TracerDef
{
const char *name;
Material *material;
FxEffectDef *effect;
unsigned int drawInterval;
float speed;
float beamLength;
float beamWidth;
float screwRadius;
float screwDist;
float colors[5][4];
}
;

struct LaserDef
{
const char *name;
Material *laserMaterial;
Material *laserLightMaterial;
FxEffectDef *effect;
LaserDef *altLaser;
scr_string_t *value;
char __pad0[104];
}
;

struct TurretHydraulicSettings
{
float minVelocity;
float maxVelocity;
snd_alias_list_t *verticalSound;
snd_alias_list_t *verticalStopSound;
snd_alias_list_t *horizontalSound;
snd_alias_list_t *horizontalStopSound;
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

enum weaponIconRatioType_t : __int32
{
WEAPON_ICON_RATIO_1TO1 = 0x0,
WEAPON_ICON_RATIO_2TO1 = 0x1,
WEAPON_ICON_RATIO_4TO1 = 0x2,
WEAPON_ICON_RATIO_COUNT = 0x3,
}
;

enum WeapStickinessType : __int32
{
WEAPSTICKINESS_NONE = 0x0,
WEAPSTICKINESS_ALL = 0x1,
WEAPSTICKINESS_ALL_ORIENT = 0x2,
WEAPSTICKINESS_GROUND = 0x3,
WEAPSTICKINESS_GROUND_WITH_YAW = 0x4,
WEAPSTICKINESS_KNIFE = 0x5,
WEAPSTICKINESS_COUNT = 0x6,
}
;

enum guidedMissileType_t : __int32
{
MISSILE_GUIDANCE_NONE = 0x0,
MISSILE_GUIDANCE_SIDEWINDER = 0x1,
MISSILE_GUIDANCE_HELLFIRE = 0x2,
MISSILE_GUIDANCE_JAVELIN = 0x3,
MISSILE_GUIDANCE_UNKNOWN0 = 0x4,
MISSILE_GUIDANCE_UNKNOWN1 = 0x5,
MISSILE_GUIDANCE_UNKNOWN2 = 0x6,
MISSILE_GUIDANCE_COUNT = 0x7,
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

enum weapAnimFiles_t : __int32
{
WEAP_ANIM_INVALID = 0xFFFFFFFF,
WEAP_ANIM_ROOT = 0x0,
WEAP_ANIM_IDLE = 0x1,
WEAP_ANIM_EMPTY_IDLE = 0x2,
WEAP_ANIM_FIRE = 0x3,
WEAP_ANIM_HOLD_FIRE = 0x4,
WEAP_ANIM_LASTSHOT = 0x5,
WEAP_ANIM_RECHAMBER = 0x6,
WEAP_ANIM_GRENADE_PRIME = 0x7,
WEAP_ANIM_GRENADE_PRIME_READY_TO_THROW = 0x8,
WEAP_ANIM_MELEE_SWIPE = 0x9,
WEAP_ANIM_MELEE_HIT = 0xA,
WEAP_ANIM_MELEE_FATAL = 0xB,
WEAP_ANIM_MELEE_MISS = 0xC,
WEAP_ANIM_MELEE_VICTIM_CROUCHING_HIT = 0xD,
WEAP_ANIM_MELEE_VICTIM_CROUCHING_FATAL = 0xE,
WEAP_ANIM_MELEE_VICTIM_CROUCHING_MISS = 0xF,
WEAP_ANIM_MELEE_ALT_STANDING = 0x10,
WEAP_ANIM_MELEE_ALT_CROUCHING = 0x11,
WEAP_ANIM_MELEE_ALT_PRONE = 0x12,
WEAP_ANIM_MELEE_ALT_JUMPING = 0x13,
WEAP_ANIM_MELEE_ALT_STANDING_VICTIM_CROUCHING = 0x14,
WEAP_ANIM_MELEE_ALT_CROUCHING_VICTIM_CROUCHING = 0x15,
WEAP_ANIM_MELEE_ALT_PRONE_VICTIM_CROUCHING = 0x16,
WEAP_ANIM_MELEE_ALT_JUMPING_VICTIM_CROUCHING = 0x17,
WEAP_ANIM_RELOAD = 0x18,
WEAP_ANIM_RELOAD_EMPTY = 0x19,
WEAP_ANIM_RELOAD_START = 0x1A,
WEAP_ANIM_RELOAD_END = 0x1B,
WEAP_ANIM_FAST_RELOAD = 0x1C,
WEAP_ANIM_FAST_RELOAD_EMPTY = 0x1D,
WEAP_ANIM_FAST_RELOAD_START = 0x1E,
WEAP_ANIM_FAST_RELOAD_END = 0x1F,
WEAP_ANIM_DUALMAG_RELOAD = 0x20,
WEAP_ANIM_DUALMAG_RELOAD_EMPTY = 0x21,
WEAP_ANIM_SPEED_RELOAD = 0x22,
WEAP_ANIM_RELOAD_FROM_ALT = 0x23,
WEAP_ANIM_RAISE = 0x24,
WEAP_ANIM_FIRST_RAISE = 0x25,
WEAP_ANIM_BREACH_RAISE = 0x26,
WEAP_ANIM_DROP = 0x27,
WEAP_ANIM_ALT_RAISE = 0x28,
WEAP_ANIM_ALT_DROP = 0x29,
WEAP_ANIM_ALT_OVERRIDE = 0x2A,
WEAP_ANIM_QUICK_RAISE = 0x2B,
WEAP_ANIM_QUICK_DROP = 0x2C,
WEAP_ANIM_EMPTY_RAISE = 0x2D,
WEAP_ANIM_EMPTY_DROP = 0x2E,
WEAP_ANIM_HYBRID_SIGHT_ON = 0x2F,
WEAP_ANIM_HYBRID_SIGHT_OFF = 0x30,
WEAP_ANIM_SPRINT_IN = 0x31,
WEAP_ANIM_SPRINT_IN_FROM_SLIDE = 0x32,
WEAP_ANIM_SPRINT_IN_CANCEL = 0x33,
WEAP_ANIM_SPRINT_LOOP = 0x34,
WEAP_ANIM_SPRINT_OUT = 0x35,
WEAP_ANIM_SPRINTANDFIRE_IN = 0x36,
WEAP_ANIM_SPRINTANDFIRE_LOOP = 0x37,
WEAP_ANIM_SPRINTANDFIRE_OUT = 0x38,
WEAP_ANIM_STUNNED_START = 0x39,
WEAP_ANIM_STUNNED_LOOP = 0x3A,
WEAP_ANIM_STUNNED_END = 0x3B,
WEAP_ANIM_THROWBACK = 0x3C,
WEAP_ANIM_DETONATE = 0x3D,
WEAP_ANIM_NIGHTVISION_WEAR = 0x3E,
WEAP_ANIM_NIGHTVISION_REMOVE = 0x3F,
WEAP_ANIM_ADS_FIRE = 0x40,
WEAP_ANIM_ADS_LASTSHOT = 0x41,
WEAP_ANIM_ADS_RECHAMBER = 0x42,
WEAP_ANIM_BLAST_FRONT = 0x43,
WEAP_ANIM_BLAST_RIGHT = 0x44,
WEAP_ANIM_BLAST_BACK = 0x45,
WEAP_ANIM_BLAST_LEFT = 0x46,
WEAP_ANIM_SLIDE_IN = 0x47,
WEAP_ANIM_SLIDE_LOOP = 0x48,
WEAP_ANIM_SLIDE_OUT_TO_SPRINT = 0x49,
WEAP_ANIM_SLIDE_OUT = 0x4A,
WEAP_ANIM_SLIDE_AND_FIRE_IN = 0x4B,
WEAP_ANIM_SLIDE_AND_FIRE_LOOP = 0x4C,
WEAP_ANIM_SLIDE_AND_FIRE_OUT = 0x4D,
WEAP_ANIM_HIGH_JUMP_IN = 0x4E,
WEAP_ANIM_HIGH_JUMP_DROP_IN = 0x4F,
WEAP_ANIM_HIGH_JUMP_DROP_LOOP = 0x50,
WEAP_ANIM_HIGH_JUMP_DROP_LAND = 0x51,
WEAP_ANIM_DODGE_GROUND_BACK = 0x52,
WEAP_ANIM_DODGE_GROUND_LEFT = 0x53,
WEAP_ANIM_DODGE_GROUND_RIGHT = 0x54,
WEAP_ANIM_DODGE_AIR_FORWARD = 0x55,
WEAP_ANIM_DODGE_AIR_BACK = 0x56,
WEAP_ANIM_DODGE_AIR_LEFT = 0x57,
WEAP_ANIM_DODGE_AIR_RIGHT = 0x58,
WEAP_ANIM_LAND_DIP = 0x59,
WEAP_ANIM_RECOIL_SETTLE = 0x5A,
WEAP_ANIM_SWIM_LOOP = 0x5B,
WEAP_ANIM_MANTLE_UP_64 = 0x5C,
WEAP_ANIM_MANTLE_UP_56 = 0x5D,
WEAP_ANIM_MANTLE_UP_48 = 0x5E,
WEAP_ANIM_MANTLE_UP_40 = 0x5F,
WEAP_ANIM_MANTLE_UP_32 = 0x60,
WEAP_ANIM_MANTLE_UP_24 = 0x61,
WEAP_ANIM_MANTLE_UP_16 = 0x62,
WEAP_ANIM_MANTLE_OVER_64 = 0x63,
WEAP_ANIM_MANTLE_OVER_56 = 0x64,
WEAP_ANIM_MANTLE_OVER_48 = 0x65,
WEAP_ANIM_MANTLE_OVER_40 = 0x66,
WEAP_ANIM_MANTLE_OVER_32 = 0x67,
WEAP_ANIM_MANTLE_OVER_24 = 0x68,
WEAP_ANIM_MANTLE_OVER_16 = 0x69,
WEAP_ANIM_GOLIATH_ENTRY = 0x6A,
WEAP_ANIM_OFFHAND_SWITCH = 0x6B,
WEAP_ANIM_HEAT_COOLDOWN_IN = 0x6C,
WEAP_ANIM_HEAT_COOLDOWN_OUT = 0x6D,
WEAP_ANIM_OVERHEAT_OUT = 0x6E,
WEAP_ANIM_SCRIPTED = 0x6F,
WEAP_ANIM_INSPECTION = 0x70,
WEAP_ANIM_RELOAD_MULTIPLE_1 = 0x71,
WEAP_ANIM_RELOAD_MULTIPLE_2 = 0x72,
WEAP_ANIM_RELOAD_MULTIPLE_3 = 0x73,
WEAP_ANIM_RELOAD_MULTIPLE_4 = 0x74,
WEAP_ANIM_RELOAD_MULTIPLE_5 = 0x75,
WEAP_ANIM_RELOAD_MULTIPLE_6 = 0x76,
WEAP_ANIM_RELOAD_MULTIPLE_7 = 0x77,
WEAP_ANIM_RELOAD_MULTIPLE_8 = 0x78,
WEAP_ANIM_RELOAD_MULTIPLE_FAST_1 = 0x79,
WEAP_ANIM_RELOAD_MULTIPLE_FAST_2 = 0x7A,
WEAP_ANIM_RELOAD_MULTIPLE_FAST_3 = 0x7B,
WEAP_ANIM_RELOAD_MULTIPLE_FAST_4 = 0x7C,
WEAP_ANIM_RELOAD_MULTIPLE_FAST_5 = 0x7D,
WEAP_ANIM_RELOAD_MULTIPLE_FAST_6 = 0x7E,
WEAP_ANIM_RELOAD_MULTIPLE_FAST_7 = 0x7F,
WEAP_ANIM_RELOAD_MULTIPLE_FAST_8 = 0x80,
WEAP_ANIM_ADS_UP = 0x81,
WEAP_ANIM_ADS_DOWN = 0x82,
WEAP_ANIM_RECOIL = 0x83,
WEAP_ALT_ANIM_ADJUST = 0x84,
WEAP_ANIM_ADDITIVE_ADS_ROOT = 0x85,
WEAP_ANIM_ADDITIVE_ADS_UP = 0x86,
WEAP_ANIM_ADDITIVE_HYBRID_SIGHT_UP_ROOT = 0x87,
WEAP_ANIM_ADDITIVE_HYBRID_SIGHT_UP = 0x88,
WEAP_ANIM_ADDITIVE_DRAG_LEFT_ROOT = 0x89,
WEAP_ANIM_ADDITIVE_DRAG_LEFT = 0x8A,
WEAP_ANIM_ADDITIVE_DRAG_RIGHT_ROOT = 0x8B,
WEAP_ANIM_ADDITIVE_DRAG_RIGHT = 0x8C,
WEAP_ANIM_ADDITIVE_DRAG_UP_ROOT = 0x8D,
WEAP_ANIM_ADDITIVE_DRAG_UP = 0x8E,
WEAP_ANIM_ADDITIVE_DRAG_DOWN_ROOT = 0x8F,
WEAP_ANIM_ADDITIVE_DRAG_DOWN = 0x90,
WEAP_ANIM_ADDITIVE_SWIM_FORWARD_ROOT = 0x91,
WEAP_ANIM_ADDITIVE_SWIM_FORWARD = 0x92,
WEAP_ANIM_ADDITIVE_SWIM_BACKWARD_ROOT = 0x93,
WEAP_ANIM_ADDITIVE_SWIM_BACKWARD = 0x94,
WEAP_ANIM_ADDITIVE_JUMP_ROOT = 0x95,
WEAP_ANIM_ADDITIVE_JUMP = 0x96,
WEAP_ANIM_ADDITIVE_JUMP_BOOST = 0x97,
WEAP_ANIM_ADDITIVE_JUMP_LAND_ROOT = 0x98,
WEAP_ANIM_ADDITIVE_JUMP_LAND = 0x99,
WEAP_ANIM_ADDITIVE_JUMP_LAND_HEAVY = 0x9A,
WEAP_ANIM_ADDITIVE_WALK_ROOT = 0x9B,
WEAP_ANIM_ADDITIVE_WALK = 0x9C,
WEAP_ANIM_ADDITIVE_HEAT_COOLDOWN_LOOP_ROOT = 0x9D,
WEAP_ANIM_ADDITIVE_HEAT_COOLDOWN_LOOP = 0x9E,
WEAP_ANIM_ADDITIVE_OVERHEAT_IN_ROOT = 0x9F,
WEAP_ANIM_ADDITIVE_OVERHEAT_IN = 0xA0,
WEAP_ANIM_ADDITIVE_OVERHEAT_LOOP_ROOT = 0xA1,
WEAP_ANIM_ADDITIVE_OVERHEAT_LOOP = 0xA2,
WEAP_ANIM_ADDITIVE_CRAWL_IN_ROOT = 0xA3,
WEAP_ANIM_ADDITIVE_CRAWL_IN = 0xA4,
WEAP_ANIM_ADDITIVE_CRAWL_LOOP_ROOT = 0xA5,
WEAP_ANIM_ADDITIVE_CRAWL_LOOP = 0xA6,
WEAP_ANIM_ADDITIVE_CRAWL_LOOP_BACK_ROOT = 0xA7,
WEAP_ANIM_ADDITIVE_CRAWL_LOOP_BACK = 0xA8,
WEAP_ANIM_ADDITIVE_CRAWL_LOOP_LEFT_ROOT = 0xA9,
WEAP_ANIM_ADDITIVE_CRAWL_LOOP_LEFT = 0xAA,
WEAP_ANIM_ADDITIVE_CRAWL_LOOP_RIGHT_ROOT = 0xAB,
WEAP_ANIM_ADDITIVE_CRAWL_LOOP_RIGHT = 0xAC,
WEAP_ANIM_ADDITIVE_PRONE_DROP_ROOT = 0xAD,
WEAP_ANIM_ADDITIVE_PRONE_DROP = 0xAE,
WEAP_ANIM_ADDITIVE_EMPTY_ROOT = 0xAF,
WEAP_ANIM_ADDITIVE_EMPTY = 0xB0,
WEAP_ANIM_ADDITIVE_MANTLE_ROOT = 0xB1,
WEAP_ANIM_ADDITIVE_MANTLE = 0xB2,
WEAP_ANIM_ADDITIVE_LOW_MANTLE_ROOT = 0xB3,
WEAP_ANIM_ADDITIVE_MANTLE_UP_24 = 0xB4,
WEAP_ANIM_ADDITIVE_MANTLE_UP_16 = 0xB5,
WEAP_ANIM_ADDITIVE_MANTLE_OVER_24 = 0xB6,
WEAP_ANIM_ADDITIVE_MANTLE_OVER_16 = 0xB7,
WEAP_ANIM_ADDITIVE_SHOT_CHARGE_IN_ROOT = 0xB8,
WEAP_ANIM_ADDITIVE_SHOT_CHARGE_IN = 0xB9,
WEAP_ANIM_ADDITIVE_SHOT_CHARGE_LOOP_ROOT = 0xBA,
WEAP_ANIM_ADDITIVE_SHOT_CHARGE_LOOP = 0xBB,
WEAP_ANIM_ADDITIVE_SCRIPTED_ROOT = 0xBC,
WEAP_ANIM_ADDITIVE_SCRIPTED = 0xBD,
NUM_WEAP_ANIMS = 0xBE,
}
;

struct StateTimers
{
int fireDelay;
int meleeDelay;
int meleeChargeDelay;
int detonateDelay;
int fireTime;
int rechamberTime;
int rechamberTimeOneHanded;
int rechamberBoltTime;
int holdFireTime;
int grenadePrimeReadyToThrowTime;
int detonateTime;
int meleeTime;
int meleeChargeTime;
int reloadTime;
int reloadShowRocketTime;
int reloadEmptyTime;
int reloadAddTime;
int reloadEmptyAddTime;
int reloadStartTime;
int reloadStartAddTime;
int reloadEndTime;
int reloadTimeDualWield;
int reloadAddTimeDualWield;
int reloadEmptyDualMag;
int reloadEmptyAddTimeDualMag;
int u25;
int u26;
int dropTime;
int raiseTime;
int altDropTime;
int altRaiseTime;
int quickDropTime;
int quickRaiseTime;
int firstRaiseTime;
int breachRaiseTime;
int emptyRaiseTime;
int emptyDropTime;
int sprintInTime;
int sprintLoopTime;
int sprintOutTime;
int stunnedTimeBegin;
int stunnedTimeLoop;
int stunnedTimeEnd;
int nightVisionWearTime;
int nightVisionWearTimeFadeOutEnd;
int nightVisionWearTimePowerUp;
int nightVisionRemoveTime;
int nightVisionRemoveTimePowerDown;
int nightVisionRemoveTimeFadeInStart;
int aiFuseTime;
int fuseTime;
int missileTime;
int primeTime;
bool bHoldFullPrime;
int blastFrontTime;
int blastRightTime;
int blastBackTime;
int blastLeftTime;
int u58;
int u59;
int u60;
int u61;
int u62;
int u63;
int u64;
int u65;
int u66;
int u67;
int u68;
int offhandSwitchTime;
int u70;
int u71;
int u72;
int u73;
int u74;
}
;

union WeaponDef::$8C9EC49DA4939354795B66A02F37984A
{
const char *szInternalName;
const char *name;
}
;

struct WeaponDef
{
union
{
const char *szInternalName;
const char *name;
};
const char *szDisplayName;
const char *szOverlayName;
XModel **gunModel;
XModel *handModel;
XModel *unknownModel;
XModel **reticleViewModels;
const char *szModeName;
XAnimParts **szXAnimsRightHanded;
XAnimParts **szXAnimsLeftHanded;
scr_string_t *hideTags;
WeaponAttachment **attachments;
XAnimParts **szXAnims;
AnimOverrideEntry *animOverrides;
SoundOverrideEntry *soundOverrides;
FXOverrideEntry *fxOverrides;
ReloadStateTimerEntry *reloadOverrides;
NoteTrackToSoundEntry *notetrackOverrides;
scr_string_t *notetrackSoundMapKeys;
scr_string_t *notetrackSoundMapValues;
scr_string_t *notetrackRumbleMapKeys;
scr_string_t *notetrackRumbleMapValues;
scr_string_t *notetrackFXMapKeys;
FxEffectDef **notetrackFXMapValues;
scr_string_t *notetrackFXMapTagValues;
scr_string_t *notetrackUnknownKeys;
char *notetrackUnknown;
scr_string_t *notetrackUnknownValues;
const char *szAltWeaponName;
FxEffectDef *viewFlashEffect;
FxEffectDef *viewBodyFlashEffect;
FxEffectDef *worldFlashEffect;
FxEffectDef *viewFlashADSEffect;
FxEffectDef *viewBodyFlashADSEffect;
FxEffectDef *effect06;
FxEffectDef *effect07;
FxEffectDef *effect08;
FxEffectDef *effect09;
FxEffectDef *effect10;
FxEffectDef *effect11;
FxEffectDef *effect12;
snd_alias_list_t *pickupSound;
snd_alias_list_t *pickupSoundPlayer;
snd_alias_list_t *ammoPickupSound;
snd_alias_list_t *ammoPickupSoundPlayer;
snd_alias_list_t *projectileSound;
snd_alias_list_t *pullbackSound;
snd_alias_list_t *pullbackSoundPlayer;
snd_alias_list_t *pullbackSoundQuick;
snd_alias_list_t *pullbackSoundQuickPlayer;
snd_alias_list_t *fireSound;
snd_alias_list_t *fireSoundPlayer;
snd_alias_list_t *fireSoundPlayerAkimbo;
snd_alias_list_t *sound13;
snd_alias_list_t *sound14;
snd_alias_list_t *sound15;
snd_alias_list_t *sound16;
snd_alias_list_t *fireLoopSound;
snd_alias_list_t *fireLoopSoundPlayer;
snd_alias_list_t *sound19;
snd_alias_list_t *sound20;
snd_alias_list_t *sound21;
snd_alias_list_t *sound22;
snd_alias_list_t *fireLoopEndPointSound;
snd_alias_list_t *fireLoopEndPointSoundPlayer;
snd_alias_list_t *fireStopSound;
snd_alias_list_t *fireStopSoundPlayer;
snd_alias_list_t *sound27;
snd_alias_list_t *sound28;
snd_alias_list_t *sound29;
snd_alias_list_t *sound30;
snd_alias_list_t *fireLastShotSound;
snd_alias_list_t *fireLastShotSoundPlayer;
snd_alias_list_t *fireFirstSound;
snd_alias_list_t *fireFirstSoundPlayer;
snd_alias_list_t *fireLastSound;
snd_alias_list_t *fireLastSoundPlayer;
snd_alias_list_t *emptyFireSound;
snd_alias_list_t *emptyFireSoundPlayer;
snd_alias_list_t *sound39;
snd_alias_list_t *meleeSwipeSound;
snd_alias_list_t *meleeSwipeSoundPlayer;
snd_alias_list_t *meleeHitSound;
snd_alias_list_t *meleeHitSoundPlayer;
snd_alias_list_t *meleeMissSound;
snd_alias_list_t *meleeMissSoundPlayer;
snd_alias_list_t *rechamberSound;
snd_alias_list_t *rechamberSoundPlayer;
snd_alias_list_t *reloadSound;
snd_alias_list_t *reloadSoundPlayer;
snd_alias_list_t *reloadEmptySound;
snd_alias_list_t *reloadEmptySoundPlayer;
snd_alias_list_t *reloadStartSound;
snd_alias_list_t *reloadStartSoundPlayer;
snd_alias_list_t *reloadEndSound;
snd_alias_list_t *reloadEndSoundPlayer;
snd_alias_list_t *detonateSound;
snd_alias_list_t *detonateSoundPlayer;
snd_alias_list_t *nightVisionWearSound;
snd_alias_list_t *nightVisionWearSoundPlayer;
snd_alias_list_t *nightVisionRemoveSound;
snd_alias_list_t *nightVisionRemoveSoundPlayer;
snd_alias_list_t *raiseSound;
snd_alias_list_t *raiseSoundPlayer;
snd_alias_list_t *sound64;
snd_alias_list_t *sound65;
snd_alias_list_t *sound66;
snd_alias_list_t *sound67;
snd_alias_list_t *putawaySound;
snd_alias_list_t *putawaySoundPlayer;
snd_alias_list_t *sound70;
snd_alias_list_t *sound71;
snd_alias_list_t *adsEnterSoundPlayer;
snd_alias_list_t *adsLeaveSoundPlayer;
snd_alias_list_t *adsCrosshairEnemySound;
snd_alias_list_t **bounceSound;
snd_alias_list_t **rollingSound;
FxEffectDef *viewShellEjectEffect;
FxEffectDef *worldShellEjectEffect;
FxEffectDef *viewLastShotEjectEffect;
FxEffectDef *worldLastShotEjectEffect;
FxEffectDef *viewMagEjectEffect;
Material *reticleCenter;
Material *reticleSide;
XModel **worldModel;
XModel *worldClipModel;
XModel *rocketModel;
XModel *knifeModel;
XModel *worldKnifeModel;
Material *hudIcon;
Material *pickupIcon;
Material *unknownIcon2;
Material *unknownIcon3;
Material *unknownIcon4;
Material *ammoCounterIcon;
const char *szAmmoName;
const char *szClipName;
const char *szSharedAmmoCapName;
PhysCollmap *physCollmap;
PhysPreset *physPreset;
const char *szUseHintString;
const char *dropHintString;
float *locationDamageMultipliers;
const char *fireRumble;
const char *fireMedRumble;
const char *fireHighRumble;
const char *meleeImpactRumble;
TracerDef *tracer1;
TracerDef *tracer2;
LaserDef *laser;
snd_alias_list_t *turretOverheatSound;
FxEffectDef *turretOverheatEffect;
const char *turretBarrelSpinRumble;
snd_alias_list_t *turretBarrelSpinMaxSnd;
snd_alias_list_t *turretBarrelSpinUpSnd[4];
snd_alias_list_t *turretBarrelSpinDownSnd[4];
snd_alias_list_t *missileConeSoundAlias;
snd_alias_list_t *missileConeSoundAliasAtBase;
XModel *stowOffsetModel;
TurretHydraulicSettings *turretHydraulicSettings;
int altWeapon;
unsigned __int8 numWeaponAttachments;
unsigned __int8 numAnimOverrides;
unsigned __int8 numSoundOverrides;
unsigned __int8 numFXOverrides;
unsigned __int8 numReloadStateTimerOverrides;
unsigned __int8 numNotetrackOverrides;
playerAnimType_t playerAnimType;
char __pad000[1];
weapType_t weapType;
weapClass_t weapClass;
PenetrateType penetrateType;
float penetrateDepth;
ImpactType impactType;
weapInventoryType_t inventoryType;
weapFireType_t fireType;
weapFireBarrels_t fireBarrels;
weapAdsFireMode_t adsFireMode;
float burstFireCooldown;
weapGreebleType_t greebleType;
weapAutoReloadType_t autoReloadType;
WeaponSlotRestriction slotRestriction;
OffhandClass offhandClass;
weapStance_t stance;
int reticleCenterSize;
int reticleSideSize;
int reticleMinOfs;
activeReticleType_t activeReticleType;
float standMove[3];
float standRot[3];
float strafeMove[3];
float strafeRot[3];
float duckedOfs[3];
float duckedMove[3];
float duckedRot[3];
float proneOfs[3];
float proneMove[3];
float proneRot[3];
float posMoveRate;
float posProneMoveRate;
float standMoveMinSpeed;
float duckedMoveMinSpeed;
float proneMoveMinSpeed;
float posRotRate;
float posProneRotRate;
weaponIconRatioType_t hudIconRatio;
weaponIconRatioType_t pickupIconRatio;
weaponIconRatioType_t ammoCounterIconRatio;
int ammoCounterClip;
int startAmmo;
int ammoIndex;
char ammoIndexUnknown;
char __pad002[3];
int clipIndex;
char clipIndexUnknown;
char __pad003[3];
int maxAmmo;
int minAmmoReq;
int clipSize;
int shotCount;
int sharedAmmoCapIndex;
int sharedAmmoCap;
int damage;
int playerDamage;
int meleeDamage;
int damageType;
StateTimers stateTimers;
StateTimers akimboStateTimers;
float autoAimRange;
float aimAssistRange;
float aimAssistRangeAds;
float aimPadding;
float enemyCrosshairRange;
float moveSpeedScale;
float adsMoveSpeedScale;
float sprintDurationScale;
float adsZoomFov;
float adsZoomInFrac;
float adsZoomOutFrac;
float adsSceneBlur;
float fU_007;
float xU_008;
ADSOverlay overlay;
WeapOverlayInteface_t overlayInterface;
float adsBobFactor;
float adsViewBobMult;
float hipSpreadStandMin;
float hipSpreadDuckedMin;
float hipSpreadProneMin;
float hipSpreadStandMax;
float xU_009;
float xU_010;
float hipSpreadDuckedMax;
float hipSpreadProneMax;
float hipSpreadDecayRate;
float hipSpreadFireAdd;
float hipSpreadTurnAdd;
float hipSpreadMoveAdd;
float hipSpreadDuckedDecay;
float hipSpreadProneDecay;
float hipReticleSidePos;
float adsIdleAmount;
float hipIdleAmount;
float adsIdleSpeed;
float hipIdleSpeed;
float idleCrouchFactor;
float idleProneFactor;
float gunMaxPitch;
float gunMaxYaw;
float adsIdleLerpStartTime;
float adsIdleLerpTime;
int adsTransInTime;
int xU_011;
int adsTransOutTime;
float xU_012;
float swayMaxAngle;
float swayLerpSpeed;
float swayPitchScale;
float swayYawScale;
float swayVertScale;
float swayHorizScale;
float swayShellShockScale;
float adsSwayMaxAngle;
float adsSwayLerpSpeed;
float adsSwayPitchScale;
float adsSwayYawScale;
float adsSwayHorizScale;
float adsSwayVertScale;
float adsViewErrorMin;
float adsViewErrorMax;
float adsFireAnimFrac;
float dualWieldViewModelOffset;
float scopeDriftDelay;
float scopeDriftLerpInTime;
float scopeDriftSteadyTime;
float scopeDriftLerpOutTime;
float scopeDriftSteadyFactor;
float scopeDriftUnsteadyFactor;
float bobVerticalFactor;
float bobHorizontalFactor;
float bobViewVerticalFactor;
float bobViewHorizontalFactor;
float stationaryZoomFov;
float stationaryZoomDelay;
float stationaryZoomLerpInTime;
float stationaryZoomLerpOutTime;
float adsDofStart;
float adsDofEnd;
float xU_020;
Material *killIcon;
Material *dpadIcon;
Material *hudProximityWarningIcon;
weaponIconRatioType_t killIconRatio;
weaponIconRatioType_t dpadIconRatio;
int fireAnimLength;
int fireAnimLengthAkimbo;
int inspectAnimTime;
int reloadAmmoAdd;
int reloadStartAdd;
int ammoDropStockMin;
int ammoDropStockMax;
int ammoDropClipPercentMin;
int ammoDropClipPercentMax;
int explosionRadius;
int explosionRadiusMin;
int explosionInnerDamage;
int explosionOuterDamage;
float damageConeAngle;
float bulletExplDmgMult;
float bulletExplRadiusMult;
int projectileSpeed;
int projectileSpeedUp;
int projectileSpeedForward;
int projectileActivateDist;
float projLifetime;
float timeToAccelerate;
float projectileCurvature;
float xU_021;
const char *projectileName;
XModel *projectileModel;
FxEffectDef *projExplosionEffect;
FxEffectDef *projDudEffect;
snd_alias_list_t *projExplosionSound;
snd_alias_list_t *projDudSound;
weapProjExposion_t projExplosion;
WeapStickinessType stickiness;
float lowAmmoWarningThreshold;
float ricochetChance;
int riotShieldHealth;
float riotShieldDamageMult;
float *parallelBounce;
float *perpendicularBounce;
FxEffectDef *projTrailEffect;
FxEffectDef *projBeaconEffect;
float projectileColor[3];
guidedMissileType_t guidedMissileType;
float maxSteeringAccel;
int projIgnitionDelay;
FxEffectDef *projIgnitionEffect;
snd_alias_list_t *projIgnitionSound;
float adsAimPitch;
float adsCrosshairInFrac;
float adsCrosshairOutFrac;
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
float adsViewKickPitchMin;
float adsViewKickPitchMax;
float adsViewKickYawMin;
float adsViewKickYawMax;
float adsViewKickMagMin;
float adsViewKickCenterSpeed;
float adsViewScatterMin;
float adsViewScatterMax;
float adsSpread;
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
float hipViewKickPitchMin;
float hipViewKickPitchMax;
float hipViewKickYawMin;
float hipViewKickYawMax;
float hipViewKickMagMin;
float hipViewKickCenterSpeed;
float hipViewScatterMin;
float hipViewScatterMax;
float xU_043;
int adsReloadTransTime;
float fightDist;
float maxDist;
const char *accuracyGraphName[2];
vec2_t *accuracyGraphKnots[2];
vec2_t *originalAccuracyGraphKnots[2];
__int16 accuracyGraphKnotCount[2];
int positionReloadTransTime;
float leftArc;
float rightArc;
float topArc;
float bottomArc;
float accuracy;
float aiSpread;
float playerSpread;
float minTurnSpeed[2];
float maxTurnSpeed[2];
float pitchConvergenceTime;
float yawConvergenceTime;
float suppressTime;
float maxRange;
float animHorRotateInc;
float playerPositionDist;
unsigned int useHintStringIndex;
unsigned int dropHintStringIndex;
float horizViewJitter;
float vertViewJitter;
float scanSpeed;
float scanAccel;
int scanPauseTime;
const char *szScript;
int minDamage;
int midDamage;
int minPlayerDamage;
int midPlayerDamage;
float maxDamageRange;
float minDamageRange;
int iU_045;
int iU_046;
int iU_047;
int iU_048;
float fU_049;
float fU_050;
float destabilizationRateTime;
float destabilizationCurvatureMax;
int destabilizeDistance;
float turretADSTime;
float turretFov;
float turretFovADS;
float turretScopeZoomRate;
float turretScopeZoomMin;
float turretScopeZoomMax;
float xU_056;
float xU_057;
float xU_058;
float xU_059;
float turretBarrelSpinSpeed;
float turretBarrelSpinUpTime;
float turretBarrelSpinDownTime;
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
float aim_automelee_lerp;
float aim_automelee_range;
float aim_automelee_region_height;
float aim_automelee_region_width;
float player_meleeHeight;
float player_meleeRange;
float player_meleeWidth;
float signatureFireTime;
int signatureNumBullets;
weapFireTimeInterpolation_t fireTimeInterpolationType;
int xU_075;
int ammoUsedPerShot;
int xU_076;
int xU_077;
int xU_078;
int iU_079;
int iU_080;
scr_string_t stowTag;
bool bU_081;
bool unknownReticleBooleanValue1;
bool unknownReticleBooleanValue2;
bool turretADSEnabled;
bool knifeAttachTagLeft;
bool knifeAlwaysAttached;
bool meleeOverrideValues;
bool bU_083;
bool bU_084;
bool sharedAmmo;
bool lockonSupported;
bool requireLockonToFire;
bool isAirburstWeapon;
bool bigExplosion;
bool noAdsWhenMagEmpty;
bool avoidDropCleanup;
bool inheritsPerks;
bool crosshairColorChange;
bool rifleBullet;
bool armorPiercing;
bool boltAction;
bool aimDownSight;
bool canHoldBreath;
bool meleeOnly;
bool bU_085;
bool bU_086;
bool canVariableZoom;
bool rechamberWhileAds;
bool bulletExplosiveDamage;
bool cookOffHold;
bool reticleSpin45;
bool reticleSideEnabled;
bool clipOnly;
bool noAmmoPickup;
bool disableSwitchToWhenEmpty;
bool bU_088;
bool hasMotionTracker;
bool bU_089;
bool noDualWield;
bool flipKillIcon;
bool actionSlotShowAmmo;
bool noPartialReload;
bool segmentedReload;
bool multipleReload;
bool blocksProne;
bool silenced;
bool isRollingGrenade;
bool projExplosionEffectForceNormalUp;
bool projExplosionEffectInheritParentDirection;
bool projImpactExplode;
bool projTrajectoryEvents;
bool projWhizByEnabled;
bool stickToPlayers;
bool stickToVehicles;
bool stickToTurrets;
bool thrownSideways;
bool hasDetonatorEmptyThrow;
bool hasDetonatorDoubleTap;
bool disableFiring;
bool timedDetonation;
bool bU_090;
bool bU_091;
bool rotate;
bool holdButtonToThrow;
bool freezeMovementWhenFiring;
bool thermalScope;
bool thermalToggle;
bool outlineEnemies;
bool altModeSameWeapon;
bool turretBarrelSpinEnabled;
bool missileConeSoundEnabled;
bool missileConeSoundPitchshiftEnabled;
bool missileConeSoundCrossfadeEnabled;
bool offhandHoldIsCancelable;
bool doNotAllowAttachmentsToOverrideSpread;
bool useFastReloadAnims;
bool dualMagReloadSupported;
bool reloadStopsAlt;
bool bU_092;
bool alwaysShatterGlassOnImpact;
bool oldWeapon;
bool bU_093;
bool bU_094;
bool xU_095;
bool hasCounterSilencer;
bool xU_097;
bool xU_098;
bool disableVariableAutosimRate;
bool bU_100;
bool bU_101;
bool bU_102;
bool bU_103;
bool bU_104;
bool cloakedWeapon;
bool adsHideWeapon;
bool adsHideHands;
bool bU_108;
bool adsBlurSceneEnabled;
bool usesSniperScope;
bool bU_111;
bool bU_112;
bool bU_113;
bool bU_114;
bool bU_115;
float adsDofPhysicalFStop;
float adsDofPhysicalFocusDistance;
float autosimSpeedScalar;
float explosionReactiveMotionParts[5];
char __pad_unknown[12];
}
;

enum operationEnum : __int32
{
OP_NOOP = 0x0,
OP_RIGHTPAREN = 0x1,
OP_MULTIPLY = 0x2,
OP_DIVIDE = 0x3,
OP_MODULUS = 0x4,
OP_ADD = 0x5,
OP_SUBTRACT = 0x6,
OP_NOT = 0x7,
OP_LESSTHAN = 0x8,
OP_LESSTHANEQUALTO = 0x9,
OP_GREATERTHAN = 0xA,
OP_GREATERTHANEQUALTO = 0xB,
OP_EQUALS = 0xC,
OP_NOTEQUAL = 0xD,
OP_AND = 0xE,
OP_OR = 0xF,
OP_LEFTPAREN = 0x10,
OP_COMMA = 0x11,
OP_BITWISEAND = 0x12,
OP_BITWISEOR = 0x13,
OP_BITWISENOT = 0x14,
OP_BITSHIFTLEFT = 0x15,
OP_BITSHIFTRIGHT = 0x16,
OP_FIRSTFUNCTIONCALL = 0x17,
OP_STATICDVARINT = 0x17,
OP_STATICDVARBOOL = 0x18,
OP_STATICDVARFLOAT = 0x19,
OP_STATICDVARSTRING = 0x1A,
OP_TOINT = 0x1B,
OP_TOSTRING = 0x1C,
OP_TOFLOAT = 0x1D,
LAST_COMMONLY_CALLED_FUNCTION = 0x1D,
OP_SIN = 0x1E,
OP_COS = 0x1F,
OP_MIN = 0x20,
OP_MAX = 0x21,
OP_MILLISECONDS = 0x22,
OP_LOCALCLIENTUIMILLISECONDS = 0x23,
OP_DVARINT = 0x24,
OP_DVARBOOL = 0x25,
OP_DVARFLOAT = 0x26,
OP_DVARSTRING = 0x27,
OP_UIACTIVE = 0x28,
OP_FLASHBANGED = 0x29,
OP_USINGVEHICLE = 0x2A,
OP_MISSILECAM = 0x2B,
OP_SCOPED = 0x2C,
OP_SCOPEDTHERMAL = 0x2D,
OP_SCOREBOARDVISIBLE = 0x2E,
OP_INKILLCAM = 0x2F,
OP_INKILLCAM_NPC = 0x30,
OP_PLAYERFIELD = 0x31,
OP_GET_PLAYER_PERK = 0x32,
OP_SELECTINGLOCATION = 0x33,
OP_SELECTINGDIRECTION = 0x34,
OP_TEAMFIELD = 0x35,
OP_OTHERTEAMFIELD = 0x36,
OP_MARINESFIELD = 0x37,
OP_OPFORFIELD = 0x38,
OP_MENUISOPEN = 0x39,
OP_WRITINGDATA = 0x3A,
OP_INLOBBY = 0x3B,
OP_INGAMELOBBY = 0x3C,
OP_INPRIVATEPARTY = 0x3D,
OP_PRIVATEPARTYHOST = 0x3E,
OP_PRIVATEPARTYHOSTINLOBBY = 0x3F,
OP_ALONEINPARTY = 0x40,
OP_ADSJAVELIN = 0x41,
OP_WEAPLOCKBLINK = 0x42,
OP_WEAPATTACKTOP = 0x43,
OP_WEAPATTACKDIRECT = 0x44,
OP_WEAPLOCKING = 0x45,
OP_WEAPLOCKED = 0x46,
OP_WEAPLOCKTOOCLOSE = 0x47,
OP_WEAPLOCKSCREENPOSX = 0x48,
OP_WEAPLOCKSCREENPOSY = 0x49,
OP_SECONDSASTIME = 0x4A,
OP_TABLELOOKUP = 0x4B,
OP_TABLELOOKUPBYROW = 0x4C,
OP_TABLEGETROWNUM = 0x4D,
OP_LOCALIZESTRING = 0x4E,
OP_LOCALVARINT = 0x4F,
OP_LOCALVARBOOL = 0x50,
OP_LOCALVARFLOAT = 0x51,
OP_LOCALVARSTRING = 0x52,
OP_TIMELEFT = 0x53,
OP_SECONDSASCOUNTDOWN = 0x54,
OP_GAMEMSGWNDACTIVE = 0x55,
OP_GAMETYPENAME = 0x56,
OP_GAMETYPE = 0x57,
OP_GAMETYPEDESCRIPTION = 0x58,
OP_SCORE = 0x59,
OP_FOLLOWING = 0x5A,
OP_SPECTATINGFREE = 0x5B,
OP_KEYBINDING = 0x5C,
OP_ACTIONSLOTUSABLE = 0x5D,
OP_HUDFADE = 0x5E,
OP_MAXPLAYERS = 0x5F,
OP_ACCEPTINGINVITE = 0x60,
OP_ISINTERMISSION = 0x61,
OP_GAMEHOST = 0x62,
OP_PARTYHASMISSINGMAPPACK = 0x63,
OP_PARTYMISSINGMAPPACKERROR = 0x64,
OP_ANYNEWMAPPACKS = 0x65,
OP_AMISELECTED = 0x66,
OP_PARTYSTATUSSTRING = 0x67,
OP_ATTACHED_CONTROLLER_COUNT = 0x68,
OP_IS_SPLIT_SCREEN_ONLINE_POSSIBLE = 0x69,
OP_SPLITSCREENPLAYERCOUNT = 0x6A,
OP_GETPLAYERDATA = 0x6B,
OP_GETPLAYERDATASPLITSCREEN = 0x6C,
OP_GET_MATCHRULES_DATA = 0x6D,
OP_GET_SAVED_MATCHRULES_METADATA = 0x6E,
OP_LEVEL_FOR_EXPERIENCE_MP = 0x6F,
OP_LEVEL_FOR_EXPERIENCE_SO = 0x70,
OP_IS_ITEM_UNLOCKED = 0x71,
OP_IS_ITEM_UNLOCKEDSPLITSCREEN = 0x72,
OP_IS_CARDICON_UNLOCKED = 0x73,
OP_IS_CARDTITLE_UNLOCKED = 0x74,
OP_IS_CARDICON_NEW = 0x75,
OP_IS_CARDTITLE_NEW = 0x76,
OP_IS_CARDICON_UNLOCKED_SPLITSCREEN = 0x77,
OP_IS_CARDTITLE_UNLOCKED_SPLITSCREEN = 0x78,
OP_IS_CARDICON_NEW_SPLITSCREEN = 0x79,
OP_IS_CARDTITLE_NEW_SPLITSCREEN = 0x7A,
OP_IS_PROFILEITEM_UNLOCKED = 0x7B,
OP_IS_PROFILEITEM_UNLOCKED_SPLITSCREEN = 0x7C,
OP_DEBUG_PRINT = 0x7D,
OP_GETPLAYERDATA_ANYBOOLTRUE = 0x7E,
OP_GETPROFILE_ANYBOOLTRUE = 0x7F,
OP_WEAPONNAME = 0x80,
OP_ISRELOADING = 0x81,
OP_SAVE_GAME_AVAILABLE = 0x82,
OP_UNLOCKED_ITEM_COUNT = 0x83,
OP_UNLOCKED_ITEM_COUNT_SPLITSCREEN = 0x84,
OP_UNLOCKED_ITEM = 0x85,
OP_UNLOCKED_ITEM_SPLITSCREEN = 0x86,
OP_RADAR_IS_JAMMED = 0x87,
OP_RADAR_JAM_INTENSITY = 0x88,
OP_RADAR_IS_ENABLED = 0x89,
OP_EMP_JAMMED = 0x8A,
OP_PLAYERADS = 0x8B,
OP_WEAPON_HEAT_ACTIVE = 0x8C,
OP_WEAPON_HEAT_VALUE = 0x8D,
OP_WEAPON_HEAT_OVERHEATED = 0x8E,
OP_SPLASH_TEXT = 0x8F,
OP_SPLASH_DESCRIPTION = 0x90,
OP_SPLASH_MATERIAL = 0x91,
OP_SPLASH_HAS_ICON = 0x92,
OP_SPLASH_ROWNUM = 0x93,
OP_GETFOCUSED_NAME = 0x94,
OP_GETFOCUSED_X = 0x95,
OP_GETFOCUSED_Y = 0x96,
OP_GETFOCUSED_W = 0x97,
OP_GETFOCUSED_H = 0x98,
OP_GETITEMDEF_X = 0x99,
OP_GETITEMDEF_Y = 0x9A,
OP_GETITEMDEF_W = 0x9B,
OP_GETITEMDEF_H = 0x9C,
OP_PLAYLISTFIELD = 0x9D,
OP_SCOREBOARD_EXTERNALMUTE_NOTICE = 0x9E,
OP_GET_MAP_NAME = 0x9F,
OP_GET_MAP_IMAGE = 0xA0,
OP_GET_MAP_CUSTOM = 0xA1,
OP_GET_MIGRATION_STATUS = 0xA2,
OP_GET_PLAYERCARD_INFO = 0xA3,
OP_IS_OFFLINE_PROFILE_SELECTED = 0xA4,
OP_COOP_PLAYERFIELD = 0xA5,
OP_IS_COOP = 0xA6,
OP_GETPARTYSTATUS = 0xA7,
OP_GETSEARCHPARAMS = 0xA8,
OP_GETTIMEPLAYED = 0xA9,
OP_IS_SELECTED_PLAYER_FRIEND = 0xAA,
OP_GETCHARBYINDEX = 0xAB,
OP_GETPLAYERPROFILEDATA = 0xAC,
OP_GETPLAYERPROFILEDATASPLITSCREEN = 0xAD,
OP_IS_PROFILE_SIGNED_IN = 0xAE,
OP_GET_WAIT_POPUP_STATUS = 0xAF,
OP_GETNATTYPE = 0xB0,
OP_GETLOCALIZEDNATTYPE = 0xB1,
OP_GET_ADJUSTED_SAFEAREA_HORIZONTAL = 0xB2,
OP_GET_ADJUSTED_SAFEAREA_VERTICAL = 0xB3,
OP_CONNECTION_INFO = 0xB4,
OP_OFFLINE_PROFILE_CAN_SAVE = 0xB5,
OP_USER_WITHOUT_OFFLINE_PROFILE = 0xB6,
OP_ALL_SPLITSCREEN_PROFILES_CAN_SAVE = 0xB7,
OP_ALL_SPLITSCREEN_PROFILES_ARE_SIGNED_IN = 0xB8,
OP_DO_WE_HAVE_MAP_PACK = 0xB9,
OP_MAY_INVITE_PLAYER_TO_PARTY = 0xBA,
OP_GETPATCHNOTES = 0xBB,
OP_GETGAMEINFOS = 0xBC,
OP_COOP_READY = 0xBD,
OP_VOTE_CAST = 0xBE,
OP_VOTE_PASSED = 0xBF,
OP_GET_MAP_VOTE_MAP_IMAGE = 0xC0,
OP_GET_MAP_VOTE_MAP_NAME = 0xC1,
OP_GET_MAP_VOTE_GAME_TYPE_NAME = 0xC2,
OP_IS_FRIEND_INVITABLE = 0xC3,
OP_IS_FRIEND_JOINABLE = 0xC4,
OP_GET_SORTED_CHALLENGE_INDEX = 0xC5,
OP_GET_SORTED_CHALLENGE_NAME = 0xC6,
OP_GET_SORTED_CHALLENGE_COUNT = 0xC7,
OP_GET_FILTER_CHALLENGE_COUNT = 0xC8,
OP_GET_FILTER_CHALLENGE_LOCKED_COUNT = 0xC9,
OP_GET_FILTER_CHALLENGE_COMPLETE_COUNT = 0xCA,
OP_IS_SORTED_CHALLENGE_TIERED = 0xCB,
OP_GET_CHALLENGE_FILTER_CACHE_COUNT = 0xCC,
OP_GET_CHALLENGE_FILTER_CACHE_COMPLETE_COUNT = 0xCD,
OP_IS_COOP_SEARCHING = 0xCE,
OP_IS_COOP_PUBLIC = 0xCF,
OP_GET_COOP_DISPLAYABLE_GROUP_NUM = 0xD0,
OP_COOP_HAS_REQUIRED_ONLINE_FILES = 0xD1,
OP_GET_TEXTWIDTH = 0xD2,
OP_GET_TEXTHEIGHT = 0xD3,
OP_DEVELOPER = 0xD4,
OP_IS_USING_AIRBURST_WEAPON = 0xD5,
OP_GET_AIRBURST_METERS = 0xD6,
OP_GET_CROSSHAIR_TRACE_METERS = 0xD7,
OP_GET_FACEBOOK_STATUS_TEXT = 0xD8,
OP_IS_FACEBOOK_LOGGED_IN = 0xD9,
OP_IS_FACEBOOK_CHECKING = 0xDA,
OP_IS_FACEBOOK_ALLOWED = 0xDB,
OP_GETPRIVATELOBBYSTATUS = 0xDC,
OP_INCLUDEDINMAPROTATION = 0xDD,
OP_SELECT = 0xDE,
OP_IS_DEMO_PLAYING = 0xDF,
OP_GET_USER_GROUP_TEXT = 0xE0,
OP_GET_USER_GROUP_COMMON_INTEREST_TOTAL = 0xE1,
OP_IS_DEMO_FOLLOW_CAMERA = 0xE2,
OP_IS_DEMO_FREE_CAMERA = 0xE3,
OP_IS_DEMO_CAPTURING_SCREENSHOT = 0xE4,
OP_PARTY_HOST_WAITING_ON_MEMBERS = 0xE5,
OP_POPUP_PARTY_MEMBER_AWAY = 0xE6,
OP_SELECTED_PARTY_MEMBER_AWAY = 0xE7,
OP_GAMETIME = 0xE8,
OP_GAMEENDTIME = 0xE9,
OP_HAS_FOCUS = 0xEA,
OP_MENU_HAS_FOCUS = 0xEB,
OP_GET_DEMO_SEGMENT_COUNT = 0xEC,
OP_GET_DEMO_SEGMENT_INFORMATION = 0xED,
OP_IS_CLIP_MODIFIED = 0xEE,
OP_IS_USING_RECIPE = 0xEF,
OP_IS_GUEST = 0xF0,
OP_GET_FACEBOOK_HELP_TEXT = 0xF1,
OP_IS_ELITE_CLAN_ALLOWED = 0xF2,
OP_IS_ENTITLEMENTS_ALLOWED = 0xF3,
OP_IS_USERGROUPS_ALLOWED = 0xF4,
OP_IS_WAITING_FOR_ONLINE_SERVICES = 0xF5,
OP_GET_TEXTWIDTHMODCASE = 0xF6,
OP_GET_SAVE_SCREEN_TITLE = 0xF7,
OP_GET_SAVE_SCREEN_DESCRIPTION = 0xF8,
OP_GET_ONLINEVAULT_SELECTEDITEM_DATA = 0xF9,
OP_ONLINEVAULT_IS_RESTRICTED = 0xFA,
OP_IS_CONTENTSERVER_TASK_IN_PROGRESS = 0xFB,
OP_IS_CONTENTSERVER_GET_TASK_PROGRESS = 0xFC,
OP_GET_RECENTGAMES_SELECTEDITEM_DATA = 0xFD,
OP_GAMETYPENAME_ABBREVIATED = 0xFE,
OP_GET_MAP_VOTE_GAME_TYPE_NAME_ABBREVIATED = 0xFF,
OP_IS_USER_SIGNED_IN_TO_LIVE = 0x100,
OP_USER_CAN_PLAY_ONLINE = 0x101,
OP_GET_FEEDER_DATA = 0x102,
OP_PARTY_CLIENTS_UP_TO_DATE = 0x103,
OP_TRUNCATETEXTWITHELLIPSIS = 0x104,
OP_UI_STARTED = 0x105,
OP_CAN_RENDER_CLIP = 0x106,
OP_GET_PREVIEW_MAP_CUSTOM = 0x107,
OP_GET_DLC_MAPS_AVAILABLE_COUNT = 0x108,
OP_IS_USER_SIGNED_IN = 0x109,
OP_USINGINTERMISSIONTIMER = 0x10A,
OP_ISUSINGCUSTOMMAPROTATION = 0x10B,
OP_MENU_IS_TOPMOST = 0x10C,
OP_FACEBOOK_IS_PLATFORM_FRIEND = 0x10D,
OP_ELITE_CLAN_IS_PLATFORM_FRIEND = 0x10E,
OP_ELITE_CLAN_IS_ME = 0x10F,
OP_ELITE_CLAN_IS_LEADER = 0x110,
OP_IS_USER_SIGNED_IN_FOR_VAULT = 0x111,
OP_GET_USING_MATCHRULES_DATA = 0x112,
OP_CAN_USER_ACCESS_ONLINEVAULT = 0x113,
OP_FRIEND_GET_GAMERTAG = 0x114,
OP_RECENTPLAYER_GET_GAMERTAG = 0x115,
OP_LIVEPARTY_GET_GAMERTAG = 0x116,
OP_FACEBOOK_GET_GAMERTAG = 0x117,
OP_ELITECLAN_GET_GAMERTAG = 0x118,
OP_LIVEPARTY_IS_ME = 0x119,
OP_LIVEPARTY_IS_LOCAL = 0x11A,
OP_DOUBLECLICK_WAS_RIGHT_CLICK = 0x11B,
OP_IS_DEMO_CLIP_RECORDING = 0x11C,
OP_GET_INDEX_FROM_STRING = 0x11D,
OP_GET_STRING_WIHTOUT_INDEX = 0x11E,
OP_ELITECLAN_GET_NAME = 0x11F,
OP_ELITECLAN_GET_HELP = 0x120,
OP_ELITECLAN_GET_MOTD = 0x121,
OP_ELITECLAN_IS_MEMBER = 0x122,
OP_ELITECLAN_IS_EMBLEM_OK = 0x123,
OP_FACEBOOKFRIENDS_SHOW_NEXT = 0x124,
OP_FACEBOOKFRIENDS_SHOW_PREV = 0x125,
OP_GET_ONLINEVAULT_FRIEND_GAMERTAG = 0x126,
OP_GET_OBJECTIVE_LIST_HEIGHT = 0x127,
OP_IS_CLIENT_DEMO_ENABLED = 0x128,
OP_IS_USER_SIGNED_IN_TO_DEMONWARE = 0x129,
OP_IS_CUSTOM_CLASS_RESTRICTED = 0x12A,
OP_IS_WEAPON_RESTRICTED = 0x12B,
OP_ANY_SPLITSCREEN_PROFILES_ARE_SIGNED_IN = 0x12C,
OP_IS_GUEST_SPLITSCREEN = 0x12D,
OP_IS_ITEM_UNLOCKED_BY_CLIENT = 0x12E,
OP_IS_ANY_USER_SIGNED_IN_TO_LIVE = 0x12F,
OP_SPLITSCREENACTIVEGAMEPADCOUNT = 0x130,
OP_SHOW_FRIEND_PLAYERCARD = 0x131,
OP_GET_FRIEND_PLAYERCARD_PRESENCE = 0x132,
OP_SHOW_RECENT_PLAYERS_GROUP_ICON = 0x133,
OP_GET_WRAPPED_TEXT_HEIGHT = 0x134,
OP_CAN_SAVE = 0x135,
OP_GET_GAME_INVITES_COUNT = 0x136,
OP_IS_SPLITSCREEN_GAMER_LIVE_ENABLED = 0x137,
OP_SO_COOP_SHOW_COMMON_GROUP_ICON = 0x138,
OP_STRIP_COLORS_FROM_STRING = 0x139,
OP_CAN_USER_ACCESS_THEATRE = 0x13A,
OP_IS_CHALLENGE_PERIODIC = 0x13B,
OP_GET_CHALLENGE_DATA = 0x13C,
OP_IS_ELITE_APP_PRESENT = 0x13D,
OP_ELITE_CLAN_SELECTED_IS_ME = 0x13E,
OP_ENOUGH_STORAGE_SPACE_FOR_CLIENT_DEMO = 0x13F,
OP_IS_USER_SIGNED_IN_FOR_COMMERCE = 0x140,
OP_GET_FACEBOOK_MENU_TEXT = 0x141,
OP_GET_FACEBOOK_IS_POSTING = 0x142,
OP_MEET_PLAYER_IS_PLATFORM_FRIEND = 0x143,
OP_IS_SELECTED_PLAYER_GUEST = 0x144,
OP_GET_SPLITSCREEN_CONTROLLER_CLIENT_NUM = 0x145,
OP_IS_CLIENT_DEMO_ENABLED_SPLITSCREEN = 0x146,
OP_ITEM_CAN_TAKE_FOCUS = 0x147,
OP_GET_TIME_SINCE_LAST_DOUBLECLICK = 0x148,
OP_IS_SERVER_LIST_REFRESHING = 0x149,
OP_IS_RECIPE_NAME_VALID = 0x14A,
OP_RECIPE_EXISTS = 0x14B,
OP_GET_FACEBOOK_OPTIONS_HELP_TEXT = 0x14C,
OP_DO_WE_HAVE_ALL_MAP_PACKS = 0x14D,
OP_IS_THERE_NEW_ELITE_ITEMS = 0x14E,
OP_IS_PAYING_SUBSCRIBER = 0x14F,
OP_LOCALUSER_HASMISSINGMAPPACK = 0x150,
OP_LOCALUSER_MISSINGMAPPACKERROR = 0x151,
OP_GET_FIRST_SPECOPS_DLC_MAP = 0x152,
OP_LOCALUSER_MISSINGMAPNAME = 0x153,
OP_SHOW_STORE_NEW = 0x154,
OP_USINGBOTS = 0x155,
OP_BOTSALLOWED = 0x156,
OP_ISCOLORBLIND = 0x157,
OP_BOTSCONNECTTYPE = 0x158,
OP_BOTSDIFFICULTY = 0x159,
OP_SPECTATINGTHIRD = 0x15A,
OP_GETPLAYERNAME = 0x15B,
OP_SQUAD_SHOW_REPORT = 0x15C,
OP_GETRANKEDPLAYERDATASPLITSCREEN = 0x15D,
OP_GETPRIVATEPLAYERDATASPLITSCREEN = 0x15E,
OP_GETCOOPPLAYERDATASPLITSCREEN = 0x15F,
OP_GETCOMMONPLAYERDATASPLITSCREEN = 0x160,
OP_GETOMNVAR = 0x161,
NUM_OPERATORS = 0x162,
}
;

struct ExpressionString
{
const char *string;
}
;

union operandInternalDataUnion
{
int intVal;
float floatVal;
ExpressionString stringVal;
Statement_s *function;
}
;

enum expDataType : __int32
{
VAL_INT = 0x0,
VAL_FLOAT = 0x1,
VAL_STRING = 0x2,
NUM_INTERNAL_DATATYPES = 0x3,
VAL_FUNCTION = 0x3,
NUM_DATATYPES = 0x4,
}
;

struct Operand
{
operandInternalDataUnion internals;
expDataType dataType;
}
;

union entryInternalData
{
operationEnum op;
Operand operand;
}
;

struct expressionEntry
{
entryInternalData data;
int type;
}
;

struct UIFunctionList
{
int totalFunctions;
Statement_s **functions;
}
;

struct StaticDvar
{
void *dvar;
char *dvarName;
}
;

struct StaticDvarList
{
int numStaticDvars;
StaticDvar **staticDvars;
}
;

struct StringList
{
int totalStrings;
const char **strings;
}
;

struct ExpressionSupportingData
{
UIFunctionList uifunctions;
StaticDvarList staticDvarList;
StringList uiStrings;
}
;

struct ExpressionPersistentState
{
int flags;
int playerDataKey[4];
int lastExecuteTime[4];
Operand lastResult[4];
}
;

struct ConditionalScript
{
MenuEventHandlerSet *eventHandlerSet;
Statement_s *eventExpression;
}
;

struct SetLocalVarData
{
const char *localVarName;
Statement_s *expression;
}
;

union EventData
{
const char *unconditionalScript;
ConditionalScript *conditionalScript;
MenuEventHandlerSet *elseScript;
SetLocalVarData *setLocalVarData;
}
;

enum EventType : unsigned __int8
{
EVENT_UNCONDITIONAL = 0x0,
EVENT_IF = 0x1,
EVENT_ELSE = 0x2,
EVENT_SET_LOCAL_VAR_BOOL = 0x3,
EVENT_SET_LOCAL_VAR_INT = 0x4,
EVENT_SET_LOCAL_VAR_FLOAT = 0x5,
EVENT_SET_LOCAL_VAR_STRING = 0x6,
EVENT_COUNT = 0x7,
}
;

struct __declspec(align(8)) MenuEventHandler
{
EventData eventData;
EventType eventType;
}
;

struct ItemKeyHandler
{
int key;
MenuEventHandlerSet *action;
ItemKeyHandler *next;
}
;

struct menuTransition
{
int transitionType;
int startTime;
float startVal;
float endVal;
float time;
int endTriggerType;
}
;

struct __declspec(align(8)) menuData_t
{
int fullScreen;
int fadeCycle;
float fadeClamp;
float fadeAmount;
float fadeInAmount;
float blurRadius;
MenuEventHandlerSet *onOpen;
MenuEventHandlerSet *onCloseRequest;
MenuEventHandlerSet *onClose;
MenuEventHandlerSet *onESC;
MenuEventHandlerSet *onFocusDueToClose;
ItemKeyHandler *onKey;
Statement_s *visibleExp;
const char *allowedBinding;
const char *soundName;
float focusColor[4];
Statement_s *rectXExp;
Statement_s *rectYExp;
Statement_s *rectWExp;
Statement_s *rectHExp;
Statement_s *openSoundExp;
Statement_s *closeSoundExp;
Statement_s *soundLoopExp;
int cursorItem[1];
menuTransition scaleTransition[1];
menuTransition alphaTransition[1];
menuTransition xTransition[1];
menuTransition yTransition[1];
ExpressionSupportingData *expressionData;
unsigned __int8 priority;
}
;

struct __declspec(align(4)) rectDef_s
{
float x;
float y;
float w;
float h;
unsigned __int8 horzAlign;
unsigned __int8 vertAlign;
}
;

struct windowDef_t
{
const char *name;
rectDef_s rect;
rectDef_s rectClient;
const char *group;
int style;
int border;
int ownerDraw;
int ownerDrawFlags;
float borderSize;
int staticFlags;
int dynamicFlags[1];
int nextTime;
float foreColor[4];
float backColor[4];
float borderColor[4];
float outlineColor[4];
float disableColor[4];
Material *background;
}
;

struct columnInfo_s
{
int xpos;
int ypos;
int width;
int height;
int maxChars;
int alignment;
}
;

struct listBoxDef_s
{
int startPos[1];
int endPos[1];
int drawPadding;
float elementWidth;
float elementHeight;
int elementStyle;
int numColumns;
columnInfo_s columnInfo[16];
MenuEventHandlerSet *onDoubleClick;
int notselectable;
int noScrollBars;
int usePaging;
float selectBorder[4];
Material *selectIcon;
Statement_s *elementHeightExp;
}
;

struct editFieldDef_s
{
float minVal;
float maxVal;
float stepVal;
float range;
int maxChars;
int maxCharsGotoNext;
int maxPaintChars;
int paintOffset;
}
;

struct multiDef_s
{
const char *dvarList[32];
const char *dvarStr[32];
float dvarValue[32];
int count;
int strDef;
}
;

struct textScrollDef_s
{
int startTime;
}
;

struct newsTickerDef_s
{
int feedId;
int speed;
int spacing;
}
;

union itemDefData_t
{
listBoxDef_s *listBox;
editFieldDef_s *editField;
multiDef_s *multi;
const char *enumDvarName;
newsTickerDef_s *ticker;
textScrollDef_s *scroll;
void *data;
}
;

enum ItemFloatExpressionTarget
{
ITEM_FLOATEXP_TGT_RECT_X = 0x0,
ITEM_FLOATEXP_TGT_RECT_Y = 0x1,
ITEM_FLOATEXP_TGT_RECT_W = 0x2,
ITEM_FLOATEXP_TGT_RECT_H = 0x3,
ITEM_FLOATEXP_TGT_FORECOLOR_R = 0x4,
ITEM_FLOATEXP_TGT_FORECOLOR_G = 0x5,
ITEM_FLOATEXP_TGT_FORECOLOR_B = 0x6,
ITEM_FLOATEXP_TGT_FORECOLOR_RGB = 0x7,
ITEM_FLOATEXP_TGT_FORECOLOR_A = 0x8,
ITEM_FLOATEXP_TGT_GLOWCOLOR_R = 0x9,
ITEM_FLOATEXP_TGT_GLOWCOLOR_G = 0xA,
ITEM_FLOATEXP_TGT_GLOWCOLOR_B = 0xB,
ITEM_FLOATEXP_TGT_GLOWCOLOR_RGB = 0xC,
ITEM_FLOATEXP_TGT_GLOWCOLOR_A = 0xD,
ITEM_FLOATEXP_TGT_BACKCOLOR_R = 0xE,
ITEM_FLOATEXP_TGT_BACKCOLOR_G = 0xF,
ITEM_FLOATEXP_TGT_BACKCOLOR_B = 0x10,
ITEM_FLOATEXP_TGT_BACKCOLOR_RGB = 0x11,
ITEM_FLOATEXP_TGT_BACKCOLOR_A = 0x12,
ITEM_FLOATEXP_TGT__COUNT = 0x13,
}
;

struct ItemFloatExpression
{
ItemFloatExpressionTarget target;
Statement_s *expression;
}
;

struct itemDef_t
{
windowDef_t window;
rectDef_s textRect[1];
int type;
int dataType;
int alignment;
int fontEnum;
int textAlignMode;
float textalignx;
float textaligny;
float textscale;
int textStyle;
int gameMsgWindowIndex;
int gameMsgWindowMode;
const char *text;
int itemFlags;
menuDef_t *parent;
MenuEventHandlerSet *mouseEnterText;
MenuEventHandlerSet *mouseExitText;
MenuEventHandlerSet *mouseEnter;
MenuEventHandlerSet *mouseExit;
MenuEventHandlerSet *action;
MenuEventHandlerSet *accept;
MenuEventHandlerSet *onFocus;
MenuEventHandlerSet *hasFocus;
MenuEventHandlerSet *leaveFocus;
const char *dvar;
const char *dvarTest;
ItemKeyHandler *onKey;
const char *enableDvar;
const char *localVar;
int dvarFlags;
snd_alias_list_t *focusSound;
float special;
int cursorPos[1];
itemDefData_t typeData;
int floatExpressionCount;
ItemFloatExpression *floatExpressions;
Statement_s *visibleExp;
Statement_s *disabledExp;
Statement_s *textExp;
Statement_s *materialExp;
float glowColor[4];
bool decayActive;
int fxBirthTime;
int fxLetterTime;
int fxDecayStartTime;
int fxDecayDuration;
int lastSoundPlayedTime;
Statement_s *textAlignYExp;
}
;

struct MenuList
{
const char *name;
int menuCount;
menuDef_t **menus;
}
;

struct cplane_s
{
float normal[3];
float dist;
unsigned __int8 type;
}
;

enum CSurfaceFlags : unsigned __int32
{
SURF_FLAG_DEFAULT = 0x0,
SURF_FLAG_BARK = 0x100000,
SURF_FLAG_BRICK = 0x200000,
SURF_FLAG_CARPET = 0x300000,
SURF_FLAG_CLOTH = 0x400000,
SURF_FLAG_CONCRETE = 0x500000,
SURF_FLAG_DIRT = 0x600000,
SURF_FLAG_FLESH = 0x700000,
SURF_FLAG_FOLIAGE_DEBRIS = 0x800000,
SURF_FLAG_GLASS = 0x900000,
SURF_FLAG_GRASS = 0xA00000,
SURF_FLAG_GRAVEL = 0xB00000,
SURF_FLAG_ICE = 0xC00000,
SURF_FLAG_METAL_SOLID = 0xD00000,
SURF_FLAG_METAL_GRATE = 0xE00000,
SURF_FLAG_MUD = 0xF00000,
SURF_FLAG_PAPER = 0x1000000,
SURF_FLAG_PLASTER = 0x1100000,
SURF_FLAG_ROCK = 0x1200000,
SURF_FLAG_SAND = 0x1300000,
SURF_FLAG_SNOW = 0x1400000,
SURF_FLAG_WATER_WAIST = 0x1500000,
SURF_FLAG_WOOD_SOLID = 0x1600000,
SURF_FLAG_ASPHALT = 0x1700000,
SURF_FLAG_CERAMIC = 0x1800000,
SURF_FLAG_PLASTIC_SOLID = 0x1900000,
SURF_FLAG_RUBBER = 0x1A00000,
SURF_FLAG_FRUIT = 0x1B00000,
SURF_FLAG_PAINTEDMETAL = 0x1C00000,
SURF_FLAG_RIOTSHIELD = 0x1D00000,
SURF_FLAG_SLUSH = 0x1E00000,
SURF_FLAG_ASPHALT_WET = 0x1F00000,
SURF_FLAG_ASPHALT_DEBRIS = 0x2000000,
SURF_FLAG_CONCRETE_WET = 0x2100000,
SURF_FLAG_CONCRETE_DEBRIS = 0x2200000,
SURF_FLAG_FOLIAGE_VEGETATION = 0x2300000,
SURF_FLAG_FOLIAGE_LEAVES = 0x2400000,
SURF_FLAG_GRASS_TALL = 0x2500000,
SURF_FLAG_METAL_HOLLOW = 0x2600000,
SURF_FLAG_METAL_VEHICLE = 0x2700000,
SURF_FLAG_METAL_THIN = 0x2800000,
SURF_FLAG_METAL_WET = 0x2900000,
SURF_FLAG_METAL_DEBRIS = 0x2A00000,
SURF_FLAG_PLASTIC_HOLLOW = 0x2B00000,
SURF_FLAG_PLASTIC_TARP = 0x2C00000,
SURF_FLAG_ROCK_WET = 0x2D00000,
SURF_FLAG_ROCK_DEBRIS = 0x2E00000,
SURF_FLAG_WATER_ANKLE = 0x2F00000,
SURF_FLAG_WATER_KNEE = 0x3000000,
SURF_FLAG_WATER_HOLLOW = 0x3100000,
SURF_FLAG_WOOD_HOLLOW = 0x3200000,
SURF_FLAG_WOOD_DEBRIS = 0x3300000,
SURF_FLAG_CUSHION = 0x3400000,
SURF_FLAG_CLIPMISSILE = 0x0,
SURF_FLAG_AI_NOSIGHT = 0x0,
SURF_FLAG_CLIPSHOT = 0x0,
SURF_FLAG_PLAYERCLIP = 0x0,
SURF_FLAG_MONSTERCLIP = 0x0,
SURF_FLAG_AICLIPALLOWDEATH = 0x0,
SURF_FLAG_VEHICLECLIP = 0x0,
SURF_FLAG_ITEMCLIP = 0x0,
SURF_FLAG_NODROP = 0x0,
SURF_FLAG_NONSOLID = 0x4000,
SURF_FLAG_NOGRAPPLE = 0x8000,
SURF_FLAG_DETAIL = 0x0,
SURF_FLAG_STRUCTURAL = 0x0,
SURF_FLAG_PORTAL = 0x80000000,
SURF_FLAG_CANSHOOTCLIP = 0x0,
SURF_FLAG_ORIGIN = 0x0,
SURF_FLAG_SKY = 0x4,
SURF_FLAG_NOCASTSHADOW = 0x40000,
SURF_FLAG_PHYSICSGEOM = 0x0,
SURF_FLAG_LIGHTPORTAL = 0x0,
SURF_FLAG_OUTDOORBOUNDS = 0x0,
SURF_FLAG_SLICK = 0x2,
SURF_FLAG_NOIMPACT = 0x10,
SURF_FLAG_NOMARKS = 0x20,
SURF_FLAG_NOPENETRATE = 0x100,
SURF_FLAG_LADDER = 0x8,
SURF_FLAG_NODAMAGE = 0x1,
SURF_FLAG_MANTLEON = 0x4000000,
SURF_FLAG_MANTLEOVER = 0x8000000,
SURF_FLAG_STAIRS = 0x200,
SURF_FLAG_SOFT = 0x1000,
SURF_FLAG_NOSTEPS = 0x2000,
SURF_FLAG_NODRAW = 0x80,
SURF_FLAG_NOLIGHTMAP = 0x400,
SURF_FLAG_NODLIGHT = 0x20000,
SURF_FLAG_TRANSSORT = 0x80000,
}
;

struct ClipMaterial
{
const char *name;
int surfaceFlags;
int contents;
}
;

struct cLeafBrushNodeLeaf_t
{
unsigned __int16 *brushes;
}
;

struct cLeafBrushNodeChildren_t
{
float dist;
float range;
unsigned __int16 childOffset[2];
}
;

union cLeafBrushNodeData_t
{
cLeafBrushNodeLeaf_t leaf;
cLeafBrushNodeChildren_t children;
}
;

struct cLeafBrushNode_s
{
unsigned __int8 axis;
__int16 leafBrushCount;
int contents;
cLeafBrushNodeData_t data;
}
;

typedef unsigned __int16 LeafBrush
;

struct BrushesCollisionTree
{
unsigned int leafbrushNodesCount;
cLeafBrushNode_s *leafbrushNodes;
unsigned int numLeafBrushes;
LeafBrush *leafbrushes;
}
;

union CollisionAabbTreeIndex
{
int firstChildIndex;
int partitionIndex;
}
;

struct CollisionAabbTree
{
float midPoint[3];
unsigned __int16 materialIndex;
unsigned __int16 childCount;
float halfSize[3];
CollisionAabbTreeIndex u;
}
;

struct PatchesCollisionTree
{
int aabbTreeCount;
CollisionAabbTree *aabbTrees;
}
;

struct SModelAabbNode
{
Bounds bounds;
unsigned __int16 firstChild;
unsigned __int16 childCount;
}
;

struct SModelsCollisionTree
{
unsigned __int16 numStaticModels;
unsigned __int16 smodelNodeCount;
SModelAabbNode *smodelNodes;
}
;

struct cbrushside_t
{
unsigned int planeIndex;
unsigned __int16 materialNum;
unsigned __int8 firstAdjacentSideOffset;
unsigned __int8 edgeCount;
}
;

typedef unsigned __int8 cbrushedge_t
;

struct cbrush_t
{
unsigned __int16 numsides;
unsigned __int16 glassPieceIndex;
cbrushside_t *sides;
cbrushedge_t *baseAdjacentSide;
__int16 axialMaterialNum[2][3];
unsigned __int8 firstAdjacentSideOffsets[2][3];
unsigned __int8 edgeCount[2][3];
}
;

struct BrushesCollisionData
{
unsigned int numBrushSides;
cbrushside_t *brushSides;
unsigned int numBrushEdges;
cbrushedge_t *brushEdges;
unsigned int numBrushes;
cbrush_t *brushes;
Bounds *brushBounds;
int *brushContents;
}
;

struct CollisionBorder
{
float distEq[3];
float zBase;
float zSlope;
float start;
float length;
}
;

struct CollisionPartition
{
unsigned __int8 triCount;
unsigned __int8 borderCount;
unsigned __int8 firstVertSegment;
int firstTri;
CollisionBorder *borders;
}
;

struct PatchesCollisionData
{
unsigned int vertCount;
vec3_t *verts;
int triCount;
unsigned __int16 *triIndices;
unsigned __int8 *triEdgeIsWalkable;
int borderCount;
CollisionBorder *borders;
int partitionCount;
CollisionPartition *partitions;
}
;

struct cStaticModel_s
{
XModel *xmodel;
float origin[3];
float invScaledAxis[3][3];
Bounds absBounds;
int lightingHandle;
}
;

struct SModelsCollisionData
{
unsigned int numStaticModels;
cStaticModel_s *staticModelList;
}
;

struct ClipInfo
{
int planeCount;
cplane_s *planes;
unsigned int numMaterials;
ClipMaterial *materials;
BrushesCollisionTree bCollisionTree;
PatchesCollisionTree pCollisionTree;
SModelsCollisionTree sCollisionTree;
BrushesCollisionData bCollisionData;
PatchesCollisionData pCollisionData;
SModelsCollisionData sCollisionData;
}
;

struct cNode_t
{
cplane_s *plane;
__int16 children[2];
}
;

struct cLeaf_t
{
unsigned int firstCollAabbIndex;
unsigned int collAabbCount;
int brushContents;
int terrainContents;
Bounds bounds;
int leafBrushNode;
}
;

struct cmodel_t
{
Bounds bounds;
float radius;
ClipInfo *info;
cLeaf_t leaf;
}
;

struct Stage
{
const char *name;
float origin[3];
unsigned __int16 triggerIndex;
unsigned __int8 sunPrimaryLightIndex;
unsigned int unk;
}
;

enum DynEntityType : __int32
{
DYNENT_TYPE_INVALID = 0x0,
DYNENT_TYPE_CLUTTER = 0x1,
DYNENT_TYPE_DESTRUCT = 0x2,
DYNENT_TYPE_HINGE = 0x3,
DYNENT_TYPE_SCRIPTABLEINST = 0x4,
DYNENT_TYPE_SCRIPTABLEPHYSICS = 0x5,
DYNENT_TYPE_LINKED = 0x6,
DYNENT_TYPE_LINKED_NOSHADOW = 0x7,
DYNENT_TYPE_COUNT = 0x8,
}
;

struct GfxPlacement
{
float quat[4];
float origin[3];
}
;

struct DynEntityHingeDef
{
float axisOrigin[3];
float axisDir[3];
bool isLimited;
float angleMin;
float angleMax;
float momentOfInertia;
float friction;
}
;

struct DynEntityLinkToDef
{
int anchorIndex;
float originOffset[3];
float angleOffset[3];
}
;

struct DynEntityDef
{
DynEntityType type;
GfxPlacement pose;
XModel *baseModel;
unsigned __int16 brushModel;
unsigned __int16 physicsBrushModel;
unsigned __int16 scriptableIndex;
unsigned __int16 health;
FxEffectDef *destroyFx;
snd_alias_list_t *sound;
PhysPreset *physPreset;
DynEntityHingeDef *hinge;
DynEntityLinkToDef *linkTo;
PhysMass mass;
int contents;
float __pad0[2];
}
;

struct DynEntityPose
{
GfxPlacement pose;
float radius;
char __pad0[4];
}
;

struct Hinge
{
float angle;
float quat[4];
float angularVel;
float torqueAccum;
bool active;
float autoDisableTimeLeft;
DynEntityHingeDef *def;
PhysPreset *physPreset;
float centerOfMassRelToAxisOriginAtAngleZero[3];
}
;

struct DynEntityClient
{
__int64 physObjId;
unsigned __int16 flags;
unsigned __int16 lightingHandle;
unsigned __int16 health;
Hinge *hinge;
XModel *activeModel;
int contents;
}
;

struct DynEntityColl
{
unsigned __int16 sector;
unsigned __int16 nextEntInSector;
float linkMins[2];
float linkMaxs[2];
}
;

enum ScriptableEventType : __int32
{
SCRIPTABLE_EVENT_MODEL = 0x0,
SCRIPTABLE_EVENT_FX = 0x1,
SCRIPTABLE_EVENT_STOP_FX = 0x2,
SCRIPTABLE_EVENT_SOUND = 0x3,
SCRIPTABLE_EVENT_ANIMATION = 0x4,
SCRIPTABLE_EVENT_EXPLODE = 0x5,
SCRIPTABLE_EVENT_HEALTHDRAIN = 0x6,
SCRIPTABLE_EVENT_PHYSICSLAUNCH = 0x7,
SCRIPTABLE_EVENT_LIGHTSETTINGS = 0x8,
SCRIPTABLE_EVENT_SUNLIGHTSETTINGS = 0x9,
SCRIPTABLE_EVENT_SHAKE = 0xA,
SCRIPTABLE_EVENT_TRANSLATE = 0xB,
SCRIPTABLE_EVENT_ROTATE = 0xC,
SCRIPTABLE_EVENT_STATECHANGE = 0xD,
SCRIPTABLE_EVENT_COUNT = 0xE,
}
;

struct ScriptableEventModelDef
{
XModel *model;
}
;

struct ScriptableEventFxDef
{
FxEffectDef *handle;
scr_string_t tagName;
unsigned __int16 loopTime;
unsigned __int8 loopTimeStreamIndex;
bool tagUseAngles;
}
;

struct ScriptableEventStopFxDef
{
FxEffectDef *handle;
scr_string_t tagName;
unsigned __int16 loopTime;
unsigned __int8 loopTimeStreamIndex;
bool tagUseAngles;
}
;

struct ScriptableEventSoundDef
{
snd_alias_list_t *alias;
bool looping;
}
;

struct ScriptableEventAnimationDef
{
const char *animName;
bool override;
bool stateful;
unsigned __int8 animEntryIndex;
unsigned __int8 animPlaybackStreamIndex;
unsigned __int16 timeOffsetMin;
unsigned __int16 timeOffsetMax;
unsigned __int16 playbackRateMin;
unsigned __int16 playbackRateMax;
unsigned __int16 blendTime;
}
;

struct ScriptableEventExplodeDef
{
unsigned __int16 forceMin;
unsigned __int16 forceMax;
unsigned __int16 radius;
unsigned __int16 damageMin;
unsigned __int16 damageMax;
bool aiAvoid;
}
;

struct ScriptableEventHealthDef
{
unsigned __int16 amount;
unsigned __int16 interval;
unsigned __int16 badPlaceRadius;
unsigned __int8 streamIndex;
}
;

struct ScriptableEventPhysicsDef
{
XModel *model;
unsigned __int8 launchDirX;
unsigned __int8 launchDirY;
unsigned __int8 launchDirZ;
unsigned __int16 explForceScale;
unsigned __int16 bulletForceScale;
}
;

struct ScriptableEventLightSettingsDef
{
unsigned __int8 color[4];
unsigned __int8 lightIndexConstIndex;
unsigned __int8 transStateStreamIndex;
unsigned __int8 useColor;
unsigned __int8 useStateTransitionTime;
unsigned __int16 intensityScaleMin;
unsigned __int16 intensityScaleMax;
unsigned __int16 radiusScaleMin;
unsigned __int16 radiusScaleMax;
unsigned __int16 transitionTimeMin;
unsigned __int16 transitionTimeMax;
const char *noteworthy;
}
;

struct ScriptableEventSunlightSettingsDef
{
unsigned __int8 color[4];
unsigned __int8 transStateStreamIndex;
unsigned __int8 flags;
unsigned __int16 intensityScaleMin;
unsigned __int16 intensityScaleMax;
unsigned __int16 pitchMin;
unsigned __int16 pitchMax;
unsigned __int16 headingMin;
unsigned __int16 headingMax;
unsigned __int16 transitionTimeMin;
unsigned __int16 transitionTimeMax;
char __pad0[6];
}
;

struct ScriptableEventShakeDef
{
const char *rumbleName;
unsigned __int16 duration;
unsigned __int16 durationFadeUp;
unsigned __int16 durationFadeDown;
unsigned __int16 radius;
unsigned __int16 exponent;
unsigned __int16 scaleEarthquake;
unsigned __int8 scalePitch;
unsigned __int8 scaleYaw;
unsigned __int8 scaleRoll;
unsigned __int8 frequencyPitch;
unsigned __int8 frequencyYaw;
unsigned __int8 frequencyRoll;
unsigned __int8 flags;
}
;

struct ScriptableEventTranslateDef
{
char __pad0[24];
const char *str;
}
;

struct ScriptableEventRotateDef
{
char __pad0[24];
const char *str;
}
;

struct ScriptableEventStateChangeDef
{
unsigned __int8 targetIndex;
unsigned __int8 delayStreamIndex;
unsigned __int16 delayMin;
unsigned __int16 delayMax;
}
;

union ScriptableEventDataUnion
{
ScriptableEventModelDef setModel;
ScriptableEventFxDef playFx;
ScriptableEventStopFxDef stopFx;
ScriptableEventSoundDef playSound;
ScriptableEventAnimationDef playAnim;
ScriptableEventExplodeDef doExplosion;
ScriptableEventHealthDef healthDrain;
ScriptableEventPhysicsDef physicsLaunch;
ScriptableEventLightSettingsDef lightSettings;
ScriptableEventSunlightSettingsDef sunlightSettings;
ScriptableEventShakeDef shake;
ScriptableEventTranslateDef translate;
ScriptableEventRotateDef rotate;
ScriptableEventStateChangeDef stateChange;
}
;

struct ScriptableEventDef
{
ScriptableEventType type;
ScriptableEventDataUnion data;
}
;

struct ScriptableStateDef
{
scr_string_t name;
scr_string_t tagName;
ScriptableEventDef *onEnterEvents;
unsigned __int8 onEnterEventCount;
unsigned __int8 damageFlags;
unsigned __int8 damageParentTransferRate;
unsigned __int8 damageParentReceiveRate;
unsigned __int16 maxHealth;
}
;

struct ScriptablePartDef
{
ScriptableStateDef *states;
scr_string_t name;
unsigned __int8 stateCount;
unsigned __int8 flags;
unsigned __int8 eventStreamTimeRemainIndex;
unsigned __int8 eventStreamNextChangeTimeIndex;
}
;

enum ScriptableNotetrackType : __int32
{
SCRIPTABLE_NT_FX = 0x0,
SCRIPTABLE_NT_SOUND = 0x1,
SCRIPTABLE_NT_COUNT = 0x2,
}
;

struct ScriptableNotetrackFxDef
{
FxEffectDef *handle;
scr_string_t tagName;
bool useAngles;
}
;

struct ScriptableNotetrackSoundDef
{
snd_alias_list_t *alias;
}
;

union ScriptableNotetrackDataUnion
{
ScriptableNotetrackFxDef playFx;
ScriptableNotetrackSoundDef playSound;
}
;

struct ScriptableNotetrackDef
{
scr_string_t name;
ScriptableNotetrackType type;
ScriptableNotetrackDataUnion data;
}
;

enum ScriptableType : __int32
{
SCRIPTABLE_TYPE_GENERAL = 0x0,
SCRIPTABLE_TYPE_CHARACTER = 0x1,
SCRIPTABLE_TYPE_COUNT = 0x2,
}
;

struct ScriptableDef
{
const char *name;
XModel *baseModel;
const char *baseCollisionBrush;
const char *destroyedCollisionBrush;
ScriptablePartDef *parts;
ScriptableNotetrackDef *notetracks;
ScriptableType type;
unsigned __int8 flags;
unsigned __int8 partCount;
unsigned __int8 serverInstancePartCount;
unsigned __int8 serverControlledPartCount;
unsigned __int8 notetrackCount;
unsigned __int8 eventStreamSize;
unsigned __int8 eventConstantsSize;
}
;

struct ScriptableInstanceTargetData
{
char __pad0[68];
}
;

struct ScriptableInstancePartState
{
unsigned __int16 curHealth;
unsigned __int8 lastExecutedStateIndex;
unsigned __int8 stateIndex;
}
;

struct ScriptableInstance
{
ScriptableDef *def;
unsigned __int8 *eventConstantsBuf;
ScriptableInstanceTargetData *targetData;
float origin[3];
float angles[3];
char __pad0[24];
scr_string_t targetname;
unsigned __int16 preBrushModel;
unsigned __int16 postBrushModel;
unsigned __int8 flags;
unsigned __int8 targetDataCount;
char __pad1[6];
XModel *currentModel;
ScriptableInstancePartState *partStates;
unsigned __int8 *eventStreamBuf;
unsigned int currentPartBits[8];
unsigned int damageOwnerEntHandle;
unsigned __int16 updateNextInstance;
unsigned __int16 linkedObject;
}
;

struct ScriptableAnimationEntry
{
const char *animName;
unsigned __int64 runtimeBuf;
}
;

struct ScriptableMapEnts
{
unsigned int instanceStateSize;
unsigned int instanceCount;
unsigned int reservedInstanceCount;
ScriptableInstance *instances;
unsigned int animEntryCount;
ScriptableAnimationEntry *animEntries;
unsigned int replicatedInstanceCount;
}
;

struct sphere_tree_t
{
char __pad0[8];
int unk_count;
char __pad1[4];
unsigned int *unk;
char __pad2[8];
}
;

struct sphere_tree_obj_t
{
char __pad0[20];
}
;

struct sphere_tree_data_t
{
int sphereTreeCount;
sphere_tree_t *sphereTree;
int sphereTreeObjCount;
sphere_tree_obj_t *sphereTreeObj;
}
;

struct grapple_magnet_t
{
char __pad0[40];
}
;

struct grapple_data_t
{
sphere_tree_data_t sphereTreeData;
grapple_magnet_t *magnet;
unsigned int magnetCount;
char __pad0[4];
}
;

struct clipMap_t
{
const char *name;
int isInUse;
ClipInfo info;
ClipInfo *pInfo;
unsigned int numNodes;
cNode_t *nodes;
unsigned int numLeafs;
cLeaf_t *leafs;
unsigned int numSubModels;
cmodel_t *cmodels;
MapEnts *mapEnts;
Stage *stages;
unsigned __int8 stageCount;
MapTriggers stageTrigger;
unsigned __int16 dynEntCount[2];
DynEntityDef *dynEntDefList[2];
DynEntityPose *dynEntPoseList[2];
DynEntityClient *dynEntClientList[2];
DynEntityColl *dynEntCollList[2];
unsigned int dynEntAnchorCount;
scr_string_t *dynEntAnchorNames;
ScriptableMapEnts scriptableMapEnts;
grapple_data_t grappleData;
unsigned int checksum;
char __pad0[60];
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

struct ComPrimaryLight
{
GfxLightType type;
unsigned __int8 canUseShadowMap;
unsigned __int8 needsDynamicShadows;
unsigned __int8 exponent;
unsigned __int8 isVolumetric;
char __pad0[3];
float color[3];
float dir[3];
float up[3];
float origin[3];
float fadeOffset[2];
float bulbRadius;
float bulbLength[3];
float radius;
float cosHalfFovOuter;
float cosHalfFovInner;
float cosHalfFovExpanded;
float rotationLimit;
float translationLimit;
float cucRotationOffsetRad;
float cucRotationSpeedRad;
float cucScrollVector[2];
float cucScaleVector[2];
float cucTransVector[2];
const char *defName;
}
;

struct ComPrimaryLightEnv
{
unsigned __int16 primaryLightIndices[4];
unsigned __int8 numIndices;
}
;

struct ComWorld
{
const char *name;
int isInUse;
unsigned int primaryLightCount;
ComPrimaryLight *primaryLights;
unsigned int primaryLightEnvCount;
ComPrimaryLightEnv *primaryLightEnvs;
}
;

struct G_GlassPiece
{
unsigned __int16 damageTaken;
unsigned __int16 collapseTime;
int lastStateChangeTime;
unsigned __int8 impactDir;
unsigned __int8 impactPos[2];
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
unsigned __int8 pad[108];
}
;

struct GlassWorld
{
const char *name;
G_GlassData *g_glassData;
}
;

struct FxGlassDef
{
float halfThickness;
float texVecs[2][2];
GfxColor color;
Material *material;
Material *materialShattered;
PhysPreset *physPreset;
FxEffectDef *pieceBreakEffect;
FxEffectDef *shatterEffect;
FxEffectDef *shatterSmallEffect;
FxEffectDef *crackDecalEffect;
snd_alias_list_t *damagedSound;
snd_alias_list_t *destroyedSound;
snd_alias_list_t *destroyedQuietSound;
float unk[2];
int numCrackRings;
bool isOpaque;
}
;

struct FxSpatialFrame
{
float quat[4];
float origin[3];
}
;

struct $03A8A7B39FA20F64B5AB79125E07CD62
{
FxSpatialFrame frame;
float radius;
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
__int64 physObjId;
__int64 physJointId;
float vel[3];
float avel[3];
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
float texCoordOrigin[2];
unsigned int supportMask;
float areaX2;
unsigned __int16 lightingIndex;
unsigned __int8 defIndex;
unsigned __int8 vertCount;
unsigned __int8 fanDataCount;
unsigned __int8 pad[1];
}
;

struct FxGlassSystem
{
int time;
int prevTime;
unsigned int defCount;
unsigned int pieceLimit;
unsigned int pieceWordCount;
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
float (*linkOrg)[3];
float *halfThickness;
unsigned __int16 *lightingHandles;
FxGlassGeometryData *initGeoData;
bool needToCompactData;
unsigned __int8 initCount;
float effectChanceAccum;
int lastPieceDeletionTime;
unsigned int initPieceCount;
FxGlassInitPieceState *initPieceStates;
}
;

struct FxWorld
{
const char *name;
FxGlassSystem glassSys;
}
;

struct GfxSky
{
int skySurfCount;
int *skyStartSurfs;
GfxImage *skyImage;
unsigned __int8 skySamplerState;
Bounds bounds;
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

struct GfxCellTreeCount
{
int aabbTreeCount;
}
;

struct GfxAabbTree
{
Bounds bounds;
int childrenOffset;
unsigned __int16 childCount;
unsigned __int16 smodelIndexCount;
unsigned __int16 *smodelIndexes;
int startSurfIndex;
unsigned __int16 surfaceCount;
unsigned __int16 pad;
}
;

struct GfxCellTree
{
GfxAabbTree *aabbTree;
}
;

struct GfxPortal
{
GfxPortalWritable writable;
DpvsPlane plane;
float (*vertices)[3];
unsigned __int16 cellIndex;
unsigned __int16 closeDistance;
unsigned __int8 vertexCount;
float hullAxis[2][3];
}
;

struct GfxPortalWritable
{
bool isQueued;
bool isAncestor;
unsigned __int8 recursionDepth;
unsigned __int8 hullPointCount;
float (*hullPoints)[2];
GfxPortal *queuedParent;
}
;

struct DpvsPlane
{
float coeffs[4];
}
;

struct GfxCell
{
Bounds bounds;
__int16 portalCount;
unsigned __int8 reflectionProbeCount;
unsigned __int8 reflectionProbeReferenceCount;
GfxPortal *portals;
unsigned __int8 *reflectionProbes;
unsigned __int8 *reflectionProbeReferences;
}
;

struct GfxPortalGroupInfo
{
char __pad0[4];
}
;

struct GfxPortalGroup
{
const char *group;
GfxPortalGroupInfo *info;
char __pad0[4];
int infoCount;
}
;

struct GfxReflectionProbeVolume
{
unsigned __int16 *data;
unsigned int count;
}
;

struct GfxReflectionProbe
{
float origin[3];
GfxReflectionProbeVolume *probeVolumes;
unsigned int probeVolumeCount;
}
;

struct GfxReflectionProbeReferenceOrigin
{
float origin[3];
}
;

struct GfxReflectionProbeReference
{
unsigned __int8 index;
}
;

struct GfxRawTexture
{
char __pad0[32];
}
;

struct GfxLightmapArray
{
GfxImage *primary;
GfxImage *secondary;
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
void *worldVb;
void *worldVbView;
}
;

struct GfxWorldVertexLayerData
{
unsigned __int8 *data;
void *layerVb;
void *layerVbView;
}
;

struct GfxDisplacementParms
{
char __pad0[16];
}
;

struct GfxWorldDraw
{
unsigned int reflectionProbeCount;
GfxImage **reflectionProbes;
GfxReflectionProbe *reflectionProbeOrigins;
GfxRawTexture *reflectionProbeTextures;
unsigned int reflectionProbeReferenceCount;
GfxReflectionProbeReferenceOrigin *reflectionProbeReferenceOrigins;
GfxReflectionProbeReference *reflectionProbeReferences;
int lightmapCount;
GfxLightmapArray *lightmaps;
GfxRawTexture *lightmapPrimaryTextures;
GfxRawTexture *lightmapSecondaryTextures;
GfxImage *lightmapOverridePrimary;
GfxImage *lightmapOverrideSecondary;
int u1[2];
int u2[2];
int u3;
unsigned int trisType;
unsigned int vertexCount;
GfxWorldVertexData vd;
unsigned int vertexLayerDataSize;
GfxWorldVertexLayerData vld;
unsigned int indexCount;
unsigned __int16 *indices;
void *indexBuffer;
void *indexBufferView;
int displacementParmsCount;
GfxDisplacementParms *displacementParms;
void *displacementParmsBuffer;
void *displacementParmsBufferView;
}
;

struct GfxLightGridEntry
{
unsigned int colorsIndex;
unsigned __int16 primaryLightEnvIndex;
unsigned __int8 unused;
unsigned __int8 needsTrace;
}
;

struct GfxLightGridColors
{
unsigned __int8 colorVec6[56][6];
}
;

struct GfxLightGridColorsHDR
{
unsigned __int8 colorVec6[56][6];
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

struct GfxLightGrid
{
bool hasLightRegions;
bool useSkyForLowZ;
unsigned int lastSunPrimaryLightIndex;
unsigned __int16 mins[3];
unsigned __int16 maxs[3];
unsigned int rowAxis;
unsigned int colAxis;
unsigned __int16 *rowDataStart;
unsigned int rawRowDataSize;
unsigned __int8 *rawRowData;
unsigned int entryCount;
GfxLightGridEntry *entries;
unsigned int colorCount;
GfxLightGridColors *colors;
char __pad0[20];
unsigned int missingGridColorIndex;
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
GfxLightGridTree tree[3];
}
;

struct GfxBrushModelWritable
{
Bounds bounds;
vec3_t origin;
vec4_t quat;
int mdaoVolumeProcessed;
}
;

struct GfxBrushModel
{
GfxBrushModelWritable writable;
Bounds bounds;
float radius;
unsigned int startSurfIndex;
unsigned __int16 surfaceCount;
int mdaoVolumeIndex;
}
;

struct MaterialMemory
{
Material *material;
int memory;
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

struct XModelDrawInfo
{
unsigned __int8 hasGfxEntIndex;
unsigned __int8 lod;
unsigned __int16 surfId;
}
;

struct GfxSceneDynModel
{
XModelDrawInfo info;
unsigned __int16 dynEntId;
}
;

struct BModelDrawInfo
{
unsigned __int16 surfId;
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
float mins[3];
float maxs[3];
float lightingOrigin[3];
}
;

struct srfTriangles_t
{
unsigned int vertexLayerData;
unsigned int firstVertex;
float maxEdgeLength;
int unk;
unsigned __int16 vertexCount;
unsigned __int16 triCount;
unsigned int baseIndex;
}
;

struct GfxSurfaceLightingAndFlagsFields
{
unsigned __int8 lightmapIndex;
unsigned __int8 reflectionProbeIndex;
unsigned __int16 primaryLightEnvIndex;
unsigned __int8 flags;
unsigned __int8 unused[3];
}
;

union GfxSurfaceLightingAndFlags
{
GfxSurfaceLightingAndFlagsFields fields;
unsigned __int64 packed;
}
;

struct GfxSurface
{
srfTriangles_t tris;
Material *material;
GfxSurfaceLightingAndFlags laf;
}
;

struct GfxSurfaceBounds
{
Bounds bounds;
unsigned __int16 mipRadius;
unsigned __int8 invHighMipRadius[5];
char __pad0[4];
char flags;
}
;

struct GfxPackedPlacement
{
float origin[3];
float axis[3][3];
float scale;
}
;

enum StaticModelFlag : __int16
{
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
unsigned __int16 cullDist;
unsigned __int16 flags;
unsigned __int16 lightingHandle;
unsigned __int16 staticModelId;
unsigned __int16 primaryLightEnvIndex;
unsigned __int16 unk0;
char unk1;
unsigned __int8 reflectionProbeIndex;
unsigned __int8 firstMtlSkinIndex;
unsigned __int8 sunShadowFlags;
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
void *lightingValuesVb;
int numLightingValues;
}
;

struct GfxStaticModelLightmapInfo
{
float offset[2];
float scale[2];
unsigned int lightmapIndex;
}
;

struct GfxStaticModelGroundLightingInfo
{
unsigned __int16 groundLighting[4];
}
;

struct GfxStaticModelLightGridLightingInfo
{
unsigned __int16 colorFloat16[4];
int a;
float b;
char __pad1[8];
}
;

union GfxStaticModelLighting
{
GfxStaticModelVertexLightingInfo vertexLightingInfo;
GfxStaticModelLightmapInfo modelLightmapInfo;
GfxStaticModelGroundLightingInfo modelGroundLightingInfo;
GfxStaticModelLightGridLightingInfo modelLightGridLightingInfo;
}
;

struct GfxSubdivVertexLightingInfo
{
int vertexLightingIndex;
void *vb;
GfxSubdivCache cache;
}
;

struct GfxDepthAndSurf
{
char __pad0[8];
}
;

typedef char *GfxWorldDpvsVoid
;

struct GfxWorldDpvsStatic
{
unsigned int smodelCount;
unsigned int subdivVertexLightingInfoCount;
unsigned int staticSurfaceCount;
unsigned int litOpaqueSurfsBegin;
unsigned int litOpaqueSurfsEnd;
unsigned int unkSurfsBegin;
unsigned int unkSurfsEnd;
unsigned int litDecalSurfsBegin;
unsigned int litDecalSurfsEnd;
unsigned int litTransSurfsBegin;
unsigned int litTransSurfsEnd;
unsigned int shadowCasterSurfsBegin;
unsigned int shadowCasterSurfsEnd;
unsigned int emissiveSurfsBegin;
unsigned int emissiveSurfsEnd;
unsigned int smodelVisDataCount;
unsigned int surfaceVisDataCount;
unsigned int *smodelVisData[4];
unsigned int *smodelUnknownVisData[27];
unsigned int *surfaceVisData[4];
unsigned int *surfaceUnknownVisData[27];
unsigned int *smodelUmbraVisData[4];
unsigned int *surfaceUmbraVisData[4];
unsigned int *lodData;
unsigned int *tessellationCutoffVisData;
unsigned int *sortedSurfIndex;
GfxStaticModelInst *smodelInsts;
GfxSurface *surfaces;
GfxSurfaceBounds *surfacesBounds;
GfxStaticModelDrawInst *smodelDrawInsts;
unsigned int *unknownSModelVisData1;
unsigned int *unknownSModelVisData2;
GfxStaticModelLighting *smodelLighting;
GfxSubdivVertexLightingInfo *subdivVertexLighting;
GfxDrawSurf *surfaceMaterials;
unsigned int *surfaceCastsSunShadow;
unsigned int sunShadowOptCount;
unsigned int sunSurfVisDataCount;
unsigned int *surfaceCastsSunShadowOpt;
GfxDepthAndSurf *surfaceDeptAndSurf;
GfxWorldDpvsVoid *constantBuffersLit;
GfxWorldDpvsVoid *constantBuffersAmbient;
int usageCount;
}
;

struct GfxWorldDpvsDynamic
{
unsigned int dynEntClientWordCount[2];
unsigned int dynEntClientCount[2];
unsigned int *dynEntCellBits[2];
unsigned __int8 *dynEntVisData[2][4];
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

struct GfxBuildInfo
{
const char *args0;
const char *args1;
const char *buildStartTime;
const char *buildEndTime;
}
;

enum FogTypes : __int8
{
FOG_NORMAL = 0x1,
FOG_DFOG = 0x2,
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
unsigned int portalGroupCount;
unsigned int lastSunPrimaryLightIndex;
unsigned int primaryLightCount;
unsigned int primaryLightEnvCount;
unsigned int sortKeyLitDecal;
unsigned int sortKeyEffectDecal;
unsigned int sortKeyTopDecal;
unsigned int sortKeyEffectAuto;
unsigned int sortKeyDistortion;
unsigned int sortKeyHair;
unsigned int sortKeyEffectBlend;
char __pad0[4];
GfxWorldDpvsPlanes dpvsPlanes;
GfxCellTreeCount *aabbTreeCounts;
GfxCellTree *aabbTrees;
GfxCell *cells;
GfxPortalGroup *portalGroup;
int unk_vec4_count_0;
char __pad1[4];
vec4_t *unk_vec4_0;
GfxWorldDraw draw;
GfxLightGrid lightGrid;
int modelCount;
GfxBrushModel *models;
Bounds unkBounds;
Bounds shadowBounds;
unsigned int checksum;
int materialMemoryCount;
MaterialMemory *materialMemory;
sunflare_t sun;
float outdoorLookupMatrix[4][4];
GfxImage *outdoorImage;
unsigned int *cellCasterBits;
unsigned int *cellHasSunLitSurfsBits;
GfxSceneDynModel *sceneDynModel;
GfxSceneDynBrush *sceneDynBrush;
unsigned int *primaryLightEntityShadowVis;
unsigned int *primaryLightDynEntShadowVis[2];
unsigned __int16 *nonSunPrimaryLightForModelDynEnt;
GfxShadowGeometry *shadowGeom;
GfxShadowGeometry *shadowGeomOptimized;
GfxLightRegion *lightRegion;
GfxWorldDpvsStatic dpvs;
GfxWorldDpvsDynamic dpvsDyn;
unsigned int mapVtxChecksum;
unsigned int heroOnlyLightCount;
GfxHeroOnlyLight *heroOnlyLights;
unsigned __int8 fogTypesAllowed;
unsigned int umbraTomeSize;
char *umbraTomeData;
umbraTomePtr_t umbraTomePtr;
unsigned int mdaoVolumesCount;
MdaoVolume *mdaoVolumes;
int unk1;
float unk2[6];
int unk3;
GfxBuildInfo buildInfo;
}
;

struct GfxLightImage
{
GfxImage *image;
unsigned __int8 samplerState;
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
PhysBrushModel *physModels;
dmPolytopeData *polytope;
dmMeshData *meshData;
unsigned int polytopeCount;
unsigned int meshDataCount;
}
;

struct Clut
{
int count0;
int count1;
int count2;
int pad;
char *unk;
const char *name;
}
;

struct pathnode_yaworient_t
{
float fLocalAngle;
float localForward[2];
}
;

union $3936EE84564F75EDA6DCBAC77A545FC8
{
pathnode_yaworient_t yaw_orient;
float angles[3];
}
;

union PathNodeParentUnion
{
scr_string_t name;
unsigned __int16 index;
}
;

enum PathNodeErrorCode : __int32
{
PNERR_NONE = 0x0,
PNERR_INSOLID = 0x1,
PNERR_FLOATING = 0x2,
PNERR_NOLINK = 0x3,
PNERR_DUPLICATE = 0x4,
PNERR_NOSTANCE = 0x5,
PNERR_INVALIDDOOR = 0x6,
PNERR_NOANGLES = 0x7,
PNERR_BADPLACEMENT = 0x8,
NUM_PATH_NODE_ERRORS = 0x9,
}
;

union $5F11B9753862CE791E23553F99FA1738
{
float minUseDistSq;
PathNodeErrorCode error;
}
;

struct pathlink_s
{
float fDist;
unsigned __int16 nodeNum;
unsigned __int8 disconnectCount;
unsigned __int8 negotiationLink;
unsigned __int8 flags;
unsigned __int8 ubBadPlaceCount[3];
}
;

struct pathnode_constant_t
{
unsigned __int16 type;
unsigned int spawnflags;
scr_string_t targetname;
scr_string_t script_linkName;
scr_string_t script_noteworthy;
scr_string_t target;
scr_string_t animscript;
int animscriptfunc;
float vLocalOrigin[3];
$3936EE84564F75EDA6DCBAC77A545FC8 ___u9;
PathNodeParentUnion parent;
$5F11B9753862CE791E23553F99FA1738 ___u11;
__int16 wOverlapNode[2];
char __pad0[4];
unsigned __int16 totalLinkCount;
pathlink_s *Links;
scr_string_t unk;
char __pad1[4];
}
;

struct SentientHandle
{
unsigned __int16 number;
unsigned __int16 infoIndex;
}
;

struct pathnode_dynamic_t
{
SentientHandle pOwner;
int iFreeTime;
int iValidTime[3];
__int16 wLinkCount;
__int16 wOverlapCount;
__int16 turretEntNumber;
unsigned __int8 userCount;
unsigned __int8 hasBadPlaceLink;
int spreadUsedTime[2];
__int16 flags;
__int16 dangerousCount;
int recentUseProxTime;
}
;

union $73F238679C0419BE2C31C6559E8604FC
{
float nodeCost;
int linkIndex;
}
;

struct pathnode_transient_t
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

struct pathnode_tree_t
{
int axis;
float dist;
pathnode_tree_info_t u;
}
;

union pathnode_tree_info_t
{
pathnode_tree_t *child[2];
pathnode_tree_nodes_t s;
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
unsigned __int8 *pathVis;
int nodeTreeCount;
pathnode_tree_t *nodeTree;
int dynamicNodeGroupCount;
PathDynamicNodeGroup *dynamicNodeGroups;
int exposureBytes;
unsigned __int8 *pathExposure;
int noPeekVisBytes;
unsigned __int8 *pathNoPeekVis;
int zoneCount;
int zonesBytes;
unsigned __int8 *pathZones;
int dynStatesBytes;
unsigned __int8 *pathDynStates;
}
;

union XAssetHeader
{
}
;

struct XAsset
{
XAssetType type;
XAssetHeader header;
}
;

struct XAssetEntry
{
XAsset asset;
char zoneIndex;
volatile char inuseMask;
unsigned int nextHash;
unsigned int nextOverride;
unsigned int nextPoolEntry;
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
void *buffer;
void *depthStencilState;
void *blendState;
}
;

typedef uint32_t GfxBlendStateBits[3]
;

struct XGfxGlobals
{
unsigned int depthStencilStateCount;
unsigned int blendStateCount;
uint64_t *depthStencilStateBits;
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

enum DBSyncMode
{
DB_LOAD_ASYNC = 0x0,
DB_LOAD_SYNC = 0x1,
DB_LOAD_ASYNC_WAIT_ALLOC = 0x2,
DB_LOAD_ASYNC_FORCE_FREE = 0x3,
DB_LOAD_ASYNC_NO_SYNC_THREADS = 0x4,
DB_LOAD_SYNC_SKIP_ALWAYS_LOADED = 0x5,
}
;

enum DBAllocFlags : __int32
{
DB_ZONE_NONE = 0x0,
DB_ZONE_COMMON = 0x1,
DB_ZONE_UI = 0x2,
DB_ZONE_GAME = 0x4,
DB_ZONE_LOAD = 0x8,
DB_ZONE_DEV = 0x10,
DB_ZONE_BASEMAP = 0x20,
DB_ZONE_TRANSIENT_POOL = 0x40,
DB_ZONE_TRANSIENT_MASK = 0x40,
DB_ZONE_CUSTOM = 0x1000,
}
;

struct XZoneInfo
{
const char *name;
int allocFlags;
int freeFlags;
}
;

struct XZoneInfoInternal
{
char name[64];
int flags;
int isBaseMap;
}
;

enum DXGI_FORMAT : __int32
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
DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE = 0xBD,
DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE = 0xBE,
DXGI_FORMAT_FORCE_UINT = 0xFFFFFFFF,
}
;

union Weapon
{
unsigned int data;
}
;

enum PlayerHandIndex : __int32
{
WEAPON_HAND_DEFAULT = 0x0,
WEAPON_HAND_RIGHT = 0x0,
WEAPON_HAND_LEFT = 0x1,
NUM_WEAPON_HANDS = 0x2,
}
;

struct PlayerWeaponCommonState
{
char __pad0[12];
Weapon weapon;
int weapFlags;
char __pad1[28];
PlayerHandIndex lastWeaponHand;
}
;

struct PlayerActiveWeaponState
{
int weapAnim;
int weaponTime;
int weaponDelay;
int weaponRestrictKickTime;
int weaponState;
int weapHandFlags;
unsigned int weaponShotCount;
}
;

enum sessionState_t : __int32
{
SESS_STATE_PLAYING = 0x0,
SESS_STATE_DEAD = 0x1,
SESS_STATE_SPECTATOR = 0x2,
SESS_STATE_INTERMISSION = 0x3,
}
;

struct usercmd_s
{
int serverTime;
unsigned int buttons;
char __pad0[20];
char forwardmove;
char rightmove;
char __pad1[34];
}
;

struct session_t
{
int connected;
usercmd_s cmd;
}
;

enum team_t : __int32
{
TEAM_FREE = 0x0,
TEAM_BAD = 0x0,
TEAM_AXIS = 0x1,
TEAM_ALLIES = 0x2,
TEAM_SPECTATOR = 0x3,
TEAM_NUM_TEAMS = 0x4,
}
;

struct gclient_s
{
char __pad0[2];
char pm_type;
char __pad1[297];
float angles[3];
char __pad2[236];
PlayerActiveWeaponState weaponState[2];
char __pad_2_almost_over[300];
PlayerWeaponCommonState weapCommon;
char __pad2_over[17620];
sessionState_t sessionState;
char __pad3[20];
session_t sess;
char __pad3_align[132];
team_t team;
char __pad4[30];
char name[32];
char __pad5[622];
int flags;
}
;

struct EntityState
{
uint16_t number;
}
;

struct EntHandle
{
uint16_t number;
uint16_t infoIndex;
}
;

struct gentity_s
{
EntityState s;
char _padding[106];
vec3_t trBase;
vec3_t trDelta;
char _padding1[12];
vec3_t currentAngles;
char pad_0008[108];
Bounds box;
Bounds absBox;
vec3_t origin;
char pad_0144[20];
gclient_s *client;
char _pad1[56];
scr_string_t script_classname;
char _pad2[20];
int flags;
int eventTime;
int clipmask;
char _pad3[60];
int missile_flags;
char _pad4[68];
EntHandle remoteControlledOwner;
char _pad5[156];
}
;

enum PLAYERCARD_CACHE_TASK_STAGE
{
PLAYERCARD_CACHE_TASK_STAGE_WAITING = 0x0,
PLAYERCARD_CACHE_TASK_STAGE_WORKING = 0x1,
PLAYERCARD_CACHE_TASK_STAGE_ALL_DONE = 0x2,
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

struct __declspec(align(8)) ClientInfo
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

struct std::ios_base
;

struct bdUserAccountID
;

struct bdHashMD5
;

struct _lambda_9a32fed5bf61b6b509b2d3f6003082a1_
;

struct bdChannelInfo
;

struct bdTags
;

struct std::locale::_Locimp
;

struct bdUPnPConfig
;

struct __FrameHandler3::TryBlockMap
;

struct bdTitleUtilities
;

struct std::_Locinfo
;

struct std::_Init_locks
;

struct std::_Lockit
;

struct __crt_stdio_output
;

struct __crt_strtox::floating_point_value
;

struct __crt_strtox
;

struct bdShutdownCompleteChunk
;

struct __ExceptionPtr
;

struct hks::dlmalloc
;

struct bdFacebookFriend
;

struct xWMA
;

struct Scr_AbstractScriptList
;

struct bdMarketplaceProduct
;

struct bdJSONTaskResult
;

struct LuaPlus::LuaStateOutFile
;

struct hks::netTransportTCPIP
;

struct D3D::CRTTextureTracker
;

struct LEAPCORE::CBaseSkin
;

struct bdConsoleID
;

struct CPartyLib
;

struct LEAPCORE::CSWVoice
;

struct hks::NetworkDebuggerUtils::DbgMessage
;

struct bdMutex
;

struct CThreadBase
;

struct FixtureJsonWriteArrayValues
;

struct JsonReadError
;

struct bdXHTTP
;

struct XAUDIO2::CX2SourceVoice
;

struct bdBufferParams
;

struct bdFeatureBanInfo
;

struct bdMailBody
;

struct hks::DebugInstance
;

struct bdSerializable
;

struct AnimationAimSet
{
scr_string_t name;
scr_string_t rootName;
scr_string_t animName[8];
unsigned __int64 rootIndex;
unsigned __int64 animIndices[8];
}
;

struct AnimationEntry
{
scr_string_t alias;
scr_string_t animName;
}
;

struct AnimationState
{
scr_string_t name;
scr_string_t notify;
float blendTime;
unsigned __int8 flags;
unsigned __int8 entryCount;
AnimationAimSet *aimSet;
AnimationEntry *animEntries;
unsigned __int64 *animIndices;
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

enum AnimationController : __int32
{
ANIMCTRL_NONE = 0x0,
ANIMCTRL_PLAYER = 0x1,
ANIMCTRL_DOG = 0x2,
ANIMCTRL_NUM = 0x3,
}
;

union AnimationClass::$A0347A865EB71909EA9E7CC50D8B0289
{
const char *className;
const char *name;
}
;

struct AnimationClass
{
union
{
const char *className;
const char *name;
};
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
FxEffectDef **effectDefs;
scr_string_t *effectTags;
}
;

struct TransientPoolSlot
{
unsigned __int64 usedSize[1];
char parentPoolIndex;
char nextFreeSlotIndex;
}
;

struct TransientPool
{
char name[64];
unsigned int entrySize[1];
unsigned __int64 numSlots;
unsigned __int64 numFiles;
unsigned __int8 *poolMemStart[1];
TransientPoolSlot *firstSlot;
unsigned __int16 freeSlotIndex;
}
;

union __declspec(align(64)) __m512
{
float m512_f32[16];
}
;

union __declspec(align(64)) __m512d
{
double m512d_f64[8];
}
;

union __declspec(align(64)) __m512i
{
__int8 m512i_i8[64];
__int16 m512i_i16[32];
__int32 m512i_i32[16];
__int64 m512i_i64[8];
unsigned __int8 m512i_u8[64];
unsigned __int16 m512i_u16[32];
unsigned __int32 m512i_u32[16];
unsigned __int64 m512i_u64[8];
}
;

struct GfxImageStreamLevelCountAndSize
{
__int32 pixelSize : 26;
__int32 levelCount : 6;
}
;

enum IMAGE_FLAG : unsigned __int8
{
IMAGE_FLAG_USE_SRGB_READS = 0x1,
IMAGE_FLAG_NOMIPMAPS = 0x2,
IMAGE_FLAG_DELAY_LOAD_PIXELS = 0x4,
IMAGE_FLAG_PARABOLOID = 0x8,
IMAGE_FLAG_HEATMAP = 0x10,
IMAGE_FLAG_STAGING = 0x20,
IMAGE_FLAG_TRANSIENT = 0x40,
}
;

union GfxTexture::$2EE8155D1ECF778FFB3D1CF05F7CC634
{
void *linemap;
void *map;
void *volmap;
void *cubemap;
GfxImageLoadDef *loadDef;
}
;

typedef unsigned __int16 uint16_t
;

struct __declspec(align(16)) _SETJMP_FLOAT128
{
unsigned __int64 Part[2];
}
;

typedef _ULonglong uint64_t
;

typedef unsigned __int64 _ULonglong
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

typedef struct _RUNTIME_FUNCTION *PRUNTIME_FUNCTION
;

struct _RUNTIME_FUNCTION
{
DWORD BeginAddress;
DWORD EndAddress;
DWORD UnwindData;
}
;

typedef unsigned int u_long
;

typedef DWORD *LPDWORD
;

typedef ULONG_PTR HCRYPTPROV
;

typedef struct addrinfo *PADDRINFOA
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

typedef unsigned __int64 uintptr_t
;

typedef size_t rsize_t
;

typedef WORD *LPWORD
;

typedef BOOL *LPBOOL
;

typedef const WCHAR *LPCWSTR
;

typedef struct _STARTUPINFOA *LPSTARTUPINFOA
;

typedef struct _PROCESS_INFORMATION *LPPROCESS_INFORMATION
;

typedef struct localeinfo_struct *_locale_t
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

typedef unsigned __int64 *PDWORD64
;

typedef struct _OVERLAPPED *LPOVERLAPPED
;

typedef HINSTANCE HMODULE
;

typedef char *va_list
;

typedef BYTE *PBYTE
;

typedef PVOID HDEVINFO
;

typedef ACCESS_MASK REGSAM
;

typedef DWORD ACCESS_MASK
;

typedef IUnknown *LPUNKNOWN
;

typedef HANDLE HLOCAL
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

typedef CHAR *PSTR
;

typedef unsigned __int8 *RPC_CSTR
;

typedef ULONG_PTR HCRYPTHASH
;

typedef int *LPINT
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

typedef struct HMIXER__ *HMIXER
;

struct HMIXER__
{
int unused;
}
;

typedef PRTL_CRITICAL_SECTION LPCRITICAL_SECTION
;

typedef struct _RTL_CRITICAL_SECTION *PRTL_CRITICAL_SECTION
;

typedef WORD LANGID
;

typedef ULONGLONG DWORDLONG
;

typedef struct _SECURITY_ATTRIBUTES *LPSECURITY_ATTRIBUTES
;

