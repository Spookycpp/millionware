#include <chrono>
#include <filesystem>
#include <functional>
#include <thread>
#include <Windows.h>

struct handle_wrapper {
	HANDLE handle;

	handle_wrapper(HANDLE handle)
		: handle(handle) {}

	~handle_wrapper() {
		CloseHandle(handle);
	}

	inline operator HANDLE() const {
		return handle;
	}

	inline bool operator ==(const HANDLE rhs) {
		return handle == rhs;
	}
};

template <typename T>
inline auto wait_on_object(T function, std::chrono::steady_clock::duration poll_interval = std::chrono::milliseconds(250), std::chrono::steady_clock::duration timeout = std::chrono::minutes(2)) -> decltype(function()) {
	const auto start_time = std::chrono::steady_clock::now();

	while (std::chrono::steady_clock::now() - start_time < timeout) {
		if (const auto result = function(); result)
			return result;

		std::this_thread::sleep_for(poll_interval);
	}

	return {};
}

inline int error_and_exit(int exit_code, std::wstring_view title, std::wstring_view message) {
	MessageBoxW(nullptr, message.data(), title.data(), MB_OK | MB_ICONERROR);

	return exit_code;
}

int main() {
	if (!std::filesystem::exists(L"millionware.dll"))
		return error_and_exit(1, L"millionware", L"can't find dll");

	const auto process_pid = wait_on_object([]() {
		auto process_id = 0ul;

		GetWindowThreadProcessId(FindWindowW(L"Valve001", nullptr), &process_id);

		return process_id;
	});

	if (process_pid == 0ul)
		return error_and_exit(2, L"millionware", L"game is not running");

	const handle_wrapper process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_pid);

	if (process_handle == INVALID_HANDLE_VALUE)
		return error_and_exit(3, L"millionware", L"can't open handle");

	wchar_t file_path[MAX_PATH] = L"";

	GetFullPathNameW(L"millionware.dll", MAX_PATH, file_path, nullptr);

	const auto allocated_buffer = reinterpret_cast<char*>(VirtualAllocEx(process_handle, nullptr, _countof(file_path) * 2, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));

	if (allocated_buffer == nullptr)
		return error_and_exit(4, L"millionware", L"can't allocate buffer");

	if (WriteProcessMemory(process_handle, allocated_buffer, file_path, _countof(file_path) * 2, nullptr) == 0)
		return error_and_exit(5, L"millionware", L"can't copy buffer");

	const handle_wrapper thread_handle = CreateRemoteThread(process_handle, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryW), allocated_buffer, 0, nullptr);

	if (thread_handle == INVALID_HANDLE_VALUE)
		return error_and_exit(6, L"millionware", L"can't start thread");

	WaitForSingleObject(thread_handle, INFINITE);
	VirtualFreeEx(process_handle, allocated_buffer, 0, MEM_RELEASE);

	return 0;
}
