#include "../core/cheat.hpp"
#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"
#include "../features/miscellaneous/miscellaneous.hpp"
#include "../utils/xorstr/xorstr.hpp"

void __fastcall hooks::emit_sound_hook(uintptr_t ecx, uintptr_t edx, uintptr_t filter, int entity_index, int channel, const char* sound_entry, int sound_entry_hash, const char* sample_name,
	float volume, float attenuation, int seed, int flags, int pitch, const vector3_t* origin, const vector3_t* direction, vector3_t* utl_vec_origins, bool update_positions, float sound_time,
	int speaker_entity, uintptr_t fds)
{
	if (FNV(sample_name) == FNV_CT("UIPanorama.popup_accept_match_beep")) {
		features::miscellaneous::auto_accept();

		if (strstr(sample_name, STR_ENC("competitive_accept_beep"))) {
			volume = 0.0f;
		}
	}

	// theres most certainly a better way to do this, no?
	if (entity_index == interfaces::engine_client->get_local_player()) {
		if (strstr(sample_name, STR_ENC("land")) && cheat::b_predicting) {
			cheat::b_predicting = false;
			cheat::landed = true;
			return;
		}

		if (strstr(sample_name, STR_ENC("suit")) && cheat::landed) {
			cheat::landed = false;
			return;
		}
	}

	hooks::emit_sound.get_original<decltype(&emit_sound_hook)>()(
		ecx, edx, filter, entity_index, channel, sound_entry, sound_entry_hash, sample_name, volume, attenuation, seed, flags, pitch,
		origin, direction, utl_vec_origins, update_positions, sound_time, speaker_entity, fds
		);
}