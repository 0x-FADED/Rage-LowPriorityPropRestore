/*
 * This file is part of the CitizenFX project - http://citizen.re/
 *
 *  Use of this source code is subject to the terms provided on the CitizenFX
 *  Collective web site, currently versioned (4) at the following web URL:
 *  https://fivem.net/terms
 */



#pragma once
#include <Dependencies/patterns/Hooking.Patterns.h>

namespace hook
{
	//taken most of this code is taken form cfx.re and modfified a little
	template<typename ValueType, typename AddressType>
	inline void put(AddressType address, ValueType value)
	{
		DWORD oldProtect;
		VirtualProtect((void*)address, sizeof(value), PAGE_EXECUTE_READWRITE, &oldProtect);

		memcpy((void*)address, &value, sizeof(value));

		VirtualProtect((void*)address, sizeof(value), oldProtect, &oldProtect);

		FlushInstructionCache (GetCurrentProcess(), (void*)address, sizeof(value)); //not sure if we need this 
	}

	template<typename T, typename TAddr>
	inline T get_address(TAddr address)
	{
		intptr_t target = *(int32_t*)(ptrdiff_t(address));
		target += (ptrdiff_t(address) + 4);

		return (T)target;
	}

	template<typename T>
	inline T get_call(T address)
	{
		intptr_t target = *(int32_t*)(uintptr_t(address) + 1);
		target += (uintptr_t(address) + 5);

		return (T)target;
	}

	//this code below is written by me for patching stuff on might not be be good but works
	template<typename Bytes, typename AddressType>
	inline void patch(AddressType address, std::initializer_list<Bytes> bytes)
	{

		DWORD oldProtect;
		VirtualProtect(reinterpret_cast<PVOID>(address), bytes.size(), PAGE_EXECUTE_READWRITE, &oldProtect);

		memcpy(reinterpret_cast<PVOID>(address), &*bytes.begin(), bytes.size());

		VirtualProtect(reinterpret_cast<PVOID>(address), bytes.size(), oldProtect, &oldProtect);

		FlushInstructionCache (GetCurrentProcess(), reinterpret_cast<PVOID>(address), bytes.size()); //not sure if we need this
	}
}