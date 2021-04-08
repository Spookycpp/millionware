#include "../core/hooks/hooks.h"

#include "../engine/logging/logging.h"
#include "../engine/security/xorstr.h"

#include "../features/miscellaneous/miscellaneous.h"

void __fastcall hooks::emit_sound(uintptr_t ecx, uintptr_t edx, uintptr_t filter, int entity_index, int channel, const char* sound_entry, int sound_entry_hash, const char* sample_name,
	float volume, float attenuation, int seed, int flags, int pitch, const vector_t* origin, const vector_t* direction, vector_t* utl_vec_origins, bool update_positions, float sound_time,
	int speaker_entity, uintptr_t fds) {

	// @note: do we port fnv from old v3 base?
	if (!strcmp(sample_name, XORSTR("UIPanorama.popup_accept_match_beep"))) {
		features::miscellaneous::auto_accept();

		if (strstr(sample_name, XORSTR("competitive_accept_beep"))) {
			volume = 0.0f;
		}
	}
	
	emit_sound_original(ecx, edx, filter, entity_index, channel, sound_entry, sound_entry_hash, sample_name, volume, attenuation, seed, flags, pitch,
		origin, direction, utl_vec_origins, update_positions, sound_time, speaker_entity, fds);
}