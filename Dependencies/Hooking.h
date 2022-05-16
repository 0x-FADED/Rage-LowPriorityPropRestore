/*
 * This file is part of the CitizenFX project - http://citizen.re/
 *
 * See LICENSE and MENTIONS in the root of the source tree for information
 * regarding licensing.
 */

#pragma once
#include <Dependencies/patterns/Hooking.Patterns.h>

namespace hook
{
	template<typename ValueType, typename AddressType>
	inline void put(AddressType address, ValueType value)
	{
		DWORD oldProtect;
		VirtualProtect((void*)address, sizeof(value), PAGE_EXECUTE_READWRITE, &oldProtect);

		memcpy((void*)address, &value, sizeof(value));

		VirtualProtect((void*)address, sizeof(value), oldProtect, &oldProtect);

		FlushInstructionCache (GetCurrentProcess(), (void*)address, sizeof(value));
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

	template<typename Bytes, typename AddressType>
	inline void patch(AddressType address, std::initializer_list<Bytes> bytes)
	{

		DWORD oldProtect;
		VirtualProtect(reinterpret_cast<PVOID>(address), bytes.size(), PAGE_EXECUTE_READWRITE, &oldProtect);

		memcpy(reinterpret_cast<PVOID>(address), &*bytes.begin(), bytes.size());

		VirtualProtect(reinterpret_cast<PVOID>(address), bytes.size(), oldProtect, &oldProtect);

		FlushInstructionCache (GetCurrentProcess(), reinterpret_cast<PVOID>(address), bytes.size());
	}
}