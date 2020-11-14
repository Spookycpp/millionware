#include "../gui/gui.hpp"
#include "../thirdparty/xorstr/xorstr.hpp"
#include "../utils/hash.hpp"
#include "menu.hpp"

void menu::frame() {
	auto& ctx = gui::get_window(HASH_FNV_CT("main"));

	ctx.window(XORSTR(L"millionware"), [&ctx]()
	{
		ctx.tab(XORSTR(L"Legit"), [&ctx]()
		{
			ctx.category(XORSTR(L"Pistols"), []() {});
			ctx.category(XORSTR(L"Heavy pistols"), []() {});
			ctx.category(XORSTR(L"Shotguns"), []() {});
			ctx.category(XORSTR(L"SMG"), []() {});
			ctx.category(XORSTR(L"Rifles"), []() {});
			ctx.category(XORSTR(L"Scout"), []() {});
			ctx.category(XORSTR(L"AWP"), []() {});
			ctx.category(XORSTR(L"Anto snipers"), []() {});
		});

		ctx.tab(XORSTR(L"Visuals"), [&ctx]()
		{
			ctx.category(XORSTR(L"Local player"), []() {});
			ctx.category(XORSTR(L"Friendlies"), []() {});
			ctx.category(XORSTR(L"Enemies"), []() {});
			ctx.category(XORSTR(L"World"), []() {});
		});

		ctx.tab(XORSTR(L"Misc"), [&ctx]() {});
		ctx.tab(XORSTR(L"Skins"), []() {});
		ctx.tab(XORSTR(L"Profile"), []() {});
	});
}
