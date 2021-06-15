#include "v2 exports.h"

__declspec(dllexport) netvars_t g_netvars;

__declspec(dllexport) interfaces_t g_interfaces;

__declspec(dllexport) patterns_t g_patterns{
    36,
    pattern_t{"engine.dll", "53 56 57 8B DA 8B F9 FF 15", 0},
    pattern_t{"engine.dll", "A1 ? ? ? ? 8B 80 ? ? ? ? C3", 0},
    pattern_t{"engine.dll", "8B 0D ? ? ? ? 8D 95 ? ? ? ? 6A 00 C6", 0},
    pattern_t{"engine.dll", "A1 ? ? ? ? B9 ? ? ? ? 56 FF 50 14 8B 34 85", 0},
    pattern_t{"engine.dll", "55 8B EC 81 EC ? ? ? ? 53 8B D9 89 5D F8 80", 0},
    pattern_t{"engine.dll", "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45", 0},
    pattern_t{"engine.dll", "8B 0D ? ? ? ? 81 F9 ? ? ? ? 75 ? A1 ? ? ? ? 35 ? ? ? ? EB ? 8B 01 FF 50 ? 83 F8 ? 0F 85 ? ? ? ? 8B 0D", 0},

    pattern_t{"client.dll", "8B 35 ? ? ? ? FF 10 0F B7 C0", 0},
    pattern_t{"client.dll", "E8 ? ? ? ? F6 86 04 01 00 00 01", 0},
    pattern_t{"client.dll", "E8 ? ? ? ? FF 76 0C 8D 48 04 E8", 0},
    pattern_t{"client.dll", "89 44 24 14 EB 08 C7 44 24 ? ? ? ? ? 8B 45", 0},
    pattern_t{"client.dll", "8B 0D ? ? ? ? 8B 46 08 68", 0},
    pattern_t{"client.dll", "0F 11 05 ? ? ? ? 83 C8 01", 0},
    pattern_t{"client.dll", "A3 ? ? ? ? 57 8B CB", 0},
    pattern_t{"client.dll", "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8", 0},
    pattern_t{"client.dll", "0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9", 0},
    pattern_t{"client.dll", "56 8B F1 85 F6 74 31", 0},
    pattern_t{"client.dll", "B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08", 0},
    pattern_t{"client.dll", "55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12", 0},
    pattern_t{"client.dll", "55 8B EC 51 53 56 8B 75 08 8B D9 57 6B FE 2C", 0},
    pattern_t{"client.dll", "55 8B EC 83 EC 0C 53 56 8B 71 58", 0},
    pattern_t{"client.dll", "55 8B EC 51 56 8B F1 85 F6 74 68 83 BE", 0},
    pattern_t{"client.dll", "A1 ? ? ? ? 56 8B F1 B9 ? ? ? ? FF 50 08", 0},
    pattern_t{"client.dll", "55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0", 0},
    pattern_t{"client.dll", "85 C0 75 30 38 86", 0},
    pattern_t{"client.dll", "B9 ? ? ? ? F3 0F 11 04 24 FF 50 10", 0},
    pattern_t{"client.dll", "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8", 0},
    pattern_t{"client.dll", "84 C0 75 04 B0 01 5F", 0},
    pattern_t{"client.dll", "F3 0F 10 86 ? ? ? ? 0F 2F 40 10 76 30", 0},

    pattern_t{"gameoverlayrenderer.dll", "FF 15 ? ? ? ? 8B F8 85 DB", 0},
    pattern_t{"gameoverlayrenderer.dll", "C7 45 ? ? ? ? ? FF 15 ? ? ? ? 8B F8", 0},
    pattern_t{"gameoverlayrenderer.dll", "FF 15 ? ? ? ? 8B F0 85 FF", 0},
    pattern_t{"gameoverlayrenderer.dll", "C7 45 ? ? ? ? ? FF 15 ? ? ? ? 8B D8", 0},
    pattern_t{"gameoverlayrenderer.dll", "56 B9 ? ? ? ? E8 ? ? ? ? 84 C0 74 1C", 0},
    pattern_t{"gameoverlayrenderer.dll", "E8 ? ? ? ? 53 FF 15 ? ? ? ? 8B C7", 0},

    pattern_t{"steamnetworkingsockets.dll", "B8 ? ? ? ? B9 ? ? ? ? 0F 43", 0},
};