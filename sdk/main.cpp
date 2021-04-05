#include <windows.h>

#include "core/cheat/cheat.h"
#include "engine/hash/hash.h"
#include "engine/input/input.h"
#include "engine/logging/logging.h"
#include "engine/render/render.h"
#include "engine/pe/pe.h"
#include "engine/security/xorstr.h"

unsigned long __stdcall initial_thread(void *base_pointer)
{
#ifdef _DEBUG
	logging::init(SEVERITY_DEBUG);
#else
	logging::init(SEVERITY_INFO);
#endif

	auto i = 0;

	if (!pe::init())
		goto load_failed;

	while (pe::get_module(XORSTR("serverbrowser.dll")) == 0u)
	{
		if (++i == 5)
		{
			logging::error(XORSTR("couldn't find the 'serverbrowser.dll' module"));

			goto load_failed;
		}

		Sleep(1000);
	}

	if (!cheat::init())
		goto load_failed;

#ifdef _DEBUG
	while (!input::is_key_down(VK_DELETE) || !input::is_key_down(VK_END))
		Sleep(100);
#endif

load_failed:
#ifdef _DEBUG
	cheat::undo();

	FreeLibraryAndExitThread((HMODULE) base_pointer, 0);
#endif

	return 0;
}

int __stdcall DllMain(void *base_pointer, unsigned int reason_to_call, void *reserved)
{
	if (reason_to_call != 1)
		return 1;

#ifdef _DEBUG
	DisableThreadLibraryCalls((HMODULE) base_pointer);
	CreateThread(nullptr, 0, &initial_thread, base_pointer, 0, nullptr);
#else
	initial_thread(base_pointer);
#endif

	return 1;
}
