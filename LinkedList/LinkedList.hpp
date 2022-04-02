#pragma once

#include <ntifs.h>
#include <ntddk.h>
#include <wdm.h>

typedef enum GET_ELEMENT_LIST : UCHAR
{
	UnicodeStringWindows,
	UnicodeString,
	AsciiString,
	UnsignedInt,
	SignedInt
};

constexpr ULONG LinkedListTag = 0x600;

/*
Linked List struct template:

#pragma pack(push,1)
typedef struct
{
	LIST_ENTRY ListEntry; //Has to be first entry (Offset 0)
	void* pCompareElement; //Has to be right after ListEntry (Offset 0x10)
}EXAMPLE, * PEXAMPLE;
#pragma pack(pop)

*/

class CLinkedList
{
	GET_ELEMENT_LIST m_Type;
	KSPIN_LOCK m_SpinLock;
	LIST_ENTRY m_ListHead;

	PLIST_ENTRY GetEntryByIndexRecursive(PLIST_ENTRY ListEntry, UCHAR i, UCHAR inc);
	void ClearRecursive(PLIST_ENTRY ListEntry);
	PLIST_ENTRY GetEntryByElementRecursive(void* pElement, PLIST_ENTRY ListEntry);

public:

	CLinkedList(GET_ELEMENT_LIST Type);
	~CLinkedList();

	void Clear();
	PLIST_ENTRY GetEntryByIndex(UCHAR i);
	PLIST_ENTRY GetEntryByElement(void* pElement);
	void Insert(bool bTail, PLIST_ENTRY pListEntry);
	void Remove(bool bTail, PLIST_ENTRY pListEntry);
	auto GetListHead() const { return &m_ListHead; }
	auto GetSpinLock() const { return &m_SpinLock; }
};