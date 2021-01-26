#include <cstdio>
#include <Windows.h>

#include "../xorstr/xorstr.hpp"
#include "error.hpp"

void utils::report_error(std::string_view error_message) {
	const auto report_id = 0xDEADBEEF;
	// @todo: report error to the server

	char buffer[128];

	sprintf_s(buffer, sizeof buffer, STR_ENC("Please reach out to us via ticket providing the report ID: %08X"), report_id);

	MessageBoxA(nullptr, buffer, nullptr, MB_OK | MB_ICONERROR);

	TerminateProcess(GetCurrentProcess(), -1);
}