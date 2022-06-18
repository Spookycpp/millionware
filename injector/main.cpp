#include <filesystem>
#include <windows.h>

#include "utils.h"

int main() {
    check_return(enable_privilege("SeDebugPrivilege"), "can't enable debug privilege");
    check_return(std::filesystem::exists("sdk.dll"), "can't find cheat dll");

    char file_path[MAX_PATH];

    check_return(GetFullPathNameA("sdk.dll", sizeof(file_path), file_path, nullptr), "can't get full path");

    const auto process_pid = wait_for_object([]() {
        auto process_id = 0ul;

        GetWindowThreadProcessId(FindWindowA("Valve001", nullptr), &process_id);

        return process_id;
    });

    check_return(process_pid, "timed out");

    const auto process_handle = unique_handle{OpenProcess(PROCESS_ALL_ACCESS, false, process_pid), &CloseHandle};

    check_return(process_handle, "can't open handle");

    const auto allocated_buffer =
        VirtualAllocEx(process_handle.get(), nullptr, strlen(file_path) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    check_return(allocated_buffer, "can't allocate buffer");
    check_return(WriteProcessMemory(process_handle.get(), allocated_buffer, file_path, strlen(file_path) + 1, nullptr),
                 "can't copy buffer");

    const auto thread_handle = unique_handle{
        CreateRemoteThread(process_handle.get(), nullptr, 0, (LPTHREAD_START_ROUTINE) LoadLibraryA, allocated_buffer, 0, nullptr),
        &CloseHandle};

    check_return(thread_handle, "can't start thread");

    WaitForSingleObject(thread_handle.get(), INFINITE);
    VirtualFreeEx(process_handle.get(), allocated_buffer, 0, MEM_RELEASE);

    return EXIT_SUCCESS;
}
