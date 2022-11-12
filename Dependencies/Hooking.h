/*
 * This file is part of the CitizenFX project - http://citizen.re/
 *
 *  Use of this source code is subject to the terms provided on the CitizenFX
 *  Collective web site, currently versioned (4) at the following web URL:
 *  https://fivem.net/terms
 */

#pragma once

namespace hook
{
	//taken most of this code is taken form cfx.re and modfified a little
	template<typename ValueType, typename AddressType>
	inline void put(AddressType address, ValueType value)
	{
		DWORD oldProtect;
		VirtualProtect((void*)address, sizeof(value), PAGE_EXECUTE_READWRITE, &oldProtect);

		std::memcpy((void*)address, &value, sizeof(value));

		VirtualProtect((void*)address, sizeof(value), oldProtect, &oldProtect);

		FlushInstructionCache (GetCurrentProcess(), (void*)address, sizeof(value)); //not sure if we need this 
	}
	//simple code to patch stuff
	template<typename T, size_t Bytes, typename AddressType>
	inline void patch(AddressType address, const T(&patch)[Bytes])
	{
		DWORD oldProtect;
		VirtualProtect(reinterpret_cast<void*>(address), std::size(patch), PAGE_EXECUTE_READWRITE, &oldProtect);

		std::memcpy(reinterpret_cast<void*>(address), patch, std::size(patch));

		VirtualProtect(reinterpret_cast<void*>(address), std::size(patch), oldProtect, &oldProtect);

		FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<void*>(address), std::size(patch)); //not sure if we need this 
	}
}