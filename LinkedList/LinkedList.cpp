#include "LinkedList.hpp"

CLinkedList::CLinkedList(GET_ELEMENT_LIST Type)
	: m_Type(Type)
{
	KeInitializeSpinLock(&m_SpinLock);
	InitializeListHead(&m_ListHead);
}

CLinkedList::~CLinkedList()
{
	Clear();
}

//Complexity: O(n)
void CLinkedList::ClearRecursive(PLIST_ENTRY ListEntry)
{
	if (ListEntry == &m_ListHead) return;
	const auto ListEntryNext = ListEntry->Flink;
	RemoveHeadList(&m_ListHead);
	ExFreePoolWithTag(ListEntry, LinkedListTag);
	ClearRecursive(ListEntryNext);
}

//Complexity: O(n)
PLIST_ENTRY CLinkedList::GetEntryByElementRecursive(void* pElement, PLIST_ENTRY ListEntry)
{
	if (ListEntry == &m_ListHead)
		return nullptr;

	void* pCompare = reinterpret_cast<PUCHAR>(ListEntry) + sizeof(LIST_ENTRY);
	switch (m_Type)
	{
	case GET_ELEMENT_LIST::AsciiString:
		if (strcmp((char*)pCompare, (char*)pElement) == 0) 
			return ListEntry;
		break;
	case GET_ELEMENT_LIST::UnicodeString:
		if (wcscmp((wchar_t*)pCompare, (wchar_t*)pElement) == 0) 
			return ListEntry;
		break;
	case GET_ELEMENT_LIST::UnicodeStringWindows:
		if (RtlEqualUnicodeString((PUNICODE_STRING)pCompare, (PUNICODE_STRING)pElement, TRUE))
			return ListEntry;
		break;
	case GET_ELEMENT_LIST::UnsignedInt:
		if (*(size_t*)pCompare == (size_t)pElement) 
			return ListEntry;
		break;
	}

	return GetEntryByElementRecursive(pElement, ListEntry->Flink);
}

PLIST_ENTRY CLinkedList::GetEntryByElement(void* pElement)
{
	const auto oldIRQL = KeAcquireSpinLockRaiseToDpc(&m_SpinLock);
	const auto pEntry = GetEntryByElementRecursive(pElement, m_ListHead.Flink);
	KeReleaseSpinLock(&m_SpinLock, oldIRQL);
	return pEntry;
}

void CLinkedList::Clear()
{
	const auto oldIRQL = KeAcquireSpinLockRaiseToDpc(&m_SpinLock);
	ClearRecursive(m_ListHead.Flink);
	KeReleaseSpinLock(&m_SpinLock, oldIRQL);
}

//Complexity: O(n)
PLIST_ENTRY CLinkedList::GetEntryByIndexRecursive(PLIST_ENTRY ListEntry, UCHAR i, UCHAR inc)
{
	return inc >= i || ListEntry == &m_ListHead ? ListEntry : GetEntryByIndexRecursive(ListEntry->Flink, i, inc + 1);
}

PLIST_ENTRY CLinkedList::GetEntryByIndex(UCHAR i)
{
	const auto oldIRQL = KeAcquireSpinLockRaiseToDpc(&m_SpinLock);
	UCHAR inc = 0;
	const auto result = GetEntryByIndexRecursive(m_ListHead.Flink, i, inc);
	KeReleaseSpinLock(&m_SpinLock, oldIRQL);
	return result;
}

void CLinkedList::Insert(bool bTail, PLIST_ENTRY pListEntry)
{
	const auto oldIRQL = KeAcquireSpinLockRaiseToDpc(&m_SpinLock);
	bTail ? InsertTailList(&m_ListHead, pListEntry)
		: InsertHeadList(&m_ListHead, pListEntry);
	KeReleaseSpinLock(&m_SpinLock, oldIRQL);
}

void CLinkedList::Remove(bool bTail, PLIST_ENTRY pListEntry)
{
	const auto oldIRQL = KeAcquireSpinLockRaiseToDpc(&m_SpinLock);

	if (IsListEmpty(&m_ListHead))
	{
		KeReleaseSpinLock(&m_SpinLock, oldIRQL);
		return;
	}

	if (!pListEntry)
	{
		bTail ? RemoveTailList(&m_ListHead)
			: RemoveHeadList(&m_ListHead);
	}
	else
	{
		RemoveEntryList(pListEntry);
	}

	KeReleaseSpinLock(&m_SpinLock, oldIRQL);
}
