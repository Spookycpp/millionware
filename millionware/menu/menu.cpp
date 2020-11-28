#include "../core/interfaces.hpp"
#include "../gui/gui.hpp"
#include "../thirdparty/xorstr/xorstr.hpp"
#include "../utils/hash.hpp"
#include "menu.hpp"

void menu::frame() {
	gui::window(XORSTR(L"millionware"), []()
	{
		gui::tab(XORSTR(L"Aimbot"), []()
		{
			gui::category(e_texture::QUESTION_MARK_22, []() {});
			gui::category(e_texture::QUESTION_MARK_22, []() {});
			gui::category(e_texture::QUESTION_MARK_22, []() {});
			gui::category(e_texture::QUESTION_MARK_22, []() {});
			gui::category(e_texture::QUESTION_MARK_22, []() {});
		});
		gui::tab(XORSTR(L"Visuals"), []()
		{
			gui::category(e_texture::QUESTION_MARK_22, []() {});
			gui::category(e_texture::QUESTION_MARK_22, []() {});
			gui::category(e_texture::QUESTION_MARK_22, []() {});
			gui::category(e_texture::QUESTION_MARK_22, []() {});
			gui::category(e_texture::QUESTION_MARK_22, []() {});
		});
		gui::tab(XORSTR(L"Misc"), []()
		{
			gui::category(e_texture::QUESTION_MARK_22, []() {});
			gui::category(e_texture::QUESTION_MARK_22, []() {});
			gui::category(e_texture::QUESTION_MARK_22, []() {});
			gui::category(e_texture::QUESTION_MARK_22, []() {});
			gui::category(e_texture::QUESTION_MARK_22, []() {});
		});
		gui::tab(XORSTR(L"Changers"), []()
		{
			gui::category(e_texture::QUESTION_MARK_22, []() {});
			gui::category(e_texture::QUESTION_MARK_22, []() {});
			gui::category(e_texture::QUESTION_MARK_22, []() {});
			gui::category(e_texture::QUESTION_MARK_22, []() {});
			gui::category(e_texture::QUESTION_MARK_22, []() {});
		});
		gui::tab(XORSTR(L"Profile"), []()
		{
			gui::category(e_texture::QUESTION_MARK_22, []() {});
			gui::category(e_texture::QUESTION_MARK_22, []() {});
			gui::category(e_texture::QUESTION_MARK_22, []() {});
			gui::category(e_texture::QUESTION_MARK_22, []() {});
			gui::category(e_texture::QUESTION_MARK_22, []() {});
		});
	});
}
