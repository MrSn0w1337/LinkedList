#pragma once

void* __cdecl operator new(size_t size, ULONG tag)
{
	const PVOID newAddress = ExAllocatePoolWithTag(NonPagedPoolNx, size, tag);
	if (newAddress) memset(newAddress, 0, size);
	return newAddress;
}

void __cdecl operator delete(void* p, unsigned __int64)
{
	ExFreePool(p);
}