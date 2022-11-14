#include "scanner.h"

	std::pair<uintptr_t, uintptr_t> GetModule(const std::wstring_view moduleName)
	{
			const static uintptr_t moduleBase = reinterpret_cast<uintptr_t>(GetModuleHandleW(moduleName.data()));
			const static uintptr_t moduleEnd = [&]()
			{
				auto ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS64>(moduleBase + reinterpret_cast<PIMAGE_DOS_HEADER>(moduleBase)->e_lfanew);
				return static_cast<uintptr_t>(moduleBase + ntHeaders->OptionalHeader.SizeOfImage);
			}();

			return { moduleBase, moduleEnd };
	}


	uintptr_t FindPattern(uintptr_t startAddress, uintptr_t maxSize, const char* mask)
	{
		std::vector<std::pair<uint8_t, bool>> pattern;

		for (size_t i = 0; i < strlen(mask);)
		{
			if (mask[i] != '?')
			{
				pattern.emplace_back(static_cast<uint8_t>(strtoul(&mask[i], nullptr, 16)), false);
				i += 3;
			}
			else
			{
				pattern.emplace_back(0x00, true);
				i += 2;
			}
		}

		const auto dataStart = reinterpret_cast<const uint8_t*>(startAddress);
		const auto dataEnd = dataStart + maxSize + 1;

		auto sig = std::search(dataStart, dataEnd, pattern.begin(), pattern.end(),
			[](uint8_t currentByte, std::pair<uint8_t, bool> Pattern)
			{
				return Pattern.second || (currentByte == Pattern.first);
			});

		if (sig == dataEnd)
			return NULL;

		return std::distance(dataStart, sig) + startAddress;
	}

	uintptr_t scanner::GetAddress(const std::wstring_view moduleName, const std::string_view pattern, ptrdiff_t offset)
	{
		if (GetModuleHandleW(moduleName.data()) != nullptr)
		{
			uintptr_t address = FindPattern(GetModule(moduleName.data()).first, GetModule(moduleName.data()).second - GetModule(moduleName.data()).first, pattern.data());

			if (address == NULL)
				throw std::runtime_error("Failed to find pattern");

			return (address + offset);
		}
	}

	uintptr_t scanner::GetOffsetFromInstruction(const std::wstring_view moduleName, const std::string_view pattern, ptrdiff_t offset)
	{
		if (GetModuleHandleW(moduleName.data()) != nullptr)
		{
			uintptr_t address = FindPattern(GetModule(moduleName.data()).first, GetModule(moduleName.data()).second - GetModule(moduleName.data()).first, pattern.data());

			if (address == NULL)
				throw std::runtime_error("Failed to find pattern");

			auto reloffset = *reinterpret_cast<int32_t*>(address + offset) + sizeof(int32_t);
			return (address + offset + reloffset);
		}
	}
