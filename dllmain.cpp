#include <Windows.h>
#include <stdexcept>
#include <memory>
#include "Dependencies/Hooking.h"
#include "Dependencies/minhook/include/MinHook.h"

#pragma comment(lib,"Dependencies/minhook/libMinHook.x64.lib")


//credits to Disquse for research assistance and LMS for some valuable advice 
enum class GameType
{
	Invalid = 0,
	GrandTheftAutoV = 1,
	RedDeadRedemption2 = 2,
};

//address of rage::fwMapData::ms_entityLevelCap default value here is 0 we want to set this to 3
static auto loc = hook::get_address<int32_t*>(hook::get_module_pattern<uint8_t>(L"RDR2.exe", "0F 45 C2 89 05 ? ? ? ? 89 05", 0xB));

typedef VOID(*func_t)();
static func_t g_origfunc = nullptr;

static VOID hk_func()
{
	auto result = g_origfunc;

	// doing this cuz game keeps setting rage::fwMapData::ms_entityLevelCap to 0

	if (!result)
	{
		return result();
	}

	const uint8_t patch[] { 0x03 };

	//this is a horrible hack
	hook::patch(loc, patch); //rage::fwMapData::ms_entityLevelCap is now 3 ..XD	
}
void modInit(GameType Game)
{
	switch (Game)
	{

	case GameType::GrandTheftAutoV:

		// credits to cfx for finding this
		// sets rage::fwMapData::ms_entityLevelCap to PRI_OPTIONAL_LOW

		hook::put<uint32_t>(hook::get_pattern<uint8_t>("BB 02 ? ? ? 39 1D", 1), 3); // for GTAV mov ebx, 0x02 to mov ebx, 0x03

		break;


	case GameType::RedDeadRedemption2:

		// we hook the function where our variable is

		MH_Initialize();

		MH_CreateHook((hook::get_call((hook::get_pattern<uint8_t>("0F 47 C7 88 05", 0x9)))), hk_func, reinterpret_cast<void**>(&g_origfunc));

		MH_EnableHook(MH_ALL_HOOKS);

		break;
	}

}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
	{
		wchar_t modulePath[MAX_PATH]{};
		GetModuleFileNameW(GetModuleHandleW(nullptr), modulePath, static_cast<DWORD>(std::size(modulePath)));

		wchar_t executableName[MAX_PATH]{};
		_wsplitpath_s(modulePath, nullptr, 0, nullptr, 0, executableName, std::size(executableName), nullptr, 0);

		auto gameType = GameType::Invalid;

		if (!_wcsicmp(executableName, L"GTA5"))
			gameType = GameType::GrandTheftAutoV;
		else if (!_wcsicmp(executableName, L"RDR2"))
			gameType = GameType::RedDeadRedemption2;

		try
		{
			if (gameType == GameType::Invalid)
				throw std::runtime_error("Trying to use the mod with an unsupported game. 'GTA5.exe' or 'RDR2.exe' are expected");
			modInit(gameType);

		}
		catch (const std::exception& e)
		{
			wchar_t buffer[2048];

			swprintf_s(buffer,
				L"An exception has occurred on startup: %hs. Failed to initialize the mod. please contact mod author"
				" the Game version which are supproted by scripthook is the minimum requirement.\n\nExecutable path: %ws", e.what(), modulePath);

			MessageBoxW(nullptr, buffer, L"Error", MB_ICONERROR);
		}
	}

	break;
	}


	return TRUE;
}
