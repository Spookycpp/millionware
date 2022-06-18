#include "v2 exports.h"

__declspec(dllexport) netvars_t g_netvars;

__declspec(dllexport) interfaces_t g_interfaces;

__declspec(dllexport) patterns_t g_patterns{
    7,

    pattern_t{"gameoverlayrenderer.dll", "FF 15 ? ? ? ? 8B F8 85 DB", 0},
    pattern_t{"gameoverlayrenderer.dll", "C7 45 ? ? ? ? ? FF 15 ? ? ? ? 8B F8", 0},
    pattern_t{"gameoverlayrenderer.dll", "FF 15 ? ? ? ? 8B F0 85 FF", 0},
    pattern_t{"gameoverlayrenderer.dll", "C7 45 ? ? ? ? ? FF 15 ? ? ? ? 8B D8", 0},
    pattern_t{"gameoverlayrenderer.dll", "56 B9 ? ? ? ? E8 ? ? ? ? 84 C0 74 1C", 0},
    pattern_t{"gameoverlayrenderer.dll", "E8 ? ? ? ? 53 FF 15 ? ? ? ? 8B C7", 0},

    pattern_t{"steamnetworkingsockets.dll", "B8 ? ? ? ? B9 ? ? ? ? 0F 43", 0},
};