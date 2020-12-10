#include "../core/cheat.hpp"
#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"
#include "../features/misc/misc.hpp"
#include "../utils/xorstr.hpp"

void __fastcall hooks::emit_sound_hook(uintptr_t ecx, uintptr_t edx, uintptr_t filter, int entity_index, int channel, const char* sound_entry, int sound_entry_hash, const char* sample_name,
	float volume, float attenuation, int seed, int flags, int pitch, const vector3_t* origin, const vector3_t* direction, vector3_t* utl_vec_origins, bool update_positions, float sound_time,
	int speaker_entity, uintptr_t fds)
{
	const auto _ = std::lock_guard(hooks::emit_sound.call_mutex);

	if (FNV(sample_name) == FNV_CT("UIPanorama.popup_accept_match_beep")) {
		features::misc::auto_accept();

		if (strstr(sample_name, XOR("competitive_accept_beep")))
			volume = 0.0f;
	}

	reinterpret_cast<decltype(&emit_sound_hook)>(hooks::emit_sound.original)(
		ecx, edx, filter, entity_index, channel, sound_entry, sound_entry_hash, sample_name, volume, attenuation, seed, flags, pitch,
		origin, direction, utl_vec_origins, update_positions, sound_time, speaker_entity, fds
	);
}