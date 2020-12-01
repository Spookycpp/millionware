#include <cstdio>
#include <Windows.h>

#include "error.hpp"
#include "xorstr.hpp"

void utils::error_and_exit(e_error_code error) {
	char buffer[128];

	sprintf_s(buffer, sizeof buffer, XORSTR(
		"Fatal error during initialization. \n"
		"Please reach out to us via a ticket. \n"
		"Additional info: E%02d"
	), static_cast<int>(error));

	MessageBoxA(nullptr, buffer, nullptr, MB_OK | MB_ICONERROR);
	TerminateProcess(GetCurrentProcess(), -1);
}

void utils::error_and_exit(e_error_code error, uint32_t info1) {
	char buffer[128];

	sprintf_s(buffer, sizeof buffer, XORSTR(
		"Fatal error during initialization. \n"
		"Please reach out to us via a ticket. \n"
		"Additional info: E%02d %08X"
	), static_cast<int>(error), info1);

	MessageBoxA(nullptr, buffer, nullptr, MB_OK | MB_ICONERROR);
	TerminateProcess(GetCurrentProcess(), -1);
}

void utils::error_and_exit(e_error_code error, uint32_t info1, uint32_t info2) {
	char buffer[164];

	sprintf_s(buffer, sizeof buffer, XORSTR(
		"Fatal error during initialization. \n"
		"Please reach out to us via a ticket. \n"
		"Additional info: E%02d %08X %08X"
	), static_cast<int>(error), info1, info2);

	MessageBoxA(nullptr, buffer, nullptr, MB_OK | MB_ICONERROR);
	TerminateProcess(GetCurrentProcess(), -1);
}

void utils::error_and_exit(e_error_code error, uint32_t info1, uint32_t info2, uint32_t info3) {
	char buffer[164];

	sprintf_s(buffer, sizeof buffer, XORSTR(
		"Fatal error during initialization. \n"
		"Please reach out to us via a ticket. \n"
		"Additional info: E%02d %08X %08X %08X"
	), static_cast<int>(error), info1, info2, info3);

	MessageBoxA(nullptr, buffer, nullptr, MB_OK | MB_ICONERROR);
	TerminateProcess(GetCurrentProcess(), -1);
}
