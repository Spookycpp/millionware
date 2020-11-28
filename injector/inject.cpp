#include "misc_utils.hpp"
#include "win_utils.hpp"

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

int main() {
	RET_CHK(impl::enable_privilege(L"SeDebugPrivilege"), L"can't enable debug privilege")
		RET_CHK(std::filesystem::exists(L"millionware.dll"), L"can't find dll")

		const auto process_pid = impl::wait_on_object([]() {
		uint32_t process_id = 0;
		GetWindowThreadProcessId(FindWindowW(L"Valve001", nullptr), reinterpret_cast<LPDWORD>(&process_id));

		return process_id;
			});

	RET_CHK(process_pid, L"timed out")

		const auto process_handle = impl::uq_handle{ OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_pid), &CloseHandle };

	RET_CHK(process_handle, L"can't open handle")

		// get full path to the dll, since this will be called from the game's context, where our dll doesn't exist within the same folder.
		wchar_t file_path[MAX_PATH] = L"";
	RET_CHK(GetFullPathNameW(L"millionware.dll", MAX_PATH, file_path, nullptr), L"can't get full path")

		const auto allocated_buffer = reinterpret_cast<char*>(VirtualAllocEx(process_handle.get(), nullptr, _countof(file_path) * 2, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));

	RET_CHK(allocated_buffer, L"can't allocate buffer")
		RET_CHK(WriteProcessMemory(process_handle.get(), allocated_buffer, file_path, _countof(file_path) * 2, nullptr), L"can't copy buffer")

		const auto thread_handle = impl::uq_handle{ CreateRemoteThread(process_handle.get(), nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryW), allocated_buffer, 0, nullptr), &CloseHandle };

	RET_CHK(thread_handle, L"can't start thread")

		WaitForSingleObject(thread_handle.get(), INFINITE);
	VirtualFreeEx(process_handle.get(), allocated_buffer, 0, MEM_RELEASE);

	return EXIT_SUCCESS;
}