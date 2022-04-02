#include <ntifs.h>
#include <ntddk.h>
#include <wdm.h>
#include "CPP.hpp"
#include "LinkedList.hpp"

void DriverUnload(PDRIVER_OBJECT pDriverObject);

//Taken from LinkedList.hpp struct template
#pragma pack(push,1)
typedef struct
{
	LIST_ENTRY ListEntry;
	size_t compareElement;
}EXAMPLE, * PEXAMPLE;
#pragma pack(pop)

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	DriverObject->DriverUnload = reinterpret_cast<PDRIVER_UNLOAD>(&DriverUnload);

	const auto pList = new (LinkedListTag) CLinkedList(GET_ELEMENT_LIST::UnsignedInt);
	DbgPrintEx(0, 0, "Linked List object created: %p\n", pList);

	for (size_t i = 0; i < 15; i++)
	{
		const auto item = reinterpret_cast<PEXAMPLE>
			(ExAllocatePoolWithTag(NonPagedPoolNx, sizeof(EXAMPLE), LinkedListTag));
		if (item)
		{
			item->compareElement = i;
			pList->Insert(true, &item->ListEntry);
		}
	}

	DbgPrintEx(0, 0, "%p\n", pList->GetEntryByElement((void*)5));
	DbgPrintEx(0, 0, "%p\n", pList->GetEntryByElement((void*)10));

	delete (pList);
	return STATUS_SUCCESS;
}

void DriverUnload(PDRIVER_OBJECT pDriverObject)
{
	DbgPrintEx(0, 0, "DriverUnload called\n");
}