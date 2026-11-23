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

struct HINSTANCE__
{
int unused;
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

struct __crt_locale_pointers
{
struct __crt_locale_data *locinfo;
struct __crt_multibyte_data *mbcinfo;
}
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

struct Concurrency::details::SchedulerProxy
;

struct Concurrency::details::_RefCounter
;

struct __ExceptionPtr
;

struct Concurrency::details::VirtualProcessor
;

struct Concurrency::details::WorkQueue
;

struct std::exception
;

struct Concurrency::details::UMSFreeVirtualProcessorRoot
;

struct Concurrency::details::_TaskCollectionBase
;

struct Concurrency::details::WorkItem
;

struct Concurrency::details::_NonReentrantLock
;

struct Concurrency::details::_Concurrent_queue_base_v4
;

struct Concurrency::details::ScheduleGroupSegmentBase
;

struct Concurrency::details::ThreadScheduler
;

struct std::bad_weak_ptr
;

struct Concurrency::details::ExternalContextBase
;

struct _LocaleUpdate
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

typedef unsigned int DWORD
;

typedef void *PVOID
;

typedef unsigned __int64 ULONG_PTR
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

struct LargeLocal
;

struct hks::Visitor
;

struct bdSecurityID
;

struct bdSecurityKey
;

struct type_info
{
_UNKNOWN *vfptr __off;
int _M_data[2];
char _M_d_name[] __strlit(C,"windows-1252");
}
;

struct _PMD
{
int mdisp;
int pdisp;
int vdisp;
}
;

struct _RTTIClassHierarchyDescriptor
{
int signature;
int attributes;
int numBaseClasses;
int baseClassArray;
}
;

struct _RTTIBaseClassDescriptor
{
int typeDescriptor;
int numContainedBases;
_PMD pmd;
int attributes;
}
;

struct _RTTICompleteObjectLocator
{
int signature;
int offset;
int cdOffset;
int typeDescriptor;
int classDescriptor;
int objectBase;
}
;

struct bdSecurityKeyMap
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

typedef unsigned __int16 WORD
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

typedef unsigned __int8 BYTE
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

typedef __int64 LONGLONG
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

typedef void *HANDLE
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

typedef _LIST_ENTRY LIST_ENTRY
;

struct _LIST_ENTRY
{
_LIST_ENTRY *Flink;
_LIST_ENTRY *Blink;
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

typedef unsigned __int16 wchar_t
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

typedef int LONG
;

union _PEB::$6F1CA9A36B21C857AE5467E073440320
{
void *KernelCallbackTable;
void *UserSharedInfoPtr;
}
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

struct EntHandle
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

struct bdSockAddr
;

struct bdNetStartParams
;

struct bdInetAddr
;

struct bdString
;

struct MatchMakingInfo
;

struct bdChunk
;

struct bdCookie
;

struct bdLobbyConnectionListener
;

struct bdStatsInfo
;

struct bdLobbyService
;

struct bdGetHostByName
;

struct bdAddr
;

struct bdDTLSAssociation
;

struct bdGroupCount
;

struct bdFileMetaData
;

struct bdHTTPWrapper
;

struct bdBitBuffer
;

struct bdQoSReplyPacket
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

struct hks::CompilerReader
;

struct hks::CompilerLexer
;

struct hks::HashTable
;

struct hks::Parser
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

struct dwQoSMultiProbeListener
;

struct bdAuthService
;

struct bdSocketRouter
;

struct bdServiceBandwidthArbitrator
;

struct overlappedTask
;

struct CustomSearchInfo_FindPath
;

struct CustomSearchInfo_FindPathWithinRadius
;

struct SentientHandle
;

struct pathnode_t
;

struct std::_Iostream_error_category
;

struct hks::CompilerParseAcceptor
;

struct std::error_category
;

struct bdQoSRequestPacket
;

struct bdMessage
;

struct MemoryHashInfo
;

struct GfxCmdBufState
;

struct bdQoSProbeListener
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

struct bdRandom
;

struct bdHashTiger192
;

struct bdMutex
;

struct bdUserAccountID
;

struct bdRemoteTask
;

struct bdInitAckChunk
;

struct bdLobbyConnection
;

struct bdTag
;

struct bdMatchMakingInfo
;

struct PublicProfileInfo
;

struct bdPacketInterceptor
;

struct bdStorage
;

struct bdBandwidthTestResults
;

struct bdBandwidthTestClient
;

struct bdStats
;

struct bdLobbyEventHandler
;

struct bdReferencable
;

struct bdHTTPWrapperBase
;

struct bdFileInfo
;

struct bdTags
;

struct bdContentStreaming
;

struct bdContextUserStorageFileInfo
;

struct bdRemoteTaskManager
;

struct bdFileData
;

struct bdSessionInvite
;

struct bdCypher3Des
;

struct bdSessionID
;

struct bdAntiCheatResponses
;

struct bdAntiCheatChallenges
;

struct bdEventLogID
;

struct bdContentStreamingBase
;

struct bdPendingBufferTransfer
;

struct bdStatsInfoEx
;

struct bdUserInfo
;

struct bdUCD
;

struct bdTaskByteBuffer
;

struct bdSummaryMetaHandler
;

struct bdPreCopyResult
;

struct bdAuthTicket
;

struct bdAntiCheatChallenge
;

struct bdUCDPublicKeyInfo
;

struct bdAntiCheatChallengeParam
;

struct bdNetImpl
;

struct bdUPnPConfig
;

struct bdGetHostByNameConfig
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

struct bdLegacyStatsInfo
;

struct bdSecurityKeyMapListener
;

struct bdNATTravClient
;

struct bdNATTravClientData
;

struct bdNATTypeDiscoveryPacketReply
;

struct bdVoteRank
;

struct bdNATTypeDiscoveryConfig
;

struct bdNATTypeDiscoveryClient
;

struct bdNATTraversalPacket
;

struct bdIPDiscoveryConfig
;

struct bdIPDiscoveryClient
;

struct bdSocketRouterConfig
;

struct bdNATTravListener
;

struct bdDTLSError
;

struct bdDTLSInitAck
;

struct bdPacketBuffer
;

struct bdPacketBufferInternal
;

struct bdDTLSInit
;

struct bdDTLSCookieEcho
;

struct bdDTLSHeader
;

struct bdNATTypeDiscoveryPacket
;

struct bdIPDiscoveryPacketReply
;

struct bdIPDiscoveryPacket
;

struct bdDTLSData
;

struct std::bad_alloc
;

struct std::bad_exception
;

struct std::out_of_range
;

struct bdDMLInfo
;

struct bdConnectionStore
;

struct bdPacket
;

struct bdConnection
;

struct bdLoopbackConnection
;

struct bdUnicastConnection
;

struct bdConnectionStatistics
;

struct bdUnreliableSendWindow
;

struct bdUnreliableReceiveWindow
;

struct bdURL
;

struct bdDataChunk
;

struct bdReliableSendWindow
;

struct bdReliableReceiveWindow
;

struct bdInitChunk
;

struct bdCookieEchoChunk
;

struct bdCookieAckChunk
;

struct bdPerformanceValue
;

struct bdHeartbeatChunk
;

struct bdHeartbeatAckChunk
;

struct bdShutdownChunk
;

struct bdFeatureBanInfo
;

struct bdShutdownAckChunk
;

struct bdShutdownCompleteChunk
;

struct bdSAckChunk
;

struct bdStopwatch
;

struct bdRunnable
;

struct bdChannelInfo
;

struct bdSemaphore
;

struct bdHashMD5
;

struct bdSocket
;

struct bdHMacSHA1
;

struct bdHashSHA256
;

struct bdChannelMember
;

struct bdCypherAES
;

struct bdStreamSocket
;

struct bdRSAKey
;

struct bdThread
;

struct bdECCKey
;

struct bdHMacSHA256
;

struct bdSequenceNumber
;

struct bdSequenceNumberStore
;

struct bdAddrHandleRefWrapper
;

struct Leaderboard
;

struct bdQoSProbeInfo
;

struct bdFileID
;

struct bdDispatcher
;

struct dxSpace
;

struct bdConnectionStoreConfig
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

struct xWMA
;

struct PhysicalMemoryPrim
{
const char *allocName;
unsigned int allocListCount;
char __pad0[4];
unsigned __int64 pos;
PhysicalMemoryAllocation allocList[32];
}
;

struct PhysicalMemoryAllocation
{
const char *name;
unsigned __int64 pos;
}
;

struct PhysicalMemory
{
char *buf;
PhysicalMemoryPrim prim[2];
}
;

typedef _JBTYPE jmp_buf[16]
;

typedef char *va_list
;

typedef unsigned __int16 wint_t
;

typedef unsigned __int64 size_t
;

typedef unsigned __int64 uintptr_t
;

typedef size_t rsize_t
;

typedef __crt_locale_pointers *_locale_t
;

typedef struct _CONTEXT *PCONTEXT
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

