#pragma once

#include <ntifs.h>

#include "mydef.h"

#define X86

//---------------------------------------
// DEFINE
//---------------------------------------
#ifdef X86
// win7 x86 EPROCESS �е�ƫ��
#define EP_OFFSET_PID (0x0b4)

#define EP_OFFSET_AC_LINKS (0x0b8)

#define EP_OFFSET_PPID (0x140)

#define EP_OFFSET_IMAGE_NAME (0x16c)

#define EP_OFFSET_OB_TABLE (0x0f4)

#define EP_OFFSET_THREAD_LIST (0x188)

#else

#define EP_OFFSET_PID (0x0b4)

#define EP_OFFSET_AC_LINKS (0x0b8)

#define EP_OFFSET_PPID (0x140)

#define EP_OFFSET_IMAGE_NAME (0x16c)

#define EP_OFFSET_OB_TABLE (0x0f4)

#define EP_OFFSET_THREAD_LIST (0x188)

#endif

#define IMAGE_NAME_TO_AC_LINKS (EP_OFFSET_IMAGE_NAME - EP_OFFSET_AC_LINKS)

#define PID_TO_AC_LINKS (EP_OFFSET_PID - EP_OFFSET_AC_LINKS)

#define PPID_TO_AC_LINKS (EP_OFFSET_PPID - EP_OFFSET_AC_LINKS)

#define PID_TO_EPROCESS (EP_OFFSET_PID)

#define PPID_TO_EPROCESS (EP_OFFSET_PPID)

#define AC_LINKS_TO_EPROCESS (EP_OFFSET_AC_LINKS)


#define GET_LIST_ENTRY_FROM_EPROCESS(_eprocessPtr) \
	((PLIST_ENTRY)\
	((PBYTE)(_eprocessPtr) + AC_LINKS_TO_EPROCESS))

#define GET_EPROCESS_FROM_LIST_ENTRY(_listEntryPtr) \
	((PEPROCESS)\
	((PBYTE)(_listEntryPtr) - AC_LINKS_TO_EPROCESS))

#define GET_IMAGE_NAME_FROM_LIST_ENTRY(_listEntryPtr) \
	((PCHAR)\
	((PBYTE)(_listEntryPtr) + IMAGE_NAME_TO_AC_LINKS))

#define GET_PID_FROM_EPROCESS(_eprocessPtr) \
	(*(PULONG)\
	((PBYTE)(_eprocessPtr) + PID_TO_EPROCESS))

#define GET_PPID_FROM_EPROCESS(_eprocessPtr) \
	(*(PULONG)\
	((PBYTE)(_eprocessPtr) + PPID_TO_EPROCESS))

//-----------------------------------------------
// GLOBAL PARAMETERS
//-----------------------------------------------


//-----------------------------------------------
// TYPES
//-----------------------------------------------
typedef struct _MY_PROC_INFO {
	ULONG Pid;
	ULONG PaPid;
	ULONG nameBytes;
	WCHAR ImageFileName[IMAGE_NAME_MAX_LEN];
}MY_PROC_INFO, *PMY_PROC_INFO;

typedef struct _MY_PROC_LIST_ENTRY {
	struct _MY_PROC_LIST_ENTRY *Next;
	PEPROCESS Process;
}MY_PROC_LIST_ENTRY, *PMY_PROC_LIST_ENTRY;


//-----------------------------------------------
// FUNCTIONS DECLARATION
//-----------------------------------------------

NTSTATUS getProcessList();

NTSTATUS getProcInfoNext(
	_Inout_ PMY_PROC_INFO ProcInfo
	);

VOID freeListPool();

VOID setActiveProcessHead(
	_In_ PLIST_ENTRY Head
	);

VOID printActiveProcesses();
