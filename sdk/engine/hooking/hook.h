#pragma once

#include <stdint.h>
#include <windows.h>

#include "../../thirdparty/hde/hde32.h"

extern "C" NTSTATUS NTAPI NtProtectVirtualMemory(HANDLE handle, void **addr, ULONG *size, ULONG new_prot, PULONG old_prot);
extern "C" NTSTATUS NTAPI NtAllocateVirtualMemory(HANDLE handle, void **addr, ULONG_PTR zero_bits, SIZE_T *size, ULONG type, ULONG protect);
extern "C" NTSTATUS NTAPI NtFreeVirtualMemory(HANDLE handle, void **addr, SIZE_T *size, ULONG type);

struct InlineHook {
    static constexpr uint8_t jmp_asm[] = {0xff, 0x25, 0x00, 0x00, 0x00, 0x00};

    bool m_init = false;

    uintptr_t m_from = 0;
    uintptr_t m_size = 0;
    uintptr_t m_to = 0;
    uintptr_t m_tramp = 0;
    uintptr_t m_back = 0;

    void make(uintptr_t from, uintptr_t to) {
        if (m_init)
            return;

        m_from = (uintptr_t) from;
        m_to = (uintptr_t) to;

        auto follow_rel32 = [](uintptr_t addr) {
            auto disp = *(int32_t *) (addr + 1);

            return addr + 5 + disp;
        };

        if (*(uint8_t *) m_from == 0xe9)
            m_from = follow_rel32(m_from);

        auto hde = hde32s();

        while (m_size < sizeof(jmp_asm))
            m_size += hde32_disasm((const void *) (m_from + m_size), &hde);

        auto old_prot = 0ul;
        auto from_addr = m_from;
        auto mem_size = m_size + 0ul;

        NtAllocateVirtualMemory((HANDLE) -1, (void **) &m_tramp, 0, &mem_size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        NtProtectVirtualMemory((HANDLE) -1, (void **) &from_addr, &mem_size, PAGE_EXECUTE_READWRITE, &old_prot);

        memcpy((void *) m_tramp, (const void *) m_from, m_size);
        memcpy((void *) (m_tramp + m_size), (const void *) jmp_asm, sizeof(jmp_asm));
        memcpy((void *) m_from, (const void *) jmp_asm, sizeof(jmp_asm));

        m_back = m_from + m_size;

        *(uint32_t *) (m_tramp + m_size + 2) = (uintptr_t) &m_back;
        *(uint32_t *) (m_from + 2) = (uintptr_t) &m_to;

        NtProtectVirtualMemory((HANDLE) -1, (void **) &from_addr, &mem_size, old_prot, &old_prot);

        m_init = true;
    }

    template <typename F, typename... As>
    auto call_original(As &&...args) {
        return ((F) m_tramp) (args...);
    }

    void restore() {
        if (!m_init)
            return;

        auto old_prot = 0ul;
        auto from_addr = m_from;
        auto mem_size = m_size + 0ul;

        NtProtectVirtualMemory((HANDLE) -1, (void **) &from_addr, &mem_size, PAGE_EXECUTE_READWRITE, &old_prot);

        memcpy((void *) m_from, (const void *) m_tramp, m_size);

        NtProtectVirtualMemory((HANDLE) -1, (void **) &from_addr, &mem_size, old_prot, &old_prot);

        NtFreeVirtualMemory((HANDLE) -1, (void **) &m_tramp, &mem_size, MEM_RELEASE);

        m_init = false;
    }
};
